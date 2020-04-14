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

#include "sgct/sgct.h"

#include "websockethandler.h"
#include "utility.hpp"
#include "game.hpp"
#include "sceneobject.hpp"
#include "player.hpp"
#include "collectiblepool.hpp"

namespace {
	std::unique_ptr<WebSocketHandler> wsHandler;

	//Container for deserialized game state info
	std::vector<PositionData> states;


	//TEMPORARY used to control rotation of all players 
	float updatedRotation{ 0 };

	//RNG stuff
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rng(-1.0f, 1.0f);

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
int main(int argc, char** argv) {
	std::vector<std::string> arg(argv + 1, argv + argc);
	Configuration config = sgct::parseArguments(arg);

	//Choose which config file (.xml) to open
	config.configFilename = rootDir + "/src/configs/fisheye_testing.xml";
	//config.configFilename = rootDir + "/src/configs/simple.xml";
	//config.configFilename = rootDir + "/src/configs/six_nodes.xml";
	//config.configFilename = rootDir + "/src/configs/two_fisheye_nodes.xml";

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

	//Game::destroy();
	Engine::destroy();
	return EXIT_SUCCESS;
}

void draw(const RenderData& data) {
	Game::getInstance().setMVP(data.modelViewProjectionMatrix);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	Game::getInstance().render();
	while (glGetError() != GL_NO_ERROR)
    {
      std::cout << "GL Error: " << glGetError() << std::endl;
    }
}

void initOGL(GLFWwindow*) {
	Game::init();

	/**********************************/
	/*			 Debug Area			  */
	/**********************************/
	constexpr float radius = 50.f;

	//for (size_t i = 0; i < 1; i++)
	//{
	//	std::shared_ptr<GameObject> temp{
	//	new Player("fish", radius, glm::quat(glm::vec3(1.f, 0.f, -1.f + 0.3 * i)), 0.f, "Player " + std::to_string(i+1), 0.5f) };
	//	Game::getInstance().addGameObject(std::move(temp));
	//}

	CollectiblePool testPool{};
	std::cout << "stop\n";
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
		Game::getInstance().rotateAllGameObjects(0.1f);
	}
	//Right
	if (key == Key::D && (action == Action::Press || action == Action::Repeat))
	{
		Game::getInstance().rotateAllGameObjects(-0.1f);
	}
}

void preSync() {
	// Do the application simulation step on the server node in here and make sure that
	// the computed state is serialized and deserialized in the encode/decode calls	

	//Run game simulation on master only
	if (Engine::instance().isMaster())
	{
		// This doesn't have to happen every frame, but why not?
		wsHandler->tick();

		//TODO implement gamelogic
		Game::getInstance().update();
	}
}

std::vector<std::byte> encode() {

	return Game::getInstance().getEncodedPositionData();
}

void decode(const std::vector<std::byte>& data, unsigned int pos) {
	// These are just two examples;  remove them and replace them with the logic of your
	// application that you need to synchronize

	//Decode position data into states vector
	deserializeObject(data, pos, states);
}

void cleanup() {
	// Cleanup all of your state, particularly the OpenGL state in here.  This function
	// should behave symmetrically to the initOGL function
}

void postSyncPreDraw() {
	//Sync gameobjects' state on clients only
	if (!Engine::instance().isMaster())
	{
		Game::getInstance().setDecodedPositionData(states);

		//Clear states for next frame, not needed but it's polite
		states.clear();
	}
	else
		return;
}

void connectionEstablished() {
	Log::Info("Connection established");
}

void connectionClosed() {
	Log::Info("Connection closed");
}

void messageReceived(const void* data, size_t length) {
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
			Game::getInstance().addGameObject(Utility::getNewPlayerData(iss));
		}

		// If first slot is 'C', the rotation angle has been sent
		if (msgType == 'C') {
			Game::getInstance().updateTurnSpeed(Utility::getTurnSpeed(iss));
		}
	}
}
