#pragma once

#include "BaseComponent.h"
#include "vector"

class Triangle
{
public:
	Triangle();
	~Triangle();

	void AddComponent(BaseComponent* newComponent);
	void Render(ID3D11DeviceContext* context);

private:
	std::vector<BaseComponent*> m_Components;
};