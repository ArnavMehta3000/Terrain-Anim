#pragma once


struct Joint
{
	std::string Name;
	int NodeId;
	Joint* Parent;


	void Print()
	{
		LOG("Name [" << Name << "]        ID [" << NodeId << "]");
		if (Parent == nullptr)
		{
			LOG("\tParent [" << "NULL" << "]");
		}
		else
		{
			LOG("\tParent [" << Parent->Name << "]        ID [" << Parent->NodeId << "]");
		}
	}
};