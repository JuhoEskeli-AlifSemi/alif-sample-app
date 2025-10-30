cmake_minimum_required(VERSION 3.13.1)

set(EXTRA_CONF_FILE ../base.conf)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(hp_app)

zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_sources(app PRIVATE main.cpp )
