#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <mpi.h>
#include <time.h>

#define MAX_SIZE 300000

#define EMPTY -1

#define RANK_MASTER 0

#define send(buf, size, dst, tag) MPI_Send(buf, size, MPI_INT, dst, tag, MPI_COMM_WORLD)
#define recv(buf, size, src, tag) MPI_Recv(buf, size, MPI_INT, src, tag, MPI_COMM_WORLD, &mpi_status)

enum tags_t
{
    TAG_VALUE,
    TAG_RESULT,
    TAG_SIZE
};

int array[MAX_SIZE];
int local[MAX_SIZE];

int array_index = 0;
int local_index = 0;

int array_size = 0;
int local_size = 0;

int node_count;
int my_rank;

MPI_Status mpi_status;

int is_master(void)
{
    return my_rank == RANK_MASTER;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int left_node_rank(void)
{
    return max(0, my_rank - 1);
}

int right_node_rank(void)
{
    return min(node_count - 1, my_rank + 1);
}

void read_array(void)
{
    int value;

    array_size = 0;

    if (is_master())
    {
        while (scanf("%d\n", &value) == 1 && array_size < MAX_SIZE)
            array[array_size++] = value;

        local_size = array_size / node_count;
    }

    MPI_Bcast(&array_size, 1, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&local_size, 1, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);

    if (my_rank == node_count - 1)
        local_size += array_size % node_count;

    close(0);
}

void print_array(int *a, int size)
{
    int i = 0;

    for (; i < size; ++i)
    {
        printf(" %5d", a[i]);

        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    puts("");
}

int getNextRank()
{
    if (my_rank + 1 == node_count)
    {
        return -1;
    }
    else
    {
        return my_rank + 1;
    }
}

int getPrevRank()
{
    if (my_rank == 0)
    {
        return -1;
    }
    else
    {
        return my_rank - 1;
    }
}

int righRankExists()
{
    if (getNextRank() == -1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int leftRankExists()
{
    if (getPrevRank() == -1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void odd_even_sort(int odd_even)
{

    // 1 = Odd
    // 0 = Even
    // print_array(local, local_size);
    if (odd_even == 0)
    {
        int leftValue, rightValue = MAX_SIZE;

        if (leftRankExists())
        {
            send(&local[0], 1, getPrevRank(), TAG_VALUE);
        }
        if (righRankExists())
        {
            send(&local[local_size - 1], 1, getNextRank(), TAG_VALUE);
        }
        if (leftRankExists())
        {
            recv(&leftValue, 1, getPrevRank(), TAG_VALUE);
            if (local[0] < leftValue)
            {
                local[0] = leftValue;
            }
        }
        if (righRankExists())
        {

            recv(&rightValue, 1, getNextRank(), TAG_VALUE);
            if (local[local_size - 1] > rightValue)
            {
                local[local_size - 1] = rightValue;
            }
        }
        for (int i = 0; i < local_size - 1; i = i + 2)
        {
            if (local[i] > local[i + 1])
            {
                int tmp = local[i];
                local[i] = local[i + 1];
                local[i + 1] = tmp;
            }
        }
    }
    else
    {
        for (int i = 1; i < local_size - 1; i = i + 2)
        {
            if (local[i] > local[i + 1])
            {
                int tmp = local[i];
                local[i] = local[i + 1];
                local[i + 1] = tmp;
            }
        }
    }
}

int arraySortedCheck(int arr[], int n)
{
    // all elements are checked and
    // all are in sorted order
    if (n == 1 || n == 0)
        return 1;
    // when an array is not in sorted order
    if (arr[n - 1] < arr[n - 2])
        return 0;
    return arraySortedCheck(arr, n - 1);
}

int main(int argc, char *argv[])
{
    int i, rc = MPI_Init(&argc, &argv);

    rc |= MPI_Comm_size(MPI_COMM_WORLD, &node_count);
    rc |= MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (rc != MPI_SUCCESS)
    {
        puts("Error initializing MPI!\n");
        return EXIT_FAILURE;
    }

    read_array();

    if (is_master())
    {
        printf("INPUT              :");
        print_array(array, array_size);
    }
    for (int i = 1; i < MAX_SIZE; i++)
    {
        MPI_Scatter(array, array_size / node_count, MPI_INT, local, array_size / node_count, MPI_INT, 0, MPI_COMM_WORLD);
        odd_even_sort(i % 2);

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Gather(local, array_size / node_count, MPI_INT, array, array_size / node_count, MPI_INT, 0, MPI_COMM_WORLD);

        if (is_master())
        {
            if (arraySortedCheck(array, array_size))
            {
                printf("SORTED ARRAY\n");
                print_array(array, array_size);
                break;
            }
        }
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}