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

void MergeSetSqrtBugProcessingFlag( BOOL flag );

static char ConfFileName[500] = "/usr/local/etc/xronostools/xronostools.conf";
static char FileForOutputHeader[500];
static char FileForOutputFooter[500];


char OutputFileName[XTL_MAX_FILENAME_SIZE + 1];

static char *Copyright[] = { 
"\nCopyright 2017 by Dana Nowell, All rights Reserved",
"Educational Institutions are herein granted the right to use",
"and modify the code for their own use.  Rights for use in",
"a sale or other Commercial enterprise require written", 
"permission from the author.",
"\n",  /* blank line */
NULL };  /* end marker */

static char *Instructions[] = { "Needs arguments: Inputfile name is required\n",
"It can be followed by:",
"\t -Debug or /Debug to allow global debug tracing of the program for locating processing errors.",
"It is NOT expected that these switches will be used in production, more of a development tool.",
NULL };  /* end marker */

/* commandline switch flags */
static BOOL TagsRequired = TRUE;
BOOL QuestionsOnly = TRUE;
char CurrentDirectory[XTL_MAX_FILENAME_SIZE + 1];
char xronosfmt_outputpath[XTL_MAX_FILENAME_SIZE + 1];

/*************************************
*************************************/
char *GetXronosfmtOutputPath( void )
{
return 	xronosfmt_outputpath;
}


/*************************************
*************************************/
static void WriteOutputFile( char *OutputFileName )
{
	FILE *Out;
	int num;
	XTL_FileStatus XTLFileStat;
	
	
	printf( "Checking to see if we need to make the directory for the output file %s\n", OutputFileName );
	MakeDirChainForFile( OutputFileName );
	
	printf( "Opening output file %s - ", OutputFileName );

	/* now write the actual output file */
	Out = fopen( OutputFileName, "w+");
	if ( Out == NULL )
		ErrorExit( "Failed to open output file" );


	puts( "Writing actual file data" );

	/* DANA - FIXUP */
	while ( ReadLine( ) == 0 )
	{	
	num = fwrite(LineBuffer, 1, strlen(LineBuffer), Out );
	}
	
	if ( !QuestionsOnly	 )  /* if the user requested we include the document structure stuff intact */
		fputs( "\\end{document}", Out ); /* write the end document line */
				
	fclose(Out);
	puts( "output file complete" );

}



/*************************************
*************************************/
static void ProcessEnvVars( void )
{

	XTL_GetEnvVariable( "XRONOSTOOLS_CONF", ConfFileName, sizeof(ConfFileName) );	
	
}

/*************************************
*************************************/
static void ProcessCommandLineSwitch( char *MySwitch, char *Value )
{
	char buf[200];
	char *p;

	if ( MySwitch == NULL || *MySwitch == 0 ) /* do we actual have a switch at all */
		return;
	
	if ( strlen( MySwitch ) >= sizeof( buf ) )  /* too long for a real switch do not overrun buffer */
		return;
	
	strcpy( buf, MySwitch );
	
		/* Make it lower case */
	for ( p = buf; *p != 0; ++p )
		*p = tolower((unsigned char) *p);
	
	if ( strcmp( "debug", buf ) == 0 )  /* debug */
		TraceOn( TRUE );
	else 
	{
		sprintf( buf, "Unknown command line switch %s", MySwitch );
		ErrorExit( buf );
	}
}


/*************************************
*************************************/
static BOOL HandleSubstitution()
{
BOOL done = FALSE;
int LineNo = 0;
char *FixedLine;
BOOL HadChanges = FALSE;

/* now process the file */
while ( !done )
	{
	if ( ReadLine( ) != 0 )
		done = TRUE;
	else
		{
		++LineNo;
		/*
		TracePrintf( "HandleSubstitution: Processing Line %d\n", LineNo );
		*/
		FixedLine = XTL_Substitute( LineBuffer );
		
		if ( FixedLine == NULL )  /* Bad things happened */
			ErrorExit( "HandleSubstitution() Line blew up" );
		
		if ( ! HadChanges )
		{
			if ( strcmp( FixedLine, LineBuffer ) != 0 )
				HadChanges = TRUE;
		}
	
		WriteToOutputBuffer( FixedLine, strlen(FixedLine) );		
		FreeMemoryBlock( FixedLine );
		}
	}

	
printf( "HandleSubstitution: Processed %d lines for substitution processing this pass\n", LineNo );

return HadChanges;

}

/*************************************
*************************************/
int main( int argc, char *argv[])
{
char *p;
char *Switch, *Value;
char InputFileName[200];
int i;
XTL_FileStatus XTLFileStat;
BOOL SageBug_Sqrt = TRUE;

/* initialize trace to off */
TraceInit(  );
SetTraceOutput( stdout );  /* defaults to stderr if not set, we want stdout */
TraceOn( FALSE );

DL_CL_ProcessArgs( argc, argv );
p = DL_CL_GetProgName( );

printf( "%s", p );
for( i = 0; Copyright[i] != NULL; ++i )
	puts( Copyright[i] );
	
if ( getcwd(CurrentDirectory, sizeof(CurrentDirectory) - 1) == NULL )
{
	CurrentDirectory[0] = 0;
	puts( "Failed to get current working directory" );
}


if (  DL_CL_GetArgCount() != 1 )
{
	for( i = 0; Instructions[i] != NULL; ++i )
		puts( Instructions[i] );
	
	ErrorExit( "\n" );
}
	
strcpy( InputFileName, DL_CL_GetArg( 0 ) );


for ( i = 0; DL_CL_GetSwitch( i, &Switch, &Value ); i++ )
{
	ProcessCommandLineSwitch( Switch, Value );
}

	/* Process and envirnmant variables */	
	ProcessEnvVars();


/* initialize the XronosIO library */
XronosIOInit();
XTL_SubVars_Init();

	/* init stuff from Conf file */
	puts( "Processing Conf file" );
	printf( "Conf File: %s\n", ConfFileName );
	if ( DL_ReadIniFile(ConfFileName) != 0 )
		ErrorExit( "Failed to read conf file" );

	SageBug_Sqrt = DL_GetIniBOOLValue( "SageBug_Sqrt", SageBug_Sqrt );

	/* set whether or not we do processing the the sqrt bug in sage from the conf file value */
	MergeSetSqrtBugProcessingFlag( SageBug_Sqrt ); 

	
	DL_DiscardIniFile();
	puts( "Done Processing Conf file" );
	/* end ini stuff */

/* output file samme as input, we merge in place */
strcpy( OutputFileName, InputFileName );

/* Load file into memory */
printf( "*** Reading input file: %s\n", InputFileName );
ReadInputFile( InputFileName );

/* handle merge - MUST be first before all other passes over the "file" */
puts( "*** Merge Processing" );
merge( InputFileName );

/* write output file */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
puts( "*** Writing Output file" );
WriteOutputFile( OutputFileName );

puts( "*** Program Complete\n" );
return 0;
}
