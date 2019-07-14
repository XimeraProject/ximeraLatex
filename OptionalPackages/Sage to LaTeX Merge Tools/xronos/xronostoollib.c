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

static char FileNameBuffer[XTL_MAX_FILENAME_SIZE + 1];

/*************************************
XTL_FileStatus XTL_CreateHelpFileName( char *TexFileName )

given a tex file name (e.g., foo.tex), 
determine the help file name

return status is an XTL_FileStatus
*************************************/
XTL_FileStatus XTL_CreateHelpFileName( char *TexFileName )
{
	char *p;

	XTL_SubVars_UpdateChar( "XTL_SV_HELPFILE", "" ); /* reset to empty string just in case */

	if ( TexFileName == NULL || *TexFileName == 0 )
		return XTL_FileBadName;

	/* does the file name passed to us end in .tex ? */		
	p = TexFileName + strlen(TexFileName) - strlen(".tex") - 1;
	if ( strcmp( p, ".tex" ) == 0 )
		return XTL_FileBadName;

	/* OK at this point we have a filename and it is a .tex file */
	strcpy( FileNameBuffer, TexFileName );
	
	for ( p = FindEndOfString( FileNameBuffer ); *p != '.'; --p ); /* find the last . */
	
	*p = 0;  /* truncate filename to remove the .tex */
	strcat( FileNameBuffer, ".HELP.tex" );
	
	XTL_SubVars_UpdateChar( "XTL_SV_HELPFILE", FileNameBuffer );
	XTL_SubVars_UpdateChar( "XTL_SV_OUTPUTFILE", TexFileName );

	return XTL_FileSuccess;
}


/*************************************
XTL_FileStatus XTL_CreateEmptyHelpFile( char *TexFileName )

given a tex file name (e.g., foo.tex), 
determine if a help file exists (e.g., foo.HELP.tex)
if not then create an empty help file

return status is an XTL_FileStatus
*************************************/
XTL_FileStatus XTL_CreateEmptyHelpFile( void )
{
	char *p;
	FILE *fh;

	p = XTL_SubVars_GetValueChar( "XTL_SV_HELPFILE" );
	if ( p == NULL )
		return XTL_FileBadName;

	strcpy( FileNameBuffer, p );	

	/* does it already exist? */
	if ( DoesFileExist( FileNameBuffer ) )
		return XTL_FileExists;
	
	/* create it */
	MakeDirChainForFile( FileNameBuffer );
	fh = fopen( FileNameBuffer, "w+" );
	
	/* failed, do not care why, probably security */
	if ( fh == NULL )
		return XTL_FileUnknownError;
	
	fclose( fh );
	
	return XTL_FileSuccess;
}


/*************************************
int XTL_CopyFileContents( char *FileName, FILE *outfile );

opens FileName and copies its contents to the outfile

returns 0 on SUCCESS
*************************************/
int XTL_CopyFileContents( char *FileName, FILE *outfile )
{
	FILE *fh;
	char buffer[1000];
	fh = fopen(FileName, "r" );
	if (fh == NULL )
		return -1;
	
	while( fgets( buffer, sizeof(buffer) - 1, fh ) != NULL )
	{
		if ( fputs(buffer, outfile) <= 0 )
			return -2;
	}
	
	fclose( fh );
	return 0;
}



/*************************************
char *XTL_FindComment( char *line );

parses the line looking for %
it ignores \%

returns a pointer to the %
*************************************/
char *XTL_FindComment( char *line )
{
char *p;

if ( line == NULL || *line == 0 )
	return NULL;

if ( *line == '%') /* this avoids walking off the front of the string during a check later/below */
	return line;
		
p = strstr( line, "%" ); /* position of first comment character in the string */
if ( p == NULL ) /* line has no embedded comments */
	return NULL;

if ( *(p-1) == '\\' ) /* Not a real commment, it is escaped */
	p = XTL_FindComment( p+1);

return p;	
}


