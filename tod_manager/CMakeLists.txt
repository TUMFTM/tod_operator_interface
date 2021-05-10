cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME tod_manager)
project(${PROJECT_NAME})

find_package(catkin REQUIRED COMPONENTS roscpp roslib tod_msgs tod_network tod_helper_functions)
find_package(PahoMqttCpp REQUIRED)
find_package(Threads)

catkin_package(
    CATKIN_DEPENDS roscpp roslib tod_msgs tod_network tod_helper_functions)

option(OPERATOR "Build operator targets" ON)
if (${OPERATOR})
    add_subdirectory(src/operator)
else()
    message("skipping ${PROJECT_NAME} operator targets")
endif()

option(VEHICLE "Build vehicle targets" ON)
if (${VEHICLE})
    add_subdirectory(src/vehicle)
else()
    message("skipping ${PROJECT_NAME} vehicle targets")
endif()

install(DIRECTORY 
    launch
    yaml
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
