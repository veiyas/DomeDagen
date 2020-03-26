//
//  Main.cpp provided under CC0 license
//

#include "sgct/sgct.h"
#include "websockethandler.h"
#include "utility.hpp"
#include "game.hpp"
#include "gameobject.hpp"
#include "sceneobject.hpp"
#include "player.hpp"
#include "model.hpp"

#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

	GLuint vertexArray = 0;
	GLuint vertArrayDataSize = 0;
	GLuint vertexPositionBuffer = 0;
	GLuint vertexTexBuffer = 0;
	GLuint indexBuffer = 0;
	GLuint vertexNormalBuffer = 0;

	GLint mvpMatrixLoc = -1;

	GLint transMatrixLoc = -1;
	glm::mat4 transMatrix{ 1.f };

	Model* test;
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
	config.configFilename = rootDir + "/src/configs/fisheye_testing.xml";
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
	//Debugging tools
	Game::getInstance();
	Game::getInstance().printShaderPrograms();
	Game::getInstance().printModelNames();

	SceneObject temp{ Game::getInstance().getModel("fish") };

	Game::getInstance().addSceneObject(temp);

	wsHandler->queueMessage("game_connect");
    Engine::instance().render();

	Game::destroy();
    Engine::destroy();
    return EXIT_SUCCESS;
}

void draw(const RenderData& data) {
	const glm::mat4 mvp = data.modelViewProjectionMatrix;
	Game::getInstance().setMVP(mvp);	

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	ShaderManager::instance().shaderProgram("player").bind();

	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(transMatrixLoc, 1, GL_FALSE, glm::value_ptr(transMatrix));

	//Game::getInstance().render();
	test->render();

	ShaderManager::instance().shaderProgram("player").unbind();
}

void initOGL(GLFWwindow*) {
	/**********************************/
	/*			  Shaders			  */
	/**********************************/
	Game::getInstance();
	test = new Model{ "C:/Users/David/source/repos/DomeDagen/src/models/fish/fish.fbx" };

	//Read shaders into strings
	//std::ifstream in_vert{ "../src/shaders/playervert.glsl" };
	//std::ifstream in_frag{ "../src/shaders/playerfrag.glsl" };
	//std::string vert;
	//std::string frag;
	//if (in_vert.good() && in_frag.good()) {
	//	vert = std::string(std::istreambuf_iterator<char>(in_vert), {});
	//	frag = std::string(std::istreambuf_iterator<char>(in_frag), {});
	//}
	//else
	//{
	//	std::cout << "ERROR OPENING SHADER FILES";
	//}
	//in_vert.close(); in_frag.close();

	//ShaderManager::instance().addShaderProgram("player", vert, frag);
	const ShaderProgram& prg = ShaderManager::instance().shaderProgram("player");

	prg.bind();
	mvpMatrixLoc = glGetUniformLocation(prg.id(), "mvp");
	transMatrixLoc = glGetUniformLocation(prg.id(), "transformation");
	prg.unbind();

	/**********************************/
	/*			  OpenGL 			  */
	/**********************************/

}

void keyboard(Key key, Modifier modifier, Action action, int) {
	if (key == Key::Esc && action == Action::Press) {
		Engine::instance().terminate();
	}

	if (key == Key::Space && modifier == Modifier::Shift && action == Action::Release) {
		Log::Info("Released space key");
		wsHandler->disconnect();
	}
	//Left
	if (key == Key::A && (action == Action::Press || action == Action::Repeat)) {
		transMatrix = glm::translate(transMatrix, glm::vec3(-1.f, 0.f, 0.f));
	}
	//Right
	if (key == Key::D && (action == Action::Press || action == Action::Repeat)) {
		transMatrix = glm::translate(transMatrix, glm::vec3(1.f, 0.f, 0.f));
	}
	//Up
	if (key == Key::W && (action == Action::Press || action == Action::Repeat)) {
		transMatrix = glm::translate(transMatrix, glm::vec3(0.f, 0.f, 1.f));
	}
	//Down
	if (key == Key::S && (action == Action::Press || action == Action::Repeat)) {
		transMatrix = glm::translate(transMatrix, glm::vec3(0.f, 0.f, -1.f));
	}
	//In
	if (key == Key::Space && (action == Action::Press || action == Action::Repeat)) {
		transMatrix = glm::translate(transMatrix, glm::vec3(0.f, -1.f, 0.f));
	}
	//Out
	if (key == Key::LeftControl && (action == Action::Press || action == Action::Repeat)) {
		transMatrix = glm::translate(transMatrix, glm::vec3(0.f, 1.f, 0.f));
	}

}

void preSync() {
	// Do the application simulation step on the server node in here and make sure that
	// the computed state is serialized and deserialized in the encode/decode calls


	//if (Engine::instance().isMaster() && wsHandler->isConnected() &&
	//    Engine::instance().currentFrameNumber() % 100 == 0)
	//{
	//    wsHandler->queueMessage("ping");
	//}



	if (Engine::instance().isMaster()) {
		// This doesn't have to happen every frame, but why not?
		wsHandler->tick();
	}
}

std::vector<std::byte> encode() {
	// These are just two examples;  remove them and replace them with the logic of your
	// application that you need to synchronize
	std::vector<std::byte> data;
	serializeObject(data, exampleInt);
	serializeObject(data, exampleString);


	return data;
}

void decode(const std::vector<std::byte>& data, unsigned int pos) {
	// These are just two examples;  remove them and replace them with the logic of your
	// application that you need to synchronize
	deserializeObject(data, pos, exampleInt);
	deserializeObject(data, pos, exampleString);
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
