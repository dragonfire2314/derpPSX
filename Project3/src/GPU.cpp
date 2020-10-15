#include "Global.h"

GPU gpu;

void GPU::CopyRectangle_CPU_to_VRAM(uw command)
{
	static int count = 0;
	static uh destX;
	static uh destY;
	static uh width;
	static uh height;
	static uw xPos = 0;
	static uw yPos = 0;
	static uw num_pixels;
	static uw* tran_data;
	static ub* outBuffer;
	if (count == 0)
	{
		destX = (command & 0xFFFF);
		destY = (command & 0xFFFF0000) >> 16;
		//printf("THIS ONE: %x\n", command);
	}
	else if (count == 1)
	{
		width = (command & 0xFFFF);
		height = (command & 0xFFFF0000) >> 16;
		num_pixels = width * height;
		tran_data = new uw[num_pixels];
		outBuffer = new ub[num_pixels * 2];
		//printf("x: %u |y: %u |W: %u |H: %u\n", destX, destY, width, height);
		//system("PAUSe");
		//printf("%x\n", command);
	}
	else
	{
		tran_data[count - 2] = command;

		//Exit condition
		if ((width * height / 2) == (count)) {

			uh* src_ptr = (uh*)(tran_data);
			uh mask_and = 0;
			uh mask_or = 0;

			for (uw row = 0; row < height;)
			{
				uh* dst_row_ptr = &VRAM[((destY + row++) % 512) * 1024];
				for (uw col = 0; col < width;)
				{
					// TODO: Handle unaligned reads...
					uh* pixel_ptr = &dst_row_ptr[(destX + col++) % 1024];
					if (((*pixel_ptr) & mask_and) == 0)
						*pixel_ptr = *(src_ptr++) | mask_or;
				}
			}

			isCommand = false;
			count = -1;
			xPos = 0;
			yPos = 0;
		}
	}
	count++;


	//static int count = 0;
	//static uh destX;
	//static uh destY;
	//static uh width;
	//static uh height;
	//static uw xPos = 0;
	//static uw yPos = 0;
	//if (count == 0) 
	//{
	//	destX = (command & 0xFFFF);
	//	destY = (command & 0xFFFF0000) >> 16;
	//	printf("THIS ONE: %x\n", command);
	//}
	//else if (count == 1) 
	//{
	//	width = (command & 0xFFFF);
	//	height = (command & 0xFFFF0000) >> 16;
	//	printf("x: %u |y: %u |W: %u |H: %u\n", destX, destY, width, height);
	//	//system("PAUSe");
	//	//printf("%x\n", command);
	//}
	//else
	//{
	//	//put into VRAM
	//	uw newX = 0;
	//	uw newY = 0;

	//	newX = xPos + destX;
	//	newY = yPos + destY;

	//	uw* temp = (uw*)VRAM;
	//	VRAM[(newY * 1024) + newX] = command & 0xFFFF;
	//	VRAM[(newY * 1024) + newX + 1] = (command & 0xFFFF0000) >> 16;

	//	xPos+=2;
	//	if (xPos >= width) {
	//		yPos++;
	//		xPos = 0;
	//	}

	//	//Exit condition
	//	if ((width * height / 2) == (count)) {
	//		isCommand = false;
	//		count = -1;
	//		xPos = 0;
	//		yPos = 0;

	//		outFile << "\n\n\n\n\n";
	//	}

	//	outFile << std::hex << std::setw(8) << std::setfill('0') << command << " ";
	//}
	//count++;
}

void GPU::Draw_Monocrome_Quad(uw command)
{
	static POLY_TYPE poly_type;
	static uw count = 0;

	static gpu_vec2 v1, v2, v3, v4;

	if (count == 0) 
	{
		//Save color
		v1.setRGB(command);
		v2.setRGB(command);
		v3.setRGB(command);
		v4.setRGB(command);
	}
	else if (count == 1)
	{
		//v1
		v1.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 2)
	{
		//v2
		v2.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 3)
	{
		//v3
		v3.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 4) 
	{
		//v4
		v4.setXY(PS_Width, PS_Height, command);

		//Set up render
		GLfloat temp[18] = {
			v1.f_x, v1.f_y, 0.0f,
			v2.f_x, v2.f_y, 0.0f,
			v3.f_x, v3.f_y, 0.0f,

			v2.f_x, v2.f_y, 0.0f,
			v4.f_x, v4.f_y, 0.0f,
			v3.f_x, v3.f_y, 0.0f
		};

		GLfloat tempColor[18] = {
			v1.r, v1.b, v1.g,
			v2.r, v2.b, v2.g,
			v3.r, v3.b, v3.g,

			v2.r, v2.b, v2.g,
			v4.r, v4.b, v4.g,
			v3.r, v3.b, v3.g
		};

		memcpy(objects[objects_index].g_color_buffer_data, tempColor, sizeof(float) * 18);
		memcpy(objects[objects_index].g_vertex_buffer_data, temp, sizeof(float) * 18);

		objects[objects_index].type = QUAD;
		objects[objects_index].color_type = SOLID;

		objects_index++;

		count = -1;
		isCommand = false;
	}
	count++;
}

