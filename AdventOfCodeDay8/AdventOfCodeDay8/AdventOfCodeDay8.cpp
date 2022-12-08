// AdventOfCodeDay8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>

const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay8/input.txt";

#define kTreeMatrixSize 99

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

int MatrixValue(int* matrix, int row, int column)
{
    return matrix[row * kTreeMatrixSize + column];
}

int ScanRow(int* treeMatrix, bool* visibiltyMatrix, int row, bool direction )
{
    int currentMax;
    int height;
    int index, stop, delta;
    int result;

    currentMax = -1;

    if (direction == true)
    {
        index = 0;
        stop = kTreeMatrixSize;
        delta = 1;
    }
    else
    {
        index = kTreeMatrixSize-1;
        stop = -1;
        delta = -1;
    }

    result = 0;
    while( index != stop )
    {
        height = MatrixValue(treeMatrix, row, index);
        if (height> currentMax)
        {
            visibiltyMatrix[row * kTreeMatrixSize + index] = true;
            result++;
            currentMax = height;
        }
        index += delta;
    }

    return result;
}

int ScanColumn(int* treeMatrix, bool* visibiltyMatrix, int column, bool direction)
{
    int currentMax;
    int height;
    int index, stop, delta;
    int result;

    currentMax = -1;

    if (direction == true)
    {
        index = 0;
        stop = kTreeMatrixSize;
        delta = 1;
    }
    else
    {
        index = kTreeMatrixSize - 1;
        stop = -1;
        delta = -1;
    }

    result = 0;
    while (index != stop)
    {
        height = MatrixValue(treeMatrix, index, column);
        if( height > currentMax)
        {
            visibiltyMatrix[index * kTreeMatrixSize + column] = true;
            result++;
            currentMax = height;
        }
        index += delta;
    }

    return result;
}

int ScenicRow(int* treeMatrix, int row, int column, bool direction)
{
    int height;
    int index, stop, delta;
    int result;
    int currentHeight;

    if (direction == true)
    {
        index = row+1;
        stop = kTreeMatrixSize;
        delta = 1;
    }
    else
    {
        index = row-1;
        stop = -1;
        delta = -1;
    }

    result = 1;
    height = MatrixValue(treeMatrix, row, column);
    while (index != stop)
    {
        currentHeight = MatrixValue(treeMatrix, index, column);
        result++;
        index += delta;
        if (height <= currentHeight)
            break;
    }

    result--;

    return result;
}

int ScenicColumn(int* treeMatrix, int row, int column, bool direction)
{
    int height;
    int index, stop, delta;
    int result;
    int currentHeight;

    if (direction == true)
    {
        index = column+1;
        stop = kTreeMatrixSize;
        delta = 1;
    }
    else
    {
        index = column-1;
        stop = -1;
        delta = -1;
    }

    result = 1;
    height = MatrixValue(treeMatrix, row, column);
    while (index != stop)
    {
        currentHeight = MatrixValue(treeMatrix, row, index);
        result++;
        index += delta;
        if (height <= currentHeight)
            break;
    }

    result--;

    return result;
}

int ScenicScore(int* treeMatrix, int row, int column)
{
    return ScenicColumn(treeMatrix, row, column, false) *
        ScenicColumn(treeMatrix, row, column, true) *
        ScenicRow(treeMatrix, row, column, false) *
        ScenicRow(treeMatrix, row, column, true);
}

int main()
{
    FILE* input;
    char buffer[100];
    errno_t error;
    int treeMatrix[kTreeMatrixSize * kTreeMatrixSize];
    bool visibilityMatrix[kTreeMatrixSize * kTreeMatrixSize];
    int row, column, index;
    int* ptrMatrix;
    bool* ptrVisibility;
    char* ptrBuffer;
    int result;
    int value;
    int maxScore;
    int score;

    printf( "Advent of Code - Day 8\n" );

    error = fopen_s( & input, fileName, "r" );

    printf( "Opening file %s => %d\n", fileName, error );

    if( error != 0 )
        return error;

    row = 0;
    column = 0;
    ptrMatrix = treeMatrix;
    ptrVisibility = visibilityMatrix;
    while( ReadLine(input,buffer)>0)
    {
        ptrBuffer = buffer;
        for (column = 0; column < kTreeMatrixSize; column++)
        {
            sscanf_s(ptrBuffer, "%1d", ptrMatrix);
            *ptrVisibility = false;
            ptrBuffer++;
            ptrMatrix++;
            ptrVisibility;
        }
    }

    result = 0;
    for (index = 0; index < kTreeMatrixSize; index++)
    {
        result += ScanColumn(treeMatrix, visibilityMatrix, index, false);
        result += ScanColumn(treeMatrix, visibilityMatrix, index, true);
        result += ScanRow(treeMatrix, visibilityMatrix, index, false);
        result += ScanRow(treeMatrix, visibilityMatrix, index, true);
    }

    printf("Matches: %d\n", result);

    result = 0;
    for (row = 0; row < kTreeMatrixSize; row++)
        for (column = 0; column < kTreeMatrixSize; column++)
            if (visibilityMatrix[row * kTreeMatrixSize + column] == true)
                result++;

    printf("Result step 1: %d\n", result);

    maxScore = 0;
    for (row = 1; row < (kTreeMatrixSize-1); row++)
        for (column = 1; column < (kTreeMatrixSize); column++)
        {
            score = ScenicScore(treeMatrix, row, column);

            if (score > maxScore)
                maxScore = score;
        }

    printf("Result step 2: %d\n", maxScore);

    return 0;
}