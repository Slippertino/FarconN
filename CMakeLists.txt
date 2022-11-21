cmake_minimum_required (VERSION 3.8)

project (FarconN)

set (CMAKE_CXX_STANDARD 17)

set(FCNN_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(FCNN_THIRDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)

add_definitions(-DBUILD_STATIC=true -DCPPCONN_PUBLIC_FUNC=)

link_directories(${FCNN_THIRDPARTY_DIR}/mysql-connector/lib64/vs14/debug)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${FCNN_THIRDPARTY_DIR}/nlohmann_json)
add_subdirectory(${FCNN_THIRDPARTY_DIR}/cpp_events)


file(GLOB_RECURSE FCNN_SOURCES  "${FCNN_INCLUDE_DIR}/*.hpp" "${FCNN_INCLUDE_DIR}/*.cpp")

message(${FCNN_SOURCES})

macro(declare_exec EXEC_NAME EXEC_PATH)
	add_executable(${EXEC_NAME} ${EXEC_PATH} ${FCNN_SOURCES})

	target_include_directories(
		${EXEC_NAME}
	PRIVATE
		${FCNN_INCLUDE_DIR}/client 
		${FCNN_INCLUDE_DIR}/general		
		${FCNN_INCLUDE_DIR}/server
		${FCNN_THIRDPARTY_DIR}/mysql-connector/include
	)

	target_link_libraries(
		${EXEC_NAME}
	PRIVATE 
		nlohmann_json::nlohmann_json 
		mysqlcppconn-static
		cpp_events
	)
endmacro()

declare_exec(server "${CMAKE_CURRENT_LIST_DIR}/example/server.cpp")
declare_exec(client1 "${CMAKE_CURRENT_LIST_DIR}/example/client1.cpp")
declare_exec(client2 "${CMAKE_CURRENT_LIST_DIR}/example/client2.cpp")
declare_exec(client3 "${CMAKE_CURRENT_LIST_DIR}/example/client3.cpp")
declare_exec(${PROJECT_NAME} main.cpp)

#add_executable(server  "${CMAKE_CURRENT_LIST_DIR}/example/server.cpp" ${FCNN_SOURCES})
#add_executable(client1 "${CMAKE_CURRENT_LIST_DIR}/example/client1.cpp" ${FCNN_SOURCES})
#add_executable(client2 "${CMAKE_CURRENT_LIST_DIR}/example/client2.cpp" ${FCNN_SOURCES})
#add_executable(client3 "${CMAKE_CURRENT_LIST_DIR}/example/client3.cpp" ${FCNN_SOURCES})
#add_executable(${PROJECT_NAME} main.cpp ${FCNN_SOURCES})

#target_include_directories(
#	${PROJECT_NAME} server client1 client2 client3 
#PUBLIC
	##${FCNN_INCLUDE_DIR}/client 
	#${FCNN_INCLUDE_DIR}/general		
	#${FCNN_INCLUDE_DIR}/server
	#${FCNN_THIRDPARTY_DIR}/mysql-connector/include
#)

#target_link_libraries(
#	${PROJECT_NAME} server client1 client2 client3 
#PRIVATE 
#	nlohmann_json::nlohmann_json 
#	mysqlcppconn-static
#	cpp_events
#)