void GPU::Draw_Shaded_Quad(uw command)
{
	static POLY_TYPE poly_type;
	static uw count = 0;

	static gpu_vec2 v1, v2, v3, v4;

	if (count == 0)
	{
		//v1 color
		v1.setRGB(command);
	}
	else if (count == 1)
	{
		//v1
		v1.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 2)
	{
		//v2 color
		v2.setRGB(command);
	}
	else if (count == 3)
	{
		//v1
		v2.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 4)
	{
		//v2 color
		v3.setRGB(command);
	}
	else if (count == 5)
	{
		//v1
		v3.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 6)
	{
		//v2 color
		v4.setRGB(command);
	}
	else if (count == 7)
	{
		//v4
		v4.setXY(PS_Width, PS_Height, command);

		//Set up render
		GLfloat temp[18] = {
			v1.f_x, v1.f_y, 0.0f,
			v2.f_x, v2.f_y, 0.0f,
			v3.f_x, v3.f_y, 0.0f,

			v2.f_x, v2.f_y, 0.0f,
			v4.f_x, v4.f_y, 0.0f,
			v3.f_x, v3.f_y, 0.0f
		};

		GLfloat tempColor[18] = {
			v1.r, v1.g, v1.b,
			v2.r, v2.g, v2.b,
			v3.r, v3.g, v3.b,
						   
			v2.r, v2.g, v2.b,
			v4.r, v4.g, v4.b,
			v3.r, v3.g, v3.b
		};

		memcpy(objects[objects_index].g_color_buffer_data, tempColor, sizeof(float) * 18);
		memcpy(objects[objects_index].g_vertex_buffer_data, temp, sizeof(float) * 18);

		objects[objects_index].type = QUAD;
		objects[objects_index].color_type = SOLID;

		objects_index++;

		count = -1;
		isCommand = false;
	}
	count++;
}

void GPU::Draw_Shaded_Triangle(uw command)
{
	static POLY_TYPE poly_type;
	static uw count = 0;

	static gpu_vec2 v1, v2, v3, v4;

	if (count == 0)
	{
		//v1 color
		v1.setRGB(command);
	}
	else if (count == 1)
	{
		//v1
		v1.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 2)
	{
		//v2 color
		v2.setRGB(command);
	}
	else if (count == 3)
	{
		//v1
		v2.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 4)
	{
		//v2 color
		v3.setRGB(command);
	}
	else if (count == 5)
	{
		//v1
		v3.setXY(PS_Width, PS_Height, command);

		//Set up render
		GLfloat temp[18] = {
			v1.f_x, v1.f_y, 0.0f,
			v2.f_x, v2.f_y, 0.0f,
			v3.f_x, v3.f_y, 0.0f,

			v1.f_x, v1.f_y, 0.0f,
			v2.f_x, v2.f_y, 0.0f,
			v3.f_x, v3.f_y, 0.0f,
		};

		GLfloat tempColor[18] = {
			v1.r, v1.g, v1.b,
			v2.r, v2.g, v2.b,
			v3.r, v3.g, v3.b,

			v1.r, v1.g, v1.b,
			v2.r, v2.g, v2.b,
			v3.r, v3.g, v3.b,
		};

		memcpy(objects[objects_index].g_color_buffer_data, tempColor, sizeof(float) * 18);
		memcpy(objects[objects_index].g_vertex_buffer_data, temp, sizeof(float) * 18);

		objects[objects_index].type = TRIANGLE;
		objects[objects_index].color_type = SOLID;

		objects_index++;

		count = -1;
		isCommand = false;
	}
	count++;
}

