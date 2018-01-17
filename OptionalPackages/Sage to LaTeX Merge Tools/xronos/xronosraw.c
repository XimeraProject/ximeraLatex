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

#include "danalib.h"

#include "xronostoollib.h"
#include "xronosio.h"



#define MIN_REPLICATION  50
#define MAX_REPLICATION 5000

static char ConfFileName[500] = "/usr/local/etc/xronostools/xronostools.conf";
static char FileForOutputHeader[500];
static char FileForOutputFooter[500];

char HelpFilePlaceHolder[200] = "XTL_SV_HELPFILE";											
				
typedef struct _QuestionCtxtLine {
	struct _QuestionCtxtLine *next;
	char *line;
} QuestionCtxtLine;

typedef struct _QuestionCtxt {
	struct _QuestionCtxt *next;
	QuestionCtxtLine *head;
	QuestionCtxtLine *tail;
	BOOL IsValid;
	char HelpFileName[XTL_MAX_FILENAME_SIZE + 1];
	char FileName[XTL_MAX_FILENAME_SIZE + 1];
} QuestionCtxt;

char LineBuffer[XIO_MAX_LINE + 1];
char CurrentDirectory[XTL_MAX_FILENAME_SIZE + 1];


static QuestionCtxt *QuestionHead;

static int ReplicationCount = 700;

static char *Copyright[] = { 
"Copyright 2017 by Dana Nowell, All rights Reserved.",
"Educational Institutions are herein granted the right to use",
"and modify the code for their own use.  Rights for use in",
"a sale or other Commercial enterprise require written", 
"permission from the author.",
"\n",  /* blank line */
NULL };  /* end marker */


static int RequiredArgs = 1;  /* how many command line arguments are required as opposed to optional */

static char *Instructions[] = { 
"Needs 1 argument: inputfile file name that contains RAW questions to build generator files from is required\n",
"It can be followed by:",
"\t -Debug or /Debug to allow global debug tracing of the program for locating processing errors.",
"It is NOT expected that these switches will be used in production, more of a development tool.",
NULL };  /* end marker */


static char SearchString[200] = "%\\tagged{";

static char OutputDirectory[XTL_MAX_FILENAME_SIZE + 1] = "./XronosOutput/";


/*************************************
*************************************/
static void ProcessEnvVars( void )
{

	TracePrintf( "ProcessEnvVars: XRONOSTOOLS_CONF %s BEFORE\n", ConfFileName );

	XTL_GetEnvVariable( "XRONOSTOOLS_CONF", ConfFileName, sizeof(ConfFileName) );	

	TracePrintf( "ProcessEnvVars: XRONOSTOOLS_CONF %s AFTER\n", ConfFileName );
	
}

/*************************************
* 
*************************************/
static QuestionCtxtLine *GetQuestionCtxtLine( void )
{
	QuestionCtxtLine *retval;
	
	retval = GetMemoryBlock( sizeof(QuestionCtxtLine) );
	retval->next = NULL;
	retval->line = NULL;
	
	return retval;
}




/*************************************
* 
*************************************/
static void AddLineToQuestionHead( char *line )
{
	QuestionCtxtLine *Entry;
	
	if ( QuestionHead == NULL )
			return;
		
	Entry = GetQuestionCtxtLine();
	Entry->line = GetMemoryBlock( strlen(line) + 1 );
	strcpy( Entry->line, line );
	
	if ( QuestionHead->head == NULL )
		QuestionHead->head = Entry;
	else
		QuestionHead->tail->next = Entry;	
		
	QuestionHead->tail = Entry;
	
	TracePrintf( "AddLineToQuestionHead: Added line  %s to QuestionCtxt %p\n", line, QuestionHead );
	return;
}



/*************************************
* 
*************************************/
static QuestionCtxt *GetQuestionCtxt( void )
{
	QuestionCtxt *retval;
	
	retval = GetMemoryBlock( sizeof(QuestionCtxt) );
	retval->next = NULL;
	retval->head = NULL;
	retval->tail = NULL;
	retval->IsValid = TRUE;
	retval->HelpFileName[0] = 0;
	retval->FileName[0] = 0;

	return retval;
}



/*************************************
* 
*************************************/
static void AddQuestionCtxt( void )
{
	QuestionCtxt *Entry;
	
	Entry = GetQuestionCtxt();
	Entry->next = QuestionHead;
	QuestionHead = Entry;
}



