#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


void Camera::init(glm::vec3 eye, glm::vec3 target, glm::vec3 up, float width, float height)
{
	Projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
	this->pos = eye;
	this->target = target;
	this->up = up;
	View = glm::lookAt(pos, target, up);
	mvp = Projection*View*glm::mat4(1.0f);
}

void Camera::updatePos()
{
	View = glm::lookAt(pos, target, up);
	mvp = Projection*View*glm::mat4(1.0f);
}

void Camera::stepLeft()
{
	glm::vec3 dir = glm::cross(pos, up);
	dir = glm::normalize(dir);
	pos += dir;
 	updatePos();
}

void Camera::stepRight()
{
	glm::vec3 dir = glm::cross(pos, up);
	dir = glm::normalize(dir);
	pos -= dir;
	updatePos();
}

void Camera::stepUp()
{
	glm::vec3 dir = glm::cross(pos, up);
	glm::vec3 dir2 = glm::normalize(glm::cross(pos, dir));
	pos -= dir2;
	updatePos();
}

void Camera::stepDown()
{
	glm::vec3 dir = glm::cross(pos, up);
	glm::vec3 dir2 = glm::normalize(glm::cross(pos, dir));
	pos += dir2;
	updatePos();
}
void Camera::zoomIn() {
	pos -= glm::normalize(pos);
	updatePos();
}
void Camera::zoomOut() {
	pos += glm::normalize(pos);
	updatePos();
}
glm::mat4 Camera::getMVP()
{
	return mvp;
}

Camera::~Camera()
{
}
