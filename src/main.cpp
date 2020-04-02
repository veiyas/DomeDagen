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

    int64_t exampleInt = 0;
	std::string exampleString;
	double currentTime = 0.0;

	GameObject* temp2; //For testing

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
	//config.configFilename = rootDir + "/src/configs/fisheye_testing.xml";
	//config.configFilename = rootDir + "/src/configs/simple.xml";
	config.configFilename = rootDir + "/src/configs/two_nodes.xml";

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
	
	//wsHandler->queueMessage("game_connect"); // crashes when run on client, bcs wsHandler is undefined
    Engine::instance().render();

	Game::destroy();
    Engine::destroy();
    return EXIT_SUCCESS;
}

void draw(const RenderData& data) {
	Game::getInstance().setMVP(data.modelViewProjectionMatrix);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	temp2->update(0);
	Game::getInstance().render();
}

void initOGL(GLFWwindow*) {
	Game::init();

	/**********************************/
	/*			 Test Area			  */
	/**********************************/
	const float radius = 50.f;

	//for (size_t i = 0; i < 30; i++)
	//{
	//	Game::getInstance().addGameObject(new SceneObject(GameObject::SCENEOBJECT, "fish", radius, glm::quat(glm::vec3(1.f - 0.1*i, -0.5f, 0)), 0.25f*i));
	//}

	GameObject* temp1 = new SceneObject(GameObject::SCENEOBJECT, "fish", radius, glm::quat(glm::vec3(-1.f, -0.5f, 0)), 0.f);
	            temp2 = new Player(GameObject::PLAYER, "fish", radius, glm::quat(glm::vec3(0.f, 0.f, 0.f)), 0.f, "hejhej");
	temp2->setScale(10.f);
	Game::getInstance().addGameObject(temp1);
	Game::getInstance().addGameObject(temp2);
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
	//if (key == Key::A && (action == Action::Press || action == Action::Repeat))
	//{
	//	std::vector<std::unique_ptr<GameObject>>& temp = Game::getInstance().getGameObjectVector();
	//	for (std::unique_ptr<GameObject>& obj : temp)
	//		obj.get()->setOrientation(obj.get()->getOrientation() - 0.25f);
	//}
	////Right
	//if (key == Key::D && (action == Action::Press || action == Action::Repeat))
	//{
	//	std::vector<std::unique_ptr<GameObject>>& temp = Game::getInstance().getGameObjectVector();
	//	for (std::unique_ptr<GameObject>& obj : temp)
	//		obj.get()->setOrientation(obj.get()->getOrientation() + 0.25f);

	if (key == Key::A && (action == Action::Press || action == Action::Repeat)) {
		temp2->setOrientation(temp2->getOrientation() + 0.1);
	}
	//Right
	if (key == Key::D && (action == Action::Press || action == Action::Repeat)) {
		temp2->setOrientation(temp2->getOrientation() - 0.1);
	}
	//Up
	if (key == Key::W && (action == Action::Press || action == Action::Repeat))
	{
	}
	//Right
	if (key == Key::D && (action == Action::Press || action == Action::Repeat))
	{
	}
	//Up
	if (key == Key::W && (action == Action::Press || action == Action::Repeat))
	{
	}
	//Down
	if (key == Key::S && (action == Action::Press || action == Action::Repeat))
	{
	}
	//In
	if (key == Key::Space && (action == Action::Press || action == Action::Repeat))
	{
	}
	//Out
	if (key == Key::LeftControl && (action == Action::Press || action == Action::Repeat))
	{
	}

}

void preSync() {
	// Do the application simulation step on the server node in here and make sure that
	// the computed state is serialized and deserialized in the encode/decode calls

	if (Engine::instance().isMaster())
	{
		// This doesn't have to happen every frame, but why not?
		wsHandler->tick();

		//TODO implement gamelogic
	}
}

std::vector<std::byte> encode() {
	// These are just two examples;  remove them and replace them with the logic of your
	// application that you need to synchronize
	std::vector<std::byte> data;
	//serializeObject(data, exampleInt);
	//serializeObject(data, exampleString);

	//Encoding testing
	//serializeObject(data, Game::getInstance().getGameObjectVector());

	return data;
}

void decode(const std::vector<std::byte>& data, unsigned int pos) {
	// These are just two examples;  remove them and replace them with the logic of your
	// application that you need to synchronize

	//deserializeObject(data, pos, exampleInt);
	//deserializeObject(data, pos, exampleString);

	//deserializeObject(data, pos, Game::getInstance().getGameObjectVector());
	std::cout << "test";

}

void cleanup() {
	// Cleanup all of your state, particularly the OpenGL state in here.  This function
	// should behave symmetrically to the initOGL function


}

void postSyncPreDraw() {
	// Apply the (now synchronized) application state before the rendering will start
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
