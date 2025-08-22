#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdint>

int main()
{
    const char *port = "/dev/ttyUSB1"; // adjust to your USB-UART port
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        std::cerr << "Error opening " << port << std::endl;
        return 1;
    }

    // Configure serial port
    termios tty{};
    if (tcgetattr(fd, &tty) != 0)
    {
        std::cerr << "Error from tcgetattr" << std::endl;
        return 1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 8;  // we expect 8 bytes (4 * uint16_t)
    tty.c_cc[VTIME] = 0; // no timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        std::cerr << "Error from tcsetattr" << std::endl;
        return 1;
    }

    // Read loop
    uint8_t raw[8];
    while (true)
    {
        int n = read(fd, raw, sizeof(raw));
        if (n == sizeof(raw))
        {
            // Reconstruct 16-bit values (little endian assumption from STM32)
            uint16_t d1 = raw[0] | (raw[1] << 8);
            uint16_t d2 = raw[2] | (raw[3] << 8);
            uint16_t d3 = raw[4] | (raw[5] << 8);
            uint16_t d4 = raw[6] | (raw[7] << 8);

            std::cout << "D1=" << d1
                      << " D2=" << d2
                      << " D3=" << d3
                      << " D4=" << d4
                      << std::endl;
        }
    }

    close(fd);
    return 0;
}
