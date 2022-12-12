// AdventOfCodeDay5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

const char * fileName = "C:/Users/aforgiel/source/repos/AdventOfCode2022/AdventOfCodeDay5/input.txt";
typedef struct {
    char storage[ 100 ];
    int size;
} TStack;

int ReadLine( FILE * file, char * buffer )
{
    char * ptr;
    int result;

    ptr = buffer;
    result = 0;

    while( fread( ptr, 1, 1, file ) > 0 )
    {
        if( * ptr == '\n' )
        {
            * ptr = NULL;
            return result;
        }
        ptr++;
        result++;
    }

    return result;
}

void StackInit( TStack * stack )
{
    stack->size = 0;
}

int StackPush( TStack * stack, char input )
{
    stack->storage[ stack->size ] = input;
    stack->size++;

    return stack->size;
}

int StackPull( TStack * stack, char * output )
{
    if( stack->size > 0 )
    {
        * output = stack->storage[ stack->size - 1 ];
        stack->size--;
    }
    else
        return -1;

    return stack->size;
}

void StackPrint( TStack * stack )
{
    int index;

    for( index = 0; index < stack->size; index++ )
        printf( "[%d] %c ", index, stack->storage[index] );
}


int PopulateStacks( FILE * file, TStack * stacks )
{
    TStack tmpStacks[9];
    char buffer[100];
    int result;
    int index;
    char content;

    result = 0;

    for (index = 0; index < 9; index++)
    {
        StackInit(&tmpStacks[index]);
        StackInit(&stacks[index]);
    }

    while( ReadLine( file, buffer ) > 0 )
    {
        if( buffer[0] == ' ' )
            break;

        for( index = 0; index < 9; index++ )
        {
            content = buffer[ 1 + index * 4 ];
            printf( "[%d]: %c ", index, content );
            if( content != ' ' )
                StackPush( & tmpStacks[ index ], content );
        }
        printf("\n");
    }

    for( index = 0; index < 9; index++ )
    {
        printf("\nTemporary stack %d: ", index );
        StackPrint( & tmpStacks[ index ] );
    }
    printf("\n");

    for( index = 0; index < 9; index++ )
    {
        while( StackPull( & tmpStacks[index], & content ) >= 0 )
            StackPush( & stacks[index], content );

        printf("\nStack %d: ", index );
        StackPrint( & stacks[ index ] );
    }
    printf("\n");

    return result;
}

int main()
{
    FILE * input;
    char buffer[ 100 ];
    errno_t error;
    TStack stacks[9];
    int quantity, source, destination;
    char content;
    int index;
    TStack tmpStack;

    printf( "Advent of Code - Day 5\n" );

    error = fopen_s( & input, fileName, "r" );

    printf( "Opening file %s => %d\n", fileName, error );

    if( error != 0 )
        return error;

    // First populate the stacks:
    PopulateStacks( input, stacks );

    // Read the blank line:
    ReadLine( input, buffer );

    while( ReadLine( input, buffer ) > 0 )
    {
        sscanf_s(buffer, "move %d from %d to %d", & quantity, & source, & destination );

        source--;
        destination--;

        printf("quantity: %d, source: %d, destination: %d\n", quantity, source, destination );

        StackInit(&tmpStack);

        while( quantity > 0 )
        {
            StackPull( & stacks[source], & content );
//            StackPush( & stacks[destination], content );
            StackPush( & tmpStack, content );
            quantity--;
        }

        // Part II:
        while( StackPull( & tmpStack, & content ) >= 0 )
            StackPush( & stacks[destination], content );

        for (index = 0; index < 9; index++)
        {
            printf("\nStack %d: ", index);
            StackPrint(&stacks[index]);
        }
        printf("\n");
    }

    for( index = 0; index < 9; index++ )
    {
        printf("\nStack %d: ", index );
        StackPrint( & stacks[ index ] );
    }
    printf("\n");

    return 0;
}