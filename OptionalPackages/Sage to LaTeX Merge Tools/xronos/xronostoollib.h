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

#ifndef XRONOSTOOLLIBH_INCLUDED

#define XRONOSTOOLLIBH_INCLUDED 1  /* define it so we do not double include */

#include "danalib.h"

#define XTL_MAX_FILENAME_SIZE 4500  /* max path is 4K max filename is 256 */


typedef enum { XTL_FileSuccess = 0, XTL_FileExists, XTL_FileDoesNotExist, XTL_FileBadName, XTL_FileUnknownError } XTL_FileStatus;

/*************************************
XTL_FileStatus XTL_CreateHelpFileName( char *TexFileName )

given a tex file name (e.g., foo.tex), 
determine the help file name

return status is an XTL_FileStatus
*************************************/
XTL_FileStatus XTL_CreateHelpFileName( char *TexFileName );

/*************************************
XTL_FileStatus XTL_CreateEmptyHelpFile( char *TexFileName )

given a tex file name (e.g., foo.tex), 
determine if a help file exists (e.g., foo.HELP.tex)
if not then create an empty help file

return status is an XTL_FileStatus
*************************************/
XTL_FileStatus XTL_CreateEmptyHelpFile( void );

/*************************************
int XTL_CopyFileContents( char *FileName, FILE *outfile );

opens FileName and copies its contents to the outfile

returns 0 on SUCCESS
*************************************/
int XTL_CopyFileContents( char *FileName, FILE *outfile );

/*************************************
int XTL_GetEnvVariable( char *name, char *var, int size )

gets an environment variable and uses it to set a buffer

returns 0 on success, -1 if not found, 
-2 if buffer too short, -3 if parameter error
*************************************/
int XTL_GetEnvVariable( char *name, char *var, int size );

/*************************************
char *XTL_FindComment( char *line );

parses the line looking for %
it ignores \%

returns a pointer to the %
*************************************/
char *XTL_FindComment( char *line );


/*************************************
char *XTL_FindClosingSquareBrace( char *line, int *Braces );

must start with a bracecount > 0
walks the line tracking the ongoing bracecount until we get to zero

returns ptr to the closing brace or NULL
*************************************/
char *XTL_FindClosingSquareBrace( char *line, int *Braces );


/*************************************
char *XTL_FindClosingCurlyBrace( char *line, int *Braces );

must start with a bracecount > 0
walks the line tracking the ongoing bracecount until we get to zero

returns ptr to the closing brace or NULL
*************************************/
char *XTL_FindClosingCurlyBrace( char *line, int *Braces );


/*************************************
void XTL_TrimQuotesInPlace( char *line );

if string has both a leading and matching trailing 
quote (double or single) this function strips them 
off in place (changes the argument string)

returns nothing
*************************************/
void XTL_TrimQuotesInPlace( char *line );



/*
XTL_SubVars functions - used to manage variables for substitutions 
*/
#define XTL_SUBVARS_INT_VALUE_NOT_FOUND -9999


/*************************************
void XTL_SubVars_Init( void )

Must be called to initialize the variables list

returns nothing
*************************************/
void XTL_SubVars_Init( void );

/*************************************
int XTL_SubVars_Add( char *key, char *Value, BOOL CaseSensitive )

updates the key with the value

returns 0 on success, -1 is key already exists, other non zero is internal error
*************************************/
int XTL_SubVars_Add( char *key, char *Value, BOOL CaseSensitive );

/*************************************
int XTL_SubVars_UpdateChar( char *key, char *Value )

updates the key with the value

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_UpdateChar( char *key, char *Value );

/*************************************
int XTL_SubVars_UpdateInt( char *key, char *Value )

updates the key with the value

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_UpdateInt( char *key, int Value );

/*************************************
int XTL_SubVars_GetValueChar( char *key, char *Value )

returns the value associated with the key 

returns NULL if key not found
*************************************/
char *XTL_SubVars_GetValueChar( char *key );

/*************************************
int XTL_SubVars_GetValueInt( char *key, char *Value )

returns the value associated with the key 

returns XTL_SUBVARS_INT_VALUE_NOT_FOUND if key not found
*************************************/
int XTL_SubVars_GetValueInt( char *key );

/*************************************
int XTL_SubVars_IncrementValue( char *key )

updates the key with the value

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_IncrementValue( char *key );

/*************************************
int XTL_SubVars_DecrementValue( char *key )

decrements the value associated with the key

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_DecrementValue( char *key );

/*************************************
char *XTL_Substitute( char *line )

Makes all substitutions it can in the line

returns a pointer to a NEW allocated bufffer, 
caller is responsible for calling FreeMemoryBlock()
*************************************/
char *XTL_Substitute( char *line );

#endif
