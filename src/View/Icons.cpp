#include "View/Icons.h"
#include "Parser/stb_image.h"

#include <string>

Icons::Icons()
{

}

Icons::~Icons()
{

}

void Icons::Load()
{
    std::string rootPath = "../assets/icons/";

    for (int32 i = 0; i < IconName::ICON_COUNT; ++i)
    {
        std::string iconPath = rootPath + std::to_string(i) + ".png";

        int32 w = 0;
        int32 h = 0;
        int32 c = 0;
        uint8* rgbaData = stbi_load(iconPath.c_str(), &w, &h, &c, 4);

        m_Icons[i] = new GLTexture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, w, h, 1, rgbaData);

        stbi_image_free(rgbaData);
    }
}

void Icons::Destroy()
{
    for (int32 i = 0; i < IconName::ICON_COUNT; ++i)
    {
        delete m_Icons[i];
        m_Icons[i] = nullptr;
    }
}
