cmake_minimum_required(VERSION 3.9)
project(raytracer)

set(CMAKE_CXX_STANDARD 11)

add_executable(raytracer main.cpp vec3.h ray.h hitable.h sphere.h hitable_list.h camera.h material.h perlin.h texture.h)