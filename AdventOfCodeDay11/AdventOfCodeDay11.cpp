// AdventOfCodeDay11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>

const char* fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay11/input.txt";

const char* kMONKEY = "Monkey ";
const char* kSTARTING_ITEMS = "  Starting items: ";
const char* kOPERATION = "  Operation: new = old ";
const char* kTEST = "  Test: divisible by ";
const char* kIFTRUE = "    If true: throw to monkey ";
const char* kIFFALSE = "    If false: throw to monkey ";
const char* kOLD = "old";

#define kMaxRound 10000

enum EInstruction {
    eUnknown = -1,
    eMonkeyDefinition,
    eStartingItems,
    eOperation,
    eTest,
    eIfTrue,
    eIfFalse
};

enum EOperand {
    eAddition = 0,
    eMultiplication
};

const char* kOperation[2] = { "+", "*" };

const std::vector<int> primes = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };

#define kOldValue 0

struct TLevel {
    std::vector<int> rest;
};

typedef TLevel* TLevelPtr;

struct TMonkey;
typedef TMonkey* TMonkeyPtr;

struct TMonkey {
    int id;
    std::vector<TLevelPtr> inventory;
    int operand;
    int constant;
    int prime;
    int monkeyIfTrue;
    int monkeyIfFalse;
    int64_t counter;
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

bool ProcessInput(TLevelPtr level, int input)
{
    int value;
    for (auto it = primes.begin(); it != primes.end(); ++it)
    {
        value = input % (*it);
        level->rest.push_back(value);
    }

    return true;
}

bool AddInput(TLevelPtr level, int input)
{
    int index;
    for (index = 0; index < primes.size(); index++)
        level->rest[index] = (level->rest[index] + input) % primes[index];

    return true;
}

bool MultiplyInput(TLevelPtr level, int input)
{
    int index;
    for (index = 0; index < primes.size(); index++)
        level->rest[index] = (level->rest[index] * input) % primes[index];

    return true;
}

bool AddSelf(TLevelPtr level)
{
    int index;
    for (index = 0; index < primes.size(); index++)
        level->rest[index] = (2 * level->rest[index]) % primes[index];

    return true;
}

bool MultiplySelf(TLevelPtr level)
{
    int index;
    for (index = 0; index < primes.size(); index++)
        level->rest[index] = (level->rest[index] * level->rest[index]) % primes[index];

    return true;
}

bool ReadMonkey(FILE* file, std::vector<TMonkey*>* monkeys)
{
    char buffer[100];
    TMonkeyPtr monkey;
    char* ptr;
    int index, count;;
    int value[10];
    int input;
    TLevelPtr level;

    do {
        if (ReadLine(file, buffer) == 0)
            return false;
    } while (FindPattern(buffer, kMONKEY) == false);

    monkey = new TMonkey();

    monkey->counter = 0;

    sscanf_s(buffer + strlen(kMONKEY), "%d", &monkey->id);

    printf("Monkey: %d\n", monkey->id);

    do {
        if (ReadLine(file, buffer) == 0)
            return false;
    } while (FindPattern(buffer, kSTARTING_ITEMS) == false);
    ptr = buffer + strlen(kSTARTING_ITEMS);

    printf("\tInventory\n");

    count = sscanf_s(ptr, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d", &value[0], &value[1], &value[2], &value[3], &value[4], &value[5], &value[6], &value[7], &value[8], &value[9]);

    for (index = 0; index < count; index++)
    {
        printf("\t\t[%d] = %d\n", index, value[index]);
        level = new TLevel();
        ProcessInput(level, value[index]);
        monkey->inventory.push_back(level);
    }

    do {
        if (ReadLine(file, buffer) == 0)
            return false;
    } while (FindPattern(buffer, kOPERATION) == false);
    ptr = buffer + strlen(kOPERATION);

    if (*ptr == '*')
        monkey->operand = eMultiplication;
    else if (*ptr == '+')
        monkey->operand = eAddition;
    else
        return false;

    ptr += 2;
    if (FindPattern(ptr, kOLD) == true)
        monkey->constant = kOldValue;
    else
        sscanf_s(ptr, "%d", &monkey->constant);

    if (monkey->constant == kOldValue)
        printf("\tnew = old %s old\n", kOperation[monkey->operand]);
    else
        printf("\tnew = old %s %d\n", kOperation[monkey->operand], monkey->constant);

    do {
        if (ReadLine(file, buffer) == 0)
            return false;
    } while (FindPattern(buffer, kTEST) == false);
    ptr = buffer + strlen(kTEST);

    sscanf_s(ptr, "%d", &input);
    for (index = 0; index < primes.size(); index++)
        if (primes[index] == input)
        {
            monkey->prime = index;
            break;
        }

    printf("\tDivisible by %d\n", primes[monkey->prime]);

    do {
        if (ReadLine(file, buffer) == 0)
            return false;
    } while (FindPattern(buffer, kIFTRUE) == false);
    ptr = buffer + strlen(kIFTRUE);

    sscanf_s(ptr, "%d", &monkey->monkeyIfTrue);

    printf("\tMonkey if true: %d\n", monkey->monkeyIfTrue);

    do {
        if (ReadLine(file, buffer) == 0)
            return false;
    } while (FindPattern(buffer, kIFFALSE) == false);
    ptr = buffer + strlen(kIFFALSE);

    sscanf_s(ptr, "%d", &monkey->monkeyIfFalse);

    printf("\tMonkey if false: %d\n", monkey->monkeyIfFalse);

    monkeys->push_back(monkey);

    return true;
}

bool MonkeyRound(std::vector<TMonkey*>* monkeys, int index)
{
    TMonkeyPtr monkey;
    TLevelPtr level;
    int i;
    TMonkeyPtr destination;
    TLevel step1, step2;

    //    printf("Round for monkey %d\n", index);

    monkey = (*monkeys)[index];
    i = 0;

    while (monkey->inventory.size() != 0)
    {
        level = *monkey->inventory.begin();
        monkey->counter++;
        switch (monkey->operand)
        {
        case eAddition:
            if (monkey->constant == kOldValue)
                AddSelf(level);
            else
                AddInput(level, monkey->constant);
            break;
        case eMultiplication:
            if (monkey->constant == kOldValue)
                MultiplySelf(level);
            else
                MultiplyInput(level, monkey->constant);
            break;
        default:
            break;
        }

        if ( level->rest[ monkey->prime ] == 0 )
        {
            destination = (*monkeys)[monkey->monkeyIfTrue];
        }
        else
        {
            destination = (*monkeys)[monkey->monkeyIfFalse];
        }

        monkey->inventory.erase(monkey->inventory.begin());
        destination->inventory.push_back(level);
        i++;
    }

    return true;
}

bool PerformRound(std::vector<TMonkey*>* monkeys)
{
    int index;

    for (index = 0; index < monkeys->size(); index++)
        MonkeyRound(monkeys, index);

    return true;
}

int main()
{
    FILE* input;
    errno_t error;
    int64_t result = 0;
    std::vector<TMonkey*> monkeys;
    int index, i;
    int firstIndex;
    int secondIndex;
    int64_t value;
    clock_t start, end;
    double time_taken;

    printf("Advent of Code - Day 11\n");

    error = fopen_s(&input, fileName, "r");

    printf("Opening file %s => %d\n", fileName, error);

    if (error != 0)
        return error;

    while (ReadMonkey(input, &monkeys) == true) {}

    start = clock();
    for (index = 1; index <= kMaxRound; index++)
    {
        PerformRound(&monkeys);
        switch (index)
        {
        case 1:
        case 20:
        case 100:
        case 200:
        case 300:
        case 400:
        case 500:
        case 600:
        case 700:
        case 800:
        case 900:
        case 1000:
        case 2000:
        case 3000:
        case 4000:
        case 5000:
        case 6000:
        case 7000:
        case 8000:
        case 9000:
        case 10000:
            end = clock();
            time_taken
                = double(end - start)
                / double(CLOCKS_PER_SEC);
            printf("\nRound %d (%f seconds):\n", index, time_taken );
            for(i = 0; i < monkeys.size(); i++)
                    printf("\t[%d] %lld\n", i, monkeys[i]->counter);
            start = clock();
            break;
        default:
            break;
        }
    }

    printf("\n");
    result = 0;

    firstIndex = -1;
    secondIndex = -1;

    value = 0;
    for( index = 0; index < monkeys.size(); index++ )
    {
        if (monkeys[index]->counter > value)
        {
            value = monkeys[index]->counter;
            firstIndex = index;
        }
    }

    printf("First monkey: %d (%lld)\n", firstIndex, monkeys[firstIndex]->counter);

    value = 0;
    for( index = 0; index < monkeys.size(); index++ )
    {
        if( index != firstIndex &&
            monkeys[index]->counter > value)
        {
            value = monkeys[index]->counter;
            secondIndex = index;
        }
    }

    printf("Second monkey: %d (%lld)\n", secondIndex, monkeys[secondIndex]->counter);

    result = monkeys[firstIndex]->counter * monkeys[secondIndex]->counter;

    printf("=> Answer: %lld\n", result );

    return 0;
}