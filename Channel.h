#ifndef _CHANNEL_H_
#define _CHANNEL_H_
#include "Keyframe.h"
#include "Tokenizer.h"
#include <vector>
#include <iostream>

class Channel {
private:
	std::string extrapolIn, extrapolOut;
	Keyframe* keyframe;
	
public:
	std::vector<Keyframe*> keyframes;
	bool load(Tokenizer& token, std::vector<Channel*>& channels, int numChannels);
	float evaluate(float time);
	void precompute();
	void computeTangents();
	void computeConstants();
	float linearTangent(Keyframe* frame1, Keyframe* frame2);
	float smoothTangent(Keyframe* frame1, Keyframe* frame2);
};

#endif