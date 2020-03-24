//
//  Main.cpp provided under CC0 license
//

#include "sgct/sgct.h"
#include "websockethandler.h"
#include "Utility.hpp"
#include "player.h";

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
	GLuint vertexPositionBuffer = 0;
	GLuint vertexColorBuffer = 0;

	GLint matrixLoc = -1;

	//FOR TESTING PURPOSES ONLY
	GLint transMatrixLoc = -1;
	glm::mat3 transMatrx{ 1.f };

	constexpr const char* vertexShader = R"(
		#version 330 core
		layout(location = 0) in vec3 vertPosition;
		layout(location = 1) in vec3 vertColor;
		uniform mat4 mvp;
		uniform mat3 transformation;
		out vec3 fragColor;
		void main() {
		gl_Position = mvp * vec4(transformation * vertPosition, 1.0);
		fragColor = vertColor;
})";

	constexpr const char* fragmentShader = R"(
		#version 330 core
		in vec3 fragColor;
		out vec4 color;
		void main() { color = vec4(fragColor, 1.0); }
)";
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
std::string rootDir = Utility::findRootDir();

/****************************
			MAIN
*****************************/
int main(int argc, char** argv) {
	//???
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

	wsHandler->queueMessage("game_connect");

    Engine::instance().render();

    Engine::destroy();
    return EXIT_SUCCESS;
}

void draw(const RenderData& data) {
	const glm::mat4 mvp = data.modelViewProjectionMatrix;

	//Vars if you need to debug each MVP matrix
	//auto t1 = data.modelMatrix;
	//auto t2 = data.viewMatrix;
	//auto t3 = data.projectionMatrix;
	//auto t4 = data.modelViewProjectionMatrix;

	ShaderManager::instance().shaderProgram("xform").bind();

	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix3fv(transMatrixLoc, 1, GL_FALSE, glm::value_ptr(transMatrx));

	glBindVertexArray(vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	ShaderManager::instance().shaderProgram("xform").unbind();
}

void initOGL(GLFWwindow*) {

	GLfloat size_mult = 1.f;
	const GLfloat positionData[] = {
		-1.f * size_mult, 0.f * size_mult, -1.f,
		 0.f * size_mult, 2.f * size_mult, -1.f,
		 1.f * size_mult, 0.f * size_mult, -1.f
	};

	const GLfloat colorData[] = {
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	};

	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &(vertexArray));
	glBindVertexArray(vertexArray);

	// generate VBO for vertex positions
	glGenBuffers(1, &vertexPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
	// upload data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// generate VBO for vertex colors
	glGenBuffers(1, &vertexColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
	// upload data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray(0);

	//Read shaders into strings
	std::ifstream in_vert{ "../src/shaders/testing_vertex.glsl" };
	std::ifstream in_frag{ "../src/shaders/testing_fragment.glsl" };
	std::string vert;
	std::string frag;
	if (in_vert.good() && in_frag.good()) {
		vert = std::string(std::istreambuf_iterator<char>(in_vert), {});
		frag = std::string(std::istreambuf_iterator<char>(in_frag), {});
	}
	else
	{
		std::cout << "ERROR OPENING SHADER FILES";
	}
	in_vert.close(); in_frag.close();

	ShaderManager::instance().addShaderProgram("xform", vert, frag);
	const ShaderProgram& prg = ShaderManager::instance().shaderProgram("xform");
	prg.bind();
	matrixLoc = glGetUniformLocation(prg.id(), "mvp");

	//Ugly testing
	transMatrixLoc = glGetUniformLocation(prg.id(), "transformation");

	prg.unbind();
}

void keyboard(Key key, Modifier modifier, Action action, int) {
	if (key == Key::Esc && action == Action::Press) {
		Engine::instance().terminate();
	}

	if (key == Key::Space && modifier == Modifier::Shift && action == Action::Release) {
		Log::Info("Released space key");
		wsHandler->disconnect();
	}

	if (key == Key::S && action == Action::Press) {

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
		transMatrx *= glm::mat3{ 0.1f };
		Log::Info("Transformation feedback");
	}
}
