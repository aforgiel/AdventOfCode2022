// AdventOfCodeDay9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>

const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay9/input.txt";

#define kRopeSize 10
#define kHeadIndex 0
#define kTailIndex (kRopeSize-1)

struct TPoint;
typedef TPoint * TPointPtr;

struct TPoint {
    int x;
    int y;
};

enum EDirection {
    eUnknown,
    eUp,
    eDown,
    eLeft,
    eRight
};

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

void PrintRope(TPointPtr rope)
{
    int index;
    int x, y;
    bool empty;

    TPoint min, max;

    min.x = max.x = rope[kHeadIndex].x;
    min.y = max.y = rope[kHeadIndex].y;

    for (index = 1; index < kRopeSize; index++)
    {
        if (min.x > rope[index].x)
            min.x = rope[index].x;
        if (min.y > rope[index].y)
            min.y = rope[index].y;
        if (max.x < rope[index].x)
            max.x = rope[index].x;
        if (max.y < rope[index].y)
            max.y = rope[index].y;
    }

    min.x--;
    min.y--;
    max.x++;
    max.y++;

    for (y = max.y; y >= min.y; y--)
    {
        for (x = min.x; x <= max.x; x++)
        {
            empty = true;
            for( index = 0; index < kRopeSize; index++ )
                if (rope[index].x == x && rope[index].y == y)
                {
                    if( index == 0 )
                        printf("H");
                    else
                        printf("%1d", index);
                    empty = false;
                    break;
                }

            if (empty == true)
                printf(".");
        }
        printf("\n");
    }
}

bool MoveHead(int direction, TPointPtr head)
{
    TPoint headDelta;

    headDelta.x = 0;
    headDelta.y = 0;

    switch (direction)
    {
    case eUp:
        headDelta.y = 1;
        break;
    case eDown:
        headDelta.y = -1;
        break;
    case eLeft:
        headDelta.x = -1;
        break;
    case eRight:
        headDelta.x = 1;
        break;
    default:
        return false;
    }

    head->x += headDelta.x;
    head->y += headDelta.y;

    return true;
}

bool MoveRope( TPointPtr rope, int index )
{
    TPoint delta;
    TPoint tailDelta;
    bool tailMoved;

    delta.x = rope[index].x - rope[index+1].x;
    delta.y = rope[index].y - rope[index+1].y;

    tailDelta.x = 0;
    tailDelta.y = 0;

    tailMoved = false;

    if (abs(delta.x) > 1)
    {
        if( delta.x > 0 )
            tailDelta.x = 1;
        else
            tailDelta.x = -1;

        if (abs(delta.y) > 0)
        {
            if (delta.y > 0)
                tailDelta.y = +1;
            else
                tailDelta.y = -1;
        }
        tailMoved = true;
    }
    else if(abs(delta.y) > 1)
    {
        if (delta.y > 0)
            tailDelta.y = 1;
        else
            tailDelta.y = -1;

        if (abs(delta.x) > 0)
        {
            if (delta.x > 0)
                tailDelta.x = +1;
            else
                tailDelta.x = -1;
        }
        tailMoved = true;
    }

    if( tailMoved == true )
    {
        rope[index+1].x += tailDelta.x;
        rope[index+1].y += tailDelta.y;
    }

    return tailMoved;
}

int RecordHistory( std::list<TPoint> * history, TPointPtr tail )
{
    bool found;

    // Search tail history:
    found = false;
    for (auto it = history->begin(); it != history->end(); ++it)
    {
        if ((*it).x == tail->x &&
            (*it).y == tail->y)
        {
            found = true;
            break;
        }
    }

    if (found == false)
        history->push_back(*tail);

    return history->size();
}

int PerformMoveRope( char * buffer, TPointPtr rope, std::list<TPoint> * history )
{
    char dir;
    int distance;
    int direction;

    dir = buffer[0];

    printf("Input: %s\n", buffer);

    sscanf_s(&buffer[2], "%d", &distance);

    switch( dir )
    {
    case 'U':
        direction = eUp;
        break;
    case 'D':
        direction = eDown;
        break;
    case 'L':
        direction = eLeft;
        break;
    case 'R':
        direction = eRight;
        break;
    default:
        printf("Unsuported move\n");
        return -1;
    }

    int index;
    int knotIndex;

    for ( index = 0; index < distance; index++)
    {
        if (MoveHead(direction, &rope[kHeadIndex]) == true)
        {
            for (knotIndex = 0; knotIndex < kTailIndex; knotIndex++)
                if (MoveRope(rope, knotIndex) == false)
                    break;
            RecordHistory(history, &rope[kTailIndex]);
        }
    }

    PrintRope(rope);

    return distance;
}

int main()
{
    FILE* input;
    char buffer[100];
    errno_t error;
    std::list<TPoint> history;
    int result;

    printf("Advent of Code - Day 9\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    TPoint rope[kRopeSize];
    int tailIndex;

    tailIndex = kRopeSize - 1;

    for (auto index = 0; index < kRopeSize; index++)
    {
        rope[index].x = 0;
        rope[index].y = 0;
    }

    RecordHistory(&history, &rope[tailIndex]);

    while (ReadLine(input, buffer) > 0)
        PerformMoveRope(buffer, rope, &history);

    result = history.size();

    printf("Answer: %d\n", result);

    return result;
}