set(BASE_HDRS
    Base/SceneView.h
    Base/GLWindow.h
    Base/Mesh.h
)
set(BASE_SRCS
    Base/SceneView.cpp
    Base/GLWindow.cpp
    Base/Mesh.cpp
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
)
set(MATH_SRCS
	Math/GenericPlatformMath.cpp
	Math/Math.cpp
)

set(UI_HDRS
    UI/UISceneView.h
)
set(UI_SRCS
    UI/UISceneView.cpp    
)

set(MISC_HDRS
    Misc/WindowsMisc.h
)
set(MISC_SRCS
    Misc/WindowsMisc.cpp
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

add_library(engine STATIC
    ${BASE_HDRS}
    ${BASE_SRCS}

    ${COMMON_HDRS}
    ${COMMON_SRCS}

    ${UI_HDRS}
    ${UI_SRCS}

    ${MISC_HDRS}
    ${MISC_SRCS}

    ${MATH_HDRS}
    ${MATH_SRCS}

    ${JOB_HDRS}
    ${JOB_SRCS}

    ${BVH_HDRS}
    ${BVH_SRCS}
)

source_group(src\\Base FILES ${BASE_HDRS} ${BASE_SRCS})
source_group(src\\Common FILES ${COMMON_HDRS} ${COMMON_SRCS})
source_group(src\\UI FILES ${UI_HDRS} ${UI_SRCS})
source_group(src\\Misc FILES ${MISC_HDRS} ${MISC_SRCS})
source_group(src\\Math FILES ${MATH_SRCS} ${MATH_HDRS})
source_group(src\\Job FILES ${JOB_HDRS} ${JOB_SRCS})
source_group(src\\Bvh FILES ${BVH_HDRS} ${BVH_SRCS})