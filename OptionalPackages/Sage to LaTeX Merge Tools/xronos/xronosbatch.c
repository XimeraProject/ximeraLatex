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

#include "dirent.h"

#include "danalib.h"
#include "xronostoollib.h"

#define XBATCH_FILENAME_MAX_LEN 500

static char ConfFileName[500] = "/usr/local/etc/xronostools/xronostools.conf";

static char *Copyright[] = { 
"\nCopyright 2017 by Dana Nowell, All rights Reserved",
"Educational Institutions are herein granted the right to use",
"and modify the code for their own use.  Rights for use in",
"a sale or other Commercial enterprise require written", 
"permission from the author.",
"\n",  /* blank line */
NULL };  /* end marker */


static int RequiredArgs = 1;  /* how many command line arguments are required as opposed to optional */

static char *Instructions[] = { 
"Needs arguments: shellscript file name is required\n",
"It can be followed by:",
"\t -Debug or /Debug to allow global debug tracing of the program for locating processing errors.",
"\t -XronosPath=\"path\" or /XronosPath=\"path\" to set the path for the xronosfmt program.",
"\t\t\t e.g., -XronosPath=\"/home/me/xronosfmt/bin/\"",
"\t -xronosfmt_outputpath=\"path\" or /xronosfmt_outputpath=\"path\" to set the -output_path flag for the xronosfmt program.",
"It is NOT expected that these switches will be used in production, more of a development tool.",
NULL };  /* end marker */


static char *ShellFileTop[] = { 
"#",
"# this file contains a group of xronosfmt runs that concatenate their output ",
"# into a single log file.  It was created by walking over a directory, finding ",
"# all the .tex files and outputting a line to run xronosfmt on that file.",
"#",
"\n",
"echo xronosbatch run on `date` > xronosbatch.log",
"\n",
NULL };  /* end marker */


BOOL IndividualLogFiles = FALSE;
BOOL CleanupPDFWorkFiles = TRUE;
BOOL CleanupSageWorkFiles = TRUE;
BOOL CleanupPDFAndSageFiles = FALSE;   /* the actual PDF file and the actual .sout file */
char CurrentDirectory[XBATCH_FILENAME_MAX_LEN + 1];
char xronos_path[XBATCH_FILENAME_MAX_LEN + 1];
char xronosfmt_outputpath[XBATCH_FILENAME_MAX_LEN + 1];


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
	char buf[300];
	char *p;
	
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
	if ( strcmp( buf, "xronosfmt_outputpath" ) == 0 )  /* output_path */
	{

		DL_TrimQuotesInPlace( Value );
		strcpy( xronosfmt_outputpath, Value );
		p = FindEndOfString(xronosfmt_outputpath); /* last character */
		if ( strlen( xronosfmt_outputpath ) > 0 && *p != '/' )
			strcat( xronosfmt_outputpath, "/" );  /* add ending / to path */	
	}
	else 		
	if ( strcmp( buf, "xronospath" ) == 0 )  /* xronospath */
	{

		DL_TrimQuotesInPlace( Value );
		strcpy( xronos_path, Value );
		p = FindEndOfString( xronos_path ); /* last character */
		if ( strlen( xronos_path ) > 0 && *p != '/' )
			strcat( xronos_path, "/" );  /* add ending / to path */	

	}
	else
		{
			sprintf( buf, "Unknown command line switch %s", MySwitch );
			ErrorExit( buf );
		}

}


/*************************************
*************************************/
void WriteDeleteFileLine( FILE *OutFile, char *BaseName, char *ExtensionString, char *LogfileName)
{
fprintf( OutFile, "echo \\# deleting file: %s.%s  >> %s \n", BaseName, ExtensionString, LogfileName );
fprintf( OutFile, "rm %s.%s  >> %s 2>&1\n", BaseName, ExtensionString, LogfileName );
}



