#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <memory>
#include <string>
#include <cmath>
#include <iomanip>
#include <atomic>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <cerrno>
#include <cstring>
#include <sys/ioctl.h>
#include <SDL2/SDL.h>

// Debug configuration
#define DEBUG_MODE 1
#define DEBUG_LEVEL 2  // 0=Errors only, 1=Warnings, 2=Info, 3=Verbose

// Debug macro
#define DEBUG_LOG(level, msg) \
    if (DEBUG_MODE && level <= DEBUG_LEVEL) { \
        std::cerr << "[DEBUG-" << level << "] " << msg << std::endl; \
    }

class PS4Controller {
private:
    // Button testing components
    std::string inputDevice_;
    int inputFd_;
    bool inputInitialized_;
    
    // Joystick components
    SDL_Joystick* joystick_;
    bool joystickInitialized_;
    
    // Control flags
    std::atomic<bool> running_;
    std::atomic<bool> shutdownRequested_;
    
    // Configuration
    static constexpr float DEADZONE_THRESHOLD = 0.1f;
    static constexpr int UPDATE_RATE_MS = 50;  // 20 FPS for responsive input
    static constexpr int MAX_RETRY_ATTEMPTS = 3;
    
    // Thread management
    std::thread buttonThread_;
    std::thread joystickThread_;

public:
    PS4Controller() : 
        inputDevice_("/dev/input/event3"),
        inputFd_(-1),
        inputInitialized_(false),
        joystick_(nullptr),
        joystickInitialized_(false),
        running_(false),
        shutdownRequested_(false) {
        DEBUG_LOG(2, "PS4Controller constructor called");
    }

    ~PS4Controller() {
        DEBUG_LOG(2, "PS4Controller destructor called");
        shutdown();
    }

    // Prevent copying
    PS4Controller(const PS4Controller&) = delete;
    PS4Controller& operator=(const PS4Controller&) = delete;

    bool initialize(const std::string& devicePath = "") {
        DEBUG_LOG(2, "Initializing PS4Controller");
        
        if (!devicePath.empty()) {
            inputDevice_ = devicePath;
        }
        
        try {
            // Initialize SDL for joystick support
            if (!initializeJoystick()) {
                std::cerr << "Failed to initialize joystick system" << std::endl;
                return false;
            }
            
            // Initialize input device for button testing
            if (!initializeInputDevice()) {
                std::cerr << "Failed to initialize input device" << std::endl;
                return false;
            }
            
            running_ = true;
            DEBUG_LOG(2, "PS4Controller initialization successful");
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            return false;
        }
    }

    void shutdown() {
        DEBUG_LOG(2, "Shutting down PS4Controller");
        shutdownRequested_ = true;
        running_ = false;
        
        // Wait for threads to finish
        if (buttonThread_.joinable()) {
            buttonThread_.join();
        }
        if (joystickThread_.joinable()) {
            joystickThread_.join();
        }
        
        cleanup();
    }

    void run() {
        if (!running_) {
            std::cerr << "Controller not initialized" << std::endl;
            return;
        }
        
        std::cout << "PS4 Controller Integrated Test Started" << std::endl;
        std::cout << "=======================================" << std::endl;
        std::cout << "Button testing: " << inputDevice_ << std::endl;
        std::cout << "Joystick testing: SDL2" << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;
        std::cout << "=======================================" << std::endl;
        
        // Start button monitoring thread
        buttonThread_ = std::thread(&PS4Controller::buttonMonitoringLoop, this);
        
        // Start joystick monitoring thread
        joystickThread_ = std::thread(&PS4Controller::joystickMonitoringLoop, this);
        
        // Main thread handles shutdown
        while (running_ && !shutdownRequested_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        shutdown();
    }

private:
    bool initializeJoystick() {
        DEBUG_LOG(2, "Initializing joystick system");
        
        if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        int numJoysticks = SDL_NumJoysticks();
        DEBUG_LOG(2, "Number of joysticks detected: " << numJoysticks);
        
        if (numJoysticks == 0) {
            std::cerr << "No joysticks detected" << std::endl;
            return false;
        }
        
        joystick_ = SDL_JoystickOpen(0);
        if (!joystick_) {
            std::cerr << "Failed to open joystick: " << SDL_GetError() << std::endl;
            return false;
        }
        
        joystickInitialized_ = true;
        std::cout << "Joystick initialized: " << SDL_JoystickName(joystick_) << std::endl;
        DEBUG_LOG(2, "Joystick initialization successful");
        return true;
    }

    bool initializeInputDevice() {
        DEBUG_LOG(2, "Initializing input device: " << inputDevice_);
        
        // Try to open the input device
        inputFd_ = open(inputDevice_.c_str(), O_RDONLY);
        if (inputFd_ < 0) {
            std::cerr << "Failed to open device '" << inputDevice_ << "': " 
                      << std::strerror(errno) << std::endl;
            
            // Try alternative device paths
            std::vector<std::string> alternativeDevices = {
                "/dev/input/event0", "/dev/input/event1", "/dev/input/event2",
                "/dev/input/event4", "/dev/input/event5", "/dev/input/event6"
            };
            
            for (const auto& device : alternativeDevices) {
                DEBUG_LOG(2, "Trying alternative device: " << device);
                inputFd_ = open(device.c_str(), O_RDONLY);
                if (inputFd_ >= 0) {
                    inputDevice_ = device;
                    std::cout << "Found working device: " << device << std::endl;
                    break;
                }
            }
            
            if (inputFd_ < 0) {
                std::cerr << "Could not find any working input device" << std::endl;
                return false;
            }
        }
        
        // Grab device to ensure events aren't consumed elsewhere
        if (ioctl(inputFd_, EVIOCGRAB, 1) < 0) {
            DEBUG_LOG(1, "Warning: Failed to grab device: " << std::strerror(errno));
            // Not fatal; continue reading
        }
        
        inputInitialized_ = true;
        DEBUG_LOG(2, "Input device initialization successful");
        return true;
    }

    void cleanup() {
        DEBUG_LOG(2, "Cleaning up resources");
        
        if (inputInitialized_ && inputFd_ >= 0) {
            close(inputFd_);
            inputFd_ = -1;
            inputInitialized_ = false;
        }
        
        if (joystickInitialized_ && joystick_) {
            SDL_JoystickClose(joystick_);
            joystick_ = nullptr;
            joystickInitialized_ = false;
        }
        
        SDL_Quit();
    }

    void buttonMonitoringLoop() {
        DEBUG_LOG(2, "Button monitoring loop started");
        
        struct input_event ev;
        while (running_ && !shutdownRequested_) {
            ssize_t n = read(inputFd_, &ev, sizeof(ev));
            
            if (n == -1) {
                if (errno == EINTR) {
                    continue;  // interrupted, retry
                }
                std::cerr << "Button read failed: " << std::strerror(errno) << std::endl;
                break;
            } else if (n != sizeof(ev)) {
                DEBUG_LOG(1, "Unexpected event size: " << n << " bytes");
                continue;
            }
            
            if (ev.type == EV_KEY) {
                if (ev.value != 1 && ev.value != 0) continue; // filter only press/release
                
                bool pressed = (ev.value == 1);
                std::string buttonName = getButtonName(ev.code);
                
                if (!buttonName.empty()) {
                    std::cout << "[BUTTON] " << buttonName << (pressed ? " PRESSED" : " RELEASED") << std::endl;
                } else {
                    DEBUG_LOG(3, "Unhandled key code: " << ev.code);
                }
            }
        }
        
        DEBUG_LOG(2, "Button monitoring loop ended");
    }

    void joystickMonitoringLoop() {
        DEBUG_LOG(2, "Joystick monitoring loop started");
        
        while (running_ && !shutdownRequested_) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                // Handle SDL events if needed
            }
            
            if (joystickInitialized_ && joystick_) {
                float throttle, steering;
                if (getJoystickValues(throttle, steering)) {
                    std::cout << "[JOYSTICK] Throttle: " << std::fixed << std::setprecision(2) 
                              << throttle << " | Steering: " << steering << std::endl;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_RATE_MS));
        }
        
