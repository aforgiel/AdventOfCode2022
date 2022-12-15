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
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay15/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay15/input.txt";
#endif

typedef struct TPoint* TPointPtr;
struct TPoint {
    int64_t x;
    int64_t y;
};

#define kSENSOR 0
#define kBEACON 1

typedef struct TSensor* TSensorPtr;
struct TSensor {
    TPoint point[2];
    int64_t distance;
};

typedef struct TRaster* TRasterPtr;
struct TRaster {
    int64_t min;
    int64_t max;
    TRasterPtr next;
};

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
    printf("(%lld,%lld)", data.x, data.y);
}

int64_t Distance(const TPoint& a, const TPoint& b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int64_t RasterLength(TRaster& raster)
{
    TRasterPtr tmp;
    int64_t result;

    result = 0;
    tmp = &raster;
    while (tmp != NULL)
    {
        result += tmp->max - tmp->min + 1;
        tmp = tmp->next;
    }

    return result;
}

void PrintRaster(TRaster& raster)
{
    TRasterPtr tmp;

    tmp = &raster;
    while (tmp != NULL)
    {
        if (tmp != &raster)
            printf("-");
        printf("[%lld,%lld]", tmp->min, tmp->max);
        tmp = tmp->next;
    }
    printf(" (%lld)\n", RasterLength(raster));
}

void DeleteRaster(TRasterPtr* raster)
{
    if ((*raster)->next != NULL)
        DeleteRaster(&(*raster)->next);

    delete* raster;
    *raster = NULL;
}

void PrintSensor(const TSensor& data)
{
    printf("Position: ");
    PrintPoint(data.point[kSENSOR]);
    printf(" Beacon: ");
    PrintPoint(data.point[kBEACON]);
    printf(" Distance: %lld\n", data.distance);
}

bool FindPattern(char** buffer, const char* pattern)
{
    char* ptrPattern;
    bool result;

    ptrPattern = const_cast<char*>(pattern);
    result = false;

    while ((*buffer) != NULL && *ptrPattern != NULL)
    {
        result = (*ptrPattern == **buffer);
        if (result == true)
            ptrPattern++;
        else
        {
            *buffer -= ptrPattern - pattern;
            ptrPattern = const_cast<char*>(pattern);
        }
        (*buffer)++;
    }

    return result;
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

const char* kSensorPattern = "Sensor at x=";
const char* kPointPattern = ", y=";
const char* kBeaconPattern = ": closest beacon is at x=";

TSensorPtr ReadSensorData(const char* buffer)
{
    char* ptr = const_cast<char*>(buffer);

    TSensorPtr result;

    result = new TSensor();

    InitPoint(result->point[kSENSOR]);
    InitPoint(result->point[kBEACON]);

    // Trust the system...
    FindPattern(&ptr, kSensorPattern);
    sscanf_s(ptr, "%lld", &result->point[kSENSOR].x);
    FindPattern(&ptr, kPointPattern);
    sscanf_s(ptr, "%lld", &result->point[kSENSOR].y);
    FindPattern(&ptr, kBeaconPattern);
    sscanf_s(ptr, "%lld", &result->point[kBEACON].x);
    FindPattern(&ptr, kPointPattern);
    sscanf_s(ptr, "%lld", &result->point[kBEACON].y);

    result->distance = Distance(result->point[kSENSOR], result->point[kBEACON]);

    return result;
}

bool FindMinMax(std::vector<TSensorPtr>& sensors, TPoint& min, TPoint& max)
{
    int index;

    for (auto it = sensors.begin(); it != sensors.end(); ++it)
        for (index = 0; index < 2; index++)
        {
            if (min.x > (*it)->point[index].x)
                min.x = (*it)->point[index].x;
            if (min.y > (*it)->point[index].y)
                min.y = (*it)->point[index].y;

            if (max.x < (*it)->point[index].x)
                max.x = (*it)->point[index].x;
            if (max.y < (*it)->point[index].y)
                max.y = (*it)->point[index].y;
        }

    return true;
}

TRasterPtr InsertRaster(TRasterPtr first, int64_t min, int64_t max)
{
    TRasterPtr result;

    result = NULL;

    if (first == NULL)
    {
        result = new TRaster();
        result->min = min;
        result->max = max;
        result->next = NULL;

        return result;
    }

    TRasterPtr ptr;
    TRasterPtr previous;

    ptr = first;
    previous = NULL;
    while (ptr != NULL)
    {

        if (max < ptr->min)
        {
            // before case
            TRasterPtr result;

            result = new TRaster();
            result->min = min;
            result->max = max;
            result->next = ptr;
            if (previous != NULL)
            {
                previous->next = result;
                return first;
            }

            return result;
        }

        if (min <= ptr->max)
        {
            // Overlap case
            TRasterPtr end;

            end = ptr;
            while (ptr != NULL)
            {
                if (ptr->min > max)
                    break;
                end->next = ptr->next;
                if (ptr != end)
                {
                    end->max = ptr->max;
                    delete ptr;
                }
                ptr = end->next;
            }

            end->min = (min < end->min) ? min : end->min;
            end->max = (max > end->max) ? max : end->max;

            return first;
        }

        previous = ptr;
        ptr = ptr->next;
    }

    result = new TRaster();
    result->min = min;
    result->max = max;
    result->next = NULL;
    previous->next = result;

    return first;
}

bool Intersect(int64_t y, TSensor& sensor, int64_t& xmin, int64_t& xmax)
{
    int64_t height;

    height = abs(sensor.point[kSENSOR].y - y);
    if (height > sensor.distance)
        return false;

    xmin = sensor.point[kSENSOR].x - sensor.distance + height;
    xmax = sensor.point[kSENSOR].x + sensor.distance - height;

    return true;
}

bool RemoveFromRaster(TRasterPtr* raster, int64_t value)
{
    TRasterPtr ptr;
    TRasterPtr previous;

    ptr = *raster;
    previous = NULL;
    while (ptr != NULL)
    {
        if (ptr->min > value)
            break;
        if (ptr->min <= value && value <= ptr->max)
        {
            if (ptr->min == value && ptr->max == value)
            {
                if (previous != NULL)
                    previous->next = ptr->next;
                else
                    *raster = ptr->next;
                delete ptr;
                return true;
            }

            if (ptr->min == value)
            {
                ptr->min++;
                return true;
            }

            if (ptr->max == value)
            {
                ptr->max--;
                return true;
            }

            TRasterPtr newRaster;

            newRaster = new TRaster();

            newRaster->min = value + 1;
            newRaster->max = ptr->max;
            newRaster->next = ptr->next;

            ptr->max = value - 1;
            ptr->next = newRaster;

            return true;
        }
        previous = ptr;
        ptr = ptr->next;
    }

    return true;
}

int64_t FindFirstHole(TRaster& raster, int64_t maxCoordinate)
{
    if (raster.min == 1)
        return 0;

    if (raster.max == maxCoordinate - 1)
        return maxCoordinate;

    return raster.max + 1;
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    int64_t result;
    char buffer[200];
    std::vector<TSensorPtr> sensors;
    int counter;
    TSensorPtr sensor;
    TPoint min, max;
    int64_t width, height;

    printf("Advent of Code - Day 15\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    // Step 1

    counter = 1;
    while (ReadLine(input, buffer) > 0)
    {
        sensor = ReadSensorData(buffer);
        if (sensor == NULL)
            break;
        printf("[%d] ", counter);
        PrintSensor(*sensor);
        sensors.push_back(sensor);
        counter++;
    }

    min.x = INT_MAX;
    min.y = INT_MAX;;
    max.x = INT_MIN;
    max.y = INT_MIN;
    FindMinMax(sensors, min, max);

    printf("Min: ");
    PrintPoint(min);
    printf("\nMax: ");
    PrintPoint(max);
    printf("\n");

    width = max.x - min.x + 1;
    height = max.y - min.y + 1;
    printf("\nwidth: %lld, height: %lld\n", width, height);

    clockStart = clock();

    TRasterPtr raster;
    int64_t xmin, xmax;
    int64_t y;
    int index;

#if( TEST_MODE == true)
    y = 10;
#else
    y = 2000000;
#endif
    printf("y=%lld: ", y);
    raster = NULL;
    for (auto it = sensors.begin(); it != sensors.end(); ++it)
        if (Intersect(y, **it, xmin, xmax) == true)
            raster = InsertRaster(raster, xmin, xmax);

    for (auto it = sensors.begin(); it != sensors.end(); ++it)
        for (index = 0; index < 2; index++)
            if ((*it)->point[index].y == y)
                RemoveFromRaster(&raster, (*it)->point[index].x);

    PrintRaster(*raster);
    result = RasterLength(*raster);
    DeleteRaster(&raster);

    printf("result 1: %lld\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    // Step 2
    int64_t maxCoordinate;

    clockStart = clock();

#if( TEST_MODE == true)
    maxCoordinate = 20;
#else
    maxCoordinate = 4000000;
#endif

    for (y = 0; y <= maxCoordinate; y++)
    {
        raster = NULL;
        for (auto it = sensors.begin(); it != sensors.end(); ++it)
            if (Intersect(y, **it, xmin, xmax) == true)
            {
                if (xmax >= 0 && xmin <= maxCoordinate)
                {
                    xmin = (xmin > 0) ? xmin : 0;
                    xmax = (xmax < maxCoordinate) ? xmax : maxCoordinate;
                    raster = InsertRaster(raster, xmin, xmax);
                }
            }

        if (RasterLength(*raster) == maxCoordinate)
        {
            printf("[%lld] :", y);
            PrintRaster(*raster);
            printf(" => GOTCHA!\n");
            break;
        }

        DeleteRaster(&raster);
    }

    if (raster == NULL)
    {
        printf("No result found...\n");
        return 0;
    }

    result = FindFirstHole(*raster, maxCoordinate);
    result *= 4000000;
    result += y;
    DeleteRaster(&raster);

    printf("Result 2: %lld\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    return 0;
}