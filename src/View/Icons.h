#pragma once

#include "Common/Common.h"
#include "Core/Texture.h"
#include "Math/Vector2.h"

#include <glad/glad.h>

enum IconName
{
    ICON_CLOSE      = 0,
    ICON_VISIBLE    = 4,
    ICON_NODE       = 31,
    ICON_LIGHT      = 34,
    ICON_MESH       = 46,
    ICON_SCENE      = 69,
    ICON_CAMERA     = 69,
    ICON_RECT       = 73,
    ICON_INVISIBLE  = 97,
    ICON_COUNT      = 108
};

class Icons
{
public:

    static void Init();

    static void Destroy();

    static void DrawImage(IconName name, Vector2 size = Vector2(0.0f, 0.0f));

    static bool DrawButton(IconName name, Vector2 size = Vector2(0.0f, 0.0f));

protected:

     static GLTexture* m_Icons[IconName::ICON_COUNT];
};

