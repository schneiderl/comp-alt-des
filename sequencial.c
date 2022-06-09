#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
typedef struct timespec Time;

#define MAX_SIZE 300000
int array[MAX_SIZE];
int local[MAX_SIZE];
int array_size = 0;

void sort(int arr[], int n)
{
    bool sorted = false;
    while (!sorted)
    {
        sorted = true;
        for (int i = 1; i <= n - 2; i = i + 2)
        {
            if (arr[i] > arr[i + 1])
            {
                int tmp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = tmp;
                sorted = false;
            }
        }
        for (int i = 0; i <= n - 2; i = i + 2)
        {
            if (arr[i] > arr[i + 1])
            {
                int tmp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = tmp;
                sorted = false;
            }
        }
    }
    return;
}

void printArray(int arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
}

int main()
{

    double t;
    Time start_time, end_time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, (struct timespec *)&start_time);

    int value;
    while (scanf("%d\n", &value) == 1 && array_size < MAX_SIZE)
        array[array_size++] = value;

    sort(array, array_size);
    printArray(array, array_size);

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, (struct timespec *)&end_time);
    t = (double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_nsec - start_time.tv_nsec) * 1e-9;

    printf("\nTOTAL TIME SPENT: %f \n", t);

    return (0);
}