void GPU::Draw_Textured_Quad(uw command)
{
	static uw count = 0;

	static gpu_vec2 v1, v2, v3, v4;

	static uh palette;
	static uh page;

	if (count == 0)
	{
		//None
		v1.setRGB(0x00000000);
		v2.setRGB(0x00000000);
		v3.setRGB(0x00000000);
		v4.setRGB(0x00000000);
	}
	else if (count == 1)
	{
		//v1
		v1.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 2)
	{
		palette = (command & 0xFFFF0000) >> 16;
		v1.setTexCoord(command);
	}
	else if (count == 3)
	{
		//v1
		v2.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 4)
	{
		page = (command & 0xFFFF0000) >> 16;
		v2.setTexCoord(command);
	}
	else if (count == 5)
	{
		//v1
		v3.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 6)
	{
		v3.setTexCoord(command);
	}
	else if (count == 7)
	{
		//v4
		v4.setXY(PS_Width, PS_Height, command);
	}
	else if (count == 8)
	{
		v4.setTexCoord(command);

		//Set up render
		GLfloat temp[18] = {
			v1.f_x, (-1) * v1.f_y, 0.0f,
			v2.f_x, (-1) * v2.f_y, 0.0f,
			v3.f_x, (-1) * v3.f_y, 0.0f,

			v2.f_x, (-1) * v2.f_y, 0.0f,
			v4.f_x, (-1) * v4.f_y, 0.0f,
			v3.f_x, (-1) * v3.f_y, 0.0f
		};

		GLfloat g_UV_buffer_data[] = {
			v1.texCoordX, v1.texCoordY,  // Upper Left
			v2.texCoordX, v2.texCoordY,  // Upper Right
			v3.texCoordX, v3.texCoordY,   // Lower left

			v2.texCoordX, v2.texCoordY,
			v4.texCoordX, v4.texCoordY,
			v3.texCoordX, v3.texCoordY
		};

		memcpy(objects[objects_index].g_uv_buffer_data, g_UV_buffer_data, sizeof(float) * 12);
		memcpy(objects[objects_index].g_vertex_buffer_data, temp, sizeof(float) * 18);

		//Make texture
		objects[objects_index].textureData = new ub[256 * 256 * 4];
		//Get clut address
		uh clutXloc = (palette & 0x3F) * 16;
		uh clutYloc = (palette & 0x7FC0) >> 6;

		uh texPageX = (page & 0xF) * 64;
		uh texPageY = (page & 0x10) >> 4;

		uw countX = 0;
		uw countY = 0;

		uh* clut = (VRAM + (clutYloc * 1024) + (clutXloc));

		//Copy 256 * 256 VRAM Block
		//memset(objects[objects_index].textureData, 255, sizeof(ub) * 256 * 256 * 3);
		const uh mask_and = 0;
		const uh mask_or = 0;

		uw src_x = texPageX;
		uw src_y = texPageY;
		uw dst_x = 0;
		uw dst_y = 0;
		uw width = 64;
		uw height = 64;

		for (uw y = 0; y < 256; y++) 
		{
			for (uw x = 0; x < 64; x++)
			{
				//Add texpageY to y
				uh pixel = VRAM[(y * 1024) + x + texPageX];
				ub p1 = (pixel & 0xF);
				ub p2 = (pixel & 0xF0) >> 4;
				ub p3 = (pixel & 0xF00) >> 8;
				ub p4 = (pixel & 0xF000) >> 12;

				uw index = ((y * 1024) + (x * 16));

				objects[objects_index].textureData[index + 3] = ((clut[p1] & 0x1F)) * 8;
				objects[objects_index].textureData[index + 2] = ((clut[p1] & 0x3E0) >> 5) * 8;
				objects[objects_index].textureData[index + 1] = ((clut[p1] & 0x7C00) >> 10) * 8;
				if (clut[p1] == 0x0) objects[objects_index].textureData[index] = 0; //Transparent
				else objects[objects_index].textureData[index] = 255; //Opeque
				
														 
				objects[objects_index].textureData[index + 7] = ((clut[p2] & 0x1F)) * 8;
				objects[objects_index].textureData[index + 6] = ((clut[p2] & 0x3E0) >> 5) * 8;
				objects[objects_index].textureData[index + 5] = ((clut[p2] & 0x7C00) >> 10) * 8;
				if (clut[p2] == 0x0) objects[objects_index].textureData[index + 4] = 0; //Transparent
				else objects[objects_index].textureData[index + 4] = 255; //Opeque
														 
				objects[objects_index].textureData[index + 11] = ((clut[p3] & 0x1F)) * 8;
				objects[objects_index].textureData[index + 10] = ((clut[p3] & 0x3E0) >> 5) * 8;
				objects[objects_index].textureData[index + 9] = ((clut[p3] & 0x7C00) >> 10) * 8;
				if (clut[p3] == 0x0) objects[objects_index].textureData[index + 8] = 0; //Transparent
				else objects[objects_index].textureData[index + 8] = 255; //Opeque
														 
				objects[objects_index].textureData[index + 15] = ((clut[p4] & 0x1F)) * 8;
				objects[objects_index].textureData[index + 14] = ((clut[p4] & 0x3E0) >> 5) * 8;
				objects[objects_index].textureData[index + 13] = ((clut[p4] & 0x7C00) >> 10) * 8;
				if (clut[p4] == 0x0) objects[objects_index].textureData[index + 12] = 0; //Transparent
				else objects[objects_index].textureData[index + 12] = 255; //Opeque
			}
		}


		objects[objects_index].type = QUAD;
		objects[objects_index].color_type = TEXTURED;

		objects_index++;

		count = -1;
		isCommand = false;
	}
	count++;
}

void GPU::gpu_init()
{
	SolidShader = gpu_LoadShaders("vertex.v", "pixel.p");
	TextureShader = gpu_LoadShaders("TextureVertex.v", "TexturePixel.p");
}

void GPU::gpu_clear()
{
	objects_index = 0;
}

void GPU::gpu_render()
{
	for (int i = 0; i < objects_index; i++) 
	{
		objects[i].setUp();
		objects[i].render(TextureShader, SolidShader);
	}
}

