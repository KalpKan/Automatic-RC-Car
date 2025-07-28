# PS4 Joystick Test - C++ Version

This is a C++ implementation of the PS4 joystick test with the same functionality as the Python version, featuring robust error handling and cross-platform support.

## Features

- **Cross-platform support**: Windows (XInput) and Unix/Linux/macOS (SDL2)
- **Robust error handling**: Comprehensive exception handling and logging
- **Deadzone implementation**: Configurable deadzone threshold (default: 0.1)
- **Real-time input monitoring**: 100ms update rate for smooth control
- **Clean shutdown**: Proper resource cleanup on exit

## Building

### Prerequisites

#### Windows
- Visual Studio 2017 or later with C++17 support
- XInput library (included with Windows SDK)

#### Unix/Linux/macOS
- C++17 compatible compiler (GCC 7+, Clang 5+)
- SDL2 development libraries
- CMake 3.10+

### Installation of Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get install libsdl2-dev cmake build-essential
```

#### macOS
```bash
brew install sdl2 cmake
```

#### CentOS/RHEL/Fedora
```bash
sudo yum install SDL2-devel cmake gcc-c++
# or for Fedora:
sudo dnf install SDL2-devel cmake gcc-c++
```

### Build Instructions

1. **Clone and navigate to the project directory:**
   ```bash
   cd "Car Testing"
   ```

2. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the project:**
   ```bash
   make
   # or on Windows with Visual Studio:
   # cmake --build . --config Release
   ```

5. **Run the program:**
   ```bash
   ./ps4_joystick_test
   ```

## Usage

1. Connect your PS4 controller to your computer
2. Run the compiled executable
3. Move the joysticks to see real-time throttle and steering values
4. Press Ctrl+C to exit

## Output Format

The program outputs throttle and steering values in the format:
```
Throttle: 0.25 | Steering: -0.50
```

- **Throttle**: Left stick Y-axis (inverted for intuitive control)
- **Steering**: Right stick X-axis (with fallback to left stick X-axis)

## Error Handling

The program includes comprehensive error handling for:
- Controller detection failures
- Initialization errors
- Runtime state reading errors
- Resource cleanup issues

All errors are logged to stderr with descriptive messages.

## Technical Details

- **Deadzone threshold**: 0.1 (configurable via `DEADZONE_THRESHOLD`)
- **Update rate**: 100ms (configurable via `UPDATE_RATE_MS`)
- **Axis normalization**: Values are normalized to [-1.0, 1.0] range
- **Memory management**: RAII principles with automatic cleanup

## Troubleshooting

### No controller detected
- Ensure the controller is properly connected
- On Windows, check Device Manager for controller recognition
- On Linux, verify udev rules for controller access

### SDL2 not found
- Install SDL2 development libraries
- Ensure CMake can find SDL2 (may need to set `SDL2_DIR`)

### Build errors
- Ensure C++17 support is available
- Check that all dependencies are properly installed
- Verify CMake version is 3.10 or higher 