// AdventOfCodeDay16.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <deque>

#define TEST_MODE false
#define COMMENT false

#if( TEST_MODE == true )
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay16/sample.txt";
#else
const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay16/input.txt";
#endif

#define kMaxTime 30
#define kNullID -1

typedef struct TValve* TValvePtr;
typedef struct THalfEdge* THalfEdgePtr;

struct THalfEdge {
    THalfEdgePtr twin;
    TValvePtr destination;
    THalfEdgePtr next;
    THalfEdgePtr previous;
    int id;
};

struct TStep {
    TValvePtr origin;
    THalfEdgePtr halfEdge;
    int orientation;
    int count;
};

struct TValve {
    std::string name;
    int flowRate;
    std::vector<std::string> tunnels;
    std::vector<THalfEdgePtr> halfEdges;
    int distance;
    TStep step;
    int score;
};

typedef std::map<std::string, TValvePtr> TScan;
typedef TScan* TScanPtr;

TScan scan;
int maxID = 1;

#define kOpenValve 1
#define kMoveToValve 2
#define kDontMove 3
#define kUnknown -1

typedef struct TAction* TActionPtr;
struct TAction {
    int what;
    TValvePtr valve;
};

typedef struct TPath* TPathPtr;
struct TPath {
    int score;
    std::deque<TAction> actions;
    std::set<TValvePtr> openValves;
};

void PrintValve(TValvePtr valve)
{
    TValvePtr child;
    int index;

    printf("Valve \"%s\", flow rate: %d, score: %d\n", valve->name.c_str(), valve->flowRate, valve->score);
    index = 0;
    for (auto it = valve->tunnels.begin(); it != valve->tunnels.end(); ++it)
    {
        child = (*scan.find(*it)).second;
        printf("\t[%d]: %s\n", index, child->name.c_str());
        index++;
    }
}

void PrintScan(TScan& scan)
{
    TValvePtr child;
    int index;

    index = 0;
    for (auto it = scan.begin(); it != scan.end(); ++it)
    {
        child = (*it).second;
        printf("[%d]: ", index);
        PrintValve(child);
        index++;
    }
}

void PrintPath(TPath& path)
{
    int index;
    int score;
    int cumulativeFlow;

    score = 0;
    cumulativeFlow = 0;
    index = 1;
    for (auto it = path.actions.begin(); it != path.actions.end(); ++it)
    {
        score += cumulativeFlow;
        printf("\t[%d] ", index);
        switch ((*it).what)
        {
        case kOpenValve:
            printf("release %d, open valve %s : sum %d\n", cumulativeFlow, (*it).valve->name.c_str(), score);
            cumulativeFlow += (*it).valve->flowRate;
            break;
        case kMoveToValve:
            printf("release %d, move to valve %s : sum %d\n", cumulativeFlow, (*it).valve->name.c_str(), score);
            break;
        default:
            printf("unknown\n");
            break;
        }
        index++;
    }

    while (index <= kMaxTime)
    {
        score += cumulativeFlow;
        printf("\t[%d] release %d : sum %d\n", index, cumulativeFlow, score);
        index++;
    }

    printf("Total release: %d\n", score);
    printf("Number of open valves: %d\n", (int)path.openValves.size());
    index = 1;
    for (auto it = path.openValves.begin(); it != path.openValves.end(); ++it)
    {
        printf("\t[%d] %s\n", index, (*it)->name.c_str());
        index++;
    }
}

bool FindPattern(char** buffer, const char* pattern)
{
    char* ptrPattern;
    bool result;

    ptrPattern = const_cast<char*>(pattern);
    result = false;

    while ((**buffer) != NULL && *ptrPattern != NULL)
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

    *ptr = NULL;
    return result;
}

bool ReadString(char** buffer, char* string, int size)
{
    char* ptr;
    int counter;

    ptr = string;
    counter = 0;

    while (counter < size && **buffer != NULL)
    {
        *ptr = **buffer;
        (*buffer)++;
        ptr++;
        counter++;
    }

    if (counter != size)
        return false;

    string[counter] = 0;

    return true;
}

