# Include path
#include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Dependency libraries
#find_package(OpenCV REQUIRED)

## Find LCM
find_package(LCM REQUIRED)

if(LCM_NOT_FOUND)
  message(STATUS "LCM is not found. Inter-process communication depends on LCM.")
endif(LCM_NOT_FOUND)

# Include files for LCM
set(LCM_GENERATED_HEADER_PATH ${CMAKE_BINARY_DIR}/include)
include_directories(${LCM_GENERATED_HEADER_PATH})


# Add libraries
set(SAMPLING_LIB_SRC
	sample_node.cpp
	sample_space.cpp
	ltl_sampling_simple.cpp
	ltl_sampling_dubins.cpp
	cbba_sampling.cpp
	region.cpp
)
add_library(sampling STATIC ${SAMPLING_LIB_SRC})
target_link_libraries(sampling transsys trajectory)
#add_library(transsys STATIC ${GRAPH_LIB_SRC})
#target_link_libraries(transsys spot bddx)



