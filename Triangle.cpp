#include "Triangle.h"

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
	for (auto& component : m_Components)
	{
		delete component;
	}
}

void Triangle::AddComponent(BaseComponent* newComponent)
{
	m_Components.push_back(newComponent);
}

void Triangle::Render(ID3D11DeviceContext* context)
{
	for (auto& component : m_Components)
	{
		component->Render(context);
	}
}
