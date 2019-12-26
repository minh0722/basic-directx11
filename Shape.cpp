#include "pch.h"
#include "Shape.h"
#include "GraphicsComponent.h"

Shape::~Shape()
{
    delete m_graphicsComponent;
}

void Shape::SetGraphicsComponent(GraphicsComponent* newComponent)
{
    m_graphicsComponent = newComponent;
}

void Shape::Render(ID3D11DeviceContext* context, bool isInstancing /*= false*/, uint32_t instanceCount /*= 1*/)
{
    m_graphicsComponent->Render(context, isInstancing, instanceCount);
}

void Shape::BakeImpostor()
{
    
}

GraphicsComponent* Shape::GetGraphicsComponent()
{
	return m_graphicsComponent;
}