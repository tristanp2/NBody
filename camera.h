#pragma once
#include <glm\glm.hpp>


//Really basic orbital cam

class Camera
{
public:
	void init(glm::vec3 eye, glm::vec3 target, glm::vec3 up, float width, float height);
	void updatePos();
	void stepLeft();
	void stepRight();
	void stepUp();
	void stepDown();
	void zoomOut();
	void zoomIn();
	glm::mat4 getMVP();
	~Camera();
private:
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 up;
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 mvp;
};

