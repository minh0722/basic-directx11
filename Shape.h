#pragma once

#include "vector"

class Id3d11DeviceContext;
class GraphicsComponent;

class Shape
{
public:
	~Shape();

	void SetGraphicsComponent(GraphicsComponent* newComponent);
	void Render(ID3D11DeviceContext* context, bool isInstancing = false, uint32_t instanceCount = 1);
	void BakeImpostor();

    GraphicsComponent* GetGraphicsComponent();

private:
	GraphicsComponent* m_graphicsComponent = nullptr;
};