#pragma once
#include <vector>
#include "Cube.h"
#include "DOF.h"
#include "Tokenizer.h"
#include <map>
#include <vector>
#include <string>
using namespace std;


class Joint {
private:
	vector<Joint*> children;
	Cube* cube;

public:
	DOF x, y, z;
	string jname;
	Joint();
	glm::vec3 offset, boxmin, boxmax;
	glm::mat4 matrix_local, matrix_world;
	bool load(Tokenizer& token, vector<Joint*>& joints);
	void addChild(Joint& joint);
	void update(glm::mat4 parent);
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
};
