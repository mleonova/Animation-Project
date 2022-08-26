#ifndef _KETFRAME_H_
#define _KETFRAME_H_
#include <iostream>

class Keyframe {
public:
	float time;
	float value;
	float tangentIn, tangentOut;
	std::string ruleIn, ruleOut;
	float A, B, C, D;
};

#endif