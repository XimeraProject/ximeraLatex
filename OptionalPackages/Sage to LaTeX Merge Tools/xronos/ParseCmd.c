/****************************************************
Copyright 2016 Dana Nowell, all rights reserved.

Standard set of functions to make life easier.  
I have been using these for decades but figured 
I should just put them all in one place.

Licensing:
Commercial use of this code is forbidden with out written permission
The code IS free to use in educational institutions 
and a license for that use is granted herein.

****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "danalib.h"


#define DL_CMD_MAX_ARGS	20

typedef struct {
	char *TheSwitch;
	char *TheArg;
} SwitchArg;


static BOOL AmInitialized = FALSE;

static char *NonSwitchArgs[DL_CMD_MAX_ARGS + 1];
static SwitchArg Switches[DL_CMD_MAX_ARGS + 1];
static int NonSwitchArgsCount = 0;
static int SwitchArgsCount = 0;

static char ProgramPath[4500];
static char ProgramName[300];


	
/*************************************
*************************************/
static void CheckInitialization( void )
{
	if ( ! AmInitialized )
		ErrorExit( "Can not call cmd line arg routines before calling DL_CL_ProcessArgs()" );
}



/*************************************
pull the cmdline switch apart and load into 
the switches array
*************************************/
static void ProcessCommandLineSwitch( char *MySwitch )
{
	char buf[300];
	char *p;
	int value;
	char *Part2;
	
	if ( MySwitch == NULL || *MySwitch == 0 ) /* do we actual have a switch at all */
		return;
	
	if ( strlen( MySwitch ) >= sizeof( buf ) )  /* too long for a real switch do not overrun buffer */
		return;
	
	strcpy( buf, MySwitch );

	
/* is it a switch with a value (e.g., -count=4) */	
	Part2 = strstr( buf, "=" );
	if ( Part2 != 0 )
	{
		*Part2 = 0;
		++Part2;
	}
		
		/* Make the switch lower case */
	for ( p = buf; *p != 0; ++p )
		*p = tolower((unsigned char) *p);


	Switches[SwitchArgsCount].TheSwitch = GetMemoryBlock( strlen(buf) + 1);
	strcpy( Switches[SwitchArgsCount].TheSwitch, TrimWhitespace(buf) );
	if ( Part2 != NULL && strlen( Part2 ) > 0 )
	{
		Switches[SwitchArgsCount].TheArg = GetMemoryBlock( strlen(Part2) + 1);
		strcpy( Switches[SwitchArgsCount].TheArg, TrimWhitespace(Part2) );
	}
	else
		Switches[SwitchArgsCount].TheArg = NULL;
	
	++SwitchArgsCount;
	
}



/*************************************
pull the program name and path apart
*************************************/
static void ProcessProgramName( char *ProgName )
{
char *p;

	strcpy( ProgramPath, ProgName );
	ProgramName[0] = 0;
	
	/* 
	walk backwards from end of strng and look for a directory marker
	if found, copy from the marker forward to the name and terminate 
	the path just AFTER the directory marker
	*/
	for ( p = FindEndOfString( ProgramPath ); p != ProgramPath; --p )
	{
		if ( *p == '/' )
		{
			strcpy( ProgramName, (p+1) );
			*(p+1) = 0;
		}
	}	
	
	/* 
	if we do not have a name, we did not find a directory marker
	which means we have no directory path before the file name
	So copy it all to the name and set the path to empty
	*/
	if ( strlen( ProgramName ) == 0 )
	{
		strcpy( ProgramName, ProgramPath );
		ProgramPath[0] = 0;
	}
}


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
BOOL DL_CL_GetSwitch( int WhichSwitchNum, char **Switch, char **Value )
{
	BOOL retval = TRUE;
	
	CheckInitialization();
	

	if ( Switch == NULL || Value == NULL )
		ErrorExit( "DL_CL_GetSwitch called with NULL arg" );


	*Switch = NULL;
	*Value = NULL;
	
	if ( WhichSwitchNum < 0 || WhichSwitchNum >= SwitchArgsCount )
		retval = FALSE;
	else
	{

		*Switch = Switches[WhichSwitchNum].TheSwitch;
		*Value = Switches[WhichSwitchNum].TheArg;
	}
	
	return retval;
}



/*************************************
char *DL_CL_GetArg( int WhichSwitchNum )
which one do you want 0? 1? ... 12?

e.g.:
char *Arg;
arg = DL_CL_GetArg( 0 )
gets the first non switch cmdline parameter
*************************************/
char *DL_CL_GetArg( int WhichOne )
{
char *retval = NULL;
	
	CheckInitialization();
	
	if ( WhichOne >= 0 || WhichOne < NonSwitchArgsCount )
		retval = NonSwitchArgs[WhichOne];

	return retval;
}



/*************************************
char *DL_CL_GetProgName( void )

returns pointer to program name part of argv[0]
*************************************/
char *DL_CL_GetProgName( void )
{
	CheckInitialization();
	
	return ProgramName;
}



/*************************************
returns pointer to program path part of argv[0]
*************************************/
char *DL_CL_GetProgPath( void )
{
	CheckInitialization();
	
	return ProgramPath;
}



/*************************************
void DL_CL_ProcessArgs( int ac, char *av[] )

call this to process the command line args.
Must be called FIRST

typical call is:
DL_CL_ProcessArgs( argc, argv )

*************************************/
void DL_CL_ProcessArgs( int ac, char *av[] )
{
int i;

	ProcessProgramName( av[0] );

	if ( ac > 1 )
	{
		for ( i = 1; i < ac; i++ )
		{
			if ( *av[i] == '-' || *av[i] == '/' )
				ProcessCommandLineSwitch( (av[i] + 1) );
			else
				NonSwitchArgs[NonSwitchArgsCount++] = av[i];

		}
	}

	AmInitialized = TRUE;
}


/*************************************
int DL_CL_GetArgCount( void )

returns the number of arguments that are NOT switches
*************************************/
int DL_CL_GetArgCount( void )
{
	CheckInitialization();

	return NonSwitchArgsCount;
}


/*************************************
int DL_CL_GetSwitchCount( void )

returns the number of arguments that are switches
*************************************/
int DL_CL_GetSwitchCount( void )
{
	CheckInitialization();

	return SwitchArgsCount;
}