uw GPU::gpuReadStat()
{
	return (
		texPageXBase |
		texPageYBase << 4 |
		semiTranparent << 5 |
		texPageColor << 7 |
		dither << 9 |
		canDraw << 10 |
		maskBit << 11 |
		isDrawing << 12 |
		interlaceField << 13 |
		reverseFlag << 14 |
		texDisable << 15 |
		hRes2 << 16 |
		hRes1 << 17 |
		vRes << 19 |
		videoMode << 20 |
		colorDepth << 21 |
		verticalInterlace << 22 |
		displayEnable << 23 |
		interuptRequest << 24 |
		dmaDataRequest << 25 |
		readyReceiveCMD_word << 26 |
		readySend_cpu << 27 |
		readyReceiveDAM_block << 28 |
		DMA_dir << 29 |
		evenOrOdd << 31
		);
}

void GPU::gpu0Command(uw* command, uw size)
{
	for (int i = 0; i < size; i++) {
		uw command_byte = ((command[i]) & 0xFF000000) >> 24;
		//Checks if command was already issued that needs params
		if (isCommand) {
			//Send this command as a param to the commands function
			std::invoke(commandFunc, this, command[i]);
			continue;
		}
		//Command not started
		switch (command_byte)
		{
		case 0x00: //NOP
			break;
		case 0x01: //Cache clear
			break;
		case 0x28: //Monochrome four-point opaque polygon
			//printf("[GPU] 0x28 Monochrome four-point opaque polygon: %x\n", command[i]);
			isCommand = true;
			commandFunc = &GPU::Draw_Monocrome_Quad;
			std::invoke(commandFunc, this, command[i]);
			break;
		case 0x2C:
			//printf("[GPU] 0x2C Textured four-point opaque polygon: %x\n", command[i]);
			isCommand = true;
			commandFunc = &GPU::Draw_Textured_Quad;
			std::invoke(commandFunc, this, command[i]);
			break;
		case 0x30:
			//printf("[GPU] 0x30 Shaded three-point opaque polygon: %x\n", command[i]);
			isCommand = true;
			commandFunc = &GPU::Draw_Shaded_Triangle;
			std::invoke(commandFunc, this, command[i]);
			break;
		case 0x38:
			//printf("[GPU] 0x38 Shaded four-point opaque polygon: %x\n", command[i]);
			isCommand = true;
			commandFunc = &GPU::Draw_Shaded_Quad;
			std::invoke(commandFunc, this, command[i]);
			break;
		case 0xa0:
			//printf("[GPU] 0xA0 Copy Rectangle\n");
			isCommand = true;
			commandFunc = &GPU::CopyRectangle_CPU_to_VRAM;
			break;
		case 0xe1:
			break;
		case 0xe2:
			break;
		case 0xe3:
			break;
		case 0xe4:
			break;
		case 0xe5:
			break;
		case 0xe6:
			break;
		default:
			printf("[GPU COMMAND] Command not found %x\n", (command[i]));
		}
	}
}

void GPU::gpu1Command(uw command)
{
	//std::cout << "[GPU1] Command: " << std::hex << command << std::endl;

	switch ((command & 0xFF000000) >> 24)
	{
	case 0x00:
		break;
	case 0x01: //Resets GPU
		break;
	case 0x02: //Acknowledge gpu intrupt
		interuptRequest = 0;
		break;
	case 0x03: //Display enable
		displayEnable = (command & 0x1);
		break;
	case 0x05:
		break;
	case 0x06:
		break;
	case 0x07:
		break;
	case 0x08:
		hRes1 = (command & 0x3);
		vRes = (command & 0x4) >> 2;
		videoMode = (command & 0x8) >> 3;
		colorDepth = (command & 0x10) >> 4;
		verticalInterlace = (command & 0x20) >> 5;
		hRes2 = (command & 0x40) >> 6;
		reverseFlag = (command & 0x80) >> 7;
		break;
	case 0x04:
		DMA_dir = command & 0x3;
		break;
	default:
		system("PAUSE");
	}

	//Set screen resolution
	if (hRes2 == 0) {
		if (hRes1 == 0) PS_Width = 256;
		if (hRes1 == 1) PS_Width = 320;
		if (hRes1 == 2) PS_Width = 512;
		if (hRes1 == 3) PS_Width = 640;
	}
	else PS_Width = 368;
	if (vRes == 0) PS_Height = 240;
	else {
		if (verticalInterlace == 1) PS_Height = 480;
	}
}

GLuint GPU::gpu_LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}











