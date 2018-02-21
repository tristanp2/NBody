#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.hpp"


/*************************************
 Code based off tutorial found here:
	http://www.opengl-tutorial.org/
*************************************/

std::string readFile(const char* filename) {
	std::ifstream file(filename, std::ios::in);
	std::string text;
	if (file.is_open()) {
		std::string Line = "";
		while (getline(file, Line))
			text += "\n" + Line;
		file.close();
	}

	return text;
}

GLuint LoadMainShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode = readFile(vertex_file_path);

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode = readFile(fragment_file_path);
	

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
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
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint LoadComputeShader(const char* compute_file_path) {
	//create shader object
	GLuint computeShaderID = glCreateShader(GL_COMPUTE_SHADER);

	//read code from file
	std::string computeShaderCode =readFile(compute_file_path);

	printf("Compiling shader : %s\n", compute_file_path);
	char const * computeSourcePointer = computeShaderCode.c_str();
	glShaderSource(computeShaderID, 1, &computeSourcePointer, NULL);
	glCompileShader(computeShaderID);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv(computeShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(computeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> computeShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(computeShaderID, InfoLogLength, NULL, &computeShaderErrorMessage[0]);
		throw std::runtime_error("error compiling: " + std::string(compute_file_path) + "\n" + std::string(&computeShaderErrorMessage[0]));
	}

	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, computeShaderID);
	glLinkProgram(programID);

	glGetProgramiv(computeShaderID, GL_LINK_STATUS, &Result);
	glGetProgramiv(computeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> programErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &programErrorMessage[0]);
		throw std::runtime_error("error linking: " + std::string(compute_file_path) + "\n" + std::string(&programErrorMessage[0]));
	}

	glDetachShader(programID, computeShaderID);
	glDeleteShader(computeShaderID);
	
	return programID;
}


