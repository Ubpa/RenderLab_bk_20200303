#项目名，默认为目录名
Ubpa_GetDirName(DIRNAME)
set(targetName ${DIRNAME})
#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
file(GLOB sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)




list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/Material.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/SurfCoord.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/AllBSDFs.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_Diffuse.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_Mirror.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_Glass.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_Emission.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_CookTorrance.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_MetalWorkflow.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_FrostedGlass.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/BSDF_Frostbite.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/MicrofacetDistribution.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/Beckmann.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/GGX.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/SchlickGGX.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/Gooch.h")

#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
list(APPEND libs "Math;Sampler;Image")

Ubpa_AddTarget(MODE ${mode} NAME ${targetName} SOURCES ${sources} LIBS ${libs})
