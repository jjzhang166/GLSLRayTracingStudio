#pragma once

#include "Common/Common.h"
#include "Core/Texture.h"

#include <glad/glad.h>

enum IconName
{
    ICON_VISIBLE    = 04,
    ICON_INVISIBLE  = 97,
    ICON_LIGHT      = 34,
    ICON_CAMERA     = 69,
    ICON_NODE       = 86,
    ICON_MESH       = 46,
    ICON_SCENE      = 69,
    ICON_COUNT      = 108
};

class Icons
{
public:

    Icons();

    virtual ~Icons();

    void Load();

    void Destroy();

    FORCEINLINE GLTexture* GetIcon(IconName name)
    {
        return m_Icons[name];
    }

protected:


    GLTexture* m_Icons[IconName::ICON_COUNT];

};