const char* kNamePattern = "Valve ";
const char* kFlowRatePattern = " has flow rate=";
const char* kTunnelPattern = "to valve"; // singular...
const char* kChildPattern = ", ";

TValvePtr ReadValveData(char* buffer)
{
    TValvePtr result;
    char* ptr;
    std::string child;
    char tmp[10];

    ptr = buffer;
    result = new TValve();

    FindPattern(&ptr, kNamePattern);
    ReadString(&ptr, tmp, 2);
    result->name = tmp;
    FindPattern(&ptr, kFlowRatePattern);
    sscanf_s(ptr, "%d", &result->flowRate);
    FindPattern(&ptr, kTunnelPattern);
    // handle the plural case!!!
    if (*ptr == 's')
        ptr += 2;
    else
        ptr++;
    do {
        ReadString(&ptr, tmp, 2);
        child = tmp;
        result->tunnels.push_back(child);
        if (FindPattern(&ptr, kChildPattern) == false)
            break;
    } while (true);

    scan[result->name] = result;

    return result;
}

bool BuildHalfEdges(TValvePtr valve)
{
    int i, j;
    THalfEdgePtr halfEdge;
    THalfEdgePtr twin;
    TValvePtr destination;

    for (i = 0; i < valve->tunnels.size(); i++)
    {
        destination = scan[valve->tunnels[i]];
        halfEdge = NULL;
        for (j = 0; j < valve->halfEdges.size(); j++)
            if (valve->halfEdges[j]->destination == destination)
                halfEdge = valve->halfEdges[j];

        twin = NULL;
        for (j = 0; j < destination->halfEdges.size(); j++)
            if (destination->halfEdges[j]->destination == valve)
            {
                twin = destination->halfEdges[j];
                break;
            }

        if (halfEdge == NULL)
        {
            halfEdge = new THalfEdge();

            halfEdge->destination = destination;
            halfEdge->next = NULL;
            halfEdge->previous = NULL;
            halfEdge->id = kNullID;

            valve->halfEdges.push_back(halfEdge);
        }

        if (twin == NULL)
        {
            twin = new THalfEdge();

            twin->destination = valve;
            twin->next = NULL;
            twin->previous = NULL;
            twin->id = kNullID;

            destination->halfEdges.push_back(twin);
        }

        halfEdge->twin = twin;
        twin->twin = halfEdge;
    }

    return true;
}

bool BuildLinks(TValvePtr valve)
{
    int i, j;
    THalfEdgePtr halfEdge;
    THalfEdgePtr next;

    for (i = 0; i < valve->halfEdges.size(); i++)
    {
        halfEdge = valve->halfEdges[i];
        if (halfEdge->next == NULL)
        {
            next = NULL;
            if (valve->halfEdges.size() == 1)
            {
                // U-turn case
                next = halfEdge->destination->halfEdges[0];
            }
            else
            {
                // Find the first non-previous half edge at destination
                for (j = 0; j < halfEdge->destination->halfEdges.size(); j++)
                {
                    next = halfEdge->destination->halfEdges[j];
                    if (next->destination != valve &&
                        next->previous == NULL)
                        break;
                }
            }

            halfEdge->next = next;
            next->previous = halfEdge;
        }
    }

    return true;
}

bool BuildID(TValvePtr valve)
{
    int i;
    THalfEdgePtr halfEdge;
    THalfEdgePtr next;

    for (i = 0; i < valve->halfEdges.size(); i++)
    {
        halfEdge = valve->halfEdges[i];
        if (halfEdge->id == kNullID)
        {
            halfEdge->id = maxID;
            maxID++;

            next = halfEdge->next;
            while (next != halfEdge)
            {
                next->id = halfEdge->id;
                next = next->next;
            }
        }
    }

    return true;
}

