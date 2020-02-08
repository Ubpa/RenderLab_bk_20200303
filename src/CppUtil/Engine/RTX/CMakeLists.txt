#项目名，默认为目录名
Ubpa_GetDirName(DIRNAME)
set(targetName ${DIRNAME})
#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
file(GLOB sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)




list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/RayTracer.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/RTX_Renderer.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/PathTracer.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BVHAccel.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/Ray.h")
#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
list(APPEND libs "Math;Intersector;Light;Material;Scene;Filter;Timer")

Ubpa_AddTarget(MODE ${mode} NAME ${targetName} SOURCES ${sources} LIBS ${libs})
