//
//  Main.cpp provided under CC0 license
//

#include "sgct/sgct.h"
#include "websockethandler.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace {
    std::unique_ptr<WebSocketHandler> wsHandler;

    int64_t exampleInt = 0;
	std::string exampleString;

	double currentTime = 0.0;

	GLuint vertexArray = 0;
	GLuint vertexPositionBuffer = 0;
	GLuint vertexColorBuffer = 0;

	GLint matrixLoc = -1;

	constexpr const char* vertexShader = R"(
		#version 330 core
		layout(location = 0) in vec3 vertPosition;
		layout(location = 1) in vec3 vertColor;
		uniform mat4 mvp;	
		out vec3 fragColor;
		void main() {
		gl_Position = mvp * vec4(vertPosition, 1.0);
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

void initOGL(GLFWwindow*) {

	GLfloat mult = 1.f;
	const GLfloat positionData[] = {
		-1.f* mult, 0.f* mult, -1.f,
		 0.f* mult, 2.f* mult, -1.f,
		 1.f* mult, 0.f* mult, -1.f
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

	ShaderManager::instance().addShaderProgram("xform", vertexShader, fragmentShader);
	const ShaderProgram& prg = ShaderManager::instance().shaderProgram("xform");
	prg.bind();
	matrixLoc = glGetUniformLocation(prg.id(), "mvp");
	prg.unbind();

}

void preSync() {
    // Do the application simulation step on the server node in here and make sure that
    // the computed state is serialized and deserialized in the encode/decode calls


    if (Engine::instance().isMaster() && wsHandler->isConnected() &&
        Engine::instance().currentFrameNumber() % 100 == 0)
    {
        wsHandler->queueMessage("ping");
    }



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


void postSyncPreDraw() {
    // Apply the (now synchronized) application state before the rendering will start
}


void draw(const RenderData& data) {
	constexpr const float Speed = 0.8f;

	/*glm::mat4 scene = glm::rotate(
		glm::mat4(1.f),
		static_cast<float>(currentTime) * Speed,
		glm::vec3(0.f, 1.f, 0.f)
	);*/

	//const glm::mat4 mvp = data.modelViewProjectionMatrix;
	auto t1 = data.modelMatrix;
	auto t2 = data.viewMatrix;
	auto t3 = data.projectionMatrix;
	auto t4 = data.modelViewProjectionMatrix;

	const glm::mat4 mvp(1.f);

	ShaderManager::instance().shaderProgram("xform").bind();

	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(t4));

	glBindVertexArray(vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	ShaderManager::instance().shaderProgram("xform").unbind();
}


void cleanup() {
    // Cleanup all of your state, particularly the OpenGL state in here.  This function
    // should behave symmetrically to the initOGL function


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


void connectionEstablished() {
    Log::Info("Connection established");


}


void connectionClosed() {
    Log::Info("Connection closed");


}


void messageReceived(const void* data, size_t length) {
    std::string_view msg = std::string_view(reinterpret_cast<const char*>(data), length);
    Log::Info("Message received: %s", msg.data());


}

void drawFun(const RenderData& data){
	const glm::mat4 mvp = data.modelViewProjectionMatrix;

	glBindVertexArray(vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	
}

int main(int argc, char** argv) {
    std::vector<std::string> arg(argv + 1, argv + argc);
    Configuration config = sgct::parseArguments(arg);
	config.configFilename = "../fisheye_testing.xml";
    config::Cluster cluster = sgct::loadCluster(config.configFilename);

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

    Engine::instance().render();

    Engine::destroy();
    return EXIT_SUCCESS;
}
