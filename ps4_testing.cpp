#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main() {
    const char* device = "/dev/input/eventX"; // Change X to your number!

    int fd = open(device, O_RDONLY);
    if (fd < 0) {
        std::cerr << "Could not open " << device << std::endl;
        return 1;
    } else {
        std::cout << "Successfully opened " << device << std::endl;
    }

    close(fd);
    return 0;
}
