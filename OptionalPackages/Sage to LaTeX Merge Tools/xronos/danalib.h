
/****************************************************
Copyright 2016 Dana Nowell, all rights reserved.

Standard set of functions to make life easier.  
I have been using these for decades but figured 
I should just put them all in one place.;

Licensing:
Commercial use of this code is forbidden with out written permission
The code IS free to use in educational institutions 
and a license for that use is granted herein.

****************************************************/

#ifndef DANALIBH_INCLUDED

#define DANALIBH_INCLUDED 1  /* define it so we do not double include */

/* 
check to see if we are building on windows 
if we are then define DANALIB_WIN so we do 
the right thing when we need to.
*/
#ifdef __MINGW32__
#define DANALIB_WIN 1
#endif

#ifdef __WIN32__
#define DANALIB_WIN 1
#endif

#ifdef __CYGWIN__
#define DANALIB_WIN 1
#endif

#ifdef _MSC_VER
#define DANALIB_WIN 1
#endif
/* END of check to see if we are building on windows */




#define DL_MAX_TRACELINE_LEN 1000  /* maximum RESULTING line in a TracePrintf */

/*************************************
*
*************************************/
typedef enum { FALSE = 0, TRUE } BOOL;


/*====================================================
This block is the trace stuff
typically used for debug runtime tracing
====================================================*/
/*************************************
*
*************************************/
void TraceInit( void );

/*************************************
*
*************************************/
void SetTraceOutput( FILE *TraceOutputFile );  /* defaults to stderr if not set */

/*************************************
*
*************************************/
void TracePrintf( char *fmt, ... );

/*************************************
*
*************************************/
void TraceOn( BOOL val );

/*************************************
*
*************************************/
void DoTrace( char *p );

/*************************************
*
*************************************/
BOOL GetTraceStatus( void );  /* returns TRUE if trace is on and outputfile is not null */

/*************************************
*
*************************************/
void DoTraceWithLine( char *p, int LineNo );



/*====================================================
This block is the Error stuff
typically used outputting errors
====================================================*/
/*************************************
*
*************************************/
void ErrorExit( char *p );


/*====================================================
This block is the string stuff
====================================================*/

/*************************************
*
*************************************/
char *TrimLeadingWhitespace( char *p );

/*************************************
*
*************************************/
void TrimTrailingWhitespace( char *p );

/*************************************
*
*************************************/
char *TrimWhitespace( char *p );

/*************************************
*
*************************************/
char *FindEndOfString( char *str );

/*************************************
void DL_TrimQuotesInPlace( char *line );

if string has both a leading and matching trailing 
quote (double or single) this function strips them 
off in place (changes the argument string)

returns nothing
*************************************/
void DL_TrimQuotesInPlace( char *line );

/*====================================================
This block is the memory stuff
====================================================*/
/*************************************
*
*************************************/
void *GetMemoryBlock( int size );

/*************************************
*
*************************************/
void FreeMemoryBlock( void *p );

/*====================================================
This block is the file stuff
====================================================*/
/*************************************
* input to this function assumes a FILE like name 
* (no ending slash like a direcotry might have)
*************************************/
BOOL DoesFileExist( char *OutputFileName );

/*************************************
* input to this function assumes a FILE not a directory
* you CAN pass a directory BUT it must end with / or
* it will be assumed to be a FILE and not a Directory
*************************************/
BOOL MakeDirChainForFile( char *OutputFileName );

/*====================================================
This block is the command line processing stuff
====================================================*/

/*************************************
BOOL DL_CL_GetSwitch( int WhichSwitchNum, char **Switch, char **Value )
which one do you want 0? 1? ... 12?
address of the char * tp put the parts into 

e.g.:
char *Sw, *Arg;
if ( DL_CL_GetSwitch( 0, &sw, &arg ) == TRUE )
gets the first switch

returns TRUE if you got values, FALSE if 
WhichSwitchNum is out of range
*************************************/
BOOL DL_CL_GetSwitch( int WhichSwitchNum, char **Switch, char **Value );


/*************************************
char *DL_CL_GetArg( int WhichSwitchNum )
which one do you want 0? 1? ... 12?

e.g.:
char *Arg;
arg = DL_CL_GetArg( 0 )
gets the first non switch cmdline parameter
*************************************/
char *DL_CL_GetArg( int WhichOne );


