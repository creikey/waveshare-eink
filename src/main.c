#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B115200

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#include <nerror1/nerror.h>
#include <stdbool.h>

#define bzero(b, len) (memset((b), '\0', (len)), (void)0) // fix deprecation

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        NERROR(stderr, high, "Provide /dev/ file for eink display");
        return 1;
    }
    int fd, c, res;
    struct termios oldtio, newtio;
    char buf[255];
    /*
        Open Device for reading and writing, not tty so not killed by ^C
    */
    fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (fd <= 0)
    {
        NERROR(stderr, high, "Running perror");
        perror(argv[1]);
        NERROR(stderr, high, "Failed to access %s for r/w", argv[1]);
        return 1;
    }
    tcgetattr(fd, &oldtio);         // Save current settings
    bzero(&newtio, sizeof(newtio)); // Clear struct for new port settings
                                    /* 
          BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
          CRTSCTS : output hardware flow control (only used if the cable has
                    all necessary lines. See sect. 7 of Serial-HOWTO)
          CS8     : 8n1 (8bit,no parity,1 stopbit)
          CLOCAL  : local connection, no modem contol
          CREAD   : enable receiving characters
    */
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    newtio.c_lflag = 0;     // non terminal esque, no echo
    newtio.c_cc[VTIME] = 1; // no inter character timer
    newtio.c_cc[VMIN] = 2;  // block read until 3 chars received

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    // Now we have context to read and write
    PNERROR(low, "Sending handshake...");
    // A5 00 09 00 CC 33 C3 3C AC
    char handshake[] = {0xA5, 0x00, 0x09, 0x00, 0xCC, 0x33, 0xC3, 0x3C, 0xAC};
    size_t got = write(fd, handshake, sizeof(handshake));
    PNERROR(low, "Handshake sent!");

    PNERROR(low, "Waiting for received 3 characters...");
    res = read(fd, buf, 2); // Returns after 3 characters, or 'OK!'
    buf[res] = '\0';        // Null terminator
    printf(":%s:%d\n", buf, res);
    PNERROR(low, "Received!");

    tcsetattr(fd, TCSANOW, &oldtio); // restore previous settings
}