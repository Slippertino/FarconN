cmake_minimum_required (VERSION 3.8)

project (FarconN)

set (CMAKE_CXX_STANDARD 17)

set(FCNN_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(FCNN_THIRDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)

add_definitions(-DBUILD_STATIC=true -DCPPCONN_PUBLIC_FUNC=)

link_directories(${FCNN_THIRDPARTY_DIR}/mysql-connector/lib64/vs14/debug)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${FCNN_THIRDPARTY_DIR}/nlohmann_json)

file(GLOB_RECURSE FCNN_SOURCES  "${FCNN_INCLUDE_DIR}/*.hpp" "${FCNN_INCLUDE_DIR}/*.cpp")

message(${FCNN_SOURCES})

add_executable(${PROJECT_NAME} main.cpp ${FCNN_SOURCES})

target_include_directories(
	${PROJECT_NAME} 
PRIVATE
	${FCNN_INCLUDE_DIR}/client 
	${FCNN_INCLUDE_DIR}/general		
	${FCNN_INCLUDE_DIR}/server
	${FCNN_THIRDPARTY_DIR}/mysql-connector/include
)

target_link_libraries(
	${PROJECT_NAME}
PRIVATE 
	nlohmann_json::nlohmann_json 
	mysqlcppconn-static
)