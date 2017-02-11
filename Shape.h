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
	void Render(ID3D11DeviceContext* context);

    GraphicsComponent* GetGraphicsComponent();

private:
	std::vector<BaseComponent*> m_Components;
};