#pragma once
#include "Joint.h"
#include <map>
#include <vector>
#include <iostream>
using namespace std;

class Skeleton {
private:
	Joint* root;

public:
	string file;
	vector<Joint*> joints;
	bool load(const char* file);
	void update();
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	glm::mat4 getWorldMatrix(int joint);
	string getName(int joint);
	Joint* getJoint(int joint);
};