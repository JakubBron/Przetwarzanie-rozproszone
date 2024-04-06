#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

struct threadData_T {
    int* data;
    int dataSize;
} threadData;

DWORD WINAPI worker(void* args) {
    struct threadData_T* data1 = (struct threadData_T*)args;
    for (int i = 0; i < data1->dataSize; i++) {
        data1->data[i] = data1->data[i] + 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    argc = 2;
    argv[1] = "4";
    if (argc < 2) {
        printf("Usage: .\main.exe testNumber [d]\n");
        printf("[d] - print debug flag");
        return 1;
    }
    int printDebugFlag = 0;
    if (argc > 2)
    {
        printDebugFlag = 1;
    }
    int maxTestNumber = atoi(argv[1]);

    for (int testNo = 1; testNo <= maxTestNumber; testNo++)
    {
        printf("Test %d / %d\n", testNo, maxTestNumber);
        // Prepare data to be processed
        int maxThreads = 8;
        int maxDataSize = 2*256*512*1024;
        int* dataToProcess = (int*)malloc(maxDataSize * sizeof(int));
        if (dataToProcess == NULL) 
        {
            printf("\n[ERROR] Malloc error!\n");
            return 1;
        }
        for (int i = 0; i < maxDataSize; ++i) {
            dataToProcess[i] = i;
        }

        if (printDebugFlag)
        {
            printf("Data before processing:\n");
            for (int i = 10; i < 15; ++i) {
                printf("\t%i: %i\n", i, dataToProcess[i]);
            }
            printf("\n");
        }

        // Prepare thread handlers and thread data
        for (int threadNo = 1; threadNo <= maxThreads; threadNo++)
        {
            printf("# of workers: %d ", threadNo);
            if (printDebugFlag)
            {
                printf("\n");
            }
            HANDLE* threads = (HANDLE*)malloc(threadNo * sizeof(HANDLE));
            if (threads == NULL) 
            {
                printf("\n[ERROR] Malloc malfunctioned.\n");
                return 1;
            }
            struct threadData_T* threadData = (struct threadData_T*)malloc(threadNo * sizeof(struct threadData_T));
            if (threadData == NULL)
            {
                printf("\n[ERROR] Malloc malfunctioned.\n");
                return 1;
            }
            for (int i = 0; i < threadNo; ++i) {
                threadData[i].data = dataToProcess + (maxDataSize / threadNo) * i;
                threadData[i].dataSize = (maxDataSize / threadNo);
            }

            DWORD id;
            clock_t start = clock();
            for (int i = 0; i < threadNo; ++i)
            {
                threads[i] = CreateThread(NULL, 0, worker, (void*)&threadData[i], 0, &id);
                if (threads[i] != INVALID_HANDLE_VALUE)
                {
                    if (printDebugFlag)
                    {
                        printf("Created %i thread that calculates %i elements from 0x%llx\n", i, threadData[i].dataSize, (unsigned long long) threadData[i].data);
                    }
                    SetThreadPriority(threads[i], THREAD_PRIORITY_NORMAL);
                }
            }

            WaitForMultipleObjects(threadNo, threads, 1, INFINITE);
            clock_t end = clock();
            float timw = (float)(end - start) / CLOCKS_PER_SEC;
            printf("time: %.10f s\n", timw);

            if (printDebugFlag)
            {
                printf("\nData after processing:\n");
                for (int i = 10; i < 15; ++i) {
                    printf("\t%i: %i\n", i, dataToProcess[i]);
                }
                printf("\n");
            }
            free(threadData);
            free(threads);
        }
        printf("\n");
        free(dataToProcess);
    }
    return 0;
}