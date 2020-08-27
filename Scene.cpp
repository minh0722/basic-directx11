#include "Scene.h"
#include "GraphicsComponent.h"
#include "renderer.h"
#include "Matrix44f.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for (uint32_t i = 0; i < m_SceneObjects.size(); ++i)
    {
        delete m_SceneObjects[i];
    }
}

void Scene::Initialize()
{
    AddCube();
}

void Scene::AddCube()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> vertexShaderInputLayout(2);
    vertexShaderInputLayout[0].SemanticName = "POSITION";
    vertexShaderInputLayout[0].SemanticIndex = 0;								// will use POSITION0 semantic
    vertexShaderInputLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// format of the input vertex
    vertexShaderInputLayout[0].InputSlot = 0;									// 0 ~ 15
    vertexShaderInputLayout[0].AlignedByteOffset = 0;
    vertexShaderInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each triangle)
    vertexShaderInputLayout[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

    vertexShaderInputLayout[1].SemanticName = "COLOR";
    vertexShaderInputLayout[1].SemanticIndex = 0;
    vertexShaderInputLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    vertexShaderInputLayout[1].InputSlot = 0;
    vertexShaderInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    vertexShaderInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    vertexShaderInputLayout[1].InstanceDataStepRate = 0;

    GraphicsComponent::GraphicsComponentDesc desc =
    {
        g_Renderer.GetDevice(),
        L"vertexShader.cso",
        L"pixelShader.cso",
        vertexShaderInputLayout
    };

    ///////////////////////////////////////////////////////////////////////
    Vector4f red = { 1.0f, 0.0f, 0.0f, 0.0f };
    Vector4f green = { 0.0f, 1.0f, 0.0f, 0.0f };
    Vector4f blue = { 0.0f, 0.0f, 1.0f, 0.0f };

    Matrix44f worldViewProj;

    // left handed coordinate system. Same as directx
    std::vector<Vertex> vertices =
    {
        { { 0.0f, 0.0f, 0.0f, 1.0f }, blue },
        { { 1.0f, 0.0f, 0.0f, 1.0f }, red },
        { { 1.0f, 0.0f, 1.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 1.0f, 1.0f }, red },
        { { 0.0f, 1.0f, 0.0f, 1.0f }, green },
        { { 1.0f, 1.0f, 0.0f, 1.0f }, green },
        { { 1.0f, 1.0f, 1.0f, 1.0f }, blue },
        { { 0.0f, 1.0f, 1.0f, 1.0f }, green },

        { { 3.0f, 0.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 3.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 3.0f, 1.0f }, red }
    };

    // do transform
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(1.0f, 0.0f, 2.0f);

    Matrix44f worldMatrix = Matrix44f(translation /** rotation*/);

    //Matrix44f viewMatrix = camera.GetViewMatrix();
    static DirectX::XMVECTOR cameraPos = { 0.0f, 3.0f, 0.0f, 1.0f };
    static DirectX::XMVECTOR lookAtPos = { 1.0f, 0.0f, 2.0f, 1.0f };
    static float fov = 120.0f;

    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPos, lookAtPos, { 0.0f, 1.0f, 0.0f, 1.0f });

    worldViewProj = worldMatrix * viewMatrix;

    DirectX::XMMATRIX perspectiveProjMatrix = DirectX::XMMatrixPerspectiveFovLH((float)(fov * RADIAN), (float)screenWidth / (float)screenHeight, 0.0f, 100.0f);

    worldViewProj = worldViewProj * perspectiveProjMatrix;

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        // multiply by world view proj matrix and divide by w
        //DirectX::XMVECTOR pos = XMVector3TransformCoord(vertices[i].pos.m_xmValues, worldViewProj.m_matrix);

        Vector4f pos = (vertices[i].pos * worldViewProj);
        pos = pos / pos.w;

        vertices[i].pos = pos;
    }

    ///////////////////////////////////////////////////////////////////////

    Shape* cube = new Shape;

    GraphicsComponent* graphicComponent = new GraphicsComponent(desc);
    graphicComponent->SetPrimitiveTopology(g_Renderer.GetContext(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    graphicComponent->SetIndexBuffer(
        g_Renderer.GetDevice(),
        { 0, 1, 4, 1, 5, 4,
        1, 2, 5, 5, 2, 6,
        7, 4, 5, 7, 5, 6,
        3, 1, 0, 3, 2, 1,
        7, 6, 2, 7, 2, 3,
        7, 3, 4, 4, 3, 0 });

    graphicComponent->SetVertexBuffer(
        g_Renderer.GetDevice(),
        vertices
    );

    cube->SetGraphicsComponent(graphicComponent);

    m_SceneObjects.push_back(cube);
}