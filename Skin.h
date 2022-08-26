#ifndef _SKIN_H_
#define _SKIN_H_

#include "core.h"
#include "Skeleton.h"

class Skin {
private:

	glm::mat4 model;
	glm::vec3 color;

	//Vertex* vertex;
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	std::vector<glm::vec3> bindPositions;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> bindNormals;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::mat4> bindings;
	std::vector<glm::mat4> normBindings;
	std::vector<std::vector<std::pair<int, float>>> sweights;

	float test = 1.0;

public:
	Skeleton* skeleton;
	Skin(Skeleton* skel);
	//Skeleton* skeleton;
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	bool load(const char* file);
	void update();
	~Skin();
};

#endif