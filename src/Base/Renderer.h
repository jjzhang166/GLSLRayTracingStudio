#pragma once

#include "Core/Scene.h"

#include <memory>
#include <vector>

class Renderer
{
public:
    enum RendererType
    {
        PBR = 0,
        Raytracing = 1
    };

public:

    Renderer()
    {

    }

    virtual ~Renderer()
    {

    }

    virtual RendererType Type() = 0;

    virtual void Init() = 0;

    virtual void Destroy() = 0;

    virtual void Update() = 0;

    virtual void Render() = 0;

    virtual void SetScene(GLScenePtr scene) = 0;
};

typedef std::shared_ptr<Renderer> RendererPtr;
typedef std::vector<RendererPtr>  RendererArray;
