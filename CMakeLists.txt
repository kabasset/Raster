CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

find_package(ElementsProject)

set(USE_SPHINX OFF
    CACHE STRING "Sphinx API documentation" 
    FORCE)

elements_project(
    Litl 1.0
    USE Elements 6.0.1
    DESCRIPTION "The light image template library")
