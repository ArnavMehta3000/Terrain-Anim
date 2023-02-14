#include "pch.h"
#include "Direct3D.h"

std::shared_ptr<Direct3D> Direct3D::s_instance;

Direct3D::Direct3D() {LOG("Created D3D") }

Direct3D::~Direct3D() { LOG("Destructed D3D") }

std::shared_ptr<Direct3D> Direct3D::Get()
{
	if (s_instance == nullptr)
		s_instance = std::make_shared<Direct3D>();
	LOG("Got D3D");
	return std::shared_ptr<Direct3D>(s_instance);
}

bool Direct3D::Init(HWND window)
{
	return false;
}
