#项目名，默认为目录名
Ubpa_GetDirName(DIRNAME)
set(targetName ${DIRNAME})
#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
file(GLOB sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/SObj.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/SObj.inl")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/Scene.h")

list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/Component.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/CmptTransform.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/CmptGeometry.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/CmptCamera.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/CmptMaterial.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Engine/CmptLight.h")

#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
set(libsG "Light;Primitive;Material;tinyxml2;StrAPI")
set(libsD "3rdParty/assimp-vc140-mtd;3rdParty/zlibstaticd;3rdParty/IrrXMLd")
set(libsR "3rdParty/assimp-vc140-mt;3rdParty/zlibstatic;3rdParty/IrrXML")

Ubpa_AddTarget_GDR(MODE ${mode} NAME ${targetName} SOURCES ${sources}
	LIBS_GENERAL ${libsG}
	LIBS_DEBUG ${libsD}
	LIBS_RELEASE ${libsR})