//#include "Global.h"
//
//GPU gpu;
//
//void GPU::gpu_init()
//{
//
//	solidColorShader = gpu_LoadShaders("vertex.v", "pixel.p");
//	textureShader = gpu_LoadShaders("TextureVertex.v", "TexturePixel.p");
//}
//
//void GPU::gpu_clear()
//{
//	objects_index = 0;
//}
//
//void GPU::gpu_render()
//{
//	for (int i = 0; i < objects_index; i++) 
//	{
//		objects[i].setUp();
//		objects[i].render();
//	}
//}
//
//
//uw GPU::gpuReadStat()
//{
//	return (
//		texPageXBase |
//		texPageYBase << 4 |
//		semiTranparent << 5 |
//		texPageColor << 7 |
//		dither << 9 |
//		canDraw << 10 |
//		maskBit << 11 |
//		isDrawing << 12 |
//		interlaceField << 13 |
//		reverseFlag << 14 |
//		texDisable << 15 |
//		hRes2 << 16 |
//		hRes1 << 17 |
//		vRes << 19 |
//		videoMode << 20 |
//		colorDepth << 21 |
//		verticalInterlace << 22 |
//		displayEnable << 23 |
//		interuptRequest << 24 |
//		dmaDataRequest << 25 |
//		readyReceiveCMD_word << 26 |
//		readySend_cpu << 27 |
//		readyReceiveDAM_block << 28 |
//		DMA_dir << 29 |
//		evenOrOdd << 31
//	);
//}
//
//void GPU::gpu0Command(uw* command, uw size)
//{
//	//syphnParams(command);
//
//	//uw command_byte = ((*command) & 0xFF000000) >> 24;
//	//ub red = (command[0] & 0x0000FF);
//	//ub green = (command[0] & 0x00FF00) >> 8;
//	//ub blue = (command[0] & 0xFF0000) >> 16;
//
//	//switch (command_byte) {
//	//case 0x00: //NOP
//	//	break;
//	//case 0x01: //Clear Catch
//	//	break;
//	//case 0x28:
//	//	std::cout << "[Linked GPU] 0x28 Monochrome four-point opaque polygon: " << std::hex << *command << std::endl;
//	//	
//	//	monocromeShape(OPAQUE, QUAD, 
//	//		gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), red, blue, green),
//	//		gpu_vec2((command[2] & 0xFFFF), ((command[2] & 0xFFFF0000) >> 16), red, blue, green),
//	//		gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), red, blue, green),
//	//		gpu_vec2((command[4] & 0xFFFF), ((command[4] & 0xFFFF0000) >> 16), red, blue, green)
//	//	);
//	//	break;
//	//case 0x2C:
//	//	std::cout << "[Linked GPU] 0x2C Textured four-point opaque polygon: " << std::hex << *command << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[1] << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[2] << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[3] << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[4] << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[5] << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[6] << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[7] << std::endl;
//	//	std::cout << "[Data]:" << std::hex << command[8] << std::endl;
//
//	//	//Diplay Black object for now
//	//	monocromeShape(OPAQUE, QUAD,
//	//		gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), 0, 0, 0),
//	//		gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), 0, 0, 0),
//	//		gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), 0, 0, 0),
//	//		gpu_vec2((command[7] & 0xFFFF), ((command[7] & 0xFFFF0000) >> 16), 0, 0, 0)
//	//	);
//	//	break;
//	//case 0x30:
//	//	std::cout << "[Linked GPU] 0x30 Shaded three-point opaque polygon: " << std::hex << *command << std::endl;
//
//	//	shadedShape(OPAQUE, TRIANGLE,
//	//		gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), red, green, blue),
//	//		gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), (command[2] & 0xFF), (command[2] & 0xFF00) >> 8, (command[2] & 0xFF0000) >> 16),
//	//		gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), (command[4] & 0xFF), (command[4] & 0xFF00) >> 8, (command[4] & 0xFF0000) >> 16),
//	//		gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), (command[4] & 0xFF), (command[4] & 0xFF00) >> 8, (command[4] & 0xFF0000) >> 16)
//	//	);
//	//	break;
//	//case 0x38:
//	//	std::cout << "[Linked GPU] 0x38 Shaded four-point opaque polygon: " << std::hex << *command << std::endl;
//
//	//	shadedShape(OPAQUE, QUAD,
//	//		gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), red, green, blue),
//	//		gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), (command[2] & 0xFF), (command[2] & 0xFF00) >> 8, (command[2] & 0xFF0000) >> 16),
//	//		gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), (command[4] & 0xFF), (command[4] & 0xFF00) >> 8, (command[4] & 0xFF0000) >> 16),
//	//		gpu_vec2((command[7] & 0xFFFF), ((command[7] & 0xFFFF0000) >> 16), (command[6] & 0xFF), (command[6] & 0xFF00) >> 8, (command[6] & 0xFF0000) >> 16)
//	//	);
//
//	//	break;
//	//case 0xa0: //Copy Rectangle
//	//	std::cout << "[Linked GPU] 0xa0 Copy Rectangle: " << std::hex << *command << std::endl;
//	//	paramSyphnCount = 2;
//	//	callBack = 0xa0;
//	//	break;
//	//case 0xe1: //TexPage setttings - TODO
//
//	//	break;
//	//case 0xe3:
//	//	std::cout << "[Linked GPU] 0xe3 Set screen top left: " << std::hex << *command << std::endl;
//	//	break;
//	//default:
//	//	std::cout << "[Linked GPU] Command Unknown: " << std::hex << *command << std::endl;
//	//	system("PAUSE");
//	//}
//}
//
//void GPU::gpu1Command(uw command)
//{
//	std::cout << "[GPU1] Command: " << std::hex << command << std::endl;
//
//	switch((command & 0xFF000000) >> 24)
//	{
//	case 0x00:
//		break;
//	case 0x01: //Resets GPU
//		break;
//	case 0x02: //Acknowledge gpu intrupt
//		interuptRequest = 0;
//		break;
//	case 0x03: //Display enable
//		displayEnable = (command & 0x1);
//		break;
//	case 0x05:
//		break;
//	case 0x06:
//		break;
//	case 0x07:
//		break;
//	case 0x08:
//		hRes1 = (command & 0x3);
//		vRes = (command & 0x4) >> 2;
//		videoMode = (command & 0x8) >> 3;
//		colorDepth = (command & 0x10) >> 4;
//		verticalInterlace = (command & 0x20) >> 5;
//		hRes2 = (command & 0x40) >> 6;
//		reverseFlag = (command & 0x80) >> 7;
//		break;
//	case 0x04:
//		DMA_dir = command & 0x3;
//		break;
//	default:
//		system("PAUSE");
//	}
//
//	//Set screen resolution
//	if (hRes2 == 0) {
//		if (hRes1 == 0) PS_Width = 256;
//		if (hRes1 == 1) PS_Width = 320;
//		if (hRes1 == 2) PS_Width = 512;
//		if (hRes1 == 3) PS_Width = 640;
//	}
//	else PS_Width = 368;
//	if (vRes == 0) PS_Height = 240;
//	else {
//		if (verticalInterlace == 1) PS_Height = 480;
//	}
//}
//
//void GPU::gpu0CommandNew(uw* command, uw size)
//{
//	//Check for params to syphn
//	if (paramSyphnCount > 0)
//	{
//		//Syhpn
//		syphnBuffer.push_back(*command);
//		//std::cout << "[PARAM GPU] " << std::hex << paramSyphnCount << std::endl;
//
//		paramSyphnCount--;
//
//		if (paramSyphnCount == 0)
//		{
//			//Trigger CallBack
//			switch (callBack)
//			{
//			case 0xA0:
//				RecDest = syphnBuffer[0];
//				RecDimention = syphnBuffer[1];
//				RecH = ((syphnBuffer[1] & 0xFFFF0000) >> 16);
//				RecW = ((syphnBuffer[1] & 0xFFFF));
//				RecSize = RecW * RecH / 8;
//				recX = (syphnBuffer[0] & 0xFFFF);
//				recY = ((syphnBuffer[0] & 0xFFFF0000) >> 16);
//				//Set up new syphn for texture data
//				paramSyphnCount = RecSize;
//				callBack = 0xA1;
//				break;
//			case 0xa1:
//				//memcpy(VRAM + (recY * 2048 + recX), syphnBuffer.data(), syphnBuffer.size());
//				uh * temp = (uh*)VRAM;
//				int count = 0;
//				for (int i = 0; i < syphnBuffer.size(); i++)
//				{
//					if (count >= RecW) { recY++; count = 0; }
//					else recX++;
//					temp[(recY * 1024) + recX] = syphnBuffer[i];
//					count++;
//				}
//				break;
//			}
//			//Clear syphn buffer
//			syphnBuffer.clear();
//		}
//		return;
//	}
//
//	//Direct call, need to syphon inputs based on command
//	if (size == 0) 
//	{
//		switch (((*command) & 0xFF000000) >> 24) 
//		{
//		case 0x00: //NOP
//			break;
//		case 0x01: // Clear Cache
//			break;
//		case 0xA0: //Copy Rectangle 
//			std::cout << "[DIRECT GPU] a0 Copy Rectangle: " << std::hex << *command << std::endl;
//			paramSyphnCount = 2;
//			callBack = 0xa0;
//			break;
//		case 0xe1: //Set TexPage
//			texPageXBase = ((*command) & 0xf);
//			texPageXBase = (((*command) & 0x10) >> 4);
//			semiTranparent = (((*command) & 0x60) >> 5);
//			texPageColor = (((*command) & 0x180) >> 7);
//			dither = (((*command) & 0x200) >> 9);
//			canDraw = (((*command) & 0x400) >> 10);
//			break;
//		default:
//			std::cout << "[DIRECT GPU] Command Unknown: " << std::hex << *command << std::endl;
//			//system("PAUSE");
//		}
//	}
//	//Linked list call
//	else 
//	{
//		uw command_byte = ((*command) & 0xFF000000) >> 24;
//		ub red = (command[0] & 0x0000FF);
//		ub green = (command[0] & 0x00FF00) >> 8;
//		ub blue = (command[0] & 0xFF0000) >> 16;
//
//		switch (command_byte) {
//		case 0x00: //NOP
//			break;
//		case 0x01: //Clear Catch
//			break;
//		case 0x28:
//			std::cout << "[Linked GPU] 0x28 Monochrome four-point opaque polygon: " << std::hex << *command << std::endl;
//
//			monocromeShape(OPAQUE, QUAD,
//				gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), red, blue, green),
//				gpu_vec2((command[2] & 0xFFFF), ((command[2] & 0xFFFF0000) >> 16), red, blue, green),
//				gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), red, blue, green),
//				gpu_vec2((command[4] & 0xFFFF), ((command[4] & 0xFFFF0000) >> 16), red, blue, green)
//			);
//			break;
//		case 0x2C:
//			std::cout << "[Linked GPU] 0x2C Textured four-point opaque polygon: " << std::hex << *command << std::endl;
//			std::cout << "[Data]:" << std::hex << command[1] << std::endl;
//			std::cout << "[Data]:" << std::hex << command[2] << std::endl;
//			std::cout << "[Data]:" << std::hex << command[3] << std::endl;
//			std::cout << "[Data]:" << std::hex << command[4] << std::endl;
//			std::cout << "[Data]:" << std::hex << command[5] << std::endl;
//			std::cout << "[Data]:" << std::hex << command[6] << std::endl;
//			std::cout << "[Data]:" << std::hex << command[7] << std::endl;
//			std::cout << "[Data]:" << std::hex << command[8] << std::endl;
//
//			//Diplay Black object for now
//			shadedShape(OPAQUE, QUAD,
//				gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), 255, 255, 0),
//				gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), 0, 255, 255),
//				gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), 255, 0, 255),
//				gpu_vec2((command[7] & 0xFFFF), ((command[7] & 0xFFFF0000) >> 16), 0, 255, 0)
//			);
//			break;
//		case 0x30:
//			std::cout << "[Linked GPU] 0x30 Shaded three-point opaque polygon: " << std::hex << *command << std::endl;
//
//			shadedShape(OPAQUE, TRIANGLE,
//				gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), red, green, blue),
//				gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), (command[2] & 0xFF), (command[2] & 0xFF00) >> 8, (command[2] & 0xFF0000) >> 16),
//				gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), (command[4] & 0xFF), (command[4] & 0xFF00) >> 8, (command[4] & 0xFF0000) >> 16),
//				gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), (command[4] & 0xFF), (command[4] & 0xFF00) >> 8, (command[4] & 0xFF0000) >> 16)
//			);
//			break;
//		case 0x38:
//			std::cout << "[Linked GPU] 0x38 Shaded four-point opaque polygon: " << std::hex << *command << std::endl;
//
//			shadedShape(OPAQUE, QUAD,
//				gpu_vec2((command[1] & 0xFFFF), ((command[1] & 0xFFFF0000) >> 16), red, green, blue),
//				gpu_vec2((command[3] & 0xFFFF), ((command[3] & 0xFFFF0000) >> 16), (command[2] & 0xFF), (command[2] & 0xFF00) >> 8, (command[2] & 0xFF0000) >> 16),
//				gpu_vec2((command[5] & 0xFFFF), ((command[5] & 0xFFFF0000) >> 16), (command[4] & 0xFF), (command[4] & 0xFF00) >> 8, (command[4] & 0xFF0000) >> 16),
//				gpu_vec2((command[7] & 0xFFFF), ((command[7] & 0xFFFF0000) >> 16), (command[6] & 0xFF), (command[6] & 0xFF00) >> 8, (command[6] & 0xFF0000) >> 16)
//			);
//
//			break;
//		//case 0xa0: //Copy Rectangle
//		//	std::cout << "[Linked GPU] 0xa0 Copy Rectangle: " << std::hex << *command << std::endl;
//		//	paramSyphnCount = 2;
//		//	callBack = 0xa0;
//		//	break;
//		case 0xe1: //TexPage setttings - TODO
//
//			break;
//		case 0xe3:
//			std::cout << "[Linked GPU] 0xe3 Set screen top left: " << std::hex << *command << std::endl;
//			break;
//		default:
//			std::cout << "[Linked GPU] Command Unknown: " << std::hex << *command << std::endl;
//			system("PAUSE");
//		}
//	}
//}
//
//void GPU::monocromeShape(ub opaque, ub type, gpu_vec2 v1, gpu_vec2 v2, gpu_vec2 v3, gpu_vec2 v4)
//{
//	//This is going to be strange
//
//	v1.normalize(PS_Width, PS_Height);
//	v2.normalize(PS_Width, PS_Height);
//	v3.normalize(PS_Width, PS_Height);
//	v4.normalize(PS_Width, PS_Height);
//
//	GLfloat temp[18] = {
//		v1.f_x, v1.f_y, 0.0f,
//		v2.f_x, v2.f_y, 0.0f,
//		v3.f_x, v3.f_y, 0.0f,
//
//		v2.f_x, v2.f_y, 0.0f,
//		v4.f_x, v4.f_y, 0.0f,
//		v3.f_x, v3.f_y, 0.0f
//	};
//
//	GLfloat tempColor[18] = {
//		v1.red, v1.blue, v1.green,
//		v1.red, v1.blue, v1.green,
//		v1.red, v1.blue, v1.green,
//		v1.red, v1.blue, v1.green,
//		v1.red, v1.blue, v1.green,
//		v1.red, v1.blue, v1.green
//	};
//
//	memcpy(objects[objects_index].g_color_buffer_data, tempColor, sizeof(float) * 18);
//	memcpy(objects[objects_index].g_vertex_buffer_data, temp, sizeof(float) * 18);
//
//	objects[objects_index].shaderID = solidColorShader;
//	objects[objects_index].type = (TYPE)type;
//
//	objects_index++;
//}
//
//void GPU::shadedShape(ub opaque, ub type, gpu_vec2 v1, gpu_vec2 v2, gpu_vec2 v3, gpu_vec2 v4)
//{
//	v1.normalize(PS_Width, PS_Height);
//	v2.normalize(PS_Width, PS_Height);
//	v3.normalize(PS_Width, PS_Height);
//	v4.normalize(PS_Width, PS_Height);
//
//	GLfloat temp[18] = {
//		v1.f_x, v1.f_y, 0.0f,
//		v2.f_x, v2.f_y, 0.0f,
//		v3.f_x, v3.f_y, 0.0f,
//
//		v2.f_x, v2.f_y, 0.0f,
//		v4.f_x, v4.f_y, 0.0f,
//		v3.f_x, v3.f_y, 0.0f
//	};
//
//	GLfloat tempColor[18] = {
//		v1.red, v1.blue, v1.green,
//		v2.red, v2.blue, v2.green,
//		v3.red, v3.blue, v3.green,
//
//		v2.red, v2.blue, v2.green,
//		v4.red, v4.blue, v4.green,
//		v3.red, v3.blue, v3.green
//	};
//
//	memcpy(objects[objects_index].g_color_buffer_data, tempColor, sizeof(float) * 18);
//	memcpy(objects[objects_index].g_vertex_buffer_data, temp, sizeof(float) * 18);
//
//	objects[objects_index].shaderID = solidColorShader;
//	objects[objects_index].type = (TYPE)type;
//
//	objects_index++;
//}
//
//void GPU::texturedShape(ub opaque, ub type, ub blend, gpu_vec2 v1, gpu_vec2 v2, gpu_vec2 v3, gpu_vec2 v4)
//{
//}
//
//GLuint GPU::gpu_LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
//{
//	// Create the shaders
//	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
//
//	// Read the Vertex Shader code from the file
//	std::string VertexShaderCode;
//	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
//	if (VertexShaderStream.is_open()) {
//		std::stringstream sstr;
//		sstr << VertexShaderStream.rdbuf();
//		VertexShaderCode = sstr.str();
//		VertexShaderStream.close();
//	}
//	else {
//		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
//		getchar();
//		return 0;
//	}
//
//	// Read the Fragment Shader code from the file
//	std::string FragmentShaderCode;
//	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
//	if (FragmentShaderStream.is_open()) {
//		std::stringstream sstr;
//		sstr << FragmentShaderStream.rdbuf();
//		FragmentShaderCode = sstr.str();
//		FragmentShaderStream.close();
//	}
//
//	GLint Result = GL_FALSE;
//	int InfoLogLength;
//
//	// Compile Vertex Shader
//	printf("Compiling shader : %s\n", vertex_file_path);
//	char const* VertexSourcePointer = VertexShaderCode.c_str();
//	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
//	glCompileShader(VertexShaderID);
//
//	// Check Vertex Shader
//	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//		printf("%s\n", &VertexShaderErrorMessage[0]);
//	}
//
//	// Compile Fragment Shader
//	printf("Compiling shader : %s\n", fragment_file_path);
//	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
//	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
//	glCompileShader(FragmentShaderID);
//
//	// Check Fragment Shader
//	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//		printf("%s\n", &FragmentShaderErrorMessage[0]);
//	}
//
//	// Link the program
//	printf("Linking program\n");
//	GLuint ProgramID = glCreateProgram();
//	glAttachShader(ProgramID, VertexShaderID);
//	glAttachShader(ProgramID, FragmentShaderID);
//	glLinkProgram(ProgramID);
//
//	// Check the program
//	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
//	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
//		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//		printf("%s\n", &ProgramErrorMessage[0]);
//	}
//
//	glDetachShader(ProgramID, VertexShaderID);
//	glDetachShader(ProgramID, FragmentShaderID);
//
//	glDeleteShader(VertexShaderID);
//	glDeleteShader(FragmentShaderID);
//
//	return ProgramID;
//}
