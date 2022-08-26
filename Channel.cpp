#include "Channel.h"

bool Channel::load(Tokenizer& token, std::vector<Channel*>& channels, int numChannels)
{

	for (int i = 0; i < numChannels; i++) {
		Channel* channel = new Channel;

		token.FindToken("extrapolate");

		for (int j = 0; j < 2; j++) {
			token.SkipWhitespace();

			char c = token.GetChar();
			std::string str = "";

			while (c != ' ') {
				str += c;
				c = token.GetChar();
				if (str == "constant") {
					str == "constant";
					break;
				}
				else if (str == "cycle_offset") {
					str == "cycle_offset";
					break;
				}
				else if (str == "cycle" && c != '_') {
					str == "cycle";
					break;
				}
				else if (str == "bounce") {
					str = "bounce";
					break;
				}
			}

			if (j == 0) {
				channel->extrapolIn = str;
			}
			else {
				channel->extrapolOut = str;
			}
		}

		token.FindToken("keys");
		int n = token.GetInt();
		token.SkipLine();

		for (int k = 0; k < n; k++) {
			Keyframe* keyframe = new Keyframe;
			keyframe->time = token.GetFloat();

			keyframe->value = token.GetFloat();

			for (int j = 0; j < 2; j++) {

				token.SkipWhitespace();
				char c = token.GetChar();
				std::string str = "";

				while (c != ' ') {
					str += c;
					if (str == "smooth") {
						str = "smooth";
						break;
					}
					else if (str == "linear") {
						str = "linear";
						break;
					}
					else if (str == "flat") {
						str = "flat";
						break;
					}

					c = token.GetChar();
				}

				if (j == 0) {
					keyframe->ruleIn = str;
				}
				else {
					keyframe->ruleOut = str;
				}
			}

			channel->keyframes.push_back(keyframe);
		}

		channels.push_back(channel);
	}

	return true;
}

void Channel::precompute() {
	computeTangents();
	computeConstants();
}

void Channel::computeTangents() {

	for (int i = 0; i < keyframes.size(); i++) {

		if (keyframes[i]->ruleIn == "flat") {
			keyframes[i]->tangentIn = 0.0;
		}

		if (keyframes[i]->ruleOut == "flat") {
			keyframes[i]->tangentOut = 0.0;
		}

		if (keyframes[i]->ruleIn == "linear") {

			if (keyframes.size() == 1) {
				keyframes[i]->tangentIn = 0.0;
			}
			else {
				if (i == 0) {
					keyframes[i]->tangentIn = 0.0;
				}
				else {

					keyframes[i]->tangentIn = linearTangent(keyframes[i-1], keyframes[i]);
				}
			}
			
		}

		if (keyframes[i]->ruleOut == "linear") {

			if (keyframes.size() < 2) {
				keyframes[i]->tangentOut = 0.0;
			}
			else {
				if (i == keyframes.size()-1) {
					keyframes[i]->tangentOut = 0.0;
				}
				else {
					keyframes[i]->tangentOut = linearTangent(keyframes[i], keyframes[i + 1]);
				}
			}
		}

		if (keyframes[i]->ruleIn == "smooth") {

			if (keyframes.size() == 1) {
				keyframes[i]->tangentIn = 0.0;

			}
			else {
				if (i == 0) {
					keyframes[i]->tangentIn = 0.0;
				}
				else if (i > 0 && i < keyframes.size() - 1) {
					keyframes[i]->tangentIn = smoothTangent(keyframes[i - 1], keyframes[i + 1]);
				}
				else {
					keyframes[i]->tangentIn = linearTangent(keyframes[i-1], keyframes[i]);
				}
			}
		}

		if (keyframes[i]->ruleOut == "smooth") {

			if (keyframes.size() == 1) {
				keyframes[i]->tangentOut = 0.0;
			}
			else {
				if (i == 0) {
					keyframes[i]->tangentOut = linearTangent(keyframes[i], keyframes[i + 1]);
				}
				else if (i > 0 && i < keyframes.size() - 1) {
					keyframes[i]->tangentOut = smoothTangent(keyframes[i - 1], keyframes[i + 1]);
				}
				else {
					keyframes[keyframes.size() - 1]->tangentOut = 0.0;
				}
			}
		}
	}
}

