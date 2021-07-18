#include "Parser/GLTFParser.h"
#include "Parser/tiny_gltf.h"

#include <algorithm>
#include <cctype>
#include <string>

std::string FileExtension(const std::string& path)
{
    std::string extension;
    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos)
    {
        extension = path.substr(idx + 1);
    }

    std::transform(extension.begin(), extension.end(), extension.begin(),
        [] (unsigned char c)
        {
            return std::tolower(c);
        }
    );

    return extension;
}

JobLoadGLTF::JobLoadGLTF(const std::string& path)
    : m_Path(path)
    , m_Scene3D(nullptr)
    , m_ProgressTotal(1)
    , m_ProgressCount(0)
{

}

JobLoadGLTF::~JobLoadGLTF()
{

}

void JobLoadGLTF::DoThreadedWork()
{
    bool result = false;
    std::string error;
    std::string warn;
    tinygltf::Model tinyModel;
    tinygltf::TinyGLTF tinyContext;
    std::string extension = FileExtension(m_Path);

    if (extension == "gltf")
    {
        tinyContext.RemoveImageLoader();
        result = tinyContext.LoadASCIIFromFile(&tinyModel, &error, &warn, m_Path);
    }
    else
    {
        result = tinyContext.LoadBinaryFromFile(&tinyModel, &error, &warn, m_Path);
    }
}
