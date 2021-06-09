cmake_minimum_required(VERSION 3.19)
project(spo_lab3 C)

set(CMAKE_C_STANDARD 99)

add_executable(spo_lab3 main.c utils.c utils.h server.c server.h client.c client.h graphics.c graphics.h)
target_link_libraries(spo_lab3 PRIVATE)