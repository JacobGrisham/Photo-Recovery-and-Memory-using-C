#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#define BLOCK_SIZE 512

int main(int argc, char *argv[])
{
    // Program should accept exactly one command-line argument, the name of a forensic image from which to recover JPEG
    // If not executed with exactly one command-line argument, remind the user of correct usage
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    // Open card.raw
    FILE *file = fopen(argv[1], "r");

    // If the forensic image cannot be opened for reading, inform the user
    if (file == NULL)
    {
        printf("Unable to open image\n");
        return 1;
    }

    // Create a new type to store a byte of data
    typedef uint8_t BYTE;
    // Create buffer that stores 512 bytes of data
    BYTE buffer[BLOCK_SIZE];
    // Create variable to keep track of bytes read
    size_t read;
    // Create boolean variable to keep track of whether jpeg read is the first jpeg
    bool first = false;
    // Create boolean variable to keep track of whether jpeg was already found
    bool found = false;
    // Create file for writing data into
    FILE *current_file;
    // Create filename variable
    char filename[100];
    // Create variable to keep track of file number
    int num = 0;

    // Open card.raw
    while (true)
    {
        // Read 512 bytes at a time into a buffer for efficiency’s sake.
        read = fread(buffer, sizeof(BYTE), BLOCK_SIZE, file);
        // If at end of file, stop
        if (read == 0)
        {
            break;
        }

        // Look for JPEGs’ signatures
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            found = true;
            // Keep track of first jpeg file
            if (!first)
            {
                first = true;
            }
            // Close that file only once you encounter another signature.
            else
            {
                fclose(current_file);
            }
            // Use sprintf to number the files the program outputs
            sprintf(filename, "%03i.jpg", num);
            // Each time you find a signature, open a new file for writing.
            current_file = fopen(filename, "w");
            fwrite(buffer, sizeof(BYTE), read, current_file);
            // Increment file number
            num++;
        }
        else
        {
            if (found)
            {
                fwrite(buffer, sizeof(BYTE), read, current_file);
            }
        }
    }
    fclose(file);
    fclose(current_file);
    return 0;
    // You should ultimately find that the card.raw contains 50 JPEGs.
}
