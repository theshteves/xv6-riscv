#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"


// Function to convert an integer to a zero-padded hexadecimal string
void int_to_hex(int value, char* hex_str) {
    char hex_digits[] = "0123456789abcdef";
    for (int i = 7; i >= 0; i--) {
        hex_str[i] = hex_digits[value & 0xf];
        value >>= 4;
    }
    hex_str[8] = '\0';  // Null-terminate the string
}


// Print the hexadecimal dump of the given data.
void print_hex_dump(char *buffer, int length, int buffer_offset) {
    char hex_digits[] = "0123456789abcdef";
    char ascii_line[17];
    int i, j;

    for (i = 0; i < length; i++) {
        // Print the hex representation (2 characters per byte)
        if (i % 16 == 0) {
            // Print the ASCII part of the previous line
            if (i > 0) {
                for (j = 0; j < 16; j++) {
                    // Print the ASCII characters for the last 16 bytes, or '.' for non-printable characters
                    if (ascii_line[j] >= 32 && ascii_line[j] <= 126)
                        write(1, &ascii_line[j], 1);
                    else
                        write(1, ".", 1);
                }
            }

            // Print the offset at the beginning of the line
            char hex_offset[9];
            int_to_hex(i + buffer_offset, hex_offset);
            write(1, "\n", 1);
            write(1, hex_offset, 8);
            write(1, ": ", 2);
        }

        // Print the hexadecimal representation of the current byte
        write(1, &hex_digits[(buffer[i] >> 4) & 0x0F], 1);
        write(1, &hex_digits[buffer[i] & 0x0F], 1);
        write(1, " ", 1);

        // Add the ASCII representation (for printable characters)
        if (buffer[i] >= 32 && buffer[i] <= 126)
            ascii_line[i % 16] = buffer[i];
        else
            ascii_line[i % 16] = '.';

        // If it's the last byte of a line, print the ASCII part
        if (i % 16 == 15 || i == length - 1) {
            for (j = i % 16 + 1; j < 16; j++) {
                write(1, "   ", 3);  // Pad remaining spaces
            }
            ascii_line[i % 16 + 1] = '\0';  // Null-terminate the ASCII string
        }
    }
    write(1, "\n", 1);
}

int main(int argc, char *argv[]) {
    int fd;
    char buffer[2048];  // Buffer to store file data (adjust size as necessary)
    int bytes_read;

    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(0);
    }

    // Open the file (or handle stdin, etc.)
    fd = open(argv[1], 0);
    if (fd < 0) {
        printf("Failed to open file: %s\n", argv[1]);
        exit(0);
    }

    // Read and print the file in chunks
    int buffer_offset = 0;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        print_hex_dump(buffer, bytes_read, buffer_offset);
        buffer_offset += bytes_read;
    }

    close(fd);
    exit(0);
}

