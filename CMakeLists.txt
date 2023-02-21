cmake_minimum_required(VERSION 3.23)
project(wxHelloWorld VERSION 0.1)

# C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 20)
	set(CMAKE_CXX_STANDARD 20)
endif()

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(wxHelloWorld
  MACOSX_BUNDLE
  HelloWorld.cpp
)

target_compile_options(wxHelloWorld PRIVATE -Wall -Wextra)
set_property(TARGET wxHelloWorld PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_link_libraries(wxHelloWorld 
  ${wxWidgets_LIBRARIES}
)


