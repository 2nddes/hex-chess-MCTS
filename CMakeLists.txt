cmake_minimum_required(VERSION 3.12)
project(MyProject)

# Add your source files here
set(SOURCES
    src/main.cpp
    src/MCTreeSearch.cpp
)

# Add your header files here
set(HEADERS
    inc/BoardState.h
    inc/MCTreeSearch.h
)

# Add any additional libraries or dependencies here
set(LIBRARIES
    # Add library names here
)

# Set the executable target
add_executable(MyProject ${SOURCES} ${HEADERS})

# Link any libraries or dependencies
target_link_libraries(MyProject ${LIBRARIES})