bool BuildTopology(void)
{
    // Build half-edges:
    for (auto it = scan.begin(); it != scan.end(); ++it)
        BuildHalfEdges((*it).second);

    // Build links:
    for (auto it = scan.begin(); it != scan.end(); ++it)
        BuildLinks((*it).second);

    // Define face ID:
    for (auto it = scan.begin(); it != scan.end(); ++it)
        BuildID((*it).second);

    return true;
}

void PrintTopology(void)
{
    TValvePtr valve;
    THalfEdgePtr halfEdge;
    int index;

    for (auto it = scan.begin(); it != scan.end(); ++it)
    {
        valve = (*it).second;
        printf("[%s]\n", valve->name.c_str());
        for (index = 0; index < valve->halfEdges.size(); index++)
        {
            halfEdge = valve->halfEdges[index];
            printf("\t[%d] -> %s, next: %s, id: %d\n", index, halfEdge->destination->name.c_str(), halfEdge->next->destination->name.c_str(), halfEdge->id);
        }
    }
}

void InitDistances(void)
{
    for (auto it = scan.begin(); it != scan.end(); ++it)
        (*it).second->distance = INT_MAX;
}

void ComputeDistances(TValvePtr origin)
{
    THalfEdgePtr halfEdge, tmp;
    int i;
    TStep step;
    std::deque<THalfEdgePtr> backlogs;
    std::set<int> visits;
    TValvePtr valve;
    int distance;
    int orientation;

    origin->distance = 0;
    origin->step.origin = NULL;
    origin->step.count = 0;
    origin->step.halfEdge = NULL;

    backlogs.clear();
    visits.clear();

    // initiate the backlog
    for (i = 0; i < origin->halfEdges.size(); i++)
    {
        tmp = origin->halfEdges[i];
        if (visits.count(tmp->id) == 0)
        {
            backlogs.push_back(tmp);
            visits.insert(tmp->id);
        }
    }

    do {
        // More work to do?
        if (backlogs.size() == 0)
            break;

        for (orientation = 0; orientation < 2; orientation++)
        {
            halfEdge = backlogs.front();

            step.origin = halfEdge->twin->destination;
            step.count = 0;
            step.halfEdge = halfEdge;
            step.orientation = orientation;
            distance = step.origin->distance;

            if (orientation == 1)
                halfEdge = halfEdge->previous->previous;

            do {
                step.count++;
                distance++;

                valve = halfEdge->destination;
                // Update distance if needed
                if (valve->distance > distance)
                {
                    valve->step = step;
                    valve->distance = distance;
                }

                // Update the backlog
                for (i = 0; i < valve->halfEdges.size(); i++)
                {
                    tmp = valve->halfEdges[i];
                    if (visits.count(tmp->id) == 0)
                    {
                        backlogs.push_back(tmp);
                        visits.insert(tmp->id);
                    }
                }
                switch (orientation)
                {
                case 0:
                    halfEdge = halfEdge->next;
                    break;
                default:
                    halfEdge = halfEdge->previous;
                    break;
                }
            } while (halfEdge != backlogs.front());
        }
        backlogs.pop_front();

    } while (true);
}

int EvaluatePath(TPath& path)
{
    int index;
    int score;
    int cumulativeFlow;

    index = 1;
    score = 0;
    cumulativeFlow = 0;
    for (auto it = path.actions.begin(); it != path.actions.end(); ++it)
    {
        score += cumulativeFlow;
        index++;
        switch ((*it).what)
        {
        case kOpenValve:
            cumulativeFlow += (*it).valve->flowRate;
            break;
        default:
            break;
        }
    }

    while (index <= kMaxTime)
    {
        score += cumulativeFlow;
        index++;
    }

    return score;
}

