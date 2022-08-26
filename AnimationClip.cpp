#include "AnimationClip.h"
#include <iostream>

AnimationClip::AnimationClip(Skeleton* skel)
{
	skeleton = skel;
}

bool AnimationClip::load(const char* file) {

	Tokenizer token;
	bool openfile = token.Open(file);

	char temp[256];
	token.GetToken(temp);

	if (strcmp(temp, "animation") == 0) {
		token.SkipLine();
		token.GetToken(temp);

		start = token.GetFloat();
		end = token.GetFloat();

		token.SkipLine();
		token.GetToken(temp);

		if (strcmp(temp, "numchannels") == 0) {
			numChannels = token.GetFloat();
		}

		channel = new Channel;
		channel->load(token, channels, numChannels);
	}

	// Finish
	token.Close();
	return true;
}

void AnimationClip::precompute() {

	for (int i = 0; i < channels.size(); i++) {

		channels[i]->precompute();
	}
}

void AnimationClip::evaluate(float time) {

	skeleton->joints[0]->offset.x = channels[0]->evaluate(time);
	skeleton->joints[0]->offset.y = channels[1]->evaluate(time);
	skeleton->joints[0]->offset.z = channels[2]->evaluate(time);

	int j = 0;
	for (int i = 3; i < channels.size(); (i = i+3)) {
		skeleton->joints[j]->x.value = channels[i]->evaluate(time);
		skeleton->joints[j]->y.value = channels[i+1]->evaluate(time);
		skeleton->joints[j]->z.value = channels[i+2]->evaluate(time);
		j++;
	}
}
