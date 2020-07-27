cmake_minimum_required (VERSION 3.8)
project ("zubrobot-ws")


#
include_directories("lib/zubr-core/include")
include_directories("lib/zubr-connector-ws/include")


# sub-projects.
add_subdirectory ("lib/zubr-core")
add_subdirectory ("lib/zubr-connector-ws")

add_subdirectory ("src")
