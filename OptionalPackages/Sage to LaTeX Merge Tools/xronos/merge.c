/****************************************************
Copyright 2017 Dana Nowell, all rights reserved.
This is a hack.  Originally written for my son, Jason Nowell 
in August 2016.  A bunch of modifications over time since then.

LOTS of assumptions in the code about other parts of the code.
Beware of modifications cascading through the code.  
This was initialy a 4-5 hour hack against verbal requirements 
and should be assumed as such.

Licensing:
Commercial use of this code is forbidden with out written permission
The code IS free to use in educational institutions 
and a license for that use is granted herein.

****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xronosfmt.h"

#define MAX_SAGE_LINES	70000
#define BUFSIZE	10000

typedef struct _LogLine {
struct _LogLine *next;
char *Line;
} LogLine;

static char buffer[BUFSIZE+1];  /* add 1 for end of string marker */
static int LineNo = 0;
static LogLine *SageLine[MAX_SAGE_LINES];
static int NextEmptySageLine = 0;
static int NextSageLineOutput = 0;

	/* context definition for the sqrt fixup code */
static BOOL ProcessForSqrtBug = TRUE;  /* once sage fixes the sqrt bug, setting this variable to FALSE will stop the fixup processing */
typedef enum { NoSqrt = 0, SquareOpen, SquareClose, CurlyOpen, CurlyClose } SQRTSTATE;
typedef struct _SqrtBugCtxt {
	struct _SqrtBugCtxt *next;
	struct _SqrtBugCtxt *parent;
	int ChildFixupCount;
	SQRTSTATE SqrtState;
	int SqrtFixupCount;
	int SquareBraces;
	int CurlyBraces;
	int LineNoSqrtStart;
	int LineNoOpenCurlyBrace;	
	int LineNoCloseCurlyBrace;
	  /* we store offset to the beginning of the buffer for position data (below) we COULD use pointers BUT 
	  offsets ensure we are buffer independant in case someone copies a buffer during proessing */
	int OSSqrtStart;
	int OSOpenSquareBrace;
	int OSCloseSquareBrace;
	int OSOpenCurlyBrace;
	int OSCloseCurlyBrace;
} SqrtBugCtxt;

static	SqrtBugCtxt *SqrtStack;


/*************************************
*************************************/
static LogLine *BuildLogLine( char *p )
{
LogLine *Line;
int len;

Line = (LogLine *) GetMemoryBlock( sizeof( LogLine ));
len = strlen( p );
Line->Line = GetMemoryBlock(  len + 1 );
strcpy( Line->Line, p );
Line->next = NULL;

return Line;
}


/*************************************

There is magic here or at least messy hard to read stuff 
goal is to discard matched empty braces 
e.g, {{5}{}{}} becomes {{5}}
*************************************/
static void RemoveUselessBraces( char *OldBuf )
{
char *p, *q, *n, *NewBuf;

p = OldBuf;
NewBuf = GetMemoryBlock(strlen(p) + 1 ); /* get a temp buffer big enough for the string plus the string terminator */
q = NewBuf;

/* walk the source string copying to the destination string while avoiding the matched empty braces */
while( *p )
{
n = p;
++n; /* next character */
if ( *p == '{' && *n == '}' )  /* pair of matched braces */
	{  /* move p to character after the matched close brace, basically skip over them */
	++p;
	++p;
	}
else
	{
	*q = *p;  /* copy the non matched brace char */
	++q;  /* move to next spot in destination buffer */
	++p;  /* advance in the source buffer */
	}
}

*q = 0;  /* ensure NULL terminated string so we have a end of string mark */	
*OldBuf = 0; /* erase old string, in case the new string is empty */
if( strlen(NewBuf) > 0 ) /* copy new string into old string location if new sting exists */
	strcpy(OldBuf, NewBuf);
	
FreeMemoryBlock( NewBuf ); /* clean up our temp buffer */
}



/*************************************
*************************************/
static char *TrimLogLine( char *OldBuf )
{
char *p, *q;

p = OldBuf;
TrimTrailingWhitespace(p);

/* club any comments like a baby seal */
q = XTL_FindComment( p );
if ( q != NULL )
	*q = 0;

	/* if we have a line, remove useless braces  */
if ( strlen(p) != 0 )
	RemoveUselessBraces( p );
	
return p;	
}

