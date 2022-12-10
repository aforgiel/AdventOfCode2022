// AdventOfCodeDay10.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>

const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay10/input.txt";

bool FindPattern(const char* buffer, const char* pattern)
{
    char* ptrBuffer;
    char* ptrPattern;

    ptrBuffer = const_cast<char*>(buffer);
    ptrPattern = const_cast<char*>(pattern);

    while (*ptrPattern != NULL)
    {
        if (*ptrPattern != *ptrBuffer)
            return false;

        ptrBuffer++;
        ptrPattern++;
    }

    return true;
}

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

const char * kNOOP = "noop";
const char* kADDX = "addx";
#define kWidth 40
#define kHeight 6

int main()
{
    FILE* input;
    char buffer[100];
    errno_t error;
    std::list<int> history;
    int x;
    int value;
    int index;
    int nextIndex;
    int result;
    char CRT[kWidth * kHeight];
    int row;
    int column;
    char* ptr;

    printf("Advent of Code - Day 10\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    x = 1;

    while (ReadLine(input, buffer) > 0)
    {
        printf("%s\n", buffer);

        if (FindPattern(buffer, kNOOP) == true)
        {
            history.push_back(x);
        }
        else if (FindPattern(buffer, kADDX) == true)
        {
            sscanf_s(&buffer[5], "%d", & value);
            printf("value: %d\n", value);
            history.push_back(x);
            history.push_back(x);
            x += value;
        }
    }

    index = 1;
    nextIndex = 20;
    result = 0;

    printf("History:\n");
    for (auto it = history.begin(); it != history.end(); ++it)
    {
        printf("[%d] %d", index, *it);

        if (index == nextIndex)
        {
            value = nextIndex * (*it);
            printf("\t=> value: %d\n", value);
            nextIndex += 40;
            result += value;
        }
        else
            printf("\n");

        index++;
    }

    printf("Signal strength: %d\n", result);

    row = 0;
    column = 0;
    ptr = CRT;
    for (auto it = history.begin(); it != history.end(); ++it)
    {
        if( column >= ((*it)-1) &&
            column < ((*it)+2) )
        {
            * ptr = '#';
        }
        else
            * ptr = '.';

        ptr++;
        column++;
        if (column == kWidth)
        {
            row++;
            column = 0;
        }
    }

    ptr = CRT;
    for (row = 0; row < kHeight; row++)
    {
        for (column = 0; column < kWidth; column++)
        {
            printf("%c", *ptr);
            ptr++;
        }
        printf("\n");
    }

    return result;
}