#默认使用目录名作为项目名
Ubpa_GetDirName(DIRNAME)
set(targetName "${TARGET_PREFIX}${DIRNAME}")

#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
file(GLOB sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.vs"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.fs"
)

list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Qt/${DIRNAME}.h")

#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
list(APPEND libs "EventManager;File;Operation;Shape;Camera;FBO;Shader;Texture;VAO;RawAPI_OGLW")

Ubpa_AddTarget(MODE ${mode} NAME ${targetName} SOURCES ${sources} LIBS ${libs})