/*************************************
returns:
  0 - success
  1 - end of file
  2 - line too long
*************************************/
static int ReadLogLine( FILE *fh )
{
char *p;
int len;

p = fgets( buffer, BUFSIZE, fh );
if ( p == NULL )
	return 1;
	
len = strlen( buffer );
if ( strlen(p) >=BUFSIZE && buffer[len-1] != '\r' && buffer[len-1] != '\n'  )  /* no end of line implies buffer too short */
	return 2;

return 0;	
}


/*************************************
Allocates memory for a context block and initializes it
*************************************/
static SqrtBugCtxt *GetSqrtBugCtxt ( void )
{
	SqrtBugCtxt *p;
	
	p = GetMemoryBlock( sizeof(SqrtBugCtxt) );
	
	memset( p, 0, sizeof(SqrtBugCtxt) );
	
		/* init to -1 because an offset of 0 is valid */
	p->OSSqrtStart = -1;
	p->OSOpenSquareBrace = -1;
	p->OSCloseSquareBrace = -1;
	p->OSOpenCurlyBrace = -1;
	p->OSCloseCurlyBrace = -1;
	
	return p;
}



/*************************************
Free a context block 
*************************************/
static void FreeSqrtBugCtxt ( SqrtBugCtxt *p )
{	
	FreeMemoryBlock( p );
}




/*************************************
*************************************/
static void ProcessLogFile( FILE *Log )
{
typedef enum { INITIAL, PROCESSING } StateType;
int Stat;
char *p;
char *MyLine;
StateType State = INITIAL;
LogLine *Current;
int LogLineNo = 0;

/* loop while we get data from the file */
while( (Stat = ReadLogLine( Log )) == 0 )
{
++LogLineNo;

/* skip comments and empty lines - a comment begins with a percent */
MyLine = TrimLogLine(buffer);	
if ( strlen(MyLine) < 1 )
	continue;
	
p = strstr( MyLine, "@sageinline" );
if ( p != NULL )
	{  /* at this point we have a new definition */
	   /*  walk past the sageinline header to the real data */
	while ( *p != 0 && *p != '{' )
		++p;
	if ( *p == 0 )
		ErrorExit("ProcessLogFile: there should be data here!" );
		
	Current = BuildLogLine( p );
	State = PROCESSING;
	SageLine[NextEmptySageLine] = Current;
    if ( ++NextEmptySageLine >= MAX_SAGE_LINES )
		ErrorExit( "ProcessLogFile: Too Many log lines" );
	}
else
	{ /* at this point it is either a continuation of the previous LogLine or an initial comment line */
	if ( State == PROCESSING )
		{
		Current->next = BuildLogLine( MyLine );
		Current = Current->next;
		}
	}
		
}

if ( Stat == 2 )
	printf( "Line too long in the Log File near line %d\n", LogLineNo );
}



/*************************************
*************************************/
static void PrintSageLine( int Which ) 
{
LogLine *Current;

DoTrace( "in - PrintSageLine\n" );
if ( Which >= NextEmptySageLine)
	ErrorExit( "Trying to print a sage substitution that does not exist" );
	
for ( Current = SageLine[Which]; Current != NULL; Current = Current->next )
	{
	WriteToOutputBuffer( Current->Line, strlen(Current->Line) );
	}

DoTrace( "out - PrintSageLine\n" );
}

