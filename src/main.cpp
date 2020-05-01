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
#include "modelmanager.hpp"

namespace {
	std::unique_ptr<WebSocketHandler> wsHandler;

	//Container for deserialized game state info
	std::vector<PlayerData> playerStates;
	std::vector<CollectibleData> collectibleStates;

	unsigned collectiblesSize;
	unsigned playersSize;
	int syncTest3;
	std::vector<int> syncTestIntVec;
	int syncTest5;
	int syncTest6;
	int syncTest7;

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

template<typename T>
void deserializeObjectFix(const std::vector<std::byte>& buffer, unsigned int& pos,
	std::vector<T>& value, unsigned valueSize);

template <typename T>
void serializeObjectFix(std::vector<std::byte>& buffer, const std::vector<T>& value, unsigned valueSize);

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

void draw(const RenderData& data)
{
	Game::instance().setMVP(data.modelViewProjectionMatrix);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	Game::instance().render();
	while (glGetError() != GL_NO_ERROR)
    {
      std::cout << "GL Error: " << glGetError() << std::endl;
    }
}

void initOGL(GLFWwindow*)
{
	ModelManager::init();
	Game::init();

	collectiblesSize = 0;
	playersSize = 0;
	syncTest3 = 3;
	syncTestIntVec.push_back(3);
	syncTest5 = 5;
	syncTest6 = 6;
	syncTest7 = 7;
	
	assert(std::is_pod<PlayerData>());
	assert(std::is_pod<CollectibleData>());

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

	const auto& collects = Game::instance().getCollectibleData();
	const auto& players = Game::instance().getPlayerData();

	output.reserve(collects.size()*sizeof(CollectibleData) + players.size()*sizeof(PlayerData));

	//Serialize the vectors sizes to circumvent sgct bug
	serializeObject(output, static_cast<unsigned>(players.size()));
	serializeObject(output, static_cast<unsigned>(collects.size()));

	//serializeObject(output, players);
	serializeObject(output, collects);
	//serializeObject(output, syncTest3);
		


	//serializeObject(output, syncTest5);
	//serializeObject(output, syncTest6);
	//serializeObject(output, syncTestIntVec);	

	return output;
}

void decode(const std::vector<std::byte>& data, unsigned int pos)
{
	if (!Game::exists()) //No point in syncing data if no instance of Game exist yet
		return;

	//Deserialize the vectors sizes to circumvent sgct bug
	deserializeObject(data, pos, playersSize);
	deserializeObject(data, pos, collectiblesSize);

	//deserializeObjectFix(data, pos, playerStates, playersSize);
	deserializeObjectFix(data, pos, collectibleStates, collectiblesSize);
	//deserializeObject(data, pos, syncTest3);
	


	//deserializeObject(data, pos, syncTest5);
	//deserializeObject(data, pos, syncTest6);
	//deserializeObjectFix(data, pos, syncTestIntVec, 1);
	

	std::cout << collectiblesSize << "\n" << playersSize << "\n\n";
}

void cleanup()
{
	// Cleanup all of your state, particularly the OpenGL state in here.  This function
	// should behave symmetrically to the initOGL function
}

void postSyncPreDraw()
{
	//Sync gameobjects' state on clients only
	if (!Engine::instance().isMaster() && Game::exists())
	{
		Game::instance().setDecodedPlayerData(playerStates);
		Game::instance().setDecodedCollectibleData(collectibleStates);
		std::cout << Game::instance().getCollectibleData().size() << "\n" << Game::instance().getPlayerData().size() << "\n\n";
	}
	
}

template <typename T>
void serializeObjectFix(std::vector<std::byte>& buffer, const std::vector<T>& value, unsigned valueSize) {
	static_assert(std::is_pod_v<T>, "Type has to be a plain-old data type");

	const uint32_t size = static_cast<uint32_t>(valueSize);
	serializeObject(buffer, size);

	if (size > 0) {
		buffer.insert(
			buffer.end(),
			reinterpret_cast<const std::byte*>(value.data()),
			reinterpret_cast<const std::byte*>(value.data()) + size * sizeof(T)
			);
	}
}

template<typename T>
void deserializeObjectFix(const std::vector<std::byte>& buffer, unsigned int& pos,
	std::vector<T>& value, unsigned valueSize)
{
	value.clear();

	uint32_t size;
	deserializeObject<uint32_t>(buffer, pos, size);
	size = valueSize;
	//pos += 4;

	if (size == 0) {
		return;
	}

	value.clear();
	value.assign(
		reinterpret_cast<const T*>(buffer.data() + pos),
		reinterpret_cast<const T*>(buffer.data() + pos + size * sizeof(T))
		);
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
        
        // If first slot is 'D', player to be removed has been sent
        if (msgType == 'D') {
            unsigned int playerId;
            iss >> playerId;
            Game::instance().disablePlayer(playerId);
        }
	}
}