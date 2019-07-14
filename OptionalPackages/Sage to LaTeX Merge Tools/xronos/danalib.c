
/****************************************************
Copyright 2016 Dana Nowell, all rights reserved.

Standard set of functions to make life easier.  
I have been using these for decades but figured 
I should just put them all in one place.

Licensing:
Commercial use of this code is forbidden with out written permission
The code IS free to use in educational institutions 
and a license for that use is granted herein.

****************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#include "dirent.h"


#include "danalib.h"

#define DL_MAX_FILENAME_SIZE 5000


static BOOL bTraceOn = FALSE;
static FILE *TraceOut;


/*************************************
*************************************/
void TraceInit( void )
{
	if ( TraceOut == NULL )
		TraceOut = stderr;
}


/*************************************
*************************************/
void SetTraceOutput( FILE *val )
{
	if ( val != NULL )
		TraceOut = val;
}


/*************************************
*************************************/
void TraceOn( BOOL val )
{
	
	if ( TraceOut == NULL )
		ErrorExit("Trace Not Initialiized" );
	
	bTraceOn = val;
}


/*************************************
*************************************/
void TracePrintf( char *fmt, ... )
{
	va_list args;
	char TraceBuffer[DL_MAX_TRACELINE_LEN+1];

	TraceBuffer[0] = 0;
	
	/* initialize valist */
	va_start( args, fmt );

	vsnprintf( TraceBuffer, sizeof(TraceBuffer) - 1, fmt, args );

	/* clean memory reserved for valist */
	va_end(args);

	if ( GetTraceStatus() )
	{
		fprintf( TraceOut, "TRACE: %s ", TraceBuffer );
		fflush( NULL );
	}
}


/*************************************
*************************************/
BOOL GetTraceStatus( void )
{
	BOOL status = bTraceOn;
	
	if ( TraceOut == NULL )
		status = FALSE;
	
	return	status;
}


/*************************************
*************************************/
void DoTrace( char *p )
{
	if ( GetTraceStatus() && p != NULL )
	{
		fprintf( TraceOut, "TRACE: %s\n", p );
	}

	fflush( NULL );
}


/*************************************
*************************************/
void DoTraceWithLine( char *p, int LineNo )
{
	if ( GetTraceStatus() && p != NULL )
	{
		fprintf( TraceOut, "TRACE: line: %d : %s\n", LineNo, p );
		fflush( NULL );
	}
}




/*************************************
*************************************/
void ErrorExit( char *p)
{

	if ( p != NULL )
		fputs( p, stderr );

	fputs( "\n", stderr );
 
	exit(1); 
}  


/*************************************
void DL_TrimQuotesInPlace( char *line );

if string has both a leading and matching trailing 
quote (double or single) this function strips them 
off in place (changes the argument string)

returns nothing
*************************************/
void DL_TrimQuotesInPlace( char *line )
{
	char Quote;
	char *MyBuffer;
	char *p;
	
	
	p = line;
	Quote = '\"';

	/* see if we start with a quote and if so, which type */
	if ( *p == Quote )
		++p;
	else 
	{
		Quote = '\'';
		if ( *p == Quote )
		{
			++p;
		}
		else
			Quote = 0;
	}
	
	/* did we start with one? */
	if ( Quote != 0 )
	{  /* OK look for a close quote of the same type */
		MyBuffer = GetMemoryBlock( strlen(line) + 1);  /* get some working space */
		strcpy( MyBuffer, p );  /* skip over the open quote */
		p = FindEndOfString( MyBuffer ); /* last character */
		if ( *p == Quote ) /* look for a close quote */
		{
			*p = 0;
			strcpy( line, MyBuffer );  /* put it back, we have matching quotes we dealt with */
		}
		
		FreeMemoryBlock( MyBuffer );  /* clean up working space */	
	}
	
}

/*************************************
*************************************/
char *TrimLeadingWhitespace( char *p )
{
if ( p == NULL || *p == 0)
	return p;

while ( strchr( "\r\n\t ", *p ) != NULL && *p != 0 )	
	{
	++p;
	}
	
return p;	
}


