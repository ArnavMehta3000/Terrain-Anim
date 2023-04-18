#pragma once


struct Joint
{
	std::string        Name               = "";
	int                NodeId             = -1;
	Matrix             AnimatedTransform  = Matrix::Identity;
	Matrix             InvBindTransform   = Matrix::Identity;
	Matrix             LocalBindTransform = Matrix::Identity;
	std::vector<Joint> Children;

	void Print();
	void CalculateInverseBindTransform(Matrix parentTransform);

private:
	bool m_isCalculated = false;
};