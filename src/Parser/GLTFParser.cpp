#include "Parser/GLTFParser.h"
#include "Parser/tiny_gltf.h"
#include "Misc/FileMisc.h"

#include <string>

LoadGLTFJob::LoadGLTFJob(const std::string& path)
    : m_Path(path)
    , m_Scene3D(nullptr)
{

}

LoadGLTFJob::~LoadGLTFJob()
{

}

void LoadGLTFJob::DoThreadedWork()
{
    bool result = false;
    std::string error;
    std::string warn;
    tinygltf::Model tinyModel;
    tinygltf::TinyGLTF tinyContext;
    std::string extension = GetFileExtension(m_Path);

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
