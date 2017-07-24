#pragma once
#include "pch.h"
#include "Shape.h"

class Scene
{
public:
    Scene();
    ~Scene();

    void Initialize();

private:
    void AddCube();

private:
    std::vector<Shape*> m_SceneObjects;
};

