Ubpa_QtBegin()

#默认使用目录名作为项目名
Ubpa_GetDirName(DIRNAME)
set(targetName "${TARGET_PREFIX}${DIRNAME}")

#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
file(GLOB sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

#list(APPEND sources "${CMAKE_SOURCE_DIR}/config/config.in")

#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
list(APPEND libs "Qt5::Widgets;PaintImgOpCreator;RawAPI_OGLW;OpThread;RTX;Scene;Viewer")
list(APPEND libs "Hierarchy;Attribute;Setting;OptixAIDenoiser")

Ubpa_AddTarget(MODE ${mode} NAME ${targetName} SOURCES ${sources} LIBS ${libs})

Ubpa_QtEnd()
