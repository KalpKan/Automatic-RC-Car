#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <memory>
#include <string>
#include <cmath>
#include <iomanip>

#ifdef _WIN32
    #include <windows.h>
    #include <XInput.h>
    #pragma comment(lib, "XInput.lib")
#else
    #include <SDL2/SDL.h>
#endif

class JoystickController {
private:
#ifdef _WIN32
    // Windows XInput implementation
    bool initialized_;
    int controllerIndex_;
#else
    // SDL2 implementation for cross-platform
    SDL_Joystick* joystick_;
    bool initialized_;
#endif

    static constexpr float DEADZONE_THRESHOLD = 0.1f;
    static constexpr int UPDATE_RATE_MS = 100;

public:
    JoystickController() : initialized_(false) {
#ifdef _WIN32
        controllerIndex_ = 0;
#else
        joystick_ = nullptr;
#endif
    }

    ~JoystickController() {
        cleanup();
    }

    bool initialize() {
        try {
#ifdef _WIN32
            // Windows XInput - no explicit initialization needed
            initialized_ = true;
            std::cout << "XInput controller system initialized" << std::endl;
#else
            // SDL2 initialization
            if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
                throw std::runtime_error("SDL initialization failed: " + std::string(SDL_GetError()));
            }

            int numJoysticks = SDL_NumJoysticks();
            if (numJoysticks == 0) {
                throw std::runtime_error("No joysticks detected");
            }

            joystick_ = SDL_JoystickOpen(0);
            if (!joystick_) {
                throw std::runtime_error("Failed to open joystick: " + std::string(SDL_GetError()));
            }

            initialized_ = true;
            std::cout << "Controller: " << SDL_JoystickName(joystick_) << std::endl;
#endif
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            return false;
        }
    }

    void cleanup() {
        if (initialized_) {
#ifdef _WIN32
            // XInput doesn't require cleanup
#else
            if (joystick_) {
                SDL_JoystickClose(joystick_);
                joystick_ = nullptr;
            }
            SDL_Quit();
#endif
            initialized_ = false;
        }
    }

    float applyDeadzone(float value) const {
        return (std::abs(value) < DEADZONE_THRESHOLD) ? 0.0f : value;
    }

    bool getControllerState(float& throttle, float& steering) {
        if (!initialized_) {
            std::cerr << "Controller not initialized" << std::endl;
            return false;
        }

        try {
#ifdef _WIN32
            // Windows XInput implementation
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            
            DWORD result = XInputGetState(controllerIndex_, &state);
            if (result != ERROR_SUCCESS) {
                std::cerr << "Failed to get controller state" << std::endl;
                return false;
            }

            // Left stick Y axis (throttle) - inverted for intuitive control
            throttle = -applyDeadzone(static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f);
            
            // Right stick X axis (steering)
            float steering2 = applyDeadzone(static_cast<float>(state.Gamepad.sThumbRX) / 32767.0f);
            float steering3 = applyDeadzone(static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f);
            
            // Use right stick if active, otherwise left stick
            steering = (std::abs(steering3) > 0.0f) ? steering3 : steering2;

#else
            // SDL2 implementation
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                // Handle SDL events if needed
            }

            // Get axis values
            float leftY = static_cast<float>(SDL_JoystickGetAxis(joystick_, 1)) / 32767.0f;
            float rightX = static_cast<float>(SDL_JoystickGetAxis(joystick_, 2)) / 32767.0f;
            float leftX = static_cast<float>(SDL_JoystickGetAxis(joystick_, 0)) / 32767.0f;

            // Apply deadzone and invert throttle
            throttle = -applyDeadzone(leftY);
            
            float steering2 = applyDeadzone(rightX);
            float steering3 = applyDeadzone(leftX);
            
            steering = (std::abs(steering3) > 0.0f) ? steering3 : steering2;
#endif
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading controller state: " << e.what() << std::endl;
            return false;
        }
    }

    void run() {
        if (!initialize()) {
            std::cerr << "Failed to initialize controller" << std::endl;
            return;
        }

        std::cout << "Controller test started. Press Ctrl+C to exit." << std::endl;
        std::cout << "Format: Throttle: [value] | Steering: [value]" << std::endl;

        try {
            while (true) {
                float throttle, steering;
                
                if (getControllerState(throttle, steering)) {
                    std::cout << "Throttle: " << std::fixed << std::setprecision(2) 
                              << throttle << " | Steering: " << steering << std::endl;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_RATE_MS));
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Unknown error occurred" << std::endl;
        }

        cleanup();
        std::cout << "Stopped." << std::endl;
    }
};

int main() {
    try {
        JoystickController controller;
        controller.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }

    return 0;
} 