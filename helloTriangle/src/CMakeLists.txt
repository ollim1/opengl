cmake_minimum_required(VERSION 3.10)

# set the project name
project(GL_hello_triangle)

# add the executable
add_executable(main main.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl")
