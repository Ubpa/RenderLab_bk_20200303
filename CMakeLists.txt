# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
cmake_policy(VERSION 3.10)

set(PROJECT_NAME "RenderLab")

# 设定项目
project (${PROJECT_NAME} VERSION 1.0)

# 修改默认的安装路径前缀
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "install prefix" FORCE)
endif()

# 在 Debug 模式下自动对 dll 和 lib 文件名后边加 d，但 exe 仍然需要手动设置
set(CMAKE_DEBUG_POSTFIX d)

# 确定 C++ 版本
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 将生成的 exe, dll, lib 等放置在 bin, lib 目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib")

# 开启 VS 的文件夹功能
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 添加自定义 CMake 模块 UbpaTool.cmake，内含一些工具函数
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(UbpaTool)

# Find the QtWidgets library
set(USE_QT_OPENGL_API ON)
find_package(Qt5 REQUIRED Widgets OpenGL)

configure_file(config/config.in ${CMAKE_SOURCE_DIR}/config/config.out)
install(FILES "${CMAKE_SOURCE_DIR}/config/config.out"
  DESTINATION config)
configure_file(config/ROOT_PATH.h.in config/ROOT_PATH.h)

include_directories (
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/include/3rdParty
	${CMAKE_BINARY_DIR}/config
	# $ENV{XXX_INCLUDE_PATH}
	# ...
)
link_directories (
	${PROJECT_SOURCE_DIR}/lib
	# $ENV{XXX_LIB_PATH}
	# ...
)

set(USE_DENOISE OFF CACHE BOOL OFF)
if(USE_DENOISE)
	add_definitions(-DUSE_DENOISE)

	set(OptiX_INSTALL_DIR "C:/ProgramData/NVIDIA Corporation/OptiX SDK 6.0.0" CACHE PATH "Path to OptiX installed location." FORCE)

	#set(CUDA_TOOLKIT_ROOT_DIR CACHE PATH "cuda root")
	find_package(CUDA 5.0 REQUIRED)

	set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
	
	include_directories (
		${OptiX_INSTALL_DIR}/include
		${CUDA_TOOLKIT_ROOT_DIR}/include
	)
	link_directories (
		${OptiX_INSTALL_DIR}/lib64
	)
endif()

FIND_PACKAGE(OpenMP REQUIRED)
if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_subdirectory (src)

include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set (CPACK_PACKAGE_CONTACT       "641614112@qq.com")
include (CPack)
