#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <stdlib.h>     /* srand, rand */
#include <fstream>
#include <iomanip>

class GPU
{
public:
	//GPU STAT
	ub texPageXBase = 0;
	ub texPageYBase = 0;
	ub semiTranparent = 0;
	ub texPageColor = 0;
	ub dither = 0;
	ub canDraw = 0;
	ub maskBit = 0;
	ub isDrawing = 0;
	ub interlaceField = 0;
	ub reverseFlag = 0;
	ub texDisable = 0;
	ub hRes1 = 0;
	ub hRes2 = 0;
	ub vRes = 0;
	ub videoMode = 0;
	ub colorDepth = 0;
	ub verticalInterlace = 0;
	ub displayEnable = 0;
	ub interuptRequest = 0;
	ub dmaDataRequest = 0;
	ub readyReceiveCMD_word = 1;
	ub readySend_cpu = 1;
	ub readyReceiveDAM_block = 1;
	ub DMA_dir = 0;
	ub evenOrOdd = 0;

	enum POLY_TYPE { TRIANGLE, QUAD };
	enum COLOR_TYPE { TEXTURED, SOLID };

	GLFWwindow* window;

	uw PS_Width = 640;
	uw PS_Height = 240;

	GLuint SolidShader;
	GLuint TextureShader;

	struct drawable {
		GLfloat g_vertex_buffer_data[18];
		GLfloat g_color_buffer_data[18];
		GLfloat g_uv_buffer_data[12];
		ub *textureData = nullptr;

		GLuint vertexbuffer;
		GLuint colorbuffer;
		GLuint UVbuffer;
		GLuint textureID;