/*************************************
*************************************/
void TrimTrailingWhitespace( char *p )
{
char *q;

if ( p == NULL || *p == 0)
	return;

q = FindEndOfString( p );
while ( strchr( "\r\n\t ", *q ) != NULL )
	{
	*q = 0;
	--q;
	if ( q < p )
		break;
	}
	
}


/*************************************
*************************************/
char *TrimWhitespace( char *p )
{
TrimTrailingWhitespace( p );
return TrimLeadingWhitespace( p );
}




/*************************************
*************************************/
void *GetMemoryBlock( int size )
{
void *p;

p = malloc( size );

if ( p == NULL )
	ErrorExit( "Failed to allocate memory" );

/* lots of assumptions in code (like end of string) that block is all zeros */	
memset( p, 0, size );	

return p;

}


/*************************************
*************************************/
void FreeMemoryBlock( void *p )
{

if ( p == NULL )
	ErrorExit( "Attempt to free NULL pointer" );

free(p);
}



/*************************************
* input to this function assumes a FILE like name 
* (no ending slash like a direcotry might have)
*************************************/
BOOL DoesFileExist( char *OutputFileName )
{
	struct stat st = {0};
	BOOL retval = TRUE;

	if ( OutputFileName == NULL || *OutputFileName == 0 )
	return FALSE;

	if (stat( OutputFileName, &st ) == -1) 
		retval = FALSE;	
	
	return retval;
}


static int DL_MKDIR( char *name  )
{
#ifdef DANALIB_WIN
	return mkdir( name );
#else
	return mkdir( name, 0700);
#endif
}


/*************************************
* input to this function assumes a FILE not a directory
* you CAN pass a directory BUT it muust end with / or
* it will be assumed to be a FILE and not a Directory
*************************************/
BOOL MakeDirChainForFile( char *OutputFileName )
{
	struct stat st = {0};
	BOOL retval = TRUE;
	char *p;
	char ParseBuffer[DL_MAX_FILENAME_SIZE + 1];

	
	if ( OutputFileName == NULL || *OutputFileName == 0 )
		return FALSE;
	
	if ( strlen( OutputFileName ) > DL_MAX_FILENAME_SIZE )
		ErrorExit( "DanaLib - MakeDirChainForFile: Filename is too long" );
	
	strcpy( ParseBuffer, OutputFileName );
	
	/* start at end and walk back until we find a /, basically strip off the filename and leave the path*/
	for ( p = FindEndOfString( ParseBuffer ); p != ParseBuffer; --p )
	{
		if ( *p == '/' )
		{
			*p = 0;
			break;
		}
	}
	
	if ( p <= ParseBuffer )  /* did not find a slash, file is "local", no directory to create */
		return FALSE;;

	if ( !DoesFileExist(ParseBuffer) ) 
	{
		MakeDirChainForFile( ParseBuffer );  /* recurse so we start at the FIRST one we need to make */
		TracePrintf( "Making directory %s\n", ParseBuffer );
		if ( DL_MKDIR( ParseBuffer) != 0 )
		{
			retval = FALSE;
			TracePrintf( "Error (%d) %s making directory %s\n", errno, strerror(errno ), ParseBuffer );
		}
			
	}
	
	return retval;
}
	


/*************************************
*************************************/
unsigned char *DL_STRUPR( unsigned char *str )
{
	unsigned char *p;
	
	if ( str == NULL )
		return NULL;
	
	for( p = str; *p != 0; ++p )
	{
		if ( *p >= 'a' && *p <= 'z' )
			*p = (unsigned char) ((unsigned int) *p + ((unsigned int) 'A' - (unsigned int) 'a'));
	}
	
}


/*************************************
*************************************/
char *FindEndOfString( char *str )
{
	char *p;
	
	if ( str == NULL || *str == 0 )
		return str;
	
	p = str;
	p += strlen(p) - 1;

	return p;
}	