/*************************************
char *DL_CL_GetProgName( void )

returns pointer to program name part of argv[0]
*************************************/
char *DL_CL_GetProgName( void );


/*************************************
returns pointer to program path part of argv[0]
*************************************/
char *DL_CL_GetProgPath( void );



/*************************************
void DL_CL_ProcessArgs( int ac, char *av[] )

call this to process the command line args.
Must be called FIRST

typical call is:
DL_CL_ProcessArgs( argc, argv )

*************************************/
void DL_CL_ProcessArgs( int ac, char *av[] );


/*************************************
int DL_CL_GetArgCount( void )

returns the number of arguments that are NOT switches
*************************************/
int DL_CL_GetArgCount( void );


/*************************************
int DL_CL_GetSwitchCount( void )

returns the number of arguments that are switches
*************************************/
int DL_CL_GetSwitchCount( void );

/*************************************
unsigned char *DL_STRUPR( unsigned char *str )

modifies the string in place to be all uppercase

returns pointer to start of string (same str as passed in)
*************************************/
unsigned char *DL_STRUPR( unsigned char *str );


/*
Ini File Processing functions
*/
#define MAX_INI_KEY_LEN		100
#define MAX_INI_LINE_LEN	1000

/*************************************
void DL_DiscardIniFile( void )

Discards the data from the ini file 
freeing up the memory used to store it.
*************************************/
void DL_DiscardIniFile( void );


/*************************************
char *DL_GetIniValue( char *key, char *DefaultValue )

MUST call DL_ReadIniFile before calling this

returns a pointer to either the ini file value if 
it exists or the default value if it does not
*************************************/
char *DL_GetIniValue( char *key, char *DefaultValue );


/*************************************
char *DL_GetIniStringValue( char *key, char *DefaultValue )

MUST call DL_ReadIniFile before calling this

returns a pointer to either the ini file value if 
it exists or the default value if it does not.  The 
ini value has any matching quotes trimed off before 
return
*************************************/
char *DL_GetIniStringValue( char *key, char *DefaultValue );


/*************************************
BOOL DL_GetIniBOOLValue( char *key, BOOL DefaultValue )

MUST call DL_ReadIniFile before calling this

checks for Y/N or Yes/no in ini file and converts 
to a BOOL and returns it.  If not found or not a valid 
value, then returns the DefaultValue.

returns a BOOL value 
*************************************/
BOOL DL_GetIniBOOLValue( char *key, BOOL DefaultValue );

/*************************************
int DL_ReadIniFile( char *filename )

returns 0 on success
returns -1 if can not open file
*************************************/
int DL_ReadIniFile( char *filename );

/*
END Ini File Processing functions
*/

/*
KeyValue list Processing functions
*/

typedef struct _DL_KeyValue_List {

struct _DL_KeyValue_List *next;
char *key;
char *value;
BOOL CaseSensitive;
} DL_KeyValue_List;

/*************************************
void DL_KeyValue_Add( DL_KeyValue_List **MyListhead, char *key, char *value, BOOL CaseSensitive )

adds the key value pair to the list

returns nothing
*************************************/
void DL_KeyValue_Add( DL_KeyValue_List **MyListhead, char *key, char *value, BOOL CaseSensitive );

/*************************************
void DL_KeyValue_PurgeAll( DL_KeyValue_List **MyListhead )

deletes the entire KeyValueSet

returns nothing
*************************************/
void DL_KeyValue_PurgeAll( DL_KeyValue_List **MyListhead );

/*************************************
void DL_KeyValue_Delete( DL_KeyValue_List **MyListhead, char *key )

deletes the entry if the key exists

returns nothing
*************************************/
void DL_KeyValue_Delete( DL_KeyValue_List **MyListhead, char *key );

/*************************************
int DL_KeyValue_Modify( DL_KeyValue_List **MyListhead, char *key, char value )

Modifies the value of gthe key if the key exists

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int DL_KeyValue_Modify( DL_KeyValue_List **MyListhead, char *key, char *value );

/*************************************
char *DL_KeyValue_Find( DL_KeyValue_List **MyListhead, char *key )

finds the key if the key exists

returns NULL if key not found, otherwise a pointer to the value
*************************************/
char *DL_KeyValue_Find( DL_KeyValue_List **MyListhead, char *key );

/*
END KeyValue list Processing functions
*/

#endif

