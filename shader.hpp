#pragma once

std::string readFile(const char* filename);
GLuint LoadMainShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint LoadComputeShader(const char* compute_file_path);
