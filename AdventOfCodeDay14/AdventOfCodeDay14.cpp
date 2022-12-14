// AdventOfCodeDay14.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true )
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay14/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay14/input.txt";
#endif

#define SOURCE_X 500
#define SOURCE_Y 0

typedef struct TPoint* TPointPtr;
struct TPoint {
    int x;
    int y;
};

typedef std::vector<TPoint> TPath;
typedef TPath* TPathPtr;

typedef std::vector<TPath> TScan;
typedef TScan* TScanPtr;

void InitPoint(TPoint& point, TPointPtr source = NULL)
{
    if (source == NULL)
    {
        point.x = 0;
        point.y = 0;

        return;
    }

    point.x = source->x;
    point.y = source->y;
}

void PrintPoint(const TPoint& data)
{
    printf("(%d,%d)", data.x, data.y);
}

void PrintPath(const TPath& data)
{
    int index;

    index = 0;
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        if (index > 0)
            printf(" -> ");
        PrintPoint(*it);
        index++;
    }
    printf("\n");
}

void PrintScan(const TScan& data)
{
    int index;

    index = 0;
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        printf("[%d] ", index);
        PrintPath(*it);
        index++;
    }
}

bool FindPattern(char** buffer, const char* pattern)
{
    char* ptrPattern;

    ptrPattern = const_cast<char*>(pattern);

    while (*ptrPattern != NULL)
    {
        if (*ptrPattern != **buffer)
            return false;

        (*buffer)++;
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

bool ReadPoint(char** ptr, TPoint& point)
{
    sscanf_s(*ptr, "%d,%d", &point.x, &point.y);

    while (**ptr != ' ' && **ptr != NULL)
    {
        (*ptr)++;
    }

    return true;
}

bool ReadPath(char* buffer, TPath& path)
{
    char* ptr;
    TPoint point;

    ptr = buffer;
    while (*ptr != NULL)
    {
        ReadPoint(&ptr, point);
        path.push_back(point);
        if (FindPattern(&ptr, " -> ") == false)
            break;
    }

    return true;
}

bool ReadScan(FILE* file, TScan& scan)
{
    char buffer[256];
    TPath path;

    while (ReadLine(file, buffer) > 0)
    {
        path.clear();
        ReadPath(buffer, path);
        scan.push_back(path);
    }

    return true;
}

bool FindMinMax(TScan& scan, TPoint& min, TPoint& max)
{
    for (auto pathIt = scan.begin(); pathIt != scan.end(); ++pathIt)
        for (auto it = (*pathIt).begin(); it != (*pathIt).end(); ++it)
        {
            if (min.x > (*it).x)
                min.x = (*it).x;
            if (min.y > (*it).y)
                min.y = (*it).y;

            if (max.x < (*it).x)
                max.x = (*it).x;
            if (max.y < (*it).y)
                max.y = (*it).y;
        }

    return true;
}

bool ClearMap(char* map, int width, int height)
{
    int index = 0;
    int end = width * height;

    while (index < end)
    {
        *map = '.';
        map++;
        index++;
    }

    return true;
}

void PrintMap(char* map, int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++)
    {
        printf("[%03d] ", y);
        for (x = 0; x < width; x++)
        {
            printf("%c", *map);
            map++;
        }
        printf("\n");
    }
}

bool BuildMap(char* map, TScan& scan, TPoint min, TPoint max, int width, bool floor)
{
    TPoint position;
    TPoint end;
    TPoint delta;
    int x, y;

    for (auto pathIt = scan.begin(); pathIt != scan.end(); ++pathIt)
    {
        auto it = (*pathIt).begin();
        position = *it;
        ++it;

        while (it != (*pathIt).end())
        {
            end = (*it);
            InitPoint(delta);
            if (position.x > end.x)
                delta.x = -1;
            else if (position.x < end.x)
                delta.x = +1;

            if (position.y > end.y)
                delta.y = -1;
            else if (position.y < end.y)
                delta.y = +1;

            do {
                x = position.x - min.x;
                y = position.y - min.y;
                map[x + y * width] = '#';

                position.x += delta.x;
                position.y += delta.y;

            } while (position.x != end.x || position.y != end.y);

            x = position.x - min.x;
            y = position.y - min.y;
            map[x + y * width] = '#';

            ++it;
        }
    }

    // Don't forget the source:
    x = SOURCE_X - min.x;
    y = SOURCE_Y - min.y;
    map[x + y * width] = '+';

    if (floor == true)
        for (x = 0; x < width; x++)
            map[x + max.y * width] = '#';

    return true;
}

bool ProduceSand(char* map, TPoint min, TPoint max, int width)
{
    TPoint position;
    TPoint nextPosition;
    int count;

    nextPosition.x = SOURCE_X;
    nextPosition.y = SOURCE_Y;
    count = 0;

#if( COMMENT == true)
    printf("ProduceSand()\n");
#endif

    do {
        InitPoint(position, &nextPosition);

#if( COMMENT == true)
        printf("\t[%d] ", count);
        PrintPoint(position);
        count++;
#endif

        if (nextPosition.y == max.y)
        {
#if( COMMENT == true)
            printf(" => Down abyss\n");
#endif
            return false;
        }

        nextPosition.y++;

        if (map[(nextPosition.x - min.x) + (nextPosition.y - min.y) * width] != '.')
        {
            if (nextPosition.x == min.x)
            {
#if( COMMENT == true)
                printf(" => Left abyss\n");
#endif
                return false;
            }

            nextPosition.x--;

            if (map[(nextPosition.x - min.x) + (nextPosition.y - min.y) * width] != '.')
            {
                if (nextPosition.x == (max.x - 1))
                {
#if( COMMENT == true)
                    printf(" => Right abyss\n");
#endif
                    return false;
                }

                nextPosition.x += 2;
                if (map[(nextPosition.x - min.x) + (nextPosition.y - min.y) * width] != '.')
                {
#if( COMMENT == true)
                    printf(" => Stuck\n");
#endif
                    map[(position.x - min.x) + (position.y - min.y) * width] = 'o';

                    if (position.x == SOURCE_X && position.y == SOURCE_Y)
                        return false;

                    return true;
                }
#if( COMMENT == true)
                else
                {
                    printf(" => Fall right\n");
                }
#endif
            }
#if( COMMENT == true)
            else
            {
                printf(" => Fall left\n");
            }
#endif
        }

#if( COMMENT == true)
        else
        {
            printf(" => Fall vertically\n");
        }
#endif

    } while (true);

    return false;
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    int result;
    TScan scan;
    TPoint min, max;
    int width, height;
    char* map;

    printf("Advent of Code - Day 14\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    // Step 1

    ReadScan(input, scan);
    PrintScan(scan);
    min.x = 500;
    min.y = 0;
    max.x = INT_MIN;
    max.y = INT_MIN;
    FindMinMax(scan, min, max);

    printf("Min: ");
    PrintPoint(min);
    printf("\nMax: ");
    PrintPoint(max);

    width = max.x - min.x + 1;
    height = max.y - min.y + 1;
    printf("\nwidth: %d, height: %d\n", width, height);

    map = new char[width * height];

    ClearMap(map, width, height);
    BuildMap(map, scan, min, max, width, false);
#if( COMMENT == true )
    printf("Initial map:\n");
    PrintMap(map, width, height);
#endif

    clockStart = clock();

    result = 0;
    while (ProduceSand(map, min, max, width) == true)
    {
        result++;
#if( COMMENT == true )
        switch (result)
        {
        case 1:
        case 2:
        case 5:
        case 22:
        case 23:
        case 24:
            printf("Step: %d\n", result);
            PrintMap(map, width, height);
            break;
        default:
            break;
        }
#endif
    }

    printf("Final map:\n");
    PrintMap(map, width, height);
    printf("result 1: %d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    // Step 2

    delete map;

    int halfWidth;

    max.y += 2;
    halfWidth = max.y - min.y + 1;

    min.x = SOURCE_X - halfWidth;
    max.x = SOURCE_X + halfWidth;

    printf("Min: ");
    PrintPoint(min);
    printf("\nMax: ");
    PrintPoint(max);

    width = max.x - min.x + 1;
    height = max.y - min.y + 1;
    printf("\nwidth: %d, height: %d\n", width, height);

    map = new char[width * height];

    ClearMap(map, width, height);
    BuildMap(map, scan, min, max, width, true);
#if( COMMENT == true )
    printf("Initial map:\n");
    PrintMap(map, width, height);
#endif

    clockStart = clock();

    result = 1;
    while (ProduceSand(map, min, max, width) == true)
        result++;

    printf("Final map:\n");
    PrintMap(map, width, height);
    printf("Result 2: %d\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    return 0;
}