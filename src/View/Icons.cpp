#include "View/Icons.h"
#include "Parser/stb_image.h"

#include "imgui.h"

#include <string>

GLTexture* Icons::m_Icons[IconName::ICON_COUNT];

void Icons::Init()
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

void Icons::DrawImage(IconName name, Vector2 inSize)
{
    GLTexture* icon = m_Icons[name];
    ImTextureID id  = (ImTextureID)(intptr_t)icon->GetTexture();

    ImVec2 size = ImVec2((float)icon->GetWidth(), (float)icon->GetHeight());
    if (inSize.x > 0.0f && inSize.y > 0.0f)
    {
        size.x = inSize.x;
        size.y = inSize.y;
    }

    ImVec2 uv0  = ImVec2(0.0f,  0.0f);
    ImVec2 uv1  = ImVec2(1.0f,  1.0f);
    ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    ImGui::PushID(name);
    ImGui::Image(id, size, uv0, uv1, tintCol, bgCol);
    ImGui::PopID();
}

bool Icons::DrawButton(IconName name, Vector2 inSize)
{
    GLTexture* icon = m_Icons[name];
    ImTextureID id  = (ImTextureID)(intptr_t)icon->GetTexture();

    ImVec2 size = ImVec2((float)icon->GetWidth(), (float)icon->GetHeight());
    if (inSize.x > 0.0f && inSize.y > 0.0f)
    {
        size.x = inSize.x;
        size.y = inSize.y;
    }

    ImVec2 uv0  = ImVec2(0.0f,  0.0f);
    ImVec2 uv1  = ImVec2(1.0f,  1.0f);
    ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    ImGui::PushID(name);
    bool result = ImGui::ImageButton(id, size, uv0, uv1, -1, bgCol, tintCol);
    ImGui::PopID();

    return result;
}
