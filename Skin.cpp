#include "Skin.h"
#include "main.h"
#include <iostream>

Skin::Skin(Skeleton* skel)
{
	skeleton = skel;
}

bool Skin::load(const char* file) {

	// Model matrix.
	model = glm::mat4(1.0f);

	// Color 
	color = glm::vec3(1.0f, 1.0f, 1.0f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);
	glGenBuffers(1, &EBO);


	Tokenizer token;
	bool openfile = token.Open(file);

	while (1) {
		char temp[256];
		token.GetToken(temp);

		if (strcmp(temp, "positions") == 0) {
			int n = token.GetInt();

			token.SkipLine();

			for (int i = 0; i < n; i++) {

				float x = token.GetFloat();
				float y = token.GetFloat();
				float z = token.GetFloat();

				glm::vec3 pos = glm::vec3(x, y, z);
				bindPositions.push_back(pos);

				positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
			}
		}
		else if (strcmp(temp, "normals") == 0) {

			int n = token.GetInt();
			token.SkipLine();

			for (int i = 0; i < n; i++) {

				float x = token.GetFloat();
				float y = token.GetFloat();
				float z = token.GetFloat();

				glm::vec3 norm = glm::vec3(x, y, z);
				bindNormals.push_back(norm);

				normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
			}
		}
		else if (strcmp(temp, "skinweights") == 0) {
			int n = token.GetInt();

			std::vector<std::pair<int, float>> jweights;

			token.SkipLine();

			for (int i = 0; i < n; i++) {
				int njoints = token.GetInt();

				for (int j = 0; j < njoints; j++) {
					std::pair<int, float> weights;

					int joint = token.GetInt();
					float weight = token.GetFloat();

					weights = std::make_pair(joint, weight);

					jweights.push_back(weights);
				}

				sweights.push_back(jweights);
				jweights.clear();

			}
		}
		else if (strcmp(temp, "triangles") == 0) {
			int n = token.GetInt();

			token.SkipLine();

			for (int i = 0; i < n * 3; i++) {

				float index = token.GetFloat();
				indices.push_back(index);
			}
		}
		else if (strcmp(temp, "bindings") == 0) {
			int n = token.GetInt();

			token.SkipLine();

			for (int i = 0; i < n; i++) {

				char temp[256];
				token.GetToken(temp);

				glm::mat4 binding;
				glm::mat4 normbinding;
				glm::vec4 vec1, vec2;

				if (strcmp(temp, "matrix") == 0) {

					token.SkipLine();

					float ax = token.GetFloat();
					float ay = token.GetFloat();
					float az = token.GetFloat();
					float one = 0.0f;

					glm::vec4 v1 = glm::vec4(ax, ay, az, one);

					float bx = token.GetFloat();
					float by = token.GetFloat();
					float bz = token.GetFloat();
					float two = 0.0f;

					glm::vec4 v2 = glm::vec4(bx, by, bz, two);

					float cx = token.GetFloat();
					float cy = token.GetFloat();
					float cz = token.GetFloat();
					float three = 0.0f;

					glm::vec4 v3 = glm::vec4(cx, cy, cz, three);

					float dx = token.GetFloat();
					float dy = token.GetFloat();
					float dz = token.GetFloat();
					float four = 1.0f;

					glm::vec4 v4 = glm::vec4(dx, dy, dz, four);

					binding[0] = v1;
					binding[1] = v2;
					binding[2] = v3;
					binding[3] = v4;
					bindings.push_back(binding);

					token.SkipWhitespace();
					token.SkipLine();
				}
			}

			token.Close();
			return true;

		}
		else {
			token.SkipLine();
		}
	}
}

void Skin::update() {

	for (int i = 0; i < bindPositions.size(); i++) {
		glm::vec4 vertex = glm::vec4(bindPositions[i], 1.0f);
		glm::vec4 normal = glm::vec4(bindNormals[i], 0.0f);

		glm::vec4 newVertex = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 newNormal = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int j = 0; j < sweights[i].size(); j++) {
			int joint = sweights[i][j].first;
			float weight = sweights[i][j].second;

			glm::mat4 worldMatrix = skeleton->getWorldMatrix(joint);
			glm::mat4 bindingMatrix = bindings[joint];

			newVertex += weight * worldMatrix * glm::inverse(bindingMatrix) * vertex;
			newNormal += weight * glm::transpose(glm::inverse(worldMatrix * glm::inverse(bindingMatrix))) * normal;

		}

		positions[i] = newVertex;
		normals[i] = glm::normalize(newNormal);
	}
}


void Skin::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	// Bind the VAO
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	//// actiavte the shader program 
	glUseProgram(shader);

	//// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (skeleton->file == "wasp.skel") {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Controls");

	if (ImGui::Button("Reset")) {

	}

	for (int i = 0; i < skeleton->joints.size(); i++) {

		string jointName = skeleton->getName(i);
		char* name = new char[jointName.length() + 1];
		strcpy(name, jointName.c_str());
		
		ImGui::Text(name);

		string xname = jointName + ".x";
		float xmax = skeleton->getJoint(i)->x.max;
		float xmin = skeleton->getJoint(i)->x.min;

		string yname = jointName + ".y";
		float ymax = skeleton->getJoint(i)->y.max;
		float ymin = skeleton->getJoint(i)->y.min;

		string zname = jointName + ".z";
		float zmax = skeleton->getJoint(i)->z.max;
		float zmin = skeleton->getJoint(i)->z.min;

		glUniform1f(glGetUniformLocation(shader, xname.c_str()), skeleton->getJoint(i)->x.value);
		glUniform1f(glGetUniformLocation(shader, yname.c_str()), skeleton->getJoint(i)->y.value);
		glUniform1f(glGetUniformLocation(shader, zname.c_str()), skeleton->getJoint(i)->z.value);

		ImGui::SliderFloat(xname.c_str(), &skeleton->getJoint(i)->x.value, xmin, xmax);
		ImGui::SliderFloat(yname.c_str(), &skeleton->getJoint(i)->y.value, ymin, ymax);
		ImGui::SliderFloat(zname.c_str(), &skeleton->getJoint(i)->z.value, zmin, zmax);
	}


	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

Skin::~Skin()
{
	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}