#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    // write a program to read named pipe
    int f;
    const char* namedPipe = "/tmp/namedPipeToRead";
    char buffer[100];
    f = open(namedPipe, O_RDONLY);
    if(f == -1)
    {
        fprintf(stderr, "error opening named pipe\n");
        return 1;
    }
    while(1)
    {
        ssize_t num_bytes = read(f, buffer, 100);
        if (num_bytes == -1) 
        {
            fprintf(stderr, "read");
            exit(EXIT_FAILURE);
        } 
        if (num_bytes != 0) 
        {
            printf("Received: %s\n", buffer);
            if(strcmp(buffer, "0000") == 0)
            {
                break;
            }
        } 
    }
    close(f);
    unlink(namedPipe);
    return 0;
}