# PS4 Controller Integrated Test

A comprehensive C++ solution that combines button testing and joystick functionality for PS4 controllers connected to Raspberry Pi via Bluetooth.

## Features

### 🎮 **Dual Functionality**
- **Button Testing**: Real-time detection of all PS4 controller buttons
- **Joystick Testing**: Continuous monitoring of analog stick inputs
- **Integrated Output**: Clear labeling of button vs joystick events

### 🔧 **Advanced Features**
- **Multi-threaded**: Separate threads for button and joystick monitoring
- **Auto-device detection**: Automatically finds the correct input device
- **Robust error handling**: Comprehensive exception handling and recovery
- **Debug system**: Configurable debug levels for troubleshooting
- **Signal handling**: Graceful shutdown with Ctrl+C
- **Resource management**: RAII principles with automatic cleanup

### 🐛 **Debugging Support**
- **Configurable debug levels**: 0=Errors, 1=Warnings, 2=Info, 3=Verbose
- **Detailed logging**: Step-by-step initialization and operation logs
- **Error reporting**: Descriptive error messages with system details
- **Device enumeration**: Automatic testing of multiple input devices

## Building

### Prerequisites

#### Raspberry Pi / Linux
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev cmake build-essential
```

#### macOS
```bash
brew install sdl2 cmake
```

### Build Instructions

1. **Navigate to project directory:**
   ```bash
   cd "Car Testing"
   ```

2. **Create and enter build directory:**
   ```bash
   mkdir build && cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the project:**
   ```bash
   make
   ```

5. **Run the integrated test:**
   ```bash
   sudo ./ps4_controller_integrated
   ```

## Usage

### Basic Usage
```bash
# Use default device (/dev/input/event3)
sudo ./ps4_controller_integrated

# Specify custom device path
sudo ./ps4_controller_integrated /dev/input/event1
```

### Output Format

#### Button Events
```
[BUTTON] Triangle PRESSED
[BUTTON] Cross RELEASED
[BUTTON] L1 PRESSED
[BUTTON] R2 PRESSED
```

#### Joystick Events
```
[JOYSTICK] Throttle: 0.75 | Steering: -0.25
[JOYSTICK] Throttle: 0.00 | Steering: 0.00
```

### Debug Output
```
[DEBUG-2] PS4Controller constructor called
[DEBUG-2] Initializing PS4Controller
[DEBUG-2] Initializing joystick system
[DEBUG-2] Number of joysticks detected: 1
[DEBUG-2] Initializing input device: /dev/input/event3
[DEBUG-2] PS4Controller initialization successful
```

## Configuration

### Debug Settings
Edit the debug configuration in `ps4_controller_integrated.cpp`:

```cpp
#define DEBUG_MODE 1        // Set to 0 to disable debug output
#define DEBUG_LEVEL 2       // 0=Errors, 1=Warnings, 2=Info, 3=Verbose
```

### Performance Settings
```cpp
static constexpr float DEADZONE_THRESHOLD = 0.1f;  // Joystick deadzone
static constexpr int UPDATE_RATE_MS = 50;          // Joystick update rate (20 FPS)
```

## Troubleshooting

### Common Issues

#### 1. "No joysticks detected"
```bash
# Check if SDL2 can detect controllers
sudo apt-get install joystick
jstest /dev/input/js0
```

#### 2. "Failed to open device"
```bash
# List available input devices
ls -la /dev/input/event*

# Check device permissions
sudo chmod 666 /dev/input/event3

# Find your controller device
cat /proc/bus/input/devices | grep -A 4 -i "ps4\|sony\|dualshock"
```

#### 3. "Permission denied"
```bash
# Add user to input group
sudo usermod -a -G input $USER

# Or run with sudo (recommended for testing)
sudo ./ps4_controller_integrated
```

#### 4. "SDL initialization failed"
```bash
# Install SDL2 development libraries
sudo apt-get install libsdl2-dev

# Check SDL2 installation
pkg-config --modversion sdl2
```

### Debug Mode

Enable verbose debugging by setting `DEBUG_LEVEL = 3`:

```cpp
#define DEBUG_LEVEL 3  // Verbose debugging
```

This will show:
- All input events (including unhandled ones)
- Device enumeration attempts
- Thread lifecycle events
- Detailed error information

### Device Detection

The program automatically tries multiple device paths:
- `/dev/input/event0` through `/dev/input/event6`
- Reports which device was successfully opened
- Provides fallback options if primary device fails

## Technical Details

### Architecture
- **Multi-threaded design**: Separate threads for button and joystick monitoring
- **Thread-safe shutdown**: Atomic flags for clean termination
- **Resource management**: RAII with automatic cleanup
- **Exception safety**: Comprehensive error handling

### Input Handling
- **Button events**: Direct Linux input event reading
- **Joystick events**: SDL2 joystick API
- **Event filtering**: Only processes relevant events
- **Deadzone application**: Configurable joystick deadzone

### Performance
- **20 FPS joystick updates**: Responsive analog input
- **Event-driven button detection**: Immediate button response
- **Minimal CPU usage**: Efficient polling and event handling
- **Memory efficient**: No memory leaks with proper cleanup

## Button Mapping

| Button | Linux Input Code | PS4 Controller |
|--------|------------------|----------------|
| Triangle | BTN_NORTH | △ |
| Cross | BTN_SOUTH | ✕ |
| Square | BTN_WEST | □ |
| Circle | BTN_EAST | ○ |
| L1 | BTN_TL | L1 |
| R1 | BTN_TR | R1 |
| L2 | BTN_TL2 | L2 |
| R2 | BTN_TR2 | R2 |
| Share | BTN_SELECT | Share |
| Options | BTN_START | Options |
| PS | BTN_MODE | PS |
| L3 | BTN_THUMBL | Left Stick Press |
| R3 | BTN_THUMBR | Right Stick Press |

## Joystick Axes

| Axis | SDL Axis | Function |
|------|----------|----------|
| Left Stick X | Axis 0 | Steering (fallback) |
| Left Stick Y | Axis 1 | Throttle (inverted) |
| Right Stick X | Axis 2 | Steering (primary) |
| Right Stick Y | Axis 3 | Unused |

## Contributing

When debugging or extending this code:

1. **Enable debug mode** for detailed logging
2. **Check device permissions** and group membership
3. **Verify SDL2 installation** and linking
4. **Test with different input devices** if issues persist
5. **Monitor system logs** for additional error information

## License

This code is part of the Car Testing project and follows the same licensing terms. 