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
    View/UISceneView.h
    View/Scene3DView.h
)
set(VIEW_SRCS
    View/UISceneView.cpp
    View/Scene3DView.cpp
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
)
set(PARSER_SRCS
    Parser/stb_image_resize.cpp
    Parser/stb_image_write.cpp
    Parser/stb_image.cpp
    Parser/tiny_gltf.cpp
    Parser/GLTFParser.cpp
)

set(RENDERER_HDRS
    Renderer/PBRRenderer.h
    Renderer/RenderScene.h
    Renderer/RayTracingRenderer.h
)
set(RENDERER_SRCS
    Renderer/RenderScene.cpp
    Renderer/PBRRenderer.cpp
    Renderer/RayTracingRenderer.cpp
)

add_library(engine STATIC
    ${BASE_HDRS}
    ${BASE_SRCS}

    ${COMMON_HDRS}
    ${COMMON_SRCS}

    ${VIEW_HDRS}
    ${VIEW_SRCS}

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
)

source_group(src\\Base FILES ${BASE_HDRS} ${BASE_SRCS})
source_group(src\\Common FILES ${COMMON_HDRS} ${COMMON_SRCS})
source_group(src\\View FILES ${VIEW_HDRS} ${VIEW_SRCS})
source_group(src\\Misc FILES ${MISC_HDRS} ${MISC_SRCS})
source_group(src\\Math FILES ${MATH_SRCS} ${MATH_HDRS})
source_group(src\\Job FILES ${JOB_HDRS} ${JOB_SRCS})
source_group(src\\Bvh FILES ${BVH_HDRS} ${BVH_SRCS})
source_group(src\\Parser FILES ${PARSER_HDRS} ${PARSER_SRCS})
source_group(src\\Renderer FILES ${RENDERER_HDRS} ${RENDERER_SRCS})