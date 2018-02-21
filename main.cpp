#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <math.h>

#include "shader.hpp"
#include "renderer.h"
#include "camera.h"


#define MAX_PARTICLES 40960
#define WIDTH 1024
#define HEIGHT 768
#define PI 3.14159265358979323846

GLFWwindow *window;
Camera camera;
std::uniform_real_distribution<float> dist(0.0f,1.0f);
std::default_random_engine gen;

struct Simulation {
	float G;
	float timestep;
	float pointSize;
	int numParticles;
	int computeIterations;
	std::vector<GLfloat> pos;
	std::vector<GLfloat> vel;
};
//generate position in a disk area
glm::vec4 generatePosition(int seed, glm::vec4 offset, float disc_radius, float disc_thickness) {
	gen.seed(seed);
	glm::vec4 pos;
	float angle = dist(gen) * 2 * PI;
	float r = dist(gen)*disc_radius;
	pos.x = cos(angle)*r + offset.x;
	pos.y = sin(angle)*r + offset.y;
	pos.z = dist(gen)*disc_thickness + offset.z;
	pos.w = 1.0f;
	return pos;
}
//velocities are generated based on position
//essentially give each particle in same direction around disc
glm::vec4 generateVelocity(glm::vec4 position, glm::vec4 offset, float speed = 0.5f) {
	//get velocity in tangential direction
	glm::vec3 pos = glm::vec3(position - offset);
	glm::vec3 vel = glm::cross(pos, glm::vec3(0, 0, 1));
	
	//saw someone else do this, dont know why yet
	//seems like it should be the inverse of the length
	float orbital_vel = sqrt(2.0*glm::length(vel));

	vel = glm::normalize(vel)*orbital_vel * speed;

	return glm::vec4(vel,0.0);
}
bool pause = true;
bool released = true;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (released) {
		if (key == GLFW_KEY_SPACE) {
			pause = !pause;
		}
		else if (key == GLFW_KEY_LEFT) {
			camera.stepLeft();
		}
		else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			camera.stepRight();
		}
		else if (key == GLFW_KEY_UP) {
			camera.stepUp();
		}
		else if (key == GLFW_KEY_DOWN) {
			camera.stepDown();
		}
		else if (key == GLFW_KEY_LEFT_CONTROL) {
			camera.zoomOut();
		}
		else if (key == GLFW_KEY_LEFT_SHIFT) {
			camera.zoomIn();
		}
		released = false;
	}
	else {
		if (action == GLFW_RELEASE) {
			released = true;
		}
	}
}
void createClusterCollisionSim(Simulation& sim) {
	sim.numParticles = MAX_PARTICLES;
	sim.pos.resize(sim.numParticles * 4);
	sim.vel.resize(sim.numParticles * 4);
	float galaxy_radius = 3.0f;
	float orbital_speed = 0.3f;
	std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
	long long start_seed = d.count();
	glm::vec4 offset1(10.0f, 0.0f, 0.0f, 0.0f);
	glm::vec4 offset2 = - offset1;
	//initial velocity for galaxy2
	float speed = 0.5f;
	glm::vec4 init_vel = speed*glm::normalize(offset1);
	//galaxy1
	for (size_t i = 0; i < sim.numParticles / 2; i++) {
		glm::vec4 gen_pos = generatePosition(start_seed + i, offset1, galaxy_radius , 1.0);
		glm::vec4 gen_vel = generateVelocity(gen_pos, offset1, orbital_speed);
		gen_vel -= init_vel;
		size_t idx = i * 4;
		sim.pos[idx++] = gen_pos.x;
		sim.pos[idx++] = gen_pos.y;
		sim.pos[idx++] = gen_pos.z;
		sim.pos[idx++] = gen_pos.w;

		idx = i * 4;

		sim.vel[idx++] = gen_vel.x;
		sim.vel[idx++] = gen_vel.y;
		sim.vel[idx++] = gen_vel.z;
		sim.vel[idx++] = gen_vel.w;
	}
	//galaxy2
	for (size_t i = sim.numParticles / 2; i < sim.numParticles; i++) {
		glm::vec4 gen_pos = generatePosition(start_seed + i, offset2, galaxy_radius, 1.0);
		glm::vec4 gen_vel = generateVelocity(gen_pos, offset2, orbital_speed);
		gen_vel += init_vel;
		size_t idx = i * 4;
		sim.pos[idx++] = gen_pos.x;
		sim.pos[idx++] = gen_pos.y;
		sim.pos[idx++] = gen_pos.z;
		sim.pos[idx++] = gen_pos.w;

		idx = i * 4;

		sim.vel[idx++] = gen_vel.x;
		sim.vel[idx++] = gen_vel.y;
		sim.vel[idx++] = gen_vel.z;
		sim.vel[idx++] = gen_vel.w;
	}
	sim.G = 6.67 * pow(10, -5);
	sim.computeIterations = 1;
	sim.timestep = 0.025;
	sim.pointSize = 1.0f;
}
void createSingleGalaxySim(Simulation& sim) {
	sim.numParticles = MAX_PARTICLES;
	sim.pos.resize(sim.numParticles * 4);
	sim.vel.resize(sim.numParticles * 4);
	float speed = 0.35f;
	std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
	long long start_seed = d.count();
	for (size_t i = 0; i < sim.numParticles; i++) {
		glm::vec4 gen_pos = generatePosition(start_seed + i, glm::vec4(0.0), 4.0, 1.0f);
		glm::vec4 gen_vel = generateVelocity(gen_pos, glm::vec4(0.0), speed);
		size_t idx = i * 4;
		sim.pos[idx++] = gen_pos.x;
		sim.pos[idx++] = gen_pos.y;
		sim.pos[idx++] = gen_pos.z;
		sim.pos[idx++] = gen_pos.w;

		idx = i * 4;

		sim.vel[idx++] = gen_vel.x;
		sim.vel[idx++] = gen_vel.y;
		sim.vel[idx++] = gen_vel.z;
		sim.vel[idx++] = gen_vel.w;
	}
	sim.G = 6.67 * pow(10, -5);
	sim.computeIterations = 1;
	sim.timestep = 0.05;
	sim.pointSize = 2.0f;
}

