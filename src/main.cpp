//
//  Main.cpp provided under CC0 license
//

#include "sgct/sgct.h"

#include "websockethandler.h"
#include "utility.hpp"
#include "game.hpp"
#include "sceneobject.hpp"
#include "player.hpp"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace {
    std::unique_ptr<WebSocketHandler> wsHandler;

	//Ref to movement states used for syncing
	std::vector<PositionData> states{ 0 };

	// abock;  This Game::getInstance() gets called at load time even before the main
	// function runs, and since you are creating the Game object in this function, this is
	// the reason for your second constructor

	int64_t exampleInt = 1;
	std::string exampleString;
	double currentTime = 0.0;

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

	//Open config .xml
	// abock;  Instead of writing that file in the source code, you can also pass it on
	// the commandline.  You can do Domedagen.exe -config configs/fisheye_testing.xml 
	// instead or hardcoding it in here
	//config.configFilename = rootDir + "/src/configs/fisheye_testing.xml";
	//config.configFilename = rootDir + "/src/configs/simple.xml";
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

void draw(const RenderData& data) {
	Game::getInstance().setMVP(data.modelViewProjectionMatrix);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	Game::getInstance().render();
}

void initOGL(GLFWwindow*) {
	Game::init();

	/**********************************/
	/*			 Test Area			  */
	/**********************************/
	const float radius = 50.f;

	//Player* temp1 = new Player("fish", radius, glm::quat(glm::vec3(1.f, 0.f, 0.f)), 0.f, "hejhej");
	//Player* temp2 = new Player("fish", radius, glm::quat(glm::vec3(0.f, 0.f, 0.f)), 0.f, "hejhej");

	//temp2->setSpeed(0.3f);
	//temp2->setScale(20.f);

	//Game::getInstance().addGameObject(temp1);
	//Game::getInstance().addGameObject(temp2);

	for (size_t i = 0; i < 100; i++)
	{
		Player* temp = new Player("fish", radius, glm::quat(glm::vec3(1.f, 0.f, -1.f + 0.05 * i)), 0.f, "hejhej");
		temp->setSpeed(0.3f);
		Game::getInstance().addGameObject(temp);
	}
	// abock;  I would recommend using a std::unique_ptr in here as that better represents
	// the transfer of ownership, so you would have:
	// std::unique_ptr<GameObject> temp1 = std::make_unique<Player>(GameObject::PLAYER, "fish", radius, glm::quat(glm::vec3(1.f, 0.f, 0.f)), 0.f, "hejhej");
	// Game::getInstance().addGameObject(std::move(temp1));
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

	auto& objList = Game::getInstance().getGameObjectMap();
	// abock; it might be nicer to add functions to the Game that will do this internally.
	// So you would have a function called setOrientation on the Game that would do the
	// iteration internally.  That way, you don't need to expose the internals of the Game
	// which would make it a bit cleaner

	//Left
	if (key == Key::A && (action == Action::Press || action == Action::Repeat))
	{
		for (auto& [id, obj] : objList)
		{
			obj->setOrientation(obj->getOrientation() + 0.1f);
		}
	}
	//Right
	if (key == Key::D && (action == Action::Press || action == Action::Repeat))
	{
		for (auto& [id, obj] : objList)
		{
			obj->setOrientation(obj->getOrientation() - 0.1f);
		}
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
	//Output data
	std::vector<std::byte> data;
	//GameObjects
	auto& gameObjects = Game::getInstance().getGameObjectMap();

	//Copy of states
	// abock;  same comment as above. compare this solution with a way where you have a
	// function in the game that would just return a vector of std::byte that already
	// contains all of the serialized information. In that way, you don't need to expose
	// the game state
	//GameObjects and their states
	auto& gameObjects = Game::getInstance().getGameObjectMap();
	auto& objectPositionStates = Game::getInstance().getMovementStates();

	//Save position data from GameObjects
	for (auto& [id, obj] : gameObjects)
	{
		objectPositionStates.push_back(obj->getMovementData(id));
	}

	//Encode the data and clear vector for next frame
	serializeObject(data, objectPositionStates);
	objectPositionStates.clear();

	return data;
}

void decode(const std::vector<std::byte>& data, unsigned int pos) {
	// These are just two examples;  remove them and replace them with the logic of your
	// application that you need to synchronize

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
		//Ref to GameObjects
		auto& gameObjects = Game::getInstance().getGameObjectMap();

		//Sync new data
		for (auto& data : states)
		{
			gameObjects[data.mId]->setMovementData(data);
		}

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
	Log::Info("Message received: %s", msg.data());

	//Feedback testing with ugly matrix handling
	std::string temp = msg.data();
	if (temp == "transform")
	{
		Log::Info("Transformation feedback");
	}
}
