#pragma once
#include "Tokenizer.h"
#include "Skeleton.h"

class Vertex {
private:
	std::vector<glm::vec3> bindPositions;
	std::vector<glm::vec4> positions;
	std::vector<glm::vec3> bindNormals;
	std::vector<glm::vec4> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::mat4> bindings;
	std::vector<glm::mat4> normBindings;
	std::vector<std::vector<std::pair<int, float>>> sweights;

public:
	Skeleton* skeleton;
	Vertex(Skeleton* skel);
	bool load(const char* file);
	void update();
	std::vector<unsigned int> getIndices();
	std::vector<glm::vec4> getNormals();
	std::vector<glm::vec4> getPositions();
};