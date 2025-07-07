#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string>
#include <cerrno>
#include <cstring>

int main(int argc, char* argv[]) {
    // Usage: sudo ./ps4_buttons [/dev/input/eventX]
    std::string device = "/dev/input/event3";  // <-- change event number as needed
    if (argc > 1) {
        device = argv[1];
    }

    // Attempt to open the device
    int fd = open(device.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cerr << "Error: Failed to open device '" << device << "': "
                  << std::strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Listening for PS4 controller button presses on '" << device << "'..." << std::endl;

    struct input_event ev;
    while (true) {
        // Read next event
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n == -1) {
            std::cerr << "Error: Read failed: " << std::strerror(errno) << std::endl;
            break;  // Exit loop on read error
        } else if (n != sizeof(ev)) {
            std::cerr << "Warning: Unexpected event size: " << n << " bytes" << std::endl;
            continue;  // Skip malformed event
        }

        // Process only key press events (value == 1)
        if (ev.type == EV_KEY) {
            if (ev.value != 1 && ev.value != 0) {
                // Skip auto-repeat or other values
                continue;
            }
            bool pressed = (ev.value == 1);

            switch (ev.code) {
                case BTN_NORTH:   std::cout << (pressed ? "Triangle pressed" : "Triangle released") << std::endl; break;
                case BTN_SOUTH:   std::cout << (pressed ? "Cross pressed"    : "Cross released")     << std::endl; break;
                case BTN_WEST:    std::cout << (pressed ? "Square pressed"   : "Square released")    << std::endl; break;
                case BTN_EAST:    std::cout << (pressed ? "Circle pressed"   : "Circle released")    << std::endl; break;
                case BTN_TL:      std::cout << (pressed ? "L1 pressed"       : "L1 released")        << std::endl; break;
                case BTN_TR:      std::cout << (pressed ? "R1 pressed"       : "R1 released")        << std::endl; break;
                case BTN_TL2:     std::cout << (pressed ? "L2 pressed"       : "L2 released")        << std::endl; break;
                case BTN_TR2:     std::cout << (pressed ? "R2 pressed"       : "R2 released")        << std::endl; break;
                case BTN_SELECT:  std::cout << (pressed ? "Share pressed"    : "Share released")     << std::endl; break;
                case BTN_START:   std::cout << (pressed ? "Options pressed"  : "Options released")   << std::endl; break;
                case BTN_MODE:    std::cout << (pressed ? "PS button pressed": "PS button released") << std::endl; break;
                case BTN_THUMBL:  std::cout << (pressed ? "L3 pressed"       : "L3 released")        << std::endl; break;
                case BTN_THUMBR:  std::cout << (pressed ? "R3 pressed"       : "R3 released")        << std::endl; break;
                default:
                    std::cerr << "Debug: Unhandled key code: " << ev.code << std::endl;
            }
        }
        // Could add EV_ABS handling here if needed
    }

    close(fd);
    return 0;
}
