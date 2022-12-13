// AdventOfCodeDay13.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true )
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay13/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay13/input.txt";
#endif

typedef struct TPacket* TPacketPtr;

struct TPacket {
    std::vector<TPacket> children;
    int value;
    bool divider;
};

#define kNoValue -1
#define kLeft 1
#define kRight 2

void PrintPacket(const TPacket& packet)
{
    if (packet.value != kNoValue)
    {
        printf("%d", packet.value);
    }
    else
    {
        printf("[");
        for (auto it = packet.children.begin(); it != packet.children.end();)
        {
            PrintPacket(*it);
            ++it;
            if (it != packet.children.end())
                printf(",");
        }
        printf("]");
    }
}

int ComparePacket(const TPacket& left, const TPacket& right)
{
#if(COMMENT == true)
    printf("Compare( ");
    PrintPacket(left);
    printf(" , ");
    PrintPacket(right);
    printf(" )\n");
#endif
    if (left.value != kNoValue && right.value != kNoValue)
    {
#if(COMMENT == true)
        printf("values: left = %d, right = %d\n", left.value, right.value);
#endif
        if (left.value == right.value)
            return 0;
        if (left.value < right.value)
            return -1;

        return 1;
    }

    if (left.value == kNoValue && right.value == kNoValue)
    {
        int result;
        int index;

#if(COMMENT == true)
        printf("list case\n");
#endif
        auto leftIt = left.children.begin();
        auto rightIt = right.children.begin();
        result = 0;
        index = 1;

        while (leftIt != left.children.end() && rightIt != right.children.end())
        {
#if(COMMENT == true)
            printf("\t[%d] ", index);
            PrintPacket(*leftIt);
            printf(" vs. ");
            PrintPacket(*rightIt);
            printf("\n");
#endif
            result = ComparePacket(*leftIt, *rightIt);

            if (result != 0)
                break;

            ++leftIt;
            ++rightIt;
            index++;
        }

        if (rightIt == right.children.end() &&
            leftIt != left.children.end())
        {
#if(COMMENT == true)
            printf("\tRight ran out of items\n");
#endif
            return +1;
        }
        if (leftIt == left.children.end() &&
            rightIt != right.children.end())
        {
#if(COMMENT == true)
            printf("\tLeft ran out of items\n");
#endif
            return -1;
        }

        return result;
    }

    if (left.value != kNoValue)
    {
#if(COMMENT == true)
        printf("\tMake left as list\n");
#endif
        TPacket newLeft;

        newLeft.children.push_back(left);
        newLeft.value = kNoValue;

        return ComparePacket(newLeft, right);
    }

    if (right.value != kNoValue)
    {
#if(COMMENT == true)
        printf("\tMake right as list\n");
#endif
        TPacket newRight;

        newRight.children.push_back(right);
        newRight.value = kNoValue;

        return ComparePacket(left, newRight);
    }

    return 0;
}

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
        result++;
        if (*ptr == '\n')
        {
            *ptr = NULL;
            return result;
        }
        ptr++;
    }

    return result;
}

bool ReadValue(TPacketPtr packet, char* buffer)
{
    int intValue;
    sscanf_s(buffer, "%d", &intValue);

    TPacket newValue;

    newValue.value = intValue;
    packet->children.push_back(newValue);

    return true;
}

TPacketPtr ReadPacket(FILE* input, char first)
{
    char buffer[20];
    char* ptr;
    char value;
    TPacketPtr result;

    result = NULL;
    ptr = buffer;

    if (first != EOF)
        value = first;
    else
        value = fgetc(input);

    while (value != EOF)
    {
        switch (value)
        {
        case '[':
            if (result == NULL)
            {
                result = new TPacket();
                result->divider = false;
                result->value = kNoValue;
            }
            else
                result->children.push_back(*ReadPacket(input, value));
            break;

        case ']':
            if (ptr != buffer)
            {
                *ptr = NULL;
                ReadValue(result, buffer);
                ptr = buffer;
            }
            return result;

        case ',':
            if (ptr != buffer)
            {
                *ptr = NULL;
                ReadValue(result, buffer);
                ptr = buffer;
            }
            break;

        case EOF:
            break;

        case '\n':
        case '\t':
        case ' ':
            break;

        default:
            *ptr = value;
            ptr++;
            break;
        }

        value = fgetc(input);
    }

    return result;
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    int result;
    int index;
    int compare;

    printf("Advent of Code - Day 13\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    // Step 1

    TPacketPtr leftPacket, rightPacket;
    index = 1;
    result = 0;

    clockStart = clock();

    do {
        leftPacket = ReadPacket(input, EOF);
        if (leftPacket == NULL)
            break;

#if(COMMENT == true)
        printf("Case %d:\n", index);
        printf("\tLeft packet: ");
        PrintPacket(*leftPacket);
        printf("\n");
#endif
        rightPacket = ReadPacket(input, EOF);
        if (rightPacket == NULL)
            break;

#if(COMMENT == true)
        printf("\tRight packet: ");
        PrintPacket(*rightPacket);
        printf("\n");
#endif

        compare = ComparePacket(*leftPacket, *rightPacket);
#if(COMMENT == true)
        printf("\t=> compare: %d\n", compare);
#endif

        if (compare == -1)
            result += index;
        index++;

        delete leftPacket;
        delete rightPacket;

    } while (true);

    printf("Result 1: %d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    // Step 2

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    std::vector<TPacketPtr> packets;
    TPacketPtr packet;

    clockStart = clock();

    index = 1;
    do {
        packet = ReadPacket(input, EOF);
        if (packet == NULL)
            break;
        packets.push_back(packet);
        index++;
    } while (true);

    packet = new TPacket();
    packet->divider = true;
    packet->value = 2;
    packets.push_back(packet);

    packet = new TPacket();
    packet->divider = true;
    packet->value = 6;
    packets.push_back(packet);

#if (COMMENT == true)
    printf("Input:\n");
    index = 1;
    for (auto it = packets.begin(); it != packets.end(); ++it)
    {
        printf("[%d]: ", index);
        PrintPacket(**it);
        printf("\n");
        index++;
    }
#endif

    std::sort(packets.begin(), packets.end(), [](TPacketPtr a, TPacketPtr b) {
        if (ComparePacket(*a, *b) == -1)
        return true;
    return false;
        });

#if (COMMENT == true)
    printf("Output:\n");
#endif
    index = 1;
    result = 1;
    for (auto it = packets.begin(); it != packets.end(); ++it)
    {
#if (COMMENT == true)
        printf("[%d]: ", index);
        PrintPacket(**it);
        printf("\n");
#endif
        if ((*it)->divider == true)
            result *= index;

        index++;
    }

    printf("Result 2: %d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    return 0;
}