/*************************************
char *XTL_FindClosingCurlyBrace( char *line, int *Braces );

must start with a bracecount > 0
walks the line tracking the ongoing bracecount until we get to zero

returns ptr to the closing brace or NULL
*************************************/
char *XTL_FindClosingCurlyBrace( char *line, int *Braces )
{
	char *p;
	char *comment;

	if ( line == NULL || *line == 0 || Braces == NULL || *Braces < 1 )
		return NULL;

	DoTrace( "In - XTL_FindClosingCurlyBrace" );

	comment = XTL_FindComment( line ); /* locate the start of any comment, we do not want to substitue inside a comment */

	for ( p = line; *p != 0; p++)
		{
		switch ( *p )
			{
			case '\\':
				++p;  /* skip next character */
				if (*p == 0 )
					--p; /* do not skip end of line */
				break;
				
				
			case '{':
				++(*Braces);
				break;
				
			case '}':
				--(*Braces);
				if ( *Braces == 0 )
				{
				DoTrace("out1 - XTL_FindClosingCurlyBrace");
				return p;
				}
				break;
				
			case '%':
				if ( comment != NULL && p == comment )
				{
					DoTrace("out2 - XTL_FindClosingCurlyBrace");
					return NULL;
				}
				break;
			}
		}	

	DoTrace("out0 - XTL_FindClosingCurlyBrace");
	return NULL;	
}




/*************************************
char *XTL_FindClosingCurlyBrace( char *line, int *Braces );

must start with a bracecount > 0
walks the line tracking the ongoing bracecount until we get to zero

returns ptr to the closing brace or NULL
*************************************/
char *XTL_FindClosingSquareBrace( char *line, int *Braces )
{
	char *p;
	char *comment;
	
	TracePrintf( "start - XTL_FindClosingSquareBrace, %p, %p\n", (void *) line, (void *) Braces);	
	
	if ( line == NULL || *line == 0 || Braces == NULL || *Braces < 1 )
		return NULL;
	
	DoTrace("in - XTL_FindClosingSquareBrace");	
	
	comment = XTL_FindComment( line ); /* locate the start of any comment, we do not want to substitue inside a comment */

	for ( p = line; *p != 0; p++)
		{
		switch ( *p )
			{
			case '\\':
				++p;  /* skip next character */
				if (*p == 0 )
					--p; /* do not skip end of line */
				break;
				
			case '[':
				++(*Braces);
				break;
				
			case ']':
				--(*Braces);
				if ( *Braces == 0 )
				{
				DoTrace("out1 - XTL_FindClosingSquareBrace");
					return p;
				}
				break;
				
			case '%':
				if ( comment != NULL && p == comment )
				{
					DoTrace("out2 - XTL_FindClosingSquareBrace");
					return NULL;
				}
				break;
			}
		}

	DoTrace("out - XTL_FindClosingSquareBrace");

	return NULL;	
}



/*************************************
void XTL_TrimQuotesInPlace( char *line );

if string has both a leading and matching trailing 
quote (double or single) this function strips them 
off in place (changes the argument string)

returns nothing
*************************************/
void XTL_TrimQuotesInPlace( char *line )
{
DL_TrimQuotesInPlace( line );
}


/*************************************
int XTL_GetEnvVariable( char *name, char *var, int size )

gets an environment variable and uses it to set a buffer

returns 0 on success, -1 if not found, 
-2 if buffer too short, -3 if parameter error
*************************************/
int XTL_GetEnvVariable( char *name, char *var, int size )
{
	char *p;
	char *retval = NULL;

	if ( name == NULL || var == NULL || size < 1 )
		return -3;
	
	p = getenv( name );
	if ( p == NULL )
		return -1;
	
	if ( strlen(p) < size )
		strcpy( var, p );
	else
		return -2;
	
	DL_TrimQuotesInPlace(var);
	
	return 0;
}







