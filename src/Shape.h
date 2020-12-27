#pragma once

#include "vector"

class Id3d11DeviceContext;
class GraphicsComponent;
class Renderer;

class Shape
{
public:
	~Shape();

	void SetGraphicsComponent(GraphicsComponent* newComponent);
	void Render(Renderer* renderer, bool isInstancing = false, uint32_t instanceCount = 1);
	void BakeImpostor(ID3D11Device* device, ID3D11DeviceContext* context);

    GraphicsComponent* GetGraphicsComponent();

private:
	GraphicsComponent* m_graphicsComponent = nullptr;
};