int AppendPath(TPath& path, TValvePtr origin, TValvePtr target)
{
    TAction action;
    TValvePtr valve, tmp;
    std::deque<TValvePtr> stack;
    THalfEdgePtr halfEdge;

    valve = target;

    while (valve != origin)
    {
        stack.push_back(valve);
        valve = valve->step.origin;
    }

    tmp = origin;
    while (stack.size() > 0)
    {
        valve = stack.back();
        stack.pop_back();

        halfEdge = valve->step.halfEdge;
        if (valve->step.orientation == 1)
            halfEdge = halfEdge->previous->previous;

        do {
            tmp = halfEdge->destination;

            if (valve->step.orientation == 0)
                halfEdge = halfEdge->next;
            else
                halfEdge = halfEdge->previous;

            if (path.actions.size() < kMaxTime)
            {
                action.what = kMoveToValve;
                action.valve = tmp;
                path.actions.push_back(action);
            }
            else
                return (int)path.actions.size();
        } while (tmp != valve);
    }

    if (target->flowRate != 0 && (int)path.actions.size() < kMaxTime)
    {
        action.valve = target;
        action.what = kOpenValve;
        path.actions.push_back(action);
        path.openValves.insert(target);
    }

    return (int)path.actions.size();
}

int RevertPath(TPath& path, TValvePtr origin, TValvePtr target)
{
    do {
        if (path.actions.back().valve == origin)
            break;

        path.actions.pop_back();

    } while (path.actions.size() > 0);

    path.openValves.erase(target);

    return (int)path.actions.size();
}

std::vector<TValvePtr> validValves;

bool CanExplore(TValvePtr target, TPath& path)
{
    return path.openValves.count(target) == 0;
}

int Explore(TValvePtr origin, TPath& path, TPath& bestPath)
{
    TValvePtr target;
    int depth;
    std::vector<TValvePtr> sortedValves;

    for (auto it = validValves.begin(); it != validValves.end(); ++it)
        if (CanExplore(*it, path) == true)
            sortedValves.push_back(*it);

    std::sort(sortedValves.begin(), sortedValves.end(), [](TValvePtr a, TValvePtr b) {return float(a->flowRate) / a->distance > float(b->flowRate) / b->distance; });

    for (auto it = sortedValves.begin(); it != sortedValves.end(); ++it)
    {
        target = *it;
        InitDistances();
        ComputeDistances(origin);

        depth = AppendPath(path, origin, target);
        if (depth == kMaxTime || path.openValves.size() == validValves.size())
        {
            path.score = EvaluatePath(path);
            if (path.score > bestPath.score)
            {
#if(COMMENT==true)
                printf("GOTCHA ! => %d\n", path.score);
                PrintPath(path);
#endif
                bestPath = path;
            }
            RevertPath(path, origin, target);

            return bestPath.score;
        }
        else
        {
            Explore(target, path, bestPath);
        }
        RevertPath(path, origin, target);
    }

    return bestPath.score;
}

int main()
{
    FILE* input;
    errno_t error;
    clock_t clockStart, clockEnd;
    double time_taken;
    int64_t result;
    char buffer[200];
    int maxScore;

    printf("Advent of Code - Day 16\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    // Read scan
    while (ReadLine(input, buffer))
        ReadValveData(buffer);
    PrintScan(scan);

    // Step 1
    clockStart = clock();

    BuildTopology();
    PrintTopology();

    validValves.clear();
    for (auto it = scan.begin(); it != scan.end(); ++it)
        if ((*it).second->flowRate != 0)
            validValves.push_back((*it).second);

    std::sort(validValves.begin(), validValves.end(), [](TValvePtr a, TValvePtr b) {return a->flowRate < b->flowRate; });

    printf("Total number of valves: %d\n", (int)validValves.size());

    TPath path, bestPath;

    path.actions.clear();
    path.openValves.clear();
    path.score = 0;
    maxScore = 0;
    bestPath.actions.clear();
    bestPath.openValves.clear();
    bestPath.score = 0;

    result = Explore(scan["AA"], path, bestPath);

    PrintPath(bestPath);

    printf("result 1: %lld\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    // Step 2
    clockStart = clock();

    result = 0;
    printf("Result 2: %lld\n", result);

    clockEnd = clock();
    time_taken
        = double(clockEnd - clockStart)
        / double(CLOCKS_PER_SEC);
    printf("Elapsed time: %f seconds\n", time_taken);

    fclose(input);

    return 0;
}