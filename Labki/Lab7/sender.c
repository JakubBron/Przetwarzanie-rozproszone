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
    const char* namedPipe = "/tmp/namedPipeToRead";
    int status = 0;
    status = mkfifo(namedPipe, 0666);
    if(status < 0)
    {
        fprintf(stderr, "named pipe creation failed");
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
        while(1)
        {
            printf("Enter text, no more than 100 characters. '0000' exits the programme.\n");
            scanf("%s%n", text, &length);
            if(length > 100)
            {
                fprintf(stderr, "message too long");
                continue;
            }
            
            write(letterSwapper[WRITE], text, strlen(text)+1);
            if(strcmp(text, "0000") == 0)
            {
                close(letterSwapper[READ]);
                close(letterSwapper[WRITE]);
                break;
            }
        }
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
            while(1)
            {
                char result[100];
                
                read(letterSwapper[READ], result, 100);

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

                write(digitChanger[WRITE], result, strlen(result)+1);

                if(strcmp(result, "0000") == 0)
                {
                    close(letterSwapper[WRITE]);
                    close(letterSwapper[READ]);
                    close(digitChanger[READ]);
                    close(digitChanger[WRITE]);
                    break;
                }
            }
        }
        else // child process, changes any digit to 0
        {
            while(1)
            {
                char result[100];
                read(digitChanger[READ], result, 100);

                for(int i=0; i<strlen(result)+1; i++)
                {
                    if(result[i] > '0' && result[i] <= '9')
                    {
                        result[i] = '0';
                    }
                }
                //printf("Changed text: %s\n", result);
                    
                // creation of a named pipe, only send data
                
                int pipe = 0;
                pipe = open(namedPipe, O_WRONLY);
                if(pipe < 0)
                {
                    fprintf(stderr, "error opening named pipe");
                }
                write(pipe, result, strlen(result)+1);
                close(pipe);
                //printf("Data sent to named pipe");
                if(strcmp(result, "0000") == 0)
                {
                    close(digitChanger[WRITE]);
                    close(digitChanger[READ]);
                    break;
                }
            }
        }
        
    }
}