//
//  Main.cpp provided under CC0 license
//
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
#include <glm/gtx/string_cast.hpp>

#include "sgct/sgct.h"

#include "websockethandler.h"
#include "utility.hpp"
#include "game.hpp"
#include "sceneobject.hpp"
#include "player.hpp"
#include "modelmanager.hpp"
#include "backgroundobject.hpp"

namespace {
	std::unique_ptr<WebSocketHandler> wsHandler;

	//Container for deserialized game state info
	std::vector<SyncableData> gameObjectStates;

	//TEMPORARY used to control rotation of all players 
	float updatedRotation{ 0 };
} // namespace

using namespace sgct;

/****************************
	FUNCTIONS DECLARATIONS
*****************************/
void initOGL(GLFWwindow*);
void draw(const RenderData& data);
void cleanup();

std::vector<std::byte> encode();
void decode(const std::vector<std::byte>& data, unsigned int pos);
void preSync();
void postSyncPreDraw();

void keyboard(Key key, Modifier modifier, Action action, int);

void connectionEstablished();
void connectionClosed();
void messageReceived(const void* data, size_t length);

/****************************
		CONSTANTS 
*****************************/
const std::string rootDir = Utility::findRootDir();

/****************************
			MAIN
*****************************/
int main(int argc, char** argv)
{
	std::vector<std::string> arg(argv + 1, argv + argc);
	Configuration config = sgct::parseArguments(arg);	

	//Choose which config file (.xml) to open
	//config.configFilename = rootDir + "/src/configs/fisheye_testing.xml";
	//config.configFilename = rootDir + "/src/configs/simple.xml";
	//config.configFilename = rootDir + "/src/configs/six_nodes.xml";
	config.configFilename = rootDir + "/src/configs/two_fisheye_nodes.xml";

	config::Cluster cluster = sgct::loadCluster(config.configFilename);

	//Provide functions to engine handles
	Engine::Callbacks callbacks;
	callbacks.initOpenGL = initOGL;
	callbacks.preSync = preSync;
	callbacks.encode = encode;
	callbacks.decode = decode;
	callbacks.postSyncPreDraw = postSyncPreDraw;
	callbacks.draw = draw;
	callbacks.cleanup = cleanup;
	callbacks.keyboard = keyboard;

	//Initialize engine
	try {
		gameObjectStates.reserve(Game::mMAXPLAYERS*3);
		Engine::create(cluster, callbacks, config);		
	}
	catch (const std::runtime_error & e) {
		Log::Error("%s", e.what());
		Engine::destroy();
		return EXIT_FAILURE;
	}

	if (Engine::instance().isMaster()) {
		wsHandler = std::make_unique<WebSocketHandler>(
			"localhost",
			81,
			connectionEstablished,
			connectionClosed,
			messageReceived
		);
		constexpr const int MessageSize = 1024;
		wsHandler->connect("example-protocol", MessageSize);
	}	
	/**********************************/
	/*			 Test Area			  */
	/**********************************/

	Engine::instance().render();

	Game::destroy();
	Engine::destroy();
	return EXIT_SUCCESS;
}

void draw(const RenderData& data)
{
	Game::instance().setMVP(data.modelViewProjectionMatrix);
	Game::instance().setV(data.viewMatrix);


	glClearColor(20.0/255.0, 157.0/255.0, 190.0/255.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Background object
	//Game::instance()
	//glDisable(GL_DEPTH_TEST);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE); // TODO This should really be enabled but the normals of the
	                          // background object are flipped atm
	glCullFace(GL_BACK);

	Game::instance().render();

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		sgct::Log::Error("GL Error: 0x%x", err);
	}
}

void initOGL(GLFWwindow*)
{
	ModelManager::init();
	Game::init();

	/**********************************/
	/*			 Debug Area			  */
	/**********************************/
	if (Engine::instance().isMaster())
	{
		for (size_t i = 0; i < 1; i++)
		{
			Game::instance().addPlayer(glm::vec3(0.f + 0.2f*i));
		}
	}
}

