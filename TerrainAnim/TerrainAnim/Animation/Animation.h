#pragma once

struct AnimationChannel
{
	enum class PathType { TRANSLATION, ROTATION, SCALE, WEIGHT };
	
	PathType Path;
	int ParentNodeIndex;
	UINT SamplerIndex;
};

struct AnimationSampler
{
	enum class InterpolationType { LINEAR, STEP, CUBICSPLINE };
	
	InterpolationType Interpolation;
	std::vector<float> Inputs;
	std::vector<Vector4> Outputs;
};

struct Animation
{
	std::string Name;
	std::vector<AnimationSampler> Samplers;
	std::vector<AnimationChannel> Channels;
	float Start = std::numeric_limits<float>::max();
	float End = std::numeric_limits<float>::min();
};