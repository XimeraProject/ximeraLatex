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

#include "xronosfmt.h"

 /* defines the maximum number of questions we will output */
#define MAX_QUESTIONS 500  /* default number */
#define ABSOLUTE_MAX_QUESTIONS 5000

static int LineNo = 0;
static char SearchString[200] = ProblemStartString;
static int MaxQuestions = MAX_QUESTIONS;



/*************************************
*  Handle each line, basically output it until we reach max questions
*************************************/
static int HandleLine( char *line )
{
static int QuestionCount = 0;
char *p;	

	p = strstr( line, SearchString );
	if ( p != 0 )
	{
		++QuestionCount;
		if ( QuestionCount > MaxQuestions )
		{  /* we are done at previous question, do not count this one and return done */
			--QuestionCount;
			*p = 0; /* tterminate line at start of MAX_QUESTIONS + 1 */
			if ( strlen(line ) > 0 ) /* if line had data before that question, write it out */
				WriteToOutputBuffer( line, strlen(line) );	

			XTL_SubVars_UpdateInt( "XTL_SV_QuestionCount", QuestionCount );
			return -1;  
		}		
	}
	
	XTL_SubVars_UpdateInt( "XTL_SV_QuestionCount", QuestionCount );

	WriteToOutputBuffer( line, strlen(line) );		
	return 0;
}



/*************************************
*  Process any options
*************************************/
void SetMaxQuestions( int num )
{
	if ( num > ABSOLUTE_MAX_QUESTIONS ) /* set an absolute max */
		num = ABSOLUTE_MAX_QUESTIONS;

	if ( num > 0 )
		MaxQuestions = num;
}


/*************************************
*  Main routine
*************************************/
int GetActualQuestionCount( void )
{
	return XTL_SubVars_GetValueInt( "XTL_SV_QuestionCount" );
}


/*************************************
*  Main routine
*************************************/
int QuestionLimit( void )
{
BOOL done = FALSE;
int status;

/* now process the file */
while ( !done )
	{
	if ( ReadLine( ) != 0 )
		done = TRUE;
	else
		{
		++LineNo;
		/*
		TracePrintf( "Processing Line %d\n", LineNo );
		*/
		
		status = HandleLine( LineBuffer );
		if ( status != 0 )  /* if not zero then either we have an error OR we reached max questions, so we are done */
			done = TRUE;
			
		if ( status > 0 )  /* positive values imply error */
			ErrorExit( "Handle Line blew up" );
		}
	}

	
printf( "Processed %d lines for QuestionLimit, output %d questions with a question limit set to %d\n", LineNo, GetActualQuestionCount( ), MaxQuestions );


return 0;
}
