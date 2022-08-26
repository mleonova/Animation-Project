#include "Skeleton.h"
#include "Tokenizer.h"
#include <fstream>
#include <iostream>

bool Skeleton::load(const char* file) {

	Tokenizer token;
	bool openfile = token.Open(file);
	bool success = token.FindToken("balljoint");

	root = new Joint;
	root->jname = "root";
	joints.push_back(root);
	root->load(token, joints);

	// Finish
	token.Close();
	return true;
}

void Skeleton::update() {
	root->update(glm::mat4(1.0f));
}

void Skeleton::draw(const glm::mat4& viewProjMtx, GLuint shader) {
	root->draw(viewProjMtx, shader);
}

string Skeleton::getName(int joint) {
	string name = joints[joint]->jname;
	return name;
}

glm::mat4 Skeleton::getWorldMatrix(int joint) {

	glm::mat4 world_mat = joints[joint]->matrix_world;

	return world_mat;
}

Joint* Skeleton::getJoint(int joint) {
	
	Joint* j = joints[joint];

	return j;
}