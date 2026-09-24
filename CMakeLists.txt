cmake_minimum_required(VERSION 3.16)
project(nite3d_win16_reconstruction LANGUAGES CXX)

add_library(nite3d_win16_timing
    src/win16_timing_model.cpp
)
target_include_directories(nite3d_win16_timing PUBLIC include)
target_compile_features(nite3d_win16_timing PUBLIC cxx_std_17)

include(CTest)
if(BUILD_TESTING)
    add_executable(win16_timing_model_test
        tests/win16_timing_model_test.cpp
    )
    target_link_libraries(win16_timing_model_test PRIVATE nite3d_win16_timing)
    add_test(NAME win16_timing_model COMMAND win16_timing_model_test)
endif()
