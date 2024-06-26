#指定cmake的最小版本号
CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

#指定项目名称
PROJECT(libqr LANGUAGES C)

IF (MSVC)
    ADD_COMPILE_OPTIONS(/utf-8)
	ADD_COMPILE_OPTIONS(/validate-charset)
ENDIF()

#添加预编译宏
ADD_DEFINITIONS(-DNO_FREETYPE)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
ADD_DEFINITIONS(-DHCPP_LEGACY_API)

#将各种文件归类到指定变量
FILE(GLOB_RECURSE HEADER_FILES "*.h")
FILE(GLOB_RECURSE SRC_FILES "*.cpp")
FILE(GLOB_RECURSE SRC_FILES "*.c")

#启用当前头文件目录
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

#查找当前目录下所有的.h和.cpp文件
SOURCE_GROUP("Header Files" FILES ${HEADER_FILES})
SOURCE_GROUP("Source Files" FILES ${SRC_FILES})

#引入库目录路径
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/x64)
#设置debug后缀名
set(CMAKE_DEBUG_POSTFIX "D") 

#将所有文件编译成EXE,并设置以WIN32的方式运行
ADD_LIBRARY(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SRC_FILES})

#设置DLL文件的输出目录
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/x64)

#链接以上lib库
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBRARIES})