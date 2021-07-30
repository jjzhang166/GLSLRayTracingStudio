set(BASE_HDRS
    Base/SceneView.h
    Base/GLWindow.h
    Base/Base.h
    Base/Renderer.h
)
set(BASE_SRCS
    Base/Base.cpp
    Base/SceneView.cpp
    Base/GLWindow.cpp
)

set(COMMON_HDRS
    Common/Assert.h
    Common/Common.h
    Common/Log.h
)
set(COMMON_SRCS
    Common/Log.cpp
)

set(MATH_HDRS
    Math/Axis.h
    Math/GenericPlatformMath.h
    Math/Math.h
    Math/Matrix4x4.h
    Math/Plane.h
    Math/PlatformMath.h
    Math/Quat.h
    Math/Vector2.h
    Math/Vector3.h
    Math/Vector4.h
    Math/WindowsPlatformMath.h
    Math/Bounds3D.h
    Math/Rectangle2D.h
    Math/WindowsPlatformAtomics.h
)
set(MATH_SRCS
    Math/GenericPlatformMath.cpp
    Math/Math.cpp
)

set(VIEW_HDRS
    View/Icons.h
    View/UISceneView.h
    View/Scene3DView.h
)
set(VIEW_SRCS
    View/Icons.cpp
    View/UISceneView.cpp
    View/Scene3DView.cpp
)

set(VIEW_COMPONENTS_HDRS
    View/Components/LogPanel.h
)
set(VIEW_COMPONENTS_SRCS
    View/Components/LogPanel.cpp
)

set(MISC_HDRS
    Misc/FileMisc.h
    Misc/WindowsMisc.h
    Misc/JobManager.h
)
set(MISC_SRCS
    Misc/FileMisc.cpp
    Misc/WindowsMisc.cpp
    Misc/JobManager.cpp
)

set(BVH_HDRS
    Bvh/Bvh.h
    Bvh/BvhTranslator.h
    Bvh/SplitBvh.h
)
set(BVH_SRCS
    Bvh/Bvh.cpp
    Bvh/BvhTranslator.cpp
    Bvh/SplitBvh.cpp
)

set(JOB_HDRS
    Job/Runnable.h
    Job/RunnableThread.h
    Job/TaskThread.h
    Job/TaskThreadPool.h
    Job/ThreadEvent.h
    Job/ThreadManager.h
    Job/ThreadTask.h
)
set(JOB_SRCS
    Job/RunnableThread.cpp
    Job/TaskThread.cpp
    Job/TaskThreadPool.cpp
    Job/ThreadEvent.cpp
    Job/ThreadManager.cpp
)

set(PARSER_HDRS
    Parser/stb_image_resize.h
    Parser/json.hpp
    Parser/stb_image_write.h
    Parser/stb_image.h
    Parser/tiny_gltf.h
    Parser/GLTFParser.h
    Parser/HDRParser.h
)
set(PARSER_SRCS
    Parser/stb_image_resize.cpp
    Parser/stb_image_write.cpp
    Parser/stb_image.cpp
    Parser/tiny_gltf.cpp
    Parser/GLTFParser.cpp
    Parser/HDRParser.cpp
)

set(RENDERER_HDRS
    Renderer/PBRRenderer.h
    Renderer/RayTracingRenderer.h
)
set(RENDERER_SRCS
    Renderer/PBRRenderer.cpp
    Renderer/RayTracingRenderer.cpp
)

set(CORE_HDRS
    Core/Program.h
    Core/Quad.h
    Core/Scene.h
    Core/Shader.h
    Core/Texture.h
)
set(CORE_SRCS
    Core/Program.cpp
    Core/Quad.cpp
    Core/Scene.cpp
    Core/Shader.cpp
    Core/Texture.cpp
)

add_library(engine STATIC
    ${BASE_HDRS}
    ${BASE_SRCS}

    ${COMMON_HDRS}
    ${COMMON_SRCS}

    ${VIEW_HDRS}
    ${VIEW_SRCS}
    ${VIEW_COMPONENTS_HDRS}
    ${VIEW_COMPONENTS_SRCS}

    ${MISC_HDRS}
    ${MISC_SRCS}

    ${MATH_HDRS}
    ${MATH_SRCS}

    ${JOB_HDRS}
    ${JOB_SRCS}

    ${BVH_HDRS}
    ${BVH_SRCS}

    ${PARSER_HDRS}
    ${PARSER_SRCS}

    ${RENDERER_HDRS}
    ${RENDERER_SRCS}

    ${CORE_HDRS}
    ${CORE_SRCS}
)

source_group(src\\Base FILES ${BASE_HDRS} ${BASE_SRCS})
source_group(src\\Common FILES ${COMMON_HDRS} ${COMMON_SRCS})
source_group(src\\View FILES ${VIEW_HDRS} ${VIEW_SRCS})
source_group(src\\View\\Components FILES ${VIEW_COMPONENTS_HDRS} ${VIEW_COMPONENTS_SRCS})
source_group(src\\Misc FILES ${MISC_HDRS} ${MISC_SRCS})
source_group(src\\Math FILES ${MATH_SRCS} ${MATH_HDRS})
source_group(src\\Job FILES ${JOB_HDRS} ${JOB_SRCS})
source_group(src\\Bvh FILES ${BVH_HDRS} ${BVH_SRCS})
source_group(src\\Parser FILES ${PARSER_HDRS} ${PARSER_SRCS})
source_group(src\\Renderer FILES ${RENDERER_HDRS} ${RENDERER_SRCS})
source_group(src\\Core FILES ${CORE_HDRS} ${CORE_SRCS})