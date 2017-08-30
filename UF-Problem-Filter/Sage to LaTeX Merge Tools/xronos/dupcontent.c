/****************************************************
Copyright 2016 Dana Nowell, all rights reserved.
This is a hack.  Written for my son, Jason Nowell 
in August 2016.

LOTS of assumptions in the code about other parts of the code.
Beware of modifications cascading through the code.  
This was a 4-5 hour hack against verbal requirements 
and should be assumed as such.
*
* main purpose is to remove redundant content from a file
*  it ALSO strips out everything BUT the questions

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
#define MAXCONTENT 50000

static int debug = 0; /*set to non zero to get tracing output */

static int LineCount;
static int ProcessingContent;
static int CurrentParenCount;
static int ContentBlocksDiscarded;
static char SearchString[200] = ProblemStartString;

typedef struct _SaveContent
	{
	char *content;
	struct _SaveContent *next;
	} SaveContent;

static SaveContent *head;
static SaveContent *CurrentContent;

/*************************************
*************************************/
static void LinkContent( SaveContent *ptr )
{
char *p;

if ( debug )
	printf( "LinkContent: Linking %d bytes\n", (int) strlen(ptr->content) );


/* first shorten buffer to what we actually need */
p = ptr->content;
ptr->content = GetMemoryBlock( strlen(p) + 1 );
strcpy( ptr->content, p );

FreeMemoryBlock( p );

if ( debug )
	printf( "LinkContent: Linking %d bytes\n", (int) strlen(ptr->content) );


if ( head == NULL )
	head = ptr;
else
	{
	ptr->next = head;
	head = ptr;
	}
		
}


/*************************************
*************************************/
static int FindCurrentContent( void )
{
SaveContent *p;
int retval = FALSE;

if ( CurrentContent == NULL )
	ErrorExit( "In FindCurrentContent() with no content block" );

if ( CurrentContent->content == NULL )
	ErrorExit( "In FindCurrentContent() with no content" );
	
for ( p = head; p != NULL; p= p->next )
	{
	if ( strcmp( p->content, CurrentContent->content ) == 0 )
		{
		retval = TRUE;
		break;
		}
	}
return retval;
}



/*************************************
*************************************/
static void ContentDone( char *remainder )
{
	
if ( CurrentContent == NULL )
	ErrorExit( "In ContentDone() with no content block" );


if ( CurrentContent->content != NULL )
	{
	if ( debug )
		puts( "ContentDone: Live content" );

	if ( !FindCurrentContent() )
		{
		WriteToOutputBuffer( SearchString, strlen(SearchString) );
		WriteToOutputBuffer( "\n", strlen("\n") );
		WriteToOutputBuffer( CurrentContent->content, strlen(CurrentContent->content) );
		WriteToOutputBuffer( "}", strlen("}") );

		if ( remainder != NULL )
			WriteToOutputBuffer( remainder, strlen(remainder) );

		WriteToOutputBuffer( "\n", strlen("\n") );
		LinkContent( CurrentContent );
		}
	else	
		{
		FreeMemoryBlock( CurrentContent->content );
		++ContentBlocksDiscarded;
		}
		
	}
else
	FreeMemoryBlock( CurrentContent );
	
CurrentContent = NULL;
ProcessingContent = FALSE;
return;
}




/*************************************
*  Handle a line with potential matching braces
*************************************/
static int HandleContentLine( char *line )
{
int retval = 0;
char *p, *q;
int FoundOne = FALSE;
int len;

if ( debug )
	printf( "HandleContentLine: Entered\n" );

q = NULL;
for ( p = line; *p != 0 && *p != '%' && !FoundOne; q = p, p++ )
	{
	switch( *p )
		{
		case '{':
			if ( q != NULL && *q == '\\' )
				{
				if ( debug )
					printf( "Line %d found open brace we should not count\n", LineCount );
				}
			else
				++CurrentParenCount;
				
			len = strlen(CurrentContent->content);
			CurrentContent->content[len] = *p;
			CurrentContent->content[len+1] = 0;
			

			break;
		case '}':
			
			if ( q != NULL && *q == '\\' )
				{
				if ( debug )
					printf( "Line %d found close brace we should not count\n", LineCount );
				}
			else
				--CurrentParenCount;
				
			if ( CurrentParenCount < 0 )
				{
				ContentDone( p+1 );
				FoundOne = TRUE; 
				if ( debug )
					printf( "Line %d setting FoundOne\n", LineCount );

				}
			else
				{
				len = strlen(CurrentContent->content);
				CurrentContent->content[len] = *p;
				CurrentContent->content[len+1] = 0;
				}
				
			break;
			
		default:
			len = strlen(CurrentContent->content);
			CurrentContent->content[len] = *p;
			CurrentContent->content[len+1] = 0;
			break;
		}
	}

if ( debug && CurrentContent != NULL && CurrentContent->content != NULL )
	{
	len = strlen(CurrentContent->content);
	printf( "HandleContentLine: content is %d long\n", len );
	}
	
if ( FoundOne )
	{
	if ( debug )
		printf( "Line %d is considered Content end\n", LineCount );
	}
else	
	if ( *p == '%' )
		strcat( CurrentContent->content, p );

return retval;
}


/*************************************
*  Handle everyline
*************************************/
BOOL IsItAFormattingLine( char *line, int len )
{
	char *TagSearchString = "\\tagged{";
	
	if ( strstr( line, TagSearchString ) != NULL )
	{
		puts( "IsItAFormattingLine: Found tagged" );
		return TRUE;
	}

	return FALSE;
}


/*************************************
*  Handle everyline
*************************************/
static int HandleLine( char *line )
{
int retval = 0;
static BOOL ProcessedQuestion = FALSE;

if ( debug )
	printf( "HandleLine: Line %s\n", line );

			
if ( strstr( line, SearchString ) != 0 )
	{
	ProcessedQuestion = TRUE;;
	if ( ProcessingContent != 0 )
		ContentDone( NULL );
	ProcessingContent = TRUE;	
	CurrentParenCount = 0;
	CurrentContent = GetMemoryBlock( sizeof( SaveContent ) );
	CurrentContent->content = GetMemoryBlock( MAXCONTENT + 1 );
	if ( debug )
		printf( "Line %d is considered Content start\n", LineCount );
	}
else
	if ( ProcessingContent != 0 )
		retval = HandleContentLine( line );
	else
		{
		if ( debug )
			printf( "Line %d is not considered content\n", LineCount );


		if ( !QuestionsOnly	&& !ProcessedQuestion )  /* if the user requested we include the document structure stuff intact */
			WriteToOutputBuffer( line, strlen(line) );			
		else
		{
		if ( IsItAFormattingLine( line, strlen(line) ) )
			WriteToOutputBuffer( line, strlen(line) );			
		}
		
		}

	

return retval;	
}


/*************************************
*  Entry point routine
*************************************/
int KillDups( void )
{
int done = 0;
int status;

	
while ( !done )
	{
	status = ReadLine( );
	if ( status != 0 )
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

	

printf( "Processed %d lines, Discarded %d Content Blocks\n", LineCount, ContentBlocksDiscarded );


return 0;
}