/*************************************
*************************************/
int main( int argc, char *argv[])
{
	char *p;
	char *Switch, *Value;
	char ParseBuffer[XBATCH_FILENAME_MAX_LEN];
	char LogfileName[XBATCH_FILENAME_MAX_LEN];
	int i;
	char EntryType[100];
	char tempbuffer[1000];
	FILE *OutFile;
	DIR *dir;
	struct dirent *ent;
	
	/* initialize trace to off */
	TraceInit( );
	SetTraceOutput( stderr ); 
	TraceOn( FALSE );

/* set default log file name */
	strcpy( LogfileName, "xronosbatch.log" );

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

	/* init stuff from Conf file */
	puts( "Processing Conf file" );
	printf( "Conf File: %s\n", ConfFileName );
	if ( DL_ReadIniFile(ConfFileName) != 0 )
		ErrorExit( "Failed to read conf file" );

	IndividualLogFiles = DL_GetIniBOOLValue( "IndividualLogFiles", IndividualLogFiles );
	
	CleanupPDFWorkFiles = DL_GetIniBOOLValue( "CleanupPDFWorkFiles", CleanupPDFWorkFiles );
	
	CleanupSageWorkFiles = DL_GetIniBOOLValue( "CleanupSageWorkFiles", CleanupSageWorkFiles );

	CleanupPDFAndSageFiles = DL_GetIniBOOLValue( "CleanupPDFAndSageFiles", CleanupPDFAndSageFiles );
	
	DL_DiscardIniFile();
	puts( "Done Processing Conf file" );
	/* end ini stuff */
	
	OutFile = fopen( DL_CL_GetArg( 0 ), "w+" );
	if ( OutFile == NULL )
		ErrorExit( "Unable to open the output file for the shell script" );
	
	
	
	/* output the shell command to the output file */
	fputs( "#!/bin/sh\n\n", OutFile );	
	
	/* output a change directory to here to the output file */
	if ( getcwd(CurrentDirectory, sizeof(CurrentDirectory) - 1) == NULL )
	{
		CurrentDirectory[0] = 0;
		puts( "# Failed to get current working directory" );
	}
	else

		fprintf( OutFile, "cd %s\n", CurrentDirectory );

		/* output the top of the shell script file to setup the log etc.*/
	for( i = 0; ShellFileTop[i] != NULL; ++i )
	{
		fputs( ShellFileTop[i], OutFile );
		fputs( "\n", OutFile );
	}
	
	

	if ( (dir = opendir ("./")) == NULL ) 
		ErrorExit( "Can not open directory" );

	
	/* print all the files and directories within directory */
	while ((ent = readdir (dir)) != NULL) 
	{
		TracePrintf( "Processing Directory Entry - %s (d_type = %d)\n", ent->d_name, (int) ent->d_type );
		switch( (int) ent->d_type )
		{
			case 4:
				strcpy( EntryType, "Directory" );  /* setting entry type for TracePrintf at end of switch */
				break;
			
			case 8:
				strcpy( EntryType, "File" );  /* setting entry type for TracePrintf at end of switch */
				strcpy( tempbuffer, ent->d_name );
				p = TrimWhitespace( tempbuffer );
				strcpy( ParseBuffer, p );
				p = strstr( ParseBuffer, "HELP.tex" );
				if ( p == NULL )  /* do not process HELP files */
				{
					p = strstr( ParseBuffer, ".tex" );
					if ( p != NULL && strlen(p) == strlen( ".tex" ) ) /* have a .tex file */
					{
						*p = 0; /* terminate truncating the .tex */
						/* show the user something to show progress during the run */
						fprintf( OutFile, "echo Processing FILE: %s.tex \n", ParseBuffer ); 
							
						/* set the log file name */
						if ( IndividualLogFiles )
						{
							sprintf( LogfileName, "%s.XB.log", ParseBuffer );
							fprintf( OutFile, "echo \\#  > %s \n", LogfileName ); /* create the EMPTY log file */
						}
						else
							strcpy( LogfileName, "xronosbatch.log" );
						
						/* write the file name/header to the log */
						fprintf( OutFile, "echo \\#\\#\\# >> %s \n", LogfileName );
						fprintf( OutFile, "echo \\#\\#\\# FILE: %s.tex - started processing at `date` >> %s \n", ParseBuffer, LogfileName );
						fprintf( OutFile, "echo \\#\\#\\# >> %s \n", LogfileName );
						
						/* run pdflatex - output to the log */
						fprintf( OutFile, "echo \t\\# Starting pdflatex >> %s \n", LogfileName );
						fprintf( OutFile, "pdflatex -synctex=1 -interaction=nonstopmode %s.tex  >> %s 2>&1 \n", ParseBuffer, LogfileName );
						fprintf( OutFile, "if [ ! -e %s.pdf ]; then\n", ParseBuffer );
						fprintf( OutFile, "echo ERROR: Failed to create PDF: %s.pdf >> %s\n", ParseBuffer, LogfileName );
						fprintf( OutFile, "fi\n" );
						if ( CleanupPDFWorkFiles )
						{
						fprintf( OutFile, "echo \t\\# Clean up pdflatex work files >> %s \n", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "aux", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "ids", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "jax", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "log", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "oc", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "out", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "synctex.gz", LogfileName );
						}
						
						/* run sage - output to the log */
						fprintf( OutFile, "echo \t\\# Starting Sage >> %s \n", LogfileName );
						fprintf( OutFile, "sage %s.sagetex.sage  >> %s 2>&1 \n", ParseBuffer, LogfileName );		
						fprintf( OutFile, "if [ ! -e %s.sagetex.sout ]; then\n", ParseBuffer );
						fprintf( OutFile, "echo ERROR: Failed to create sout: %s.sagetex.sout >> %s\n", ParseBuffer, LogfileName );
						fprintf( OutFile, "fi\n" );
						if ( CleanupSageWorkFiles )
						{
						fprintf( OutFile, "echo \t\\# Clean up sagemath work files >> %s \n", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "sagetex.sage", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "sagetex.sage.py", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "sagetex.scmd", LogfileName );
						}
						
						/* run xronosmerge - output to the log */
						fprintf(OutFile, "xronosmerge %s.tex ",  ParseBuffer );
						fprintf(OutFile, " >> %s 2>&1 \n", LogfileName );
						
						/* run xronosfmt - output to the log */
						fprintf(OutFile, "xronosfmt %s.tex ",  ParseBuffer );
						fprintf(OutFile, " >> %s 2>&1 \n", LogfileName );
						
						fprintf(OutFile, "xronosfinal " );
						if ( strlen(xronosfmt_outputpath) > 0 )
							fprintf( OutFile, "%s", xronosfmt_outputpath );
						fprintf(OutFile, "%s.tex ",  ParseBuffer );
						fprintf(OutFile, " >> %s 2>&1 \n", LogfileName );
						
						if ( CleanupPDFAndSageFiles )
						{
						fprintf( OutFile, "echo \t\\# Clean up pdf and sage files - not just work files >> %s \n", LogfileName );					
						WriteDeleteFileLine( OutFile, ParseBuffer, "pdf", LogfileName );
						WriteDeleteFileLine( OutFile, ParseBuffer, "sagetex.sout", LogfileName );
						}

						fprintf( OutFile, "echo \t\\# Ended processing at `date` >> %s \n\n", LogfileName );

					}
				}
				break;
				
			default:
				sprintf( EntryType, "unknown(%d)", (int) ent->d_type );
				break;
		}
		TracePrintf( "Finished Directory Entry - %s, type %s\n", ent->d_name, EntryType );
	}
	
	fprintf( OutFile, "echo ====================\n" );
	fprintf( OutFile, "echo ====== Errors ======\n" );
	fprintf( OutFile, "echo ====================\n" );
	fprintf( OutFile, "grep \"ERROR: \" %s\n", LogfileName );
	fprintf( OutFile, "if [  $? = 0 ]; then \n" );
	fprintf( OutFile, "echo -n Hit return to continue: \n" );
	fprintf( OutFile, "read foo\n" );
	fprintf( OutFile, "fi\n" );

	fprintf( OutFile, "echo \\#\\#\\# >> xronosbatch.log \n" );
	fprintf( OutFile, "echo \\#\\#\\#  Finished entire xronosbatch.sh run on `date` >> xronosbatch.log \n" );
	fprintf( OutFile, "echo \\#\\#\\# >> xronosbatch.log \n" );
						
	closedir (dir);
	fclose( OutFile );

	return 0;
}
