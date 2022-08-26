#ifndef _ANIMATIONCLIP_H_
#define _ANIMATIONCLIP_H_

#include <vector>
#include <iostream>
#include "Tokenizer.h"
#include "Channel.h"
#include "Keyframe.h"
#include "Skeleton.h"

class AnimationClip {
private:
	int numChannels;
	Channel* channel;
	std::vector<Channel*> channels;

public:
	Skeleton* skeleton;
	AnimationClip(Skeleton* skel);
	float start;
	float end;
	bool load(const char* file);
	void precompute();
	void evaluate(float time);
};

#endif