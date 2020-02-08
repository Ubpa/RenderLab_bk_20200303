#项目名，默认为目录名
Ubpa_GetDirName(DIRNAME)
set(targetName ${DIRNAME})

#多个源文件用 ';' 分隔
#如：set(sources "main.cpp;src_2.cpp;help.h")
list(APPEND sources "[NO_USE].cpp")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Singleton.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Array2D.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/LStorage.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/GStorage.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Singleton.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/RandSet.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Ptr.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/HeapObj.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/TypeMap.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Error.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/FunctionTraits.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/KDTree.h")
list(APPEND sources "${CMAKE_SOURCE_DIR}/include/CppUtil/Basic/Node.h")

#多个库文件用 ';' 分隔，如果为空，则为 ""
#如：set(libs "targetName;xxx.lib")
set(libs "")

Ubpa_AddTarget(MODE ${mode} NAME ${targetName} SOURCES ${sources} LIBS ${libs})