cmake_minimum_required(VERSION 3.23)

project(Chip8)

find_package(SDL3 REQUIRED CONFIG)

add_executable(Chip8)

target_sources(Chip8
	PRIVATE
	 index.c	
	 graphics.c
	 chip8.c
)


target_link_libraries(Chip8
	PRIVATE
	 SDL3::SDL3

)

