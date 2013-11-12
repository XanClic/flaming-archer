cmake_minimum_required(VERSION 2.8)
project(exercise1)

# Use shared libraries
set(BUILD_SHARED_LIBS 1)

# Set the default build type to be release
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()


# Needed cgv packages
find_package(cgv COMPONENTS gui render math
			 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../../../framework/cmake")

# Needed cgv libraries
find_package(cgv_gl)

# A viewer shall be created
find_package(cgv_viewer)

# Create the plugin
cgv_add_module(exercise1
	../../exercise1.cxx
	../../main.cxx
	../../exercise1.h
	../../obj_reader.h
	../../obj_reader.cxx
        ../../dake/particles.h
        ../../dake/particles.cxx
        ../../dake/texture.h
        ../../dake/texture.cxx
        ../../dake/vector.h
        ../../dake/matrix.h
        ../../dake/matrix.cxx)

# Set include directories
include_directories(
	../..
        ../../dake
	${cgv_INCLUDE_DIRS}
	${cgv_gl_INCLUDE_DIRS}
)

target_link_libraries(exercise1 ${cgv_LIBRARIES} ${cgv_gl_LIBRARIES})

# Set the viewer working directory to point at the
# the source files
cgv_set_viewer_workdir("../../")

# Add a viewer
cgv_add_viewer(launch-exercise1
	exercise1
	cg_fltk
	crg_stereo_view)


