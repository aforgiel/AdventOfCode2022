// AdventOfCodeDay6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay6/input.txt";
typedef struct {
    char storage[100];
    int size;
} TStack;

int ReadLine(FILE* file, char* buffer)
{
    char* ptr;
    int result;

    ptr = buffer;
    result = 0;

    while (fread(ptr, 1, 1, file) > 0)
    {
        if (*ptr == '\n')
        {
            *ptr = NULL;
            return result;
        }
        ptr++;
        result++;
    }

    return result;
}

void StackInit(TStack* stack)
{
    stack->size = 0;
}

int StackPush(TStack* stack, char input)
{
    stack->storage[stack->size] = input;
    stack->size++;

    return stack->size;
}

int StackPull(TStack* stack, char* output)
{
    if (stack->size > 0)
    {
        *output = stack->storage[stack->size - 1];
        stack->size--;
    }
    else
        return -1;

    return stack->size;
}

void StackPrint(TStack* stack)
{
    int index;

    for (index = 0; index < stack->size; index++)
        printf("[%d] %c ", index, stack->storage[index]);
}

#define kPacketSize 14

int main()
{
    FILE* input;
    char buffer[kPacketSize+1];
    char* ptr;
    errno_t error;
    int result;
    int i, j;
    bool next;

    printf("Advent of Code - Day 6\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    buffer[kPacketSize] = NULL;
    ptr = buffer;

    fread( buffer,1,kPacketSize-1,input);
    buffer[kPacketSize-1] = ' ';

    printf("initial buffer: %s\n", buffer);

    ptr = & buffer[kPacketSize-1];
    result = kPacketSize;

    while( fread( ptr, 1, 1, input ) > 0 )
    {
        printf("[%d] %s\n", result, buffer );

        next = true;

        for( i = 0; ( i < ( kPacketSize - 1 ) ) && next; i++ )
            for( j = i + 1; ( j < kPacketSize ) && next; j++ )
            {
                if( buffer[i] == buffer[j] )
                    next = false;
                printf("\t%d-%d: %d\n", i, j, next);
            }

        if (next == true)
        {
            printf("\n\nFound: %d !!!\n", result );
            break;
        }

        result++;
        ptr++;
        if( ptr > & buffer[kPacketSize-1] )
            ptr = buffer;
    }

    return 0;
}