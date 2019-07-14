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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "danalib.h"
#include "xronostoollib.h"
#include "xronosio.h"


#define XIO_MAX_FILESIZE 10000000  /* maximum input or output file allowed */


char LineBuffer[XIO_MAX_LINE + 1];    /* the current line we get/result when we call ReadLine() */
static char *InBuff, *OutBuff;   /* internal buffers used as input/output for each pass over the "file" */
static int InLen, OutLen, InPos;  /* buffer lengths and the current position, or "read pointer" */
static char IO_FileForHeader[500];
static char IO_FileForFooter[500];



/*************************************
*************************************/
static void CheckIOInit( void )
{
	if ( InBuff == NULL )  /* if not initialized, bail out */
		ErrorExit( "XronosIO not initialized.  Did you call XronosIOInit()?" );

}



/*************************************
int XronosIO_SetHeaderFile( char *filename )

stores the header filename for later use;

returns 0 on success, -1 on error
*************************************/
int XronosIO_SetHeaderFile( char *filename )
{
	strncpy( IO_FileForHeader, filename, sizeof(IO_FileForHeader) - 1);

	if ( strlen( IO_FileForHeader ) != strlen( filename ) )
		return -1;
	
	return 0;
}

/*************************************
int XronosIO_SetFooterFile( char *filename )

stores the footer filename for later use;

returns 0 on success, -1 on error
*************************************/
int XronosIO_SetFooterFile( char *filename )
{
	strncpy( IO_FileForFooter, filename, sizeof(IO_FileForFooter) - 1);
	
	if ( strlen( IO_FileForFooter ) != strlen( filename ) )
		return -1;
	
	return 0;
}

/*************************************
void XronosIOInit( void )

Initializes the XronosIO library of functions
*************************************/
void XronosIOInit( void )
{
		/* initialize buffers to use as virtual files */
	InBuff = GetMemoryBlock(XIO_MAX_FILESIZE + 1);
	OutBuff = GetMemoryBlock(XIO_MAX_FILESIZE + 1);

}


	
	
/*************************************
int ReadLine( void )

reads data from the virtual file that was initialized by 
ReadInputFile().  Returns it one line (\n) at a time.  By
returns I mean places in into LineBuffer[] and terminates 
the string.

returns:
  0 - success
  1 - end of file
  2 - line too long
  3 - Not Initialized
*************************************/
int ReadLine( void )
{

int LinePos;
BOOL HaveReturn = FALSE;

CheckIOInit( );  /* if not initialized, bail out */

if ( InPos >= InLen )
	return 1;

/* walk the input buffer copying to the working buffer until we get an end of line */
for( LinePos = 0; InBuff[InPos] != 0; ++LinePos )
{
	if ( LinePos >= XIO_MAX_LINE )
		return 2;
	
	if ( InBuff[InPos] == '\r' || InBuff[InPos] == '\n' )
		HaveReturn = TRUE;
	else
		if ( HaveReturn )
			break;
		
	LineBuffer[LinePos] = InBuff[InPos];
	++InPos;
}

LineBuffer[LinePos] = 0;

return 0;	
}





/*************************************
void WriteToOutputBuffer( char *line, int size )

Takes the string pointer to by line for size bytes and 
places it into the output buffer

*************************************/
void WriteToOutputBuffer( char *line, int size )
{
	CheckIOInit( );  /* if not initialized, bail out */

	if ( (OutLen + size) >= XIO_MAX_FILESIZE )
		ErrorExit( "output file too large" );
	
	memcpy( &OutBuff[OutLen], line, size );
	OutLen += size;
	OutBuff[OutLen] = 0;
	
}



/*************************************
void RecycleBuffers( void )

if the output buffer is not empty, copy it to the 
input buffer and reset all the lengths/positions/ptrs 
*************************************/
void RecycleBuffers( void )
{
	CheckIOInit( );  /* if not initialized, bail out */

	if ( OutLen > 0 )
	{
		memcpy( InBuff, OutBuff, OutLen+1);  /* copy output over to new input */
		InLen = OutLen;  /* set the new input length */
		OutLen = 0;  /* new output is zero */
		OutBuff[0] = 0; 
	}
	
	InPos = 0; /* we reposition the "read pointer" back to the start of the "new input file",, see ReadLine() function above */
}


/*************************************
void ReadInputFile( char *InputFileName )

read the physical file and place it into the buffers to treat virtually

*************************************/
static void IO_ConcatInputFile( char *InputFileName )
{
	FILE *Input;
	int num;

	TracePrintf( "ReadInputFile: Opening input file: %s\n", InputFileName );
	
	Input = fopen( InputFileName, "r" );
	if ( Input == NULL )
		ErrorExit( "Failed to open input file" );
	
	num = fread(&InBuff[InLen], 1, XIO_MAX_FILESIZE - InLen, Input );
	fclose( Input );
	
	if  (num < 1 )
		ErrorExit( "Bad read on input file" );
	
	InLen += num;
	InBuff[InLen] = 0;
	
/*	InLen = strlen(InBuff); */
	if ( InLen >= XIO_MAX_FILESIZE )
		ErrorExit( "Input File too large" );
		
}

/*************************************
void ReadInputFile( char *InputFileName )

read the physical file and place it into the buffers to treat virtually

*************************************/
void ReadInputFile( char *InputFileName )
{
	CheckIOInit( );  /* if not initialized, bail out */
	
	InLen = 0;
	if ( strlen(IO_FileForHeader) > 0 )
	{
		IO_ConcatInputFile( IO_FileForHeader );
		TracePrintf( "ReadInputFile: Buffer has %d characters\n", InLen );

	}
		
	IO_ConcatInputFile( InputFileName );
	TracePrintf( "ReadInputFile: Buffer has %d characters\n", InLen );

	if ( strlen(IO_FileForFooter) > 0 )
	{
		IO_ConcatInputFile( IO_FileForFooter );
		TracePrintf( "ReadInputFile: Buffer has %d characters\n", InLen );
	}
		
}

