#项目名，默认为目录名
Ubpa_GetDirName(DIRNAME)
set(targetName ${DIRNAME})
#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
file(GLOB sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)




list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/Light.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/AreaLight.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/PointLight.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/SphereLight.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/DirectionalLight.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/SpotLight.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/InfiniteAreaLight.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/DiskLight.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/CapsuleLight.h")

#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
list(APPEND libs "primitive")

Ubpa_AddTarget(MODE ${mode} NAME ${targetName} SOURCES ${sources} LIBS ${libs})
