cmake_minimum_required (VERSION 3.8)


#
##
find_package(Boost COMPONENTS system)


#
##
set(LIBS
	zubr-connector-ws
	zubr-core
)

##
set(LIBS
	${LIBS}
)

##
set(LIBS
	${LIBS}
	ssl
	crypto
)

##
set(LIBS
	${LIBS}
	pthread
)


# 
add_executable (${PROJECT_NAME} "zubrobot-ws.cpp")


#
target_link_libraries(${PROJECT_NAME} ${LIBS} ${Boost_SYSTEM_LIBRARY})
