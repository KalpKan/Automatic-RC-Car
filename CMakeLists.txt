cmake_minimum_required(VERSION 3.10)
project(PS4JoystickTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(WIN32)
    # Windows with XInput
    add_executable(ps4_joystick_test ps4_joystick_test.cpp)
    target_link_libraries(ps4_joystick_test XInput)
    
    # Integrated PS4 controller test (Windows version)
    add_executable(ps4_controller_integrated ps4_controller_integrated.cpp)
    target_link_libraries(ps4_controller_integrated XInput)
else()
    # Unix/Linux/macOS with SDL2
    find_package(SDL2 REQUIRED)
    add_executable(ps4_joystick_test ps4_joystick_test.cpp)
    target_link_libraries(ps4_joystick_test SDL2::SDL2)
    
    # Integrated PS4 controller test (Linux/Raspberry Pi version)
    add_executable(ps4_controller_integrated ps4_controller_integrated.cpp)
    target_link_libraries(ps4_controller_integrated SDL2::SDL2 pthread)
endif()

# Compiler-specific flags
if(MSVC)
    target_compile_options(ps4_joystick_test PRIVATE /W4)
else()
    target_compile_options(ps4_joystick_test PRIVATE -Wall -Wextra -Wpedantic)
endif() 