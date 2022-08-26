#include <iostream>
#include "Vertex.h"

Vertex::Vertex(Skeleton* skel)
{
	skeleton = skel;
}

bool Vertex::load(const char* file) {
	Tokenizer token;
	bool openfile = token.Open(file);

	while (1) {
		char temp[256];
		token.GetToken(temp);

		if (strcmp(temp, "positions") == 0) {
			int n = token.GetInt();
			//std::cout << "n : " << n << std::endl;

			token.SkipLine();

			for (int i = 0; i < n; i++) {

				float x = token.GetFloat();
				float y = token.GetFloat();
				float z = token.GetFloat();

				glm::vec3 pos = glm::vec3(x, y, z);
				bindPositions.push_back(pos);
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

			glm::vec4 vec;

			token.SkipLine();

			for (int i = 0; i < n; i++) {

				char temp[256];
				token.GetToken(temp);

				glm::mat4 binding;
				glm::mat4 normbinding;
				glm::vec4 vec1, vec2;

				if (strcmp(temp, "matrix") == 0) {

					token.SkipLine();

					for (int row = 0; row < 4; row++) {

						for (int col = 0; col < 3; col++) {
							float index = token.GetFloat();
							binding[col][row] = index;
							normbinding[col][row] = index;
						}
					}

					vec1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
					binding[3] = vec1;
					bindings.push_back(binding);

					vec2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
					normbinding[3] = vec2;
					normBindings.push_back(normbinding);

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

void Vertex::update() {

	for (int i = 0; i < bindPositions.size(); i++) {
		glm::vec4 vertex = glm::vec4(bindPositions[i], 1.0f);
		glm::vec4 normal = glm::vec4(bindNormals[i], 1.0f);

		glm::vec4 newVertex = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 newNormal = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int j = 0; j < sweights[i].size(); j++) {
			int joint = sweights[i][j].first;
			float weight = sweights[i][j].second;

			glm::mat4 worldMatrix = skeleton->getWorldMatrix(joint);
			glm::mat4 binding = bindings[joint];

			glm::mat4 normBinding = normBindings[joint];

			newVertex += weight * worldMatrix * glm::inverse(binding) * vertex;
			newNormal += weight * worldMatrix * glm::inverse(binding) * normal;

			positions.push_back(newVertex);
			normals.push_back(glm::normalize(newNormal));
		}
	}
}

std::vector<unsigned int> Vertex::getIndices()
{
	return indices;
}

std::vector<glm::vec4> Vertex::getNormals()
{
	return normals;
}

std::vector<glm::vec4> Vertex::getPositions()
{
	return positions;
}