/*************************************
*************************************/
static void CleanUpFixupStack ( int LineNum, int CurrentOffset )
{
	SqrtBugCtxt *ctxt, *prevctxt;
	int LoopCount = 0;
	BOOL CleanUp = TRUE;
	int StackAdjust;
	int ParentAdjust;
	
	
				/* clean up stack */
	prevctxt = NULL;
	ctxt = SqrtStack; 
	while( ctxt != NULL )
	{
		CleanUp = TRUE;
		++LoopCount;
		if ( ctxt->LineNoCloseCurlyBrace == 0 )  /* not at end yet */
			CleanUp = FALSE;

		if ( ctxt->LineNoCloseCurlyBrace > LineNum )  /* not at end yet */
			CleanUp = FALSE;
		
		if ( ctxt->LineNoCloseCurlyBrace == LineNum && ctxt->OSCloseCurlyBrace > CurrentOffset )  /* not at end yet */
			CleanUp = FALSE;
		
		if ( CleanUp )  /* stack entry is old news, sage on this line or next lines are after SQRT close */
		{
			TracePrintf( "Found dead \\sqrt[]{} in stack on line: %d, sqrt started on line %d\n", LineNo, ctxt->LineNoSqrtStart );
		
			/* delink from stack */
			if ( prevctxt != NULL )
				prevctxt->next = ctxt->next;
			else
				SqrtStack = ctxt->next;
			
			StackAdjust = 3 * (ctxt->SqrtFixupCount + ctxt->ChildFixupCount);
			ParentAdjust = 4 * (ctxt->ChildFixupCount + ctxt->SqrtFixupCount);
			
			TracePrintf( "Cleanup ctxt %p line: %d, sqrt started on line %d, stack adj is %d, Parent Adjust, if exists, is %d\n", 
					(void *) ctxt, LineNo, ctxt->LineNoSqrtStart, StackAdjust, ParentAdjust );;
			
			if ( ctxt->parent != NULL )
				ctxt->parent->ChildFixupCount += ParentAdjust;
			
			/* OK, handle the SQRT bug by tossing 3 additional lines for each sage command in the sqrt*/
			if ( StackAdjust != 0 )
			{
			TracePrintf( "CleanUpFixupStack - ctxt %p, fixupcount %d\n", (void *) ctxt, ctxt->SqrtFixupCount );
			NextSageLineOutput += StackAdjust;
			}	

			FreeSqrtBugCtxt( ctxt ); /* free entry */
			ctxt = SqrtStack; /* make another pass over the stack */
		}
		else  /* entry still active */
		{
			prevctxt = ctxt;
			ctxt = ctxt->next;
		}
	
	}

}



/*************************************
*************************************/
static BOOL IsSageInCtxt ( SqrtBugCtxt *ctxt, int LineNum, int pos )
{
	BOOL WeCare;
	
	WeCare = TRUE;  /* assume this context counts */
	if ( ctxt->OSOpenCurlyBrace < 0 ) /* no open curly brace yet - Not us */
	{
		TracePrintf( "IsSageInCtxt no match - ctxt %p, No open curly\n", (void *) ctxt );
		WeCare = FALSE;
	}

	if ( ctxt->LineNoCloseCurlyBrace >= 0 && ctxt->LineNoCloseCurlyBrace < LineNum ) /* our context closed before the sage entry - not us */
	{
		TracePrintf( "IsSageInCtxt no match - ctxt %p, after we existed\n", (void *) ctxt );

		WeCare = FALSE;
	}
		
	/* at this point we know the sage entry falls with in our line range what about position on the line */
	if ( WeCare && ctxt->LineNoOpenCurlyBrace == LineNum && ctxt->OSOpenCurlyBrace > pos ) /* before open curly brace - Not us */
	{
		TracePrintf( "IsSageInCtxt no match - ctxt %p, before we existed\n", (void *) ctxt );	
		WeCare = FALSE;
	}
		
	if ( WeCare && ctxt->LineNoCloseCurlyBrace == LineNum && ctxt->OSCloseCurlyBrace < pos ) /* after close curly brace - Not us */
	{
		TracePrintf( "IsSageInCtxt no match - ctxt %p, same line but after we existed\n", (void *) ctxt );	
		WeCare = FALSE;
	}

	return WeCare;
}


/*************************************
*************************************/
static SqrtBugCtxt *FindSqrtCtxtForSage( SqrtBugCtxt *startCtxt, int LineNum, int pos )
{
	SqrtBugCtxt *ctxt = NULL;

	
TracePrintf( "in - FindSqrtCtxtForSage line: %d, sage pos %d\n", LineNum, pos);	
	
	
	for( ctxt = startCtxt; ctxt != NULL; ctxt = ctxt->next )
	{
	if ( IsSageInCtxt ( ctxt, LineNum, pos ) )
		break;	
	}

	if ( ctxt == NULL )
	{
		DoTrace(  "out - FindSqrtCtxtForSage - NULL return" );	
	}
	else
	{
		DoTrace( "out - FindSqrtCtxtForSage - good return" );	
	}
	
	return ctxt;
}



/*************************************
*************************************/
static void AdjustFixupStackForSage ( int LineNum, int CurrentOffset )
{
	SqrtBugCtxt *ctxt;

	CleanUpFixupStack ( LineNum, CurrentOffset );   /* always clenup before adjusting */

	ctxt = FindSqrtCtxtForSage( SqrtStack, LineNum, CurrentOffset );
	if ( ctxt != NULL )
		(ctxt->SqrtFixupCount)++;
}