float Channel::linearTangent(Keyframe* frame1, Keyframe* frame2) {
	float tangent = (frame2->value - frame1->value) / (frame2->time - frame1->time);
	return tangent;
}

float Channel::smoothTangent(Keyframe* frame1, Keyframe* frame2) {
	float tangent = (frame2->value - frame1->value) / (frame2->time - frame1->time);
	return tangent;
}

void Channel::computeConstants() {

	for (int i = 0; i < keyframes.size(); i++) {
		if (i < keyframes.size()-1) {
			glm::vec4 coeff = glm::vec4(0.0, 0.0, 0.0, 0.0);
			glm::mat4 mat = glm::mat4(2,-3, 0, 1, -2, 3, 0, 0, 1, -2, 1, 0, 1, -1, 0, 0);
			glm::vec4 temp = glm::vec4(keyframes[i]->value, 
									  keyframes[i+1]->value, 
									  (keyframes[i+1]->time - keyframes[i]->time) * keyframes[i]->tangentOut,
									  (keyframes[i + 1]->time - keyframes[i]->time) * keyframes[i+1]->tangentIn);
		
			coeff = mat * temp;
			keyframes[i]->A = coeff[0];
			keyframes[i]->B = coeff[1];
			keyframes[i]->C = coeff[2];
			keyframes[i]->D = coeff[3];
		}
		else {
			keyframes[i]->A = 0.0;
			keyframes[i]->B = 0.0;
			keyframes[i]->C = 0.0;
			keyframes[i]->D = 0.0;
		}
	}
}

float Channel::evaluate(float time) {

	if (keyframes.size() == 0) {
		return 0;
	}
	else if (keyframes.size() == 1) {
		return keyframes[0]->value;
	}
	
	for (int i = 0; i < keyframes.size(); i++) {

		if (time == keyframes[i]->time) {
			// time falls exactly on some key --> return key value
			return keyframes[i]->value;
		}
		
		if (i < keyframes.size() - 1) {
			if (keyframes[i]->time < time && time < keyframes[i + 1]->time) {

				float u = (time - keyframes[i]->time) / (keyframes[i + 1]->time - keyframes[i]->time);
				float x = keyframes[i]->D + u * (keyframes[i]->C + u * (keyframes[i]->B + u * (keyframes[i]->A)));
				return x;
			}
		}
	}

	if (time < keyframes[0]->time) {
		if (extrapolIn == "constant") {
			return keyframes[0]->value;
		}
		else if (extrapolIn == "cycle_offset") {
			float temp = floor((keyframes[keyframes.size() - 1]->time - time) / (keyframes[keyframes.size() - 1]->time - keyframes[0]->time));
			return -temp * (keyframes[keyframes.size() - 1]->value) + (evaluate(time + (keyframes[keyframes.size() - 1]->time - keyframes[0]->time) * temp) - keyframes[0]->value);
		}
		else if (extrapolIn == "cycle") {

			float temp = floor((keyframes[keyframes.size()-1]->time - time) / (keyframes[keyframes.size()-1]->time - keyframes[0]->time));
			return  evaluate(time + (keyframes[keyframes.size()-1]->time - keyframes[0]->time) * temp);
		}
	}

	if (time > keyframes[keyframes.size()-1]->time) {
		if (extrapolOut == "constant") {
			return keyframes[keyframes.size() - 1]->value;
		}
		else if (extrapolOut == "cycle_offset") {
			float temp = floor((time - keyframes[0]->time) / (keyframes[keyframes.size()-1]->time - keyframes[0]->time));
			return temp * (keyframes.back()->value) + (evaluate(time - (keyframes[keyframes.size()-1]->time - keyframes[0]->time) * temp) - keyframes[0]->value);
		}
		else if (extrapolOut == "cycle") {
			float temp = floor((time - keyframes[0]->time) / (keyframes[keyframes.size() -1]->time - keyframes[0]->time));
			return  evaluate(time - (keyframes[keyframes.size()-1]->time - keyframes[0]->time) * temp);
		}
	}
}

