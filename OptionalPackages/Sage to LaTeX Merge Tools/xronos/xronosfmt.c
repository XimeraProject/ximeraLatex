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
BOOL SB_Texttt_removal = TRUE;
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

	puts( "Writing file header data" );

	if ( strlen( FileForOutputHeader ) > 0 )  /* if we have a fileheader */
	{
		if ( XTL_CopyFileContents( FileForOutputHeader, Out) != 0 )
			ErrorExit( "Unable to copy FileForOutputHeader" );
	}

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
static int AddSearchReplacePair( char *Search, char *Replace )
{
	if ( Search == NULL || Replace == NULL )
		return -999;

	Search = TrimWhitespace(Search);
	Replace = TrimWhitespace(Replace);
	DL_TrimQuotesInPlace( Search );
	DL_TrimQuotesInPlace( Replace );
	
	printf( "Adding Search and Replace Pair: Search is \"%s\"\n", Search );

	return  XTL_SubVars_Add( Search, Replace, TRUE );
}


/*************************************
*************************************/
static void InitUserDefinedSubstitution( void )
{
	char *SearchString, *ReplaceString;
	char Tempbuffer[50];
	int i;

	for ( i = 1; i < 100; i++ )
	{
	sprintf( Tempbuffer, "XF_USERSUB%02d_SEARCH", i );
	SearchString = DL_GetIniValue( Tempbuffer, NULL );
	sprintf( Tempbuffer, "XF_USERSUB%02d_REPLACE", i );
	ReplaceString = DL_GetIniValue( Tempbuffer, NULL );
	if ( SearchString != NULL && ReplaceString != NULL )
		AddSearchReplacePair( SearchString, ReplaceString );
	}

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

	p = DL_GetIniValue( "XF_MaxQuestions", NULL );
	if ( p != NULL )
	{
		SetMaxQuestions( atoi(p) );
	}
	
	QuestionsOnly = DL_GetIniBOOLValue( "XF_QuestionsOnly", QuestionsOnly );
	
	TagsRequired = DL_GetIniBOOLValue( "XF_TagsRequired", TagsRequired );
	
	SB_Texttt_removal = DL_GetIniBOOLValue( "SageBug_Texttt_removal", SB_Texttt_removal );

	InitUserDefinedSubstitution( );
	
	p = DL_GetIniStringValue( "XF_OutputFileHeader", NULL );
	if ( p != NULL )
	{
	strcpy( FileForOutputHeader, p );
	}
	
	p = DL_GetIniStringValue( "XF_OutputFileFooter", NULL );
	if ( p != NULL )
	{
	strcpy( FileForOutputFooter, p );
	}
	
	p = DL_GetIniStringValue( "XF_InputFileHeader", NULL );
	if ( p != NULL )
	{
	if ( XronosIO_SetHeaderFile( p ) != 0 )
		ErrorExit( "failed to set XF_InputFileHeader value" );
	printf( "Set Input HeaderFile: %s\n", p );
	}
	
	p = DL_GetIniStringValue( "XF_InputFileFooter", NULL );
	if ( p != NULL )
	{
	if ( XronosIO_SetFooterFile( p ) != 0 )
		ErrorExit( "failed to set XF_InputFileFooter value" );
	printf( "Set Input FooterFile: %s\n", p );
	}
	
	DL_DiscardIniFile();
	puts( "Done Processing Conf file" );
	/* end ini stuff */

strcpy( OutputFileName, InputFileName );

/* Load file into memory */
printf( "*** Reading input file: %s\n", InputFileName );
ReadInputFile( InputFileName );

/* kill dups */
puts( "*** Killing Dups" );
KillDups( );

/* deal with braces */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
puts( "*** Braces processing" );
Braces( );

/* deal with question limit on file size */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
puts( "*** Question Limit processing" );
QuestionLimit( );

if ( SB_Texttt_removal )
{
	/* deal with \text{\texttt{}} sage bug */
	RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
	puts( "*** Texttt Removal processing" );
	TextttRemoval( );
}

/* deal with substitution processing - should be JUST before WriteOutputFile() */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
puts( "*** Internal variable substitution processing" );
HandleSubstitution( );

/* write output file */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
printf( "*** Writing Output file; %s\n", OutputFileName );
WriteOutputFile( OutputFileName );

puts( "*** Program Complete\n" );
return 0;
}