/*************************************
*************************************/
static void ProcessSageLine (char *line, char *LineStart )
{
int count;
char *p;
char *comment;
SqrtBugCtxt *ctxt;
int Offset;

if ( line == NULL || *line == 0 )
	return;

if ( *line == '%' ) /* line is a comment */
	{
	WriteToOutputBuffer( line, strlen(line) );
	return;
	}


comment = XTL_FindComment( line ); /* locate the start of any comment, we do not want to substitue inside a comment */
p = strstr( line, "\\sage{" ); /* find the first sage block */
if ( p == NULL ) /* line has no embedded sage commands */
	{
	WriteToOutputBuffer( line, strlen(line) );
	return;
	}

if ( comment != NULL && p > comment ) /* sage block is in the comment, we are done with this line */
	{
	WriteToOutputBuffer( line, strlen(line) );
	return;
	}

Offset = (int)(p - LineStart);

TracePrintf( "in - ProcessSageLine - sage at offset %d\n", Offset);	

CleanUpFixupStack ( LineNo, Offset );

AdjustFixupStackForSage ( LineNo, Offset );
	

*p = 0; /* terminate the string at the start of the sage block */

WriteToOutputBuffer( line, strlen(line) ); /* write out the first part of the line */

PrintSageLine( NextSageLineOutput );  /* Write out the sage data */

++NextSageLineOutput; /* Tee up the next one */


for ( ++p; *p != '{' && *p != 0; p++ ); /* spin to the { in the \sage{ */
if ( *p == '{' )
		++p; /* skip the open brace */


count = 1;	
p = XTL_FindClosingCurlyBrace( p, &count );
if ( p == NULL )
{
DoTrace("out1 - ProcessSageLine");	
	return;
}

++p;   /* skip the close brace */

	/* at this point we are either at the end of the line or just after the ending brace of the sage construct */
if ( p != 0 ) /* not end of line */
	{
	DoTrace( "Recurse - ProcessSageLine" );	
	ProcessSageLine( p, LineStart ); /* recurse to process the remainder of the line */
	}

DoTrace( "out0 - ProcessSageLine" );	
	
}


	
/*************************************
*************************************/
static void ProcessLineForSqrtFixup ( char *line, SqrtBugCtxt *ctxt, char *LineStart )
{
	char *p;
	int Offset;
	
	if ( line == NULL || *line == 0 ) /*  recursed to the end of line */
		return;
	 
	if ( *line == '%' ) /* line is a comment */
		return;

	if ( ctxt == NULL )
		return;
		
TracePrintf( "ctxt %p for line %d, line: %s", (void *) ctxt, LineNo, line );
TracePrintf( "Start State: %d\n", ctxt->SqrtState );

	p = line;  /* setup for state process that follows */
	
if ( ctxt->SqrtState == SquareOpen )
{
	p = XTL_FindClosingSquareBrace( p, &(ctxt->SquareBraces) );
	if ( p != NULL )
	{
		Offset = (int) (p - LineStart);
		ctxt->OSCloseSquareBrace = Offset;	/* store offset from start of line */		
		ctxt->SqrtState = SquareClose;
		++p;
	}
}

if ( ctxt->SqrtState == SquareClose )
{
	for ( ; *p != 0 && *p != '{'; p++ ); /* advance to the open brace in the sqrt[ string */
	if ( *p == '{' )
		{		
		Offset = (int) (p - LineStart);
		ctxt->OSOpenCurlyBrace = Offset;  	/* store offset from start of line */		
		ctxt->SqrtState = CurlyOpen;	
		ctxt->LineNoOpenCurlyBrace = LineNo;		
		++p; /* skip the brace now that we found it */	
		ctxt->CurlyBraces = 1;
		}	
}

if ( ctxt->SqrtState == CurlyOpen )
{
	p = XTL_FindClosingCurlyBrace( p, &(ctxt->CurlyBraces) );
	if ( p != NULL )
	{			
		Offset = (int) (p - LineStart);
		ctxt->OSCloseCurlyBrace = Offset;			/* store offset from start of line */		
		ctxt->SqrtState = CurlyClose;
		ctxt->LineNoCloseCurlyBrace = LineNo;
		TracePrintf( "ctxt: %p close line %d, open offset %d, close offset %d\n", 
			(void *) ctxt, ctxt->LineNoCloseCurlyBrace, ctxt->OSOpenCurlyBrace, ctxt->OSCloseCurlyBrace );
		++p;
	}
}

TracePrintf( "End State: %d\n", ctxt->SqrtState );
}