/*************************************
*************************************/
static BOOL IsReplacableInput( char *line )
{
	char *p;
	int BraceCount;
	char ParseBuffer[XIO_MAX_LINE + 1];
	BOOL retval = FALSE;

	if ( line == NULL || *line == 0 )
		return FALSE;
	
	p  = strstr( line, "\\input{" );
	if ( p != 0 ) /* input line we MAY need to patch it up */
	{
		p += strlen( "\\input{" );
		strcpy( ParseBuffer, p );  /* copy from the { on to end of line */
		BraceCount = 1;
		p = XTL_FindClosingCurlyBrace( ParseBuffer, &BraceCount );
		if ( p != NULL )  /* if we find it, terminate the parsing there */
			*p = 0;
		p = strstr( ParseBuffer, HelpFilePlaceHolder );
		if ( p != NULL )
		{
			retval = TRUE;
		}
	}

	return retval;	
}


/*************************************
*************************************/
void ProcessCommandLineSwitch( char *MySwitch, char *Value )
{
	char buf[300];
	char *p;
	int RCount;
	
	if ( MySwitch == NULL || *MySwitch == 0 ) /* do we actual have a switch at all */
		return;
	
	if ( strlen( MySwitch ) >= sizeof( buf ) )  /* too long for a real switch do not overrun buffer */
		return;
	
	strcpy( buf, MySwitch );
	
	
		/* Make it lower case */
	for ( p = buf; *p != 0; ++p )
		*p = tolower((unsigned char) *p);

	if ( strcmp( buf, "debug" ) == 0 )  /* debug */
		TraceOn( TRUE );
	else
	{
		sprintf( buf, "Unknown command line switch %s", MySwitch );
		ErrorExit( buf );
	}

}


/*************************************
*  Handle each line, basically output it until we reach max questions
*************************************/
static int HandleLine( char *line )
{
char *p;
char ParseBuffer[XIO_MAX_LINE + 1];
char *TopicTag;
char *TypeTag;
char *FileTag;
int BraceCount;


	TracePrintf( "HandleLine: Line: %s\n", line );
	
	p = strstr( line, SearchString );
	if ( p != 0 )
	{
		AddQuestionCtxt();
		AddLineToQuestionHead( line );
		strcpy( ParseBuffer, line );
		
		/* see if we have the close brace for the tagged line, if so terminate the line there for parsing */
		p = strstr( ParseBuffer, SearchString );
		p += strlen(SearchString);
		BraceCount = 1;
		
		p = XTL_FindClosingCurlyBrace( p, &BraceCount );
		if ( p != NULL )
			*p = 0;
		
		TopicTag = strstr( ParseBuffer, "Topic@" );
		TypeTag = strstr( ParseBuffer, "Type@" );
		FileTag = strstr( ParseBuffer, "File@" );

		if ( TopicTag == NULL )
		{
			QuestionHead->IsValid = FALSE;
			puts( "Missing Topic Tag" );
		}
		else
		{
			TopicTag += strlen( "Topic@" );
			p = strstr( TopicTag, "," );
			if ( p != NULL )
				*p = 0;
			TopicTag = TrimWhitespace( TopicTag );
		}
		
		if ( TypeTag == NULL )
		{
			QuestionHead->IsValid = FALSE;
			puts( "Missing Type Tag" );
		}
		else
		{		
			TypeTag += strlen( "Type@" );
			p = strstr( TypeTag, "," );
			if ( p != NULL )
				*p = 0;
			TypeTag = TrimWhitespace( TypeTag );
		}
		
		if ( FileTag == NULL )
		{
			QuestionHead->IsValid = FALSE;
			puts( "Missing File Tag" );
		}
		else
		{
			FileTag += strlen( "File@" );
			p = strstr( FileTag, "," );
			if ( p != NULL )
				*p = 0;
			FileTag = TrimWhitespace( FileTag );
		}
				
		QuestionHead->FileName[0] = 0;
		
		if ( QuestionHead->IsValid )
		{
			strcat( QuestionHead->FileName, TopicTag );
			strcat( QuestionHead->FileName, "-");
			strcat( QuestionHead->FileName, TypeTag );
			strcat( QuestionHead->FileName, "-");
			strcat( QuestionHead->FileName, FileTag );
			strcpy( QuestionHead->HelpFileName, QuestionHead->FileName ); /* same base filenames */
			
			strcat( QuestionHead->HelpFileName, ".HELP.tex" );  /* add .HELP.tex file extension to the help file */
			strcat( QuestionHead->FileName, ".tex" );  /* add .tex file extension to the content file */
		}
		
	}
	else
	{
		if ( QuestionHead != NULL )
			AddLineToQuestionHead( line );			
	}
	
	
	return 0;
}