        DEBUG_LOG(2, "Joystick monitoring loop ended");
    }

    bool getJoystickValues(float& throttle, float& steering) {
        if (!joystickInitialized_ || !joystick_) {
            return false;
        }
        
        try {
            // Get axis values (normalized to [-1.0, 1.0])
            float leftY = static_cast<float>(SDL_JoystickGetAxis(joystick_, 1)) / 32767.0f;
            float rightX = static_cast<float>(SDL_JoystickGetAxis(joystick_, 2)) / 32767.0f;
            float leftX = static_cast<float>(SDL_JoystickGetAxis(joystick_, 0)) / 32767.0f;
            
            // Apply deadzone and invert throttle for intuitive control
            throttle = -applyDeadzone(leftY);
            
            float steering2 = applyDeadzone(rightX);
            float steering3 = applyDeadzone(leftX);
            
            // Use right stick if active, otherwise left stick
            steering = (std::abs(steering3) > 0.0f) ? steering3 : steering2;
            
            return true;
            
        } catch (const std::exception& e) {
            DEBUG_LOG(1, "Error reading joystick values: " << e.what());
            return false;
        }
    }

    float applyDeadzone(float value) const {
        return (std::abs(value) < DEADZONE_THRESHOLD) ? 0.0f : value;
    }

    std::string getButtonName(int code) {
        switch (code) {
            case BTN_NORTH:   return "Triangle";
            case BTN_SOUTH:   return "Cross";
            case BTN_WEST:    return "Square";
            case BTN_EAST:    return "Circle";
            case BTN_TL:      return "L1";
            case BTN_TR:      return "R1";
            case BTN_TL2:     return "L2";
            case BTN_TR2:     return "R2";
            case BTN_SELECT:  return "Share";
            case BTN_START:   return "Options";
            case BTN_MODE:    return "PS";
            case BTN_THUMBL:  return "L3";
            case BTN_THUMBR:  return "R3";
            default:          return "";
        }
    }
};

// Global controller instance for signal handling
static PS4Controller* g_controller = nullptr;

void signalHandler(int signal) {
    if (g_controller) {
        std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
        g_controller->shutdown();
    }
}

int main(int argc, char* argv[]) {
    std::string devicePath = "/dev/input/event3";
    
    // Parse command line arguments
    if (argc > 1) {
        devicePath = argv[1];
    }
    
    std::cout << "PS4 Controller Integrated Test" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Device path: " << devicePath << std::endl;
    std::cout << "Debug mode: " << (DEBUG_MODE ? "ON" : "OFF") << std::endl;
    std::cout << "Debug level: " << DEBUG_LEVEL << std::endl;
    std::cout << "==============================" << std::endl;
    
    // Set up signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        PS4Controller controller;
        g_controller = &controller;
        
        if (!controller.initialize(devicePath)) {
            std::cerr << "Failed to initialize controller" << std::endl;
            return 1;
        }
        
        controller.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
    
    g_controller = nullptr;
    std::cout << "Program terminated successfully" << std::endl;
    return 0;
} 