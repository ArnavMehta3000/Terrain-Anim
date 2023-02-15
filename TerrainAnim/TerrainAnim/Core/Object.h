#pragma once
#include "Core/Structures.h"

// Interface for anything that can be updated and rendered to screen
class Object
{
public:
	virtual void Update(float dt, const InputEvent& input) = 0;
	virtual void Render() = 0;
};