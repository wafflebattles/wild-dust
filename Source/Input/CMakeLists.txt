file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)
add_pch(Precompiled.h Precompiled.cpp ${SOURCES})
add_library(Input SHARED ${SOURCES} ${HEADERS})
add_definitions(-DBUILDING_INPUT)
include_directories(${STB_INCLUDEDIR})
target_link_libraries(Input ${SDL2_LIBS} Core)