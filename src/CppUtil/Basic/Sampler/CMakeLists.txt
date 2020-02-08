#项目名，默认为目录名
Ubpa_GetDirName(DIRNAME)
set(targetName ${DIRNAME})
#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
file(GLOB sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)




list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Sampler2D.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Sampler3D.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/UniformGridSampler2D.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/CosineWeightedHemisphereSampler3D.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/AliasMethod.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/BasicSampler.h")
#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
list(APPEND libs "Math")

Ubpa_AddTarget(MODE ${mode} NAME ${targetName} SOURCES ${sources} LIBS ${libs})