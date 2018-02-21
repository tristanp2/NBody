#include "renderer.h"
#include "shader.hpp"
#include <math.h>




Renderer::Renderer(unsigned int numParticles, unsigned int computeIterations, float G, float point_size)
{
	//TODO: Get points to not be square, maybe get textured points
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glPointSize(point_size);
	this->G = G;
	//G = 6.67 * pow(10, -5);
	CreateObjects();
	this->numParticles = numParticles;
	this->computeIterations = computeIterations;
	InitShaders();
	SetUniforms();
}

void Renderer::PopulateParticles(const std::vector<GLfloat>& pos, const std::vector<GLfloat>& vel)
{
	//position upload
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesPosVBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, pos.size() * sizeof(GLfloat), pos.data(), 0);
	
	//velocity upload
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesVelSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, vel.size() * sizeof(GLfloat), vel.data(), 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//cross fingers
	//this should allow the position buffer to be accessed from interaction shader
	//and the velocity buffer to be accessed between both compute shaders
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particlesPosVBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, particlesVelSSBO);
}

void Renderer::StepSimulation(float dt)
{
	float step = dt / computeIterations;
	//This doesnt cause the compute pass to crash for some reason
	int num_groups = ceil(float(numParticles) / 64.0);
	if (num_groups < 1) num_groups = 1;
	for (int i = 0; i < computeIterations; i++) {
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glUseProgram(interactionShader);
		glUniform1f(dtUniform, step);
		glUniform1f(gUniform, G);
		glUniform1i(numUniform, numParticles);
		glDispatchCompute(num_groups, 1, 1);
		
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glUseProgram(integrationShader);
		glUniform1f(dtUniform, step);
		glDispatchCompute(num_groups, 1, 1);
	}
	
}

void Renderer::Render(glm::mat4 mvp)
{
	glUseProgram(mainShader);
	glBindVertexArray(particlesVAO);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, numParticles);
	glBindVertexArray(0);
}

void Renderer::InitShaders()
{
	mainShader = LoadMainShaders("shader.vert", "shader.frag");
	interactionShader = LoadComputeShader("interaction.comp");
	integrationShader = LoadComputeShader("integration.comp");
}

void Renderer::CreateObjects()
{
	//Particle VAO
	glGenVertexArrays(1, &particlesVAO);
	glBindVertexArray(particlesVAO);

	//Position
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &particlesPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, particlesPosVBO);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Velocity
	glEnableVertexAttribArray(2);
	glGenBuffers(1, &particlesVelSSBO);
	glBindBuffer(GL_ARRAY_BUFFER, particlesVelSSBO);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Renderer::SetUniforms()
{
	matrixID = glGetUniformLocation(mainShader, "MVP");
	dtUniform = 0;
	gUniform = 1;
	numUniform = 2;
}
