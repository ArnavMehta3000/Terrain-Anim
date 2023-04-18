#include "pch.h"
#include "Animation/Animation.h"

void Joint::Print()
{
	LOG("Name [" << Name << "]   ID [" << NodeId << "]   Children [" << Children.size() << "]");
	
	for (auto& child : Children)
	{
		LOG("----- Child Name [" << child.Name << "]   Child ID [" << child.NodeId << "]");
	}

	LOG("\n")
}

void Joint::CalculateInverseBindTransform(Matrix parentTransform)
{
	if (m_isCalculated)
	{
		LOG(Name << " already calculated...returning");
		return;
	}

	Matrix bindTransform = parentTransform * LocalBindTransform;
	for (auto& child : Children)
	{
		child.CalculateInverseBindTransform(InvBindTransform);
	}
	m_isCalculated = true;
}