void keyboard(Key key, Modifier modifier, Action action, int)
{
	if (key == Key::Esc && action == Action::Press) {
		Engine::instance().terminate();
	}
	if (key == Key::Space && modifier == Modifier::Shift && action == Action::Release)
	{
		Log::Info("Released space key, disconnecting");
		wsHandler->disconnect();
	}

	//Left
	if (key == Key::A && (action == Action::Press || action == Action::Repeat))
	{
		Game::instance().rotateAllPlayers(0.1f);
	}
	//Right
	if (key == Key::D && (action == Action::Press || action == Action::Repeat))
	{
		Game::instance().rotateAllPlayers(-0.1f);
	}
}

void preSync()
{
	// Do the application simulation step on the server node in here and make sure that
	// the computed state is serialized and deserialized in the encode/decode calls

	//Run game simulation on master only
	if (Engine::instance().isMaster())
	{
		wsHandler->tick();
		Game::instance().update();
	}
}

std::vector<std::byte> encode()
{
	std::vector<std::byte> output;

	//For some reason everything has to to be put in one vector to avoid sgct syncing bugs
	serializeObject(output, Game::instance().getSyncableData());

	return output;
}

void decode(const std::vector<std::byte>& data, unsigned int pos)
{
	if (!Game::exists()) //No point in syncing data if no instance of Game exist yet
		return;

	//For some reason everything has to to be put in one vector to avoid sgct syncing bugs
	deserializeObject(data, pos, gameObjectStates);
}

void cleanup()
{
	// Cleanup all of your state, particularly the OpenGL state in here.  This function
	// should behave symmetrically to the initOGL function
}

void postSyncPreDraw()
{
	//Sync gameobjects' state on clients only
	if (!Engine::instance().isMaster() && Game::exists() && gameObjectStates.size() > 0)
	{
		Game::instance().setSyncableData(std::move(gameObjectStates));
	}
}

void connectionEstablished()
{
	Log::Info("Connection established");
}

void connectionClosed()
{
	Log::Info("Connection closed");
}

void messageReceived(const void* data, size_t length)
{
	std::string_view msg = std::string_view(reinterpret_cast<const char*>(data), length);
	//Log::Info("Message received: %s", msg.data());

	std::string message = msg.data();

	if (!message.empty())
	{
		//Get an easily manipulated stream of the message and read type of message
		std::istringstream iss(message);
		char msgType;
		iss >> msgType;

		// If first slot is 'N', a name and unique ID has been sent
		if (msgType == 'N') {
			Log::Info("Player connected: %s", message.c_str());
			Game::instance().addPlayer(Utility::getNewPlayerData(iss));
		}

		// If first slot is 'C', the rotation angle has been sent
		if (msgType == 'C') {
			Game::instance().updateTurnSpeed(Utility::getTurnSpeed(iss));
		}
        
        // If first slot is 'D', player to be deleted has been sent
        if (msgType == 'D') {
            unsigned int playerId;
            iss >> playerId;
            Game::instance().disablePlayer(playerId);
        }
        
        // If first slot is 'I', player's ID has been sent
        if (msgType == 'I') {
            unsigned int playerId;
            iss >> playerId;
            // Send colour information back to server
            std::pair<glm::vec3, glm::vec3> colours = Game::instance().getPlayerColours(playerId);
            std::string colourOne = glm::to_string(colours.first);
            std::string colourTwo = glm::to_string(colours.second);
            
            wsHandler->queueMessage("A " + colourOne);
            wsHandler->queueMessage("B " + colourTwo);
        }
        
        // If first slot is 'P', player's ID has been sent
        if (msgType == 'P') {
            unsigned int playerId;
            iss >> playerId;
            // Send colour information back to server
            const int playerPoints = Game::instance().getPlayerPoints(playerId);
            std::string points = std::to_string(playerPoints);
            Log::Info("Points: %s", points.c_str());

            //const int playerPoints = 150;
            wsHandler->queueMessage("P " + std::to_string(playerPoints));
        }
	}
}
