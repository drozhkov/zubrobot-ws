cmake_minimum_required (VERSION 3.8)


#
##
find_package(Boost COMPONENTS system log log_setup)


#
##
set(LIBS
	zubr-connector-ws
	zubr-core
)

##
set(LIBS
	${LIBS}
	${Boost_LOG_LIBRARY} 
	${Boost_LOG_SETUP_LIBRARY} 
	${Boost_SYSTEM_LIBRARY}
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
add_executable (${PROJECT_NAME} 
	zubrobot-ws.cpp 
	conf.cpp 
	bot.cpp
)


#
target_link_libraries(${PROJECT_NAME} ${LIBS})
