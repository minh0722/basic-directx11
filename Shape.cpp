#include "pch.h"
#include "Shape.h"
#include "BaseComponent.h"


Shape::Shape()
{
}

Shape::~Shape()
{
	for (auto& component : m_Components)
	{
		delete component;
	}
}

void Shape::AddComponent(BaseComponent* newComponent)
{
	m_Components.push_back(newComponent);
}

void Shape::Render(ID3D11DeviceContext* context)
{
	for (auto& component : m_Components)
	{
		component->Render(context);
	}
}
