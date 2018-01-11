#pragma once

#include "vector"

class BaseComponent;
class Id3d11DeviceContext;
class GraphicsComponent;

class Shape
{
public:
	Shape();
	~Shape();

	void AddComponent(BaseComponent* newComponent);
	void Render(ID3D11DeviceContext* context, bool isInstancing = false, size_t instanceCount = 1);

    GraphicsComponent* GetGraphicsComponent();

private:
	std::vector<BaseComponent*> m_Components;
};