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

void Shape::Render(Renderer* renderer, bool isInstancing /*= false*/, uint32_t instanceCount /*= 1*/)
{
    m_graphicsComponent->Render(renderer, isInstancing, instanceCount);
}

void Shape::BakeImpostor(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_graphicsComponent->BakeImpostor(device, context);
}

GraphicsComponent* Shape::GetGraphicsComponent()
{
	return m_graphicsComponent;
}