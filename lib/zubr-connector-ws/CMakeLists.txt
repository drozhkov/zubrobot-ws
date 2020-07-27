cmake_minimum_required (VERSION 3.8)
project(zubr-connector-ws)


#
add_library(${PROJECT_NAME}
	src/ConnectorWs.cpp
	src/Request.cpp
	src/Response.cpp
	src/Digest.cpp
)
