#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string>
#include <cerrno>
#include <cstring>
#include <sys/ioctl.h>

int main(int argc, char* argv[]) {
    // Usage: sudo ./ps4_buttons [/dev/input/eventX]
    std::string device = "/dev/input/event3";  // <-- change event number as needed
    if (argc > 1) {
        device = argv[1];
    }

    // Open the device
    int fd = open(device.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cerr << "Error: Failed to open device '" << device << "': "
                  << std::strerror(errno) << std::endl;
        return 1;
    }

    // Grab device to ensure events aren't consumed elsewhere
    if (ioctl(fd, EVIOCGRAB, 1) < 0) {
        std::cerr << "Warning: Failed to grab device: " << std::strerror(errno) << std::endl;
        // Not fatal; continue reading
    }

    std::cout << "Listening for PS4 controller button presses on '" << device << "'..." << std::endl;

    struct input_event ev;
    while (true) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n == -1) {
            if (errno == EINTR) continue;  // interrupted, retry
            std::cerr << "Error: Read failed: " << std::strerror(errno) << std::endl;
            break;
        } else if (n != sizeof(ev)) {
            std::cerr << "Warning: Unexpected event size: " << n << " bytes" << std::endl;
            continue;
        }

        // Debug: print all events
        // std::cerr << "DBG type=" << ev.type << " code=" << ev.code << " value=" << ev.value << std::endl;

        if (ev.type == EV_KEY) {
            if (ev.value != 1 && ev.value != 0) continue; // filter only press/release
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
                    std::cerr << "Debug: Unhandled key code: " << ev.code
                              << " (type=" << ev.type << ", value=" << ev.value << ")" << std::endl;
            }
        }
    }

    close(fd);
    return 0;
}
