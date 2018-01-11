#include "pch.h"
#include "Shape.h"
#include "BaseComponent.h"
#include "GraphicsComponent.h"

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

void Shape::Render(ID3D11DeviceContext* context, bool isInstancing /*= false*/, uint32_t instanceCount /*= 1*/)
{
	for (auto& component : m_Components)
	{
		component->Render(context, isInstancing, instanceCount);
	}
}

GraphicsComponent* Shape::GetGraphicsComponent()
{
    GraphicsComponent* graphic = nullptr;
    for (size_t i = 0; i < m_Components.size(); ++i)
    {
        graphic = dynamic_cast<GraphicsComponent*>(m_Components[i]);
        if (graphic)
        {
            return graphic;
        }
    }
    return graphic;
}