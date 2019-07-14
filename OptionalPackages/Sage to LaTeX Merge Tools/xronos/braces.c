/****************************************************
Copyright 2016 Dana Nowell, all rights reserved.
This is a hack.  Written for my son, Jason Nowell 
in August 2016.

LOTS of assumptions in the code about other parts of the code.
Beware of modifications cascading through the code.  
This was a 4-5 hour hack against verbal requirements 
and should be assumed as such.
*
* main purpose is to remove redundant matching braces from a line


Licensing:
Commercial use of this code is forbidden with out written permission
The code IS free to use in educational institutions 
and a license for that use is granted herein.

****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xronosfmt.h"


#define BUFSIZE	10000
#define MAX_RECURSE 200

static int debug = 0; /*set to non zero to get tracing output */
static char buffer[BUFSIZE+1];
static char BraceBuffer[BUFSIZE+1];


typedef struct _SavePos
	{
	int Count;
	char *position;
	} SavePos;



/*************************************
*  Handle a line with potential matching braces
*************************************/
static int HandleBracedLine( char *line )
{
int retval = 0;
char *p, *q;
int CurrentCount = 0;
SavePos SaveCount[MAX_RECURSE];
int FoundOne = FALSE;
int i, SaveIndex = 0;
static int RecursionDepth;

if ( ++RecursionDepth >= MAX_RECURSE )
	ErrorExit( "HandleBracedLine: Too much recursion" );

memset( SaveCount, 0, sizeof(SaveCount) );
q = NULL;
for ( p = line; *p != 0 && *p != '%' && !FoundOne; q = p++ )
	{
	switch( *p )
		{
		case '{':
			if (q != NULL && *q == *p )
				{
				SaveCount[SaveIndex].Count = CurrentCount;
				SaveCount[SaveIndex].position = p;
				if ( debug )
					printf( "BEGIN SaveCount[%d]=%d\n", SaveIndex, SaveCount[SaveIndex].Count );
						
				++SaveIndex;
				}
			++CurrentCount;
			break;
		case '}':
			if (q != NULL && *q == *p )
				{
				if ( debug )
					printf( "Matching closes at %d\n", CurrentCount );

					for( i = 0; i < SaveIndex  && !FoundOne; i++ )
					{
					if ( debug )
						printf( "END SaveCount[%d]=%d\n", i, SaveCount[i].Count );
						
					if( CurrentCount == SaveCount[i].Count )
						{
						if ( debug )
							{
							printf( "Got a live one at index %d\n", i );
							printf( "Line: %s\n", line );
							}
						FoundOne = TRUE;
						memmove( q, p, strlen(p)+1 );
						if ( debug )
							{
							printf( "New Line1: %s\n", line );
							}

							memmove( SaveCount[i].position, SaveCount[i].position + 1, strlen(SaveCount[i].position) );
						if ( debug )
							{
							printf( "New Line: %s\n", line );
							}
						}
					}
				fflush( NULL );
				}

				--CurrentCount;
			break;
			
		default:
			if ( q != NULL && *q == '}' )
				{
				if ( debug )				
					{
					printf( "In default with a prev brace, SaveIndex is %d\n", SaveIndex );
					fflush( NULL );
					}
					
				while ( SaveIndex > 0 && CurrentCount <= SaveCount[SaveIndex - 1].Count )
					--SaveIndex;
				}
			break;
		}
	}

	
if ( FoundOne )
	{
	if ( debug )
		{
		puts( "HandleBracedLine: Recursing" );
		fflush( NULL );
		}
	retval = HandleBracedLine( line );
	}
else
	WriteToOutputBuffer( line, strlen(line) );
	
--RecursionDepth;	
return retval;
}




/*************************************
*  Handle everyline
*************************************/
static int HandleLine( char *line )
{
int retval = 0;
/* 
	if the line does not have both a pair of 
	open braces and a pair of close braces
	then just write it out
*/	
if ( strstr( line, "{{") == 0 || strstr( line, "}}") == 0 )
	{
	if ( debug )
		puts( "This line has no matching double braces" );
	WriteToOutputBuffer( line, strlen(line) );
	}
else
	retval = HandleBracedLine( line );
	

return retval;	
}


/*************************************
*  Main routine
*************************************/
int Braces( void )
{
int done = 0;
int LineCount = 0;


	
while ( !done )
	{
	if ( ReadLine( ) != 0 )
		done = TRUE;
	else
		{
		++LineCount;
		if ( debug )
			printf( "Processing Line %d\n", LineCount );
		if ( HandleLine( LineBuffer ) != 0 )
			ErrorExit( "Handle Line blew up" );
		}
	}

	
printf( "Processed %d lines for braces\n", LineCount );

return 0;
}