int main() {
	//int temp;
	//std::cout << "renderdoc???" << std::endl;
	//std::cin >> temp;
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(WIDTH, HEIGHT, "NBody", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);


	std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
	long long start_seed = d.count();
	Simulation currentSim;
	createClusterCollisionSim(currentSim);
	//createSingleGalaxySim(currentSim);
	try{
		Renderer renderer(currentSim.numParticles, currentSim.computeIterations, currentSim.G, currentSim.pointSize);

		renderer.PopulateParticles(currentSim.pos, currentSim.vel);

		camera.init(glm::vec3(0, -15, 10), glm::vec3(0), glm::vec3(0, 0, 1), WIDTH, HEIGHT);
		float smoothing = 0.9;
		auto last = std::chrono::steady_clock::now();
		char title[80];
		int update_thresh = 30, update_count = 30;
		float measurement = 0;
		do {
			auto frame_start = std::chrono::steady_clock::now();
			std::chrono::duration<double> current = frame_start - last;
			last = frame_start;
			measurement = smoothing*measurement + (current.count() * (1.0 - smoothing));
			float fps = 1 / measurement;
			if (update_count >= update_thresh) {
				sprintf_s(title, "NBody: N = %d\t| %.2f fps", MAX_PARTICLES, fps);
				glfwSetWindowTitle(window, title);
				update_count = 0;
			}
			else
				update_count++;

			if(!pause)
				renderer.StepSimulation(currentSim.timestep);
			renderer.Render(camera.getMVP());

			// Swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		} // Check if the ESC key was pressed or the window was closed
		while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		int n;
		std::cin >> n;
		return EXIT_FAILURE;
	}
		

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return EXIT_SUCCESS;
}