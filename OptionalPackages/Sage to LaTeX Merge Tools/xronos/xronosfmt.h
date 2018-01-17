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

#include "danalib.h"

#include "xronostoollib.h"
#include "xronosio.h"

#define ProblemStartString "\\latexProblemContent{"

/* global typedefs, enums etc. */




/* globals located in xronosfmt.c but shared */
extern char OutputFileName[XTL_MAX_FILENAME_SIZE + 1];
extern char CurrentDirectory[XTL_MAX_FILENAME_SIZE + 1];
extern BOOL QuestionsOnly;

/* function prototypes */
void merge( char *InputFileName );
int KillDups( void );
int Braces( void );
int QuestionLimit( void );
int TextttRemoval( void );
void SetMaxQuestions( int num );
int GetActualQuestionCount( void );
void BuildOutputHeader( BOOL TagsRequired );
int WriteOutputHeader( FILE *out );
char *XronosfmtOptionGet( char *name );
char *GetXronosfmtOutputPath( void );  /* value of the command line switch output_path */
