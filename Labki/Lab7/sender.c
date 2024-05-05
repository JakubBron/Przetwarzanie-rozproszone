#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define READ 0
#define WRITE 1

char text[100];
int length = 0;

int main()
{
    printf("Enter text, no more than 100 characters:\n");
    scanf("%s%n", text, &length);
    if(length > 100)
    {
        fprintf(stderr, "message too long");
        return 1;
    }

    int letterSwapper[2], digitChanger[2];
    if(pipe(letterSwapper) != 0) 
    {
        fprintf(stderr, "pipe for letterSwapper failed");
        return 1;
    }
    
    if(pipe(digitChanger) != 0)
    {
        fprintf(stderr, "pipe for digitChanger failed");
        return 1;
    }


    pid_t pid, pid2;
    pid = fork();
    if(pid < 0)
    {
        fprintf(stderr, "fork() failed");
        return 1;
    }

    if(pid > 0) // parent process, writes text to buffer
    {
        close(letterSwapper[READ]);
        write(letterSwapper[WRITE], text, strlen(text)+1);
        close(letterSwapper[WRITE]);
    }
    else// child process, now to choose between letterSwapper and digitChanger
    {
        pid2 = fork();
        if(pid2 < 0)
        {
            fprintf(stderr, "fork() failed - the second one");
            return 1;
        }

        if(pid2 > 0) // parent process, swaps letters and sends to digitSwapper
        {
            char result[100];
            close(letterSwapper[WRITE]);
            read(letterSwapper[READ], result, 100);
            close(letterSwapper[READ]);

            for(int i = 0; i < strlen(result)+1; i++)
            {
                if(result[i] >= 'a' && result[i] <= 'z')
                {
                    result[i] = result[i] - 32;
                }
                else if(result[i] >= 'A' && result[i] <= 'Z')
                {
                    result[i] = result[i] + 32;
                }
            }
            //printf("Swapped text: %s\n", result);

            close(digitChanger[READ]);
            write(digitChanger[WRITE], result, strlen(result)+1);
            close(digitChanger[WRITE]);
        }
        else // child process, changes any digit to 0
        {
            char result[100];
            close(digitChanger[WRITE]);
            read(digitChanger[READ], result, 100);
            close(digitChanger[READ]);

            for(int i=0; i<strlen(result)+1; i++)
            {
                if(result[i] > '0' && result[i] <= '9')
                {
                    result[i] = '0';
                }
            }
            //printf("Changed text: %s\n", result);
                
            // creation of a named pipe, only send data
            const char* namedPipe = "/tmp/namedPipeToRead";
            int status = 0;
            status = mkfifo(namedPipe, 0666);
            if(status < 0)
            {
                fprintf(stderr, "named pipe creation failed");
            }
            int pipe = 0;
            pipe = open(namedPipe, O_WRONLY);
            if(pipe < 0)
            {
                fprintf(stderr, "error opening named pipe");
            }
            write(pipe, result, strlen(result)+1);
            close(pipe);
            //printf("Data sent to named pipe");
        }
        
    }
}