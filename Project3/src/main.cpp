#include "Global.h"

#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>

//Temp stuff
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
//#include <stb_image_write.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../extern/stb_image_write.h"

using std::uint8_t;
using std::uint32_t;
using std::size_t;


#define WIDTH 640 * 2
#define HEIGHT 480 * 2

void opengl_init();

GLFWwindow* window;

int main() 
{
	//OPENGL stuff
	opengl_init();

	//MY STUFF

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
	};

	//static const GLfloat g_UV_buffer_data[] = {
	//	0.0f, 1.0f,  // Upper Left
	//	1.0f, 1.0f,  // Upper Right
	//	0.0f, 0.0f,   // Lower left

	//	1.0f, 1.0f,
	//	1.0f, 0.0f,
	//	0.0f, 0.0f
	//};

	static const GLfloat g_UV_buffer_data[] = {
		0.0f, 0.0f,  // Upper Left
		1.0f, 0.0f,  // Upper Right
		0.0f, 1.0f,   // Lower left

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_UV_buffer_data), g_UV_buffer_data, GL_STATIC_DRAW);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, gpu.VRAM);

	GLuint TextureShader = gpu.gpu_LoadShaders("TextureVertex.v", "TexturePixel.p");
	
	printf("%p\n", gpu.VRAM);

	//END

	emu.init("TO DO");
	emu.reset();

	do {
		//CPU
		for (int i = 0; i < 564480; i++) {
			cpu.step(false);
		}
		io.i_stat = io.i_stat | 0x01;
		gpu.evenOrOdd ^= 1;

		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//gpu.gpu_mod();

		gpu.gpu_render(); //TODO - make renders not delete if repeating between frames
		gpu.gpu_clear();

		glDisable(GL_BLEND);
		glUseProgram(TextureShader);

		//// 1st attribute buffer : vertices
		//glEnableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, gpu.VRAM);
		//glBindTexture(GL_TEXTURE_2D, textureID);

		//glEnableVertexAttribArray(1);
		//glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//// Draw the triangle !
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);


		////Convert VRAM
		//ub* PIXELS = new ub[1024 * 512 * 3];
		//for (int i = 0; i < 1024 * 512; i++) {
		//	uh temp = gpu.VRAM[i];
		//	PIXELS[(i * 3)    ] = ((temp & 0x1F) * 8);
		//	PIXELS[(i * 3) + 1] = ((temp & 0x3E0) >> 5 ) * 8;
		//	PIXELS[(i * 3) + 2] = ((temp & 0x7C00) >> 10) * 8;
		//}

		//stbi_write_bmp("Test.bmp", 1024, 512, 3, PIXELS);

		//system("PAUSE");

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}

void opengl_init()
{
	glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{

	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(WIDTH, HEIGHT, "DerpPSX", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}