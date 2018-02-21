#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <vector>
#include <glm\glm.hpp>

class Renderer
{
public:
	Renderer(unsigned int numParticles, unsigned int computeIterations, float G, float point_size);
	void PopulateParticles(const std::vector<GLfloat>& pos, const std::vector<GLfloat>& vel);
	void StepSimulation(float dt);
	void Render(glm::mat4 mvp);
private:
	GLuint particlesVAO;
	GLuint particlesPosVBO;
	GLuint particlesVelSSBO;
	GLuint matrixID;
	GLuint dtUniform;
	GLuint gUniform;
	GLuint numUniform;
	GLuint mainShader;
	GLuint interactionShader;
	GLuint integrationShader;
	int width;
	int height;
	float G;
	unsigned int numParticles;
	unsigned int computeIterations;
	
	void InitShaders();
	void CreateObjects();
	void SetUniforms();
};

