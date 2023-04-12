#include "pch.h"
#include "Animation/Animation.h"

void Joint::Print()
{
	LOG("Name [" << Name << "] --- ID [" << NodeId << "]");
	if (Parent == nullptr)
	{
		LOG("\tParent [" << "NULL" << "]");
	}
	else
	{
		LOG("\tParent [" << Parent->Name << "] --- ID [" << Parent->NodeId << "]");
	}
}
