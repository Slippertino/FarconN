cmake_minimum_required (VERSION 3.8)

project (FarconN)

set(FCNN_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(FCNN_THIRDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${FCNN_THIRDPARTY_DIR}/nlohmann_json)

file(GLOB_RECURSE FCNN_SOURCES  "${FCNN_INCLUDE_DIR}/*.hpp" "${FCNN_INCLUDE_DIR}/*.cpp")

message(${FCNN_SOURCES})

add_executable(${PROJECT_NAME} main.cpp ${FCNN_SOURCES})

target_link_directories(
	${PROJECT_NAME} 
PRIVATE
	${FCNN_INCLUDE_DIR}/client 
	${FCNN_INCLUDE_DIR}/general		
	${FCNN_INCLUDE_DIR}/server
)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)