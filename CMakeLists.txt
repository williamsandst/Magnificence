###########################################
### CMake Setup for magnificence-chess  ###
###########################################
# Includes a macro for improving IDE compatability, mainly focused on Windows Visual Studios"
# Only tested on Windows 10

# Project setup
cmake_minimum_required(VERSION 3.10)

project(magnificence-chess
    LANGUAGES C CXX
    VERSION 1.0.0
    DESCRIPTION "UCI C++ Chess Engine"
)

# Flags to support the compiler intrinsics needed for MoveGen
set(CMAKE_CXX_FLAGS "-mbmi2 -pthread -g")

### Add source files
# Add source files recursively from /src
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp)
	
# Add header files recursively from /src
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)
   
# Set up executable with src files
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES})
    
    
    
####################################################################################
### Macros for improving IDEs and such, should not be modified per project basis ###
####################################################################################
    
# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
    # Set working dir to be next to the exe
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    # Set the correct project as Startup
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()