/*************************************
*************************************/
static void ProcessForSqrtFixup ( char *line )
{
	char *p, *pSqrtStart, *comment;
	char SqrtBuffer[XIO_MAX_LINE + 1];	
	SqrtBugCtxt *ctxt;
	int count = 0;

	
	if ( line == NULL || *line == 0 ) /*  recursed to the end of line */
		return;
	 
	if ( *line == '%' ) /* line is a comment */
		return;

	CleanUpFixupStack( LineNo, 0 );
	
	strcpy( SqrtBuffer, line );
	comment = XTL_FindComment( SqrtBuffer ); /* locate the start of any comment, we do not want to substitue inside a comment */
	if ( comment != NULL )
			*comment = 0;  /* truncate the temp buffer at the comment since we do not do output */

	p = SqrtBuffer;  /* setup for state process that follows */

	/* process old stack entries updating their states */
	for( ctxt = SqrtStack; ctxt != NULL; ctxt = ctxt->next )
		ProcessLineForSqrtFixup ( p, ctxt, SqrtBuffer );
	
	/* does this line have any sqrt statements we care about */
	do 
	{
		pSqrtStart = strstr( p, "\\sqrt[" ); /* find the first sqrt with optional args block */
		if ( pSqrtStart != NULL )  /* we have a live one */
		{
			++count;
			ctxt = GetSqrtBugCtxt();
			TracePrintf( "Found \\sqrt[]{} for fixup on line: %d added ctxt %p\n", LineNo, (void *) ctxt );		
			ctxt->parent = FindSqrtCtxtForSage(SqrtStack, LineNo, (int)(pSqrtStart - SqrtBuffer));
			ctxt->OSSqrtStart = pSqrtStart - SqrtBuffer;  /* store offset from start of line */		
			ctxt->LineNoSqrtStart = LineNo;
			ctxt->SqrtFixupCount = 0;
			for ( p = pSqrtStart; *p != 0 && *p != '['; p++ ); /* advance to the open brace in the sqrt[ string */
			ctxt->OSOpenSquareBrace = p - SqrtBuffer;   /* store offset from start of line */		
			++p; /* skip the squarebrace now that we found it */
			ctxt->SqrtState = SquareOpen;
			ctxt->SquareBraces = 1;
			ctxt->next = SqrtStack;  /* link into stack */
			SqrtStack = ctxt;
			ProcessLineForSqrtFixup ( p, ctxt, SqrtBuffer );
		}
	} while ( pSqrtStart != NULL );

	if ( count > 0 )
	{
		TracePrintf( "Line has %d sqrt[] calls\n", count );
	}
	
}


/*************************************
*************************************/
static void ProcessInputFile ( void )
{
typedef enum { DATA, SILENT } StateType;
int Stat;
StateType State = DATA;


while( (Stat = ReadLine(  )) == 0 )
{
++LineNo;

if ( State == SILENT )
	{
	if ( strstr( LineBuffer, "\\end{sagesilent}" ) != NULL )
		State = DATA;
	}
else
	if ( strstr( LineBuffer, "\\begin{sagesilent}" ) != NULL )
		State = SILENT;
	else
	{
		if ( ProcessForSqrtBug )
			ProcessForSqrtFixup( LineBuffer );
		
		ProcessSageLine( LineBuffer, LineBuffer );
	}
}

if ( Stat == 2 )
	printf( "Line too long in tex file near line %d\n", LineNo );

}


/*************************************
*************************************/
void merge( char *InputFileName )
{

char LogFileName[100];
char *p;
FILE *Log;


strcpy( LogFileName, InputFileName );

p = strstr( LogFileName, ".tex" );
if ( p == NULL )
	ErrorExit( "Need a .tex file" );
*p = 0;

strcat( LogFileName, ".sagetex.sout" );
	
Log = fopen( LogFileName, "r" );
if ( Log == NULL )
	{
	sprintf( LineBuffer,  "Failed to open Log file: %s", LogFileName );
	ErrorExit( LineBuffer );
	}

ProcessLogFile( Log );
fclose( Log );

printf( "Log File Processed: %d entries\n", NextEmptySageLine );

ProcessInputFile(  );

printf( "Input File Processed: %d lines\n", LineNo );


if ( NextEmptySageLine != NextSageLineOutput )
	printf( "Sage log entries count mismatch, log entries: %d, entries used %d\n", NextEmptySageLine, NextSageLineOutput );

}

/*************************************
*************************************/
void MergeSetSqrtBugProcessingFlag( BOOL flag )
{
	ProcessForSqrtBug = flag;
}

