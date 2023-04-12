#pragma once

namespace tinygltf
{
	class Node;
}

struct Joint
{
	std::string Name;
	tinygltf::Node JointNode;
	Joint* Parent;
};