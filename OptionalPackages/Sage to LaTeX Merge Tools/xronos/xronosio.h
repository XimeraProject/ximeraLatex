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

/* 
What is the purpose of these routines?

In processing this stuff it became apparent that I would need 
multiple passes over the file.  In many cases the pass would 
modify the file for the next round of processing but in some 
cases we simply build context to carry forward for other passes.

So lots of temp files and/or a dattabase to hold context and ...
Well you did read in the header that this is a hack, right?

So read whole file into memory, pass over it and build context 
then read it again to process it. Or read it and modify it then 
the next pass can read/modify that output.  

So we read it into memory (ReadInputFile) then read the data 
from the internal buffer one line at a time (ReadLine) to 
"process it" if we are modifying it, we write out the new file 
(WriteToOutputBuffer) when done we reset for the next pass 
(RecycleBuffers).  The reset will check too see if we wrote 
any output, if NOT it resets the read ptr to the start and we 
can go again.  If we DID write, it copies the output buffers 
to the input buffers and resets the positioning for the "new file".
We can start processing for the next round by calling ReadLine 
again and go forward from there.

So basically we are setting up a REALLY dumb (and specific) 
virtualized file read/write setup for the multiple passes. 
This avoids temp files, databases, ... and hopefully hides 
most of the magic in these routines.

NOTE: the maximum size file that should be processed with 
these functions is about 9 MB.  That is input side, output 
side or any of the interim steps should not exceed.  Not 
JUST input or output.  Basically a restriction of the 
internal buffers sides, do not want to walk off the end.
*/


#ifndef XRONOSIOH_INCLUDED

#define XRONOSIOH_INCLUDED 1  /* define it so we do not double include */

#define XIO_MAX_LINE  5000 	/* maximum text line supported */

/* LineBuffer[] the current line we get/result when we call ReadLine() */
extern char LineBuffer[XIO_MAX_LINE + 1];    /* allow for the string terminator */


/*************************************
void XronosIOInit( void )

Initializes the XronosIO library of functions
*************************************/
void XronosIOInit( void );

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
int ReadLine( void );

/*************************************
void WriteToOutputBuffer( char *line, int size )

Takes the string pointer to by line for size bytes and 
places it into the output buffer

*************************************/
void WriteToOutputBuffer( char *line, int size );

/*************************************
void RecycleBuffers( void )

if the output buffer is not empty, copy it to the 
input buffer and reset all the lengths/positions/ptrs 
*************************************/
void RecycleBuffers( void );

/*************************************
void ReadInputFile( char *InputFileName )

read the physical file and place it into the buffers to treat virtually

*************************************/
void ReadInputFile( char *InputFileName );

/*************************************
int XronosIO_SetHeaderFile( char *filename )

stores the header filename for later use
this file will be added to the top of the
file read in ReadInputFile()

returns 0 on success, -1 on error
*************************************/
int XronosIO_SetHeaderFile( char *filename );

/*************************************
int XronosIO_SetFooterFile( char *filename )

stores the footer filename for later use
this file will be added to the bottom of the
file read in ReadInputFile()

returns 0 on success, -1 on error
*************************************/
int XronosIO_SetFooterFile( char *filename );


#endif

