// AdventOfCodeDay7.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>

const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay7/input.txt";

struct TFile;
typedef TFile* TFilePtr;

struct TFile {
    std::string name;
    int size;
};

struct TDirectory;
typedef TDirectory* TDirectoryPtr;

struct TDirectory {
    TDirectoryPtr parent;
    std::string name;
    std::list<TDirectoryPtr> children;
    std::list<TFilePtr> files;
    int size;
};

enum ECommand {
    eList,
    eChangeDirectoryRoot,
    eChangeDirectoryUp,
    eChangeDirectoryDown,
    eUnknown };

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

bool FindPattern(const char* buffer, const char* pattern)
{
    char* ptrBuffer;
    char* ptrPattern;

    ptrBuffer = const_cast< char*>(buffer);
    ptrPattern = const_cast<char*>(pattern);

    while( * ptrPattern != NULL )
    {
        if (*ptrPattern != *ptrBuffer)
            return false;

        ptrBuffer++;
        ptrPattern++;
    }

    return true;
}

bool ProcessCommand(
    FILE * file,
    TDirectoryPtr rootDirectory,
    TDirectoryPtr* currentDirectory,
    char* line )
{
    int command;
    char newDirectoryName[100];

    if (line[0] != '$')
        return false;

    command = eUnknown;

    if( FindPattern( & line[2], "ls" ) )
        command = eList;
    else if( FindPattern( & line[2], "cd" ) )
    {
        if( FindPattern( & line[5], "/") )
            command = eChangeDirectoryRoot;
        else if( FindPattern( & line[5], "..") )
            command = eChangeDirectoryUp;
        else
        {
            command = eChangeDirectoryDown;
            sscanf_s(&line[5], "%s", newDirectoryName, 100 );
        }
    }

    if( command == eUnknown )
        return false;

    switch( command )
    {
        case eList:
        {
            printf("List of files\n");
            while (ReadLine(file, line) > 0)
            {
                if (line[0] == '$')
                    break;

                printf("input: %s", line);

                if (FindPattern(line, "dir"))
                {
                    TDirectoryPtr newDirectory;

                    newDirectory = new TDirectory();

                    newDirectory->name = &line[4];
                    newDirectory->parent = * currentDirectory;
                    newDirectory->size = 0;

                    (*currentDirectory)->children.push_back( newDirectory );

                    printf(" => Add directory %s\n", newDirectory->name.c_str() );
                }
                else
                {
                    int size;
                    char name[100];
                    TFilePtr newFile;

                    sscanf_s(line, "%d %s", &size, name, 100);

                    newFile = new TFile();

                    newFile->name = name;
                    newFile->size = size;

                    (*currentDirectory)->files.push_back(newFile);

                    printf(" => Add file %s (%d)\n", name, size );
                }
            }
            break;
        }
        case eChangeDirectoryRoot:
            printf("Back to root\n");
            *currentDirectory = rootDirectory;
            ReadLine( file, line );
            break;
        case eChangeDirectoryUp:
            printf("Move up\n" );
            *currentDirectory = (*currentDirectory)->parent;
            ReadLine(file, line);
            break;
        case eChangeDirectoryDown:
            printf("Move to %s\n", newDirectoryName );
            for( auto it = (*currentDirectory)->children.begin(); it != (*currentDirectory)->children.end(); ++it)
                if ((*it)->name == newDirectoryName)
                {
                    *currentDirectory = * it;
                    break;
                }
            ReadLine(file, line);

            break;
        default:
            break;
    }

    printf("Current directory: %s\n", (*currentDirectory)->name.c_str());

    return true;
}

int ComputeDirectorySize( TDirectoryPtr directory )
{
    int result;

    result = 0;

    for (auto it = directory->files.begin(); it != directory->files.end(); ++it)
        result += (*it)->size;

    for (auto dir = directory->children.begin(); dir != directory->children.end(); ++dir)
        result += ComputeDirectorySize(*dir);

    directory->size = result;

    return result;
}

int SumDirectorySizeWithThreshold(TDirectoryPtr directory, int threshold)
{
    int result;

    result = 0;

    if (directory->size <= threshold)
        result += directory->size;

    for (auto dir = directory->children.begin(); dir != directory->children.end(); ++dir)
        result += SumDirectorySizeWithThreshold(*dir, threshold);

    return result;
}

int FindSuitableDirectory( TDirectoryPtr directory, TDirectoryPtr * candidate, int requiredSize )
{
    if( directory->size > requiredSize )
    {
        if (*candidate == NULL)
            *candidate = directory;
        else
        {
            if (directory->size < (*candidate)->size)
                *candidate = directory;
        }
    }

    for (auto dir = directory->children.begin(); dir != directory->children.end(); ++dir)
        FindSuitableDirectory( * dir, candidate, requiredSize );

    if (*candidate == NULL)
        return 0;

    return (*candidate)->size;
}

int main()
{
    FILE* input;
    char buffer[100];
    errno_t error;
    TDirectory rootDirectory;
    TDirectoryPtr currentDirectory;

    printf( "Advent of Code - Day 7\n" );

    error = fopen_s( & input, fileName, "r" );

    printf( "Opening file %s => %d\n", fileName, error );

    if( error != 0 )
        return error;

    rootDirectory.name = "/";
    rootDirectory.children.clear();
    currentDirectory = & rootDirectory;

    ReadLine(input, buffer);
    while( true )
        if( ProcessCommand(
            input,
            & rootDirectory,
            & currentDirectory,
            buffer ) == false )
            break;

    ComputeDirectorySize(&rootDirectory);

    printf("Total size: %d\n", rootDirectory.size );

    int totalSize;

    totalSize = SumDirectorySizeWithThreshold(&rootDirectory, 100000);

    printf("Question 1: Total size: %d\n", totalSize);

    int diskSize = 70000000;
    int freeSpace = diskSize - rootDirectory.size;
    int requiredSpace = 30000000;
    int minimumDirSize = requiredSpace - freeSpace;

    printf("Free space: %d\n", freeSpace );
    printf("Minimum directory size: %d\n", minimumDirSize);

    TDirectoryPtr candidate;

    candidate = NULL;

    FindSuitableDirectory(&rootDirectory, &candidate, minimumDirSize);

    printf("Candidate: %s (%d)", candidate->name.c_str(), candidate->size);

    return 0;
}