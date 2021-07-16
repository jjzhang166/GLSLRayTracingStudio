set(BASE_HDRS
    Base/SceneView.h
    Base/GLWindow.h
)
set(BASE_SRCS
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

add_library(engine STATIC
    ${BASE_HDRS}
    ${BASE_SRCS}

    ${COMMON_HDRS}
    ${COMMON_SRCS}

    ${UI_HDRS}
    ${UI_SRCS}

    ${MISC_HDRS}
    ${MISC_SRCS}
)

source_group(src\\Base FILES ${BASE_HDRS} ${BASE_SRCS})
source_group(src\\Common FILES ${COMMON_HDRS} ${COMMON_SRCS})
source_group(src\\UI FILES ${UI_HDRS} ${UI_SRCS})
source_group(src\\Misc FILES ${MISC_HDRS} ${MISC_SRCS})
source_group(src\\Math FILES ${MATH_SRCS} ${MATH_HDRS})