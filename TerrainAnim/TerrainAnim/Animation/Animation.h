#pragma once


struct Joint
{
	std::string Name = "";
	int NodeId       = -1;
	Joint* Parent    = nullptr;

	void Print();
};

struct JointTransform
{

};