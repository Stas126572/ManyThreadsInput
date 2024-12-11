add_executable(deadlock main.cpp)
set_target_properties(deadlock PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(
        deadlock
        PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(deadlock PRIVATE
                -g
                -Wall
                -Wextra
                -Werror
                -pedantic
                # only race-condition detection:
                -fsanitize=thread # only for debug and test builds!
        )
        target_link_options(deadlock PRIVATE
                -fsanitize=thread
        )
endif()
