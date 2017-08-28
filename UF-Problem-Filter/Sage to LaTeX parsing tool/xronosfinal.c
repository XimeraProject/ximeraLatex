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
char CurrentDirectory[XTL_MAX_FILENAME_SIZE + 1];
char xronosfmt_outputpath[XTL_MAX_FILENAME_SIZE + 1];

static int QuestionCount = 0;

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
	
	/* Create an empty HELP.tex if we need to */
	XTLFileStat = XTL_CreateEmptyHelpFile( );
	if ( XTLFileStat == XTL_FileExists )
		printf( "HELP file for TEX file %s already exists, leaving it alone\n", OutputFileName);
	else				
	if ( XTLFileStat != XTL_FileSuccess )
		printf( "Failed to create HELP file for TEX file %s, continuing on anyway\n", OutputFileName );
				
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
	
	/* we built a custom header before (see BuildHeader.c) now write it out */
	if ( WriteOutputHeader( Out ) != 0 )
		ErrorExit( "Error writing output file header" );

	puts( "Writing actual file data" );

	/* DANA - FIXUP */
	while ( ReadLine( ) == 0 )
	{	
	num = fwrite(LineBuffer, 1, strlen(LineBuffer), Out );
	}
	
	if ( !QuestionsOnly	 )  /* if the user requested we include the document structure stuff intact */
		fputs( "\\end{document}", Out ); /* write the end document line */
#if 0
		else
		fputs( "\\fi \t\t%% end of \\ifquestionCount near top of file", Out ); /* write the end of the if for the question count conditional at the top of the document */
#endif
				
	if ( strlen( FileForOutputFooter ) > 0 )  /* if we have a fileheader */
	{
		fputs( "\n", Out ); /* write the end of line */
		if ( XTL_CopyFileContents( FileForOutputFooter, Out) != 0 )
			ErrorExit( "Unable to copy FileForOutputFooter" );
	}
	
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
*  Handle each line, basically output it until we reach max questions
*************************************/
static int HandleQCLine( char *line )
{
char *p;	
static char SearchString[200] = ProblemStartString;

	p = strstr( line, SearchString );
	if ( p != 0 )
	{
		++QuestionCount;
	}
	
	XTL_SubVars_UpdateInt( "XTL_SV_QuestionCount", QuestionCount );

	WriteToOutputBuffer( line, strlen(line) );		
	return 0;
}




/*************************************
*************************************/
static int CalcQuestionCount( void )
{
BOOL done = FALSE;
int status;
int LineNo = 0;

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
		
		status = HandleQCLine( LineBuffer );
		if ( status != 0 )  /* if not zero then either we have an error OR we reached max questions, so we are done */
			done = TRUE;
			
		if ( status > 0 )  /* positive values imply error */
			ErrorExit( "HandleQCLine blew up" );
		}
	}

return 0;
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

	p = DL_GetIniValue( "XF_QuestionsOnly", NULL );
	if ( p != NULL )
	{
		DL_STRUPR( (unsigned char *) p );
		if ( strcmp( p, "N" ) == 0 || strcmp( p, "NO" ) == 0 )
			QuestionsOnly = FALSE;
	}
	
	p = DL_GetIniValue( "XF_TagsRequired", NULL );
	if ( p != NULL )
	{
		DL_STRUPR( (unsigned char *) p );
		if ( strcmp( p, "N" ) == 0 || strcmp( p, "NO" ) == 0 )
			TagsRequired = FALSE;
	}
	
	p = DL_GetIniValue( "XFinal_OutputDir", NULL );
	if ( p != NULL )
	{
	XTL_TrimQuotesInPlace( p );
	strcpy( xronosfmt_outputpath, p );
	}
	
	p = DL_GetIniValue( "XFinal_OutputFileHeader", NULL );
	if ( p != NULL )
	{
	XTL_TrimQuotesInPlace( p );
	strcpy( FileForOutputHeader, p );
	}
	
	p = DL_GetIniValue( "XFinal_OutputFileFooter", NULL );
	if ( p != NULL )
	{
	XTL_TrimQuotesInPlace( p );
	strcpy( FileForOutputFooter, p );
	}
	
	p = DL_GetIniValue( "XFinal_InputFileHeader", NULL );
	if ( p != NULL )
	{
	XTL_TrimQuotesInPlace( p );
	if ( XronosIO_SetHeaderFile( p ) != 0 )
		ErrorExit( "failed to set XF_InputFileHeader value" );
	printf( "Set Input HeaderFile: %s\n", p );
	}
	
	p = DL_GetIniValue( "XF_InputFileFooter", NULL );
	if ( p != NULL )
	{
	XTL_TrimQuotesInPlace( p );
	if ( XronosIO_SetFooterFile( p ) != 0 )
		ErrorExit( "failed to set XF_InputFileFooter value" );
	printf( "Set Input FooterFile: %s\n", p );
	}
	
	DL_DiscardIniFile();
	puts( "Done Processing Conf file" );
	/* end ini stuff */

/* terminate the output path correctly */	
if ( strlen(xronosfmt_outputpath) > 0 )
{
	p = FindEndOfString(xronosfmt_outputpath); /* last character */
	if ( *p != '/' )
		strcat( xronosfmt_outputpath, "/" );  /* add ending / to path */
	printf( "xronosfmt output path: %s\n", xronosfmt_outputpath );
}

/* Load file into memory */
printf( "*** Reading input file: %s\n", InputFileName );
ReadInputFile( InputFileName );

#if 0
/* handle merge - MUST be first before all other passes over the "file" */
puts( "*** Merge Processing" );
merge( InputFileName );

/* Build Output file header (in memory)*/
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
#endif 
puts( "*** Build Output Header for later" );
BuildOutputHeader( TagsRequired );

/* build the help file name now that BuildOutputHeader has set the outputfilename */
XTLFileStat = XTL_CreateHelpFileName( OutputFileName );
if ( XTLFileStat != XTL_FileSuccess )
	printf( "Failed to create HELP file name for TEX file %s, continuing on anyway\n", OutputFileName );
				
#if 0 
/* kill dups */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
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
#endif 

/* deal with question counts */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
puts( "*** Question Limit processing" );
CalcQuestionCount( );


/* deal with substitution processing - should be JUST before WriteOutputFile() */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
puts( "*** Internal variable substitution processing" );
HandleSubstitution( );

/* write output file */
RecycleBuffers( );  /* recycle the previous pass output to the new pass input */
puts( "*** Writing Output file" );
WriteOutputFile( OutputFileName );

printf( "xronosfinal Question Count for file %s is %d\n", OutputFileName, QuestionCount );
puts( "*** Program Complete\n" );
return 0;
}
