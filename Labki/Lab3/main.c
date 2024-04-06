#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if(argc < 2) {
        puts("Now enought arguments.");
        return 1;
    }

    int n = atoi(argv[1]);

    while(n > 0) 
    {
        n--;
        int parentPid = getppid();
        pid_t childProcessToWait;

        pid_t pid = fork();
        if(pid < 0) 
        {
            perror("failed to fork");
            return 1;
        }
        else if(pid == 0) 
        {
            // Child process
            if(n%2 == 1) 
            {
                continue;
            }
            else
            {
                wait(NULL);
                return 0;
            }
        } 
        else 
        {
            // Parent process
            if(n%2 == 1) 
            {
                childProcessToWait = pid;
            }
        }

        pid = fork();
        if(pid < 0) 
        {
            perror("failed to fork");
            return 1;
        }
        if(pid == 0) 
        {
            // Child process
            if(n%2 == 0) 
            {
                continue;
            } 
            else
            {
                wait(NULL);
                return 0;
            }
        } 
        else
        {
            // Parent process
            if(n%2 == 0) 
            {
                childProcessToWait = pid;
            }    
        }

        waitpid(childProcessToWait, NULL, 0);
        return 0;
    }
    // Printing a tree
    execlp("ps", "ps", "-u", getenv("USER"), "--forest", NULL);
    return 0;
}