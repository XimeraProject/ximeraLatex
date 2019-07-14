/****************************************************
Copyright 2017 Dana Nowell, all rights reserved.
This is a hack.  Written for my son, Jason Nowell 
in August 2017.

LOTS of assumptions in the code about other parts of the code.
Beware of modifications cascading through the code.  
This was a hack against verbal requirements 
and should be assumed as such.

Licensing:
Commercial use of this code is forbidden with out written permission
The code IS free to use in educational institutions 
and a license for that use is granted herein.

****************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "xronosfmt.h"

#define SEARCH_TEXT "\\text{"
#define SEARCH_TEXT_TT "\\texttt{"

typedef enum { TR_IDLE = 0, TR_TEXT_TT_BRACE, TR_TEXT_BRACE } BUFFERSTATE;

static int LineNo = 0;
static char SearchString[200];
static int BraceCount = 0;
static int BracesRemoved = 0;
static int TextttProcessed = 0;

static char *TextttBuffer = NULL;
static int TextttBufferSize = 0;
static BUFFERSTATE TextttBufferState = TR_IDLE;


/*************************************
*************************************/
static void EnlargeTextttBuffer( void )
{
	char *p;

	TextttBufferSize += 1000;
	p = TextttBuffer;
	TextttBuffer = GetMemoryBlock( TextttBufferSize );
	if ( p != NULL )
	{
		if ( strlen( p ) > 0 )
			strcpy( TextttBuffer, p );
			
		FreeMemoryBlock( p );
	}

}


/*************************************
goal is to concat the new string onto the existing buffer
if the buffer is too small we resize it to allow for the 
concat operation
*************************************/
static void AddToTextttBuffer( char *NewText )
{
	int len = 0;
	char *New;

	if ( TextttBuffer == NULL )
		EnlargeTextttBuffer();
		
	if ( NewText == NULL || strlen(NewText) < 1 )
		return;

	len = strlen( NewText ) + strlen( TextttBuffer ) + 1; /* old text + new text + end of string */
	
	while( TextttBufferSize < len )
		EnlargeTextttBuffer();
		
	strcat( TextttBuffer, NewText );
}




/*************************************
*************************************/
static void StripAllBracesFromStringInPlace(char *string)
{
	char *New;
	char *p, *q;
	BOOL FoundOne = FALSE;
	
	if ( string == NULL || strlen(string) < 1 )
		return;
	
	/* 
	new string can not be MUCH larger than current string as we 
	are removing braces (there may be none).  BUT we are adding a 
	close brace so we	will allocate a bit more buffer and hope it 
	avoids resizng 	the buffer just to add the closing brace.  Yeah, 
	it is a hack but it will likely save a memory allocate and a 
	string copy so I will take it in the name of efficiency.  If I 
	am wrong my buffer is a bit too big OR I do the alloc and copy 
	anyway, so no real harm/foul.
	*/
	New = GetMemoryBlock( strlen(string) + 5 );  
	
	q = New;  /* starting point for buffer copy destination */
	for( p = string; *p; ++p )
	{
		if ( *p != '{' && *p != '}' )
		{
			*q = *p;
			++q;
		}
		else
		{
			FoundOne = TRUE;
			++BracesRemoved;
		}
	}
	
		/* if we found one then do the copy, else skip the copy */
	if ( FoundOne )  
	{
		strcpy( string, New );
	}
	
	FreeMemoryBlock( New );

}


/*************************************
*  Handle the end of the search string processing
*************************************/
static void RecycleTextttBuffer( void )
{
	if ( TextttBuffer != NULL )
		FreeMemoryBlock( TextttBuffer );
	
	TextttBuffer = NULL;
	TextttBufferSize = 0;
	TextttBufferState = TR_IDLE;

}


/*************************************
*  Handle the end of the search string processing
*************************************/
static void HandleTextttEnd( void )
{
	if ( TextttBuffer == NULL )
		ErrorExit( "In HandleTextttEnd with NULL TextttBuffer, something went REALLY REALLY wrong" );
	
	StripAllBracesFromStringInPlace(TextttBuffer);

	/* 
	we need to include the close brace for the \text( call 
	but we need to ensure it does not get stripped out so
	we add it after the call to strip the braces
	*/
	AddToTextttBuffer( "}" );  

	WriteToOutputBuffer( TextttBuffer, strlen(TextttBuffer) );	
	
	RecycleTextttBuffer();
}




/*************************************
*  Handle everyline
*************************************/
static int HandleLine( char *line )
{
	int retval = 0;
	static BOOL ProcessedQuestion = FALSE;
	char *NewTexttt;
	char *p;
	
	if ( TextttBuffer != NULL )
	{
/* printf( "DANA TextttBuffer is not null\n" ); */
		p = XTL_FindClosingCurlyBrace(line, &BraceCount);
		if ( p != NULL )
		{
			*p = 0;  /* kill the brace */
			++p;	/* skip over to the post brace part of the line */
			AddToTextttBuffer( line );  /* add the line upto where the brace used to be */
			if ( TextttBufferState == TR_TEXT_BRACE )
			{
				HandleTextttEnd(  );
			}
			else
			{
				/* at this point we finished the texttt call and skipped the close brace for it */
				BraceCount = 1;  /* set the count to 1 so we find the close brace for the text */
				TextttBufferState = TR_TEXT_BRACE;
			}
			
			if ( strlen(p) > 0 )
				retval = HandleLine( p );
		}
		else
		{
			AddToTextttBuffer( line );
		}
		
	return retval;	
	}

			
	NewTexttt = strstr( line, SearchString );
	if ( NewTexttt == NULL )
		WriteToOutputBuffer( line, strlen(line) );			
	else
	{
/* printf( "DANA processing a hit\n" ); */
		
		++TextttProcessed;
		p = NewTexttt;
		p += strlen(SEARCH_TEXT);
		*p = 0;
		if ( strlen( line ) > 0 )
			WriteToOutputBuffer( line, strlen(line) );
		
		p += strlen(SEARCH_TEXT_TT); /* skip over the texttt start */
		BraceCount = 1;		/* set the count to 1 so we find the close brace for the texttt to get rid on it */
		TextttBufferState = TR_TEXT_TT_BRACE;
		EnlargeTextttBuffer();  /* get a buffer to ensure TextttBuffer is not NULL */
		if ( strlen( p ) > 0 )
			retval = HandleLine( p );
	}	
	
	return retval;	
}


/*************************************
*  Main routine
*************************************/
int TextttRemoval( void )
{
	BOOL done = FALSE;
	int status;
	BOOL debug  = TRUE;
	
	strcpy( SearchString, SEARCH_TEXT);
	strcat( SearchString, SEARCH_TEXT_TT);
	
/* printf( "DANA Using search string: %s\n", SearchString ); */

	/* now process the file */
	while ( !done )
		{
		if ( ReadLine( ) != 0 )
			done = TRUE;
		else
			{
			++LineNo;
			if ( debug )
				printf( "TextttRemoval: Processing Line %d, text: %s\n", LineNo, LineBuffer );
	
			if ( HandleLine( LineBuffer ) != 0 )
				ErrorExit( "TextttRemoval: HandleLine() blew up" );
			}
		}
	
		
	printf( "Processed %d lines for TextttRemoval, Processed %d \\text{\\texttt{}} sets, removed %d braces\n", LineNo, TextttProcessed, BracesRemoved );
	
	
	return 0;
}