		void setUp() 
		{
			if (textureData) {
				glEnable(GL_BLEND);
				glDeleteTextures(1, &textureID);
				glGenTextures(1, &textureID);
				glBindTexture(GL_TEXTURE_2D, textureID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, textureData);
			}
			else {
				glDisable(GL_BLEND);
			}

			glDeleteBuffers(1, &vertexbuffer);

			glGenBuffers(1, &vertexbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

			glDeleteBuffers(1, &colorbuffer);

			glGenBuffers(1, &colorbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

			glDeleteBuffers(1, &UVbuffer);

			glGenBuffers(1, &UVbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
		}
		void render(GLuint st, GLuint ss)
		{
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			if (color_type == SOLID) {
				glUseProgram(ss);

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			}
			else {
				glUseProgram(st);

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			}

			if (type == TRIANGLE) glDrawArrays(GL_TRIANGLES, 0, 3);
			if (type == QUAD)     glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			if (textureData) delete textureData;
		}
		COLOR_TYPE color_type = SOLID;
		POLY_TYPE type = TRIANGLE;
	};

	drawable objects[1024];
	uw objects_index = 0;

	//Vertex Stuct
	struct gpu_vec2
	{
		void setXY(uw width, uw height, uw vertCom) {
			uw x, y;

			x = (vertCom & 0xFFFF);
			y = (vertCom & 0xFFFF0000) >> 16;

			if (x == 0) f_x = -1.0f;
			f_x = (float)(((((float)x - 0)/(width - 0)) * 2.0f) - 1);
		
			if (y == 0) f_y = -1.0f;
			f_y = (float)(((((float)y - 0) / (height - 0)) * 2.0f) - 1);
		}
		void setRGB(uw colorCom) 
		{
			r = (colorCom & 0xFF) / 255.0f;
			g = ((colorCom & 0xFF00) >> 8) / 255.0f;
			b = ((colorCom & 0xFF0000) >> 16) / 255.0f;
		}
		void setTexCoord(uw texCom) 
		{
			texCoordX = (texCom & 0xFF) / 255.0f;
			texCoordY = ((texCom & 0xFF00) >> 8) / 255.0f;
		}

		float r, g, b;
		float f_x, f_y;
		float texCoordX, texCoordY;
	};

	bool isCommand = false;
	void(GPU::*commandFunc)(uw);

	void CopyRectangle_CPU_to_VRAM(uw command);
	void Draw_Monocrome_Quad(uw command);
	void Draw_Shaded_Quad(uw command);
	void Draw_Shaded_Triangle(uw command);
	void Draw_Textured_Quad(uw command);
public:
	//1mb of VRAM
	uh* VRAM;

	std::fstream outFile;

	GPU() {
		VRAM = new uh[1024 * 512];

		srand(0);

		uh* temp = (uh*)VRAM;

		for (int i = 0; i < 0xFFFFF / 2; i++) {
			temp[i] = 0;
		}

		outFile.open("out.txt", std::ios::out | std::ios::binary);
	}
	~GPU() {}

	void gpu_mod() 
	{
		uh* temp = (uh*)VRAM;

		for (int i = 0; i < 0xFFFFF / 2; i++) {
			temp[i] = rand() % 0xFFFF;
		}
	}

	void gpu_init();
	void gpu_clear();
	void gpu_render();
	GLuint gpu_LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	uw gpuReadStat();

	void gpu0Command(uw* command, uw size);
	void gpu1Command(uw command);

};

extern GPU gpu;




//#pragma once
//
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/ext/matrix_transform.hpp>
//
//#include <stdlib.h>     /* srand, rand */
//
//class GPU
//{
//public:
//	//GPU STAT
//	ub texPageXBase = 0;
//	ub texPageYBase = 0;
//	ub semiTranparent = 0;
//	ub texPageColor = 0;
//	ub dither = 0;
//	ub canDraw = 0;
//	ub maskBit = 0;
//	ub isDrawing = 0;
//	ub interlaceField = 0;
//	ub reverseFlag = 0;
//	ub texDisable = 0;
//	ub hRes1 = 0;
//	ub hRes2 = 0;
//	ub vRes = 0;
//	ub videoMode = 0;
//	ub colorDepth = 0;
//	ub verticalInterlace = 0;
//	ub displayEnable = 0;
//	ub interuptRequest = 0;
//	ub dmaDataRequest = 0;
//	ub readyReceiveCMD_word = 1;
//	ub readySend_cpu = 1;
//	ub readyReceiveDAM_block = 1;
//	ub DMA_dir = 0;
//	ub evenOrOdd = 0;
//
//
//
//	GLFWwindow* window;
//
//	uw PS_Width = 640;
//	uw PS_Height = 240;
//
//	GLuint solidColorShader;
//	GLuint textureShader;
//
//	enum { SEMI_TRANPARENT = 0, OPAQUE };
//	enum TYPE { TRIANGLE = 0, QUAD };
//	enum BLEND { TEXTURE_BLEND = 0, RAW_TEXTURE };
//	enum SHADER_TYPE { TEXTURED = 0, SOLID };
//
//	//Single quad element
//	struct drawable {
//		GLfloat g_vertex_buffer_data[18];
//		GLfloat g_color_buffer_data[18];
//
//		GLuint vertexbuffer;
//		GLuint colorbuffer;
//		void setUp() 
//		{
//			glDeleteBuffers(1, &vertexbuffer);
//
//			glGenBuffers(1, &vertexbuffer);
//			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//			glDeleteBuffers(1, &colorbuffer);
//
//			glGenBuffers(1, &colorbuffer);
//			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
//		}
//		void render()
//		{
//			glUseProgram(shaderID);
//
//			glEnableVertexAttribArray(0);
//			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//
//			glEnableVertexAttribArray(1);
//			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
//			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//
//			if (type == TRIANGLE) glDrawArrays(GL_TRIANGLES, 0, 3);
//			if (type == QUAD)     glDrawArrays(GL_TRIANGLES, 0, 6);
//			glDisableVertexAttribArray(0);
//			glDisableVertexAttribArray(1);
//		}
//
//		TYPE type = TRIANGLE;
//		GLuint shaderID;
//	};
//
//	drawable objects[1024];
//	uw objects_index = 0;
//
//	uw paramSyphnCount = 0;
//	std::vector<uw> syphnBuffer;
//	uw callBack = 0;
//
//	uw RecDest;
//	uw RecDimention;
//	uh recX, recY;
//	uw RecSize;
//	uw RecH, RecW;
//	uw recCounter;
//	ub dataTransferFlag = 0;
//
//	struct gpu_vec2 {
//		gpu_vec2(uh _x, uh _y, ub r, ub b, ub g) {
//			x = _x;
//			y = _y;
//			red = (float)r / 255.0f;
//			green = (float)g / 255.0f;
//			blue = (float)b / 255.0f;
//		}
//
//		void normalize(uw width, uw height) {
//			if (x == 0) f_x = -1.0f;
//			f_x = (float)(((((float)x - 0)/(width - 0)) * 2.0f) - 1);
//
//			if (y == 0) f_y = -1.0f;
//			f_y = (float)(((((float)y - 0) / (height - 0)) * 2.0f) - 1);
//		}
//
//		uh x;
//		uh y;
//		float f_x;
//		float f_y;
//		float red;
//		float blue;
//		float green;
//	};
//public:
//	//1mb of VRAM
//	ub *VRAM;
//
//	GPU() {
//		VRAM = new ub[0xFFFFF];
//
//		srand(0);
//
//		uh* temp = (uh*)VRAM;
//
//		for (int i = 0; i < 0xFFFFF / 2; i++) {
//			temp[i] = 0;
//		}
//	}
//	~GPU(){}
//
//	void gpu_init();
//	void gpu_clear();
//	void gpu_render();
//	GLuint gpu_LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
//
//	uw gpuReadStat();
//
//	void gpu0Command(uw* command, uw size);
//	void gpu1Command(uw command);
//
//	void gpu0CommandNew(uw* command, uw size);
//
//	void monocromeShape(ub opaque, ub type, gpu_vec2 v1, gpu_vec2 v2, gpu_vec2 v3, gpu_vec2 v4);
//	void shadedShape(ub opaque, ub type, gpu_vec2 v1, gpu_vec2 v2, gpu_vec2 v3, gpu_vec2 v4);
//	void texturedShape(ub opaque, ub type, ub blend, gpu_vec2 v1, gpu_vec2 v2, gpu_vec2 v3, gpu_vec2 v4);
//};
//
//extern GPU gpu;