/*************************************
*************************************/
int main( int argc, char *argv[] )
{
	char *p;
	char *Switch, *Value;
	char ParseBuffer[XTL_MAX_FILENAME_SIZE];
	int i;
	int LineCount = 0;
	int Qcount;
	QuestionCtxt *QuestionPtr;
	FILE *Output;
	QuestionCtxtLine *QuestionLine;
	BOOL WroteTag = FALSE;
	XTL_FileStatus XTLFileStat;
	BOOL status;

	
	/* initialize trace to off */
	TraceInit( );
	SetTraceOutput( stdout ); 
	TraceOn( FALSE );
	DL_CL_ProcessArgs( argc, argv );


	/* display copyright */
	p = DL_CL_GetProgName( );	
	fprintf( stderr, "%s\n", p );
	for( i = 0; Copyright[i] != NULL; ++i )
	{
		fputs( Copyright[i], stderr );
		fputs( "\n", stderr );
	}

	/* process command line switches to modify behavior */
	for ( i = 0; DL_CL_GetSwitch( i, &Switch, &Value ); i++ )
	{
		ProcessCommandLineSwitch( Switch, Value );
	}


	/* display instructions */	
	if ( DL_CL_GetArgCount() < RequiredArgs )
	{
		for( i = 0; Instructions[i] != NULL; ++i )
		{
			fputs( Instructions[i], stderr );
			fputs( "\n", stderr );
		}
	
		ErrorExit( "\n" );
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
		ErrorExit( "Failed to read ini file" );
	
	p = DL_GetIniStringValue( "XronosrawOutputDir", NULL );
	if ( p != NULL )
	{
		strncpy( OutputDirectory, p, sizeof(OutputDirectory) - 1 );
		OutputDirectory[sizeof(OutputDirectory) - 1] = 0;
	}
	
	p = DL_GetIniStringValue( "HelpFilePlaceHolder", NULL );
	if ( p != NULL )
	{
		strncpy( HelpFilePlaceHolder, p, sizeof(HelpFilePlaceHolder) - 1 );
		HelpFilePlaceHolder[sizeof(HelpFilePlaceHolder) - 1] = 0;
	}
	
	p = DL_GetIniValue( "RepeatCount", "700" );
	i = atoi( p );
	if ( i >= MIN_REPLICATION && i <= MAX_REPLICATION )
		ReplicationCount = i;
	
	printf( "ReplicationCount: %d\n", ReplicationCount );
	
	p = DL_GetIniStringValue( "XR_InputFileHeader", NULL );
	if ( p != NULL )
	{
	if ( XronosIO_SetHeaderFile( p ) != 0 )
		ErrorExit( "failed to set XR_FileHeader value" );
	printf( "Set InputHeaderFile: %s\n", p );
	}
	
	p = DL_GetIniStringValue( "XR_InputFileFooter", NULL );
	if ( p != NULL )
	{
	if ( XronosIO_SetFooterFile( p ) != 0 )
		ErrorExit( "failed to set XR_FileFooter value" );
	printf( "Set InputFooterFile: %s\n", p );
	}
	
	p = DL_GetIniStringValue( "XR_OutputFileHeader", NULL );
	if ( p != NULL )
	{
	strcpy( FileForOutputHeader, p );
	printf( "Set OutputHeaderFile: %s\n", p );
	}
	
	p = DL_GetIniStringValue( "XR_OutputFileFooter", NULL );
	if ( p != NULL )
	{
	strcpy( FileForOutputFooter, p );
	printf( "Set OutputFooterFile: %s\n", p );
	}
	

	DL_DiscardIniFile();
	puts( "Done Processing Conf file" );
	/* end ini stuff */

	/* terminate the output path correctly */	
	if ( strlen(OutputDirectory) > 0 )
	{
		p = FindEndOfString(OutputDirectory); /* last character */
		if ( *p != '/' )
			strcat( OutputDirectory, "/" );  /* add ending / to path */
		printf( "XronosOutputDir: %s\n", OutputDirectory );
	}


	printf( "%s - Reading Inputfile %s\n", argv[0], DL_CL_GetArg( 0 ) );
	ReadInputFile( DL_CL_GetArg( 0 ) );


	printf( "\tProcessing Inputfile %s\n", DL_CL_GetArg( 0 ) );
	while ( ReadLine( ) == 0 )
	{
		++LineCount;
		TracePrintf( "Processing Line %d\n", LineCount );

		if ( HandleLine( LineBuffer ) != 0 )
			ErrorExit( "Handle Line blew up" );

	}

	/*
	TraceOn( TRUE );
	*/
	
	puts( "\tProcessed, Getting ready to write the output" );

	MakeDirChainForFile( OutputDirectory );
	
	/* check to see if the any question is all blank lines and comments
	if so, invalidate it */
	for ( QuestionPtr = QuestionHead; QuestionPtr != NULL; QuestionPtr = QuestionPtr->next )
	{
		if ( QuestionPtr->IsValid ) 
		{
			status = FALSE;  /* assume bad */
			for( QuestionLine = QuestionPtr->head; QuestionLine != NULL; QuestionLine = QuestionLine->next )
			{
				if ( QuestionLine->line != NULL  && strlen(QuestionLine->line) > 0 && *(QuestionLine->line) != '%' )
				{
					strcpy( LineBuffer, QuestionLine->line );
					p = XTL_FindComment( LineBuffer );
					if ( p != NULL )
						*p = 0;
					p = TrimWhitespace( LineBuffer );
					if ( strlen( p ) > 0 )
					{
						status = TRUE;	/* has non comment non blank line, keep it */
						break;
					}
				}
			}
			if ( ! status )
				QuestionPtr->IsValid = FALSE;
		}
	}
	
	
	for ( QuestionPtr = QuestionHead; QuestionPtr != NULL; QuestionPtr = QuestionPtr->next )
	{
		TracePrintf( "QuestionPtr %p, in write loop, IsValid %d, FileName %s\n", (void *) QuestionPtr, (int) QuestionPtr->IsValid, QuestionPtr->FileName );
		if ( QuestionPtr->IsValid ) 
		{
			strcpy( ParseBuffer, OutputDirectory );
			strcat( ParseBuffer, QuestionPtr->HelpFileName );		
			Output = fopen( ParseBuffer, "w+" );  /* create the HELP file */
			if ( Output == NULL )  
				printf( "Failed to open HELP file %s, continuing on anyway\n", QuestionPtr->HelpFileName );
			else
				fclose( Output );
			
			strcpy( ParseBuffer, OutputDirectory );
			strcat( ParseBuffer, QuestionPtr->FileName );
			
			/* create the empty help file is necessary */
			XTLFileStat = XTL_CreateHelpFileName( ParseBuffer );
			if ( XTLFileStat != XTL_FileSuccess )
				printf( "Failed to create HELP file name for TEX file %s, continuing on anyway\n", QuestionPtr->FileName );
			else
			{
				XTLFileStat = XTL_CreateEmptyHelpFile( );
				if ( XTLFileStat == XTL_FileExists )
					printf( "HELP file for TEX file %s already exists, leaving it alone\n", QuestionPtr->FileName );
				else				
				if ( XTLFileStat != XTL_FileSuccess )
					printf( "Failed to create HELP file for TEX file %s, continuing on anyway\n", QuestionPtr->FileName );
			}
			/* OK, now process the file */			
			TracePrintf( "Opening output file %s\n", ParseBuffer );
			if ( getcwd(CurrentDirectory, sizeof(CurrentDirectory) - 1) != NULL )
				TracePrintf( "CurrentDirectory %s\n", CurrentDirectory );
			Output = fopen( ParseBuffer, "w+" );
			if ( Output == NULL )  
			{
				printf( "Failed to open output file %s\n", QuestionPtr->FileName );
			}
			else
			{
				if ( strlen( FileForOutputHeader ) > 0 )  /* if we have a fileheader */
				{
					if ( XTL_CopyFileContents( FileForOutputHeader, Output) != 0 )
						ErrorExit( "Unable to copy FileForOutputHeader" );
				}

				printf( "\tWriting output file %s\n", QuestionPtr->FileName );
				
				/* we want to have a single tagged line so track it and do not replicate it */
				WroteTag = FALSE;
				
				for ( Qcount = 0; Qcount < ReplicationCount; ++Qcount )
				{
					for( QuestionLine = QuestionPtr->head; QuestionLine != NULL; QuestionLine = QuestionLine->next )
					{
						/*
						TracePrintf( "QuestionPtr %p, QuestionLine %p\n", (void *) QuestionPtr, (void *) QuestionLine );
						*/
						if ( QuestionLine->line != NULL  && strlen(QuestionLine->line) > 0 )
						{
							/* is it a tagged line ? */
							DoTrace( "found a tagged line");
							if ( strstr( QuestionLine->line, SearchString ) != 0 )
							{
							/* YES! if we have already written it once, do not do it again */
								if ( !WroteTag )
								{
								/* Not Yet! we will write it and mark it done */
								DoTrace( "We will use this tagged line");


								fprintf( Output, "%s", QuestionLine->line );
								WroteTag = TRUE;
								}
							}
							else						
							{	
								fprintf( Output, "%s", QuestionLine->line );
							}
						}
					}
				
				}

			if ( strlen( FileForOutputFooter ) > 0 )  /* if we have a fileheader */
			{
				fputs( "\n", Output ); /* write the end of line */
				if ( XTL_CopyFileContents( FileForOutputFooter, Output) != 0 )
					ErrorExit( "Unable to copy FileForOutputFooter" );
			}
	
					
			fclose( Output );
			}
		}
		
	}
	
	puts( "Program Complete" );
 
	return 0;
}


