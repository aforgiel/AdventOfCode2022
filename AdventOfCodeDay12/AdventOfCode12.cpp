// AdventOfCodeDay12.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true )
    #define WIDTH 8
    #define HEIGHT 5
    const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay12/sample.txt";
#else
    #define WIDTH 154
    #define HEIGHT 41
    const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay12/input.txt";
#endif

char map[WIDTH * HEIGHT];
int visit[WIDTH * HEIGHT];

#define kNoVisit -1
    
typedef struct TPoint* TPointPtr;

struct TPoint {
    int x;
    int y;
};

void InitPoint(TPointPtr point, TPointPtr source = NULL )
{
    if (source == NULL)
    {
        point->x = 0;
        point->y = 0;

        return;
    }

    point->x = source->x;
    point->y = source->y;
}

void AddPoint(TPointPtr a, TPointPtr b)
{
    a->x += b->x;
    a->y += b->y;
}

enum EDirection {
    eUnknown = -1,
    eUp = 0,
    eDown = 1,
    eLeft = 2,
    eRight = 3
};

typedef struct TMove* TMovePtr;

struct TMove {
    TPoint destination;
    int step;
};

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

#define ARRAY_VALUE( map, x, y ) (map[y*WIDTH+x])

std::vector<TMove> queue;
std::vector<int> result;

bool Explore( TMove move, TPointPtr destination, bool backward = false )
{
    int direction;
    TPoint delta;
    TPoint nextPosition;
    int climb;
    bool possible;

#if( COMMENT == true )
    printf("Explore(%d,%d,%d)\n", move.destination.x, move.destination.y, move.step);
#endif

    if (backward == true)
    {
        if (ARRAY_VALUE(map, move.destination.x, move.destination.y) == 'a')
        {
#if( COMMENT == true )
            printf("\tGOTCHA!!! (%d)\n", move.step);
#endif
            return true;
        }
    }
    else
    {
        if (move.destination.x == destination->x &&
            move.destination.y == destination->y)
        {
#if( COMMENT == true )
            printf("\tGOTCHA!!! (%d)\n", move.step);
#endif
            return true;
        }
    }

    for (direction = 0; direction < 4; direction++)
    {
        InitPoint(&delta);
        possible = false;
        switch (direction)
        {
        case eUp:
            if (move.destination.y > 0)
            {
                delta.y = -1;
                possible = true;
            }
            break;
        case eDown:
            if (move.destination.y < (HEIGHT - 1))
            {
                delta.y = +1;
                possible = true;
            }
            break;
        case eLeft:
            if (move.destination.x > 0)
            {
                delta.x = -1;
                possible = true;
            }
            break;
        case eRight:
            if (move.destination.x < (WIDTH - 1))
            {
                delta.x = +1;
                possible = true;
            }
            break;
        default:
            break;
        }

        if (possible == true)
        {
            InitPoint(&nextPosition, &move.destination);
            AddPoint(&nextPosition, &delta);

            if ((ARRAY_VALUE(visit, nextPosition.x, nextPosition.y) == kNoVisit) ||
                (ARRAY_VALUE(visit, nextPosition.x, nextPosition.y) > (move.step + 1)))
            {
                if ((backward == false &&
                    (ARRAY_VALUE(map, nextPosition.x, nextPosition.y) -
                        ARRAY_VALUE(map, move.destination.x, move.destination.y)) <= 1) ||
                    (backward == true &&
                        (ARRAY_VALUE(map, move.destination.x, move.destination.y) -
                            ARRAY_VALUE(map, nextPosition.x, nextPosition.y)) <= 1))
                {
                    TMove newMove;

                    newMove.destination = nextPosition;
                    newMove.step = move.step + 1;

#if( COMMENT == true )
                    printf("\tadd move (%d,%d,%d)\n", newMove.destination.x, newMove.destination.y, newMove.step);
#endif
                    ARRAY_VALUE(visit, newMove.destination.x, newMove.destination.y) = newMove.step;
                    queue.push_back(newMove);
                }
            }
        }
    }

    return false;
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    char* mapPtr;
    int* visitPtr;
    int x, y;
    TPoint start, destination;
    TMove move;

    printf("Advent of Code - Day 12\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    mapPtr = map;
    while (ReadLine(input, mapPtr) > 0)
    {
        mapPtr += WIDTH;
    }

    printf("Read map:\n");

    mapPtr = map;
    visitPtr = visit;
    for (y = 0; y < HEIGHT; y++)
    {
        for (x = 0; x < WIDTH; x++)
        {
            printf("%c", *mapPtr);
            *visitPtr = kNoVisit;
            switch (*mapPtr)
            {
            case 'S':
                start.x = x;
                start.y = y;
                *mapPtr = 'a';
                break;
            case 'E':
                destination.x = x;
                destination.y = y;
                *mapPtr = 'z';
                break;
            default:
                break;
            }
            mapPtr++;
            visitPtr++;
        }
        printf("\n");
    }

    printf("Start: {%d,%d}\n", start.x, start.y);
    printf("Destination: {%d,%d}\n", destination.x, destination.y);

    clockStart = clock();

    InitPoint(&move.destination, &start);
    move.step = 0;
    ARRAY_VALUE(visit, move.destination.x, move.destination.y) = 0;
    queue.push_back(move);

    while (queue.size() > 0)
    {
        move = queue.back();
        queue.pop_back();
        if( Explore(move, &destination) == true )
            result.push_back(ARRAY_VALUE(visit,move.destination.x,move.destination.y));
    }

    std::sort(result.begin(), result.end());

    if (result.size() > 0)
        printf("Result 1: %d\n", result[0]);
    else
        printf("No result...\n");

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    result.clear();

    visitPtr = visit;
    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++)
        {
            *visitPtr = kNoVisit;
            visitPtr++;
        }

    clockStart = clock();

    InitPoint(&move.destination, &destination);
    move.step = 0;
    ARRAY_VALUE(visit, move.destination.x, move.destination.y) = 0;
    queue.push_back(move);

    while (queue.size() > 0)
    {
        move = queue.back();
        queue.pop_back();
        if( Explore(move, &start, true ) == true )
            result.push_back(ARRAY_VALUE(visit,move.destination.x,move.destination.y));
    }

    std::sort(result.begin(), result.end());

    if (result.size() > 0)
        printf("Result 2: %d\n", result[0]);
    else
        printf("No result...\n");

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    return 0;
}