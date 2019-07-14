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
#include "danalib.h"


static DL_KeyValue_List *head = NULL;


/*************************************
*************************************/
static void ProcessIniLine( unsigned char *MyLine )
{
	char *p;
	char *key, *value;
	
	if ( MyLine == NULL || strlen((char *)MyLine) < 1 )
		return;
	
	/* trim off comment char */
	p = strchr( (char *) MyLine, '#' );
	if ( p != NULL )
		*p = 0;
	
	p = TrimWhitespace( (char *) MyLine );
	if ( strlen( (char *) p ) < 1 ) /* empty line */
		return;
	
	/* break into key and value */
	key = p;
	p = strchr( (char *) p, '=' );
	if ( p == NULL )  /* no equal implies not key and value */
		return;
	
	*p = 0;
	value = p + 1;
	key = TrimWhitespace( (char *) key );
	value = TrimWhitespace( (char *) value );

	DL_KeyValue_Add( &head, (char *) key, (char *) value, FALSE );
}


/*************************************
*************************************/
void DL_DiscardIniFile( void )
{

DL_KeyValue_PurgeAll( &head );
		
}



/*************************************
*************************************/
int DL_ReadIniFile( char *filename )
{
	FILE *fh;
	unsigned char IniLine[ MAX_INI_LINE_LEN +1 ];
	
	if ( filename == NULL )
		return -1;
	
	fh = fopen( filename, "r" );
	if ( fh == NULL )
		return -1;
	
	while( fgets((char *)IniLine, sizeof(IniLine), fh ) != NULL )
	{
		ProcessIniLine( IniLine );
	}

	fclose( fh );
	return 0;
}



/*************************************
*************************************/
char *DL_GetIniValue( char *key, char *DefaultValue )
{

char *p;

p = DL_KeyValue_Find( &head, key );
if ( p == NULL )
	p = DefaultValue;

return p;
}



/*************************************

*************************************/
BOOL DL_GetIniBOOLValue( char *key, BOOL DefaultValue )
{
	char *p;
	BOOL retval;
	
	retval = DefaultValue;
	
	p = DL_GetIniValue( key, NULL );
	if ( p != NULL )
	{
		DL_STRUPR( (unsigned char *) p );
		if ( strcmp( p, "N" ) == 0 || strcmp( p, "NO" ) == 0 )
			retval = FALSE;
		else if ( strcmp( p, "Y" ) == 0 || strcmp( p, "YES" ) == 0 )
			retval = TRUE;
		else
			printf( "%f conf file entry has unknown value, should be Yes or No\n", key );
	}
	
	return retval;
}


/*************************************

*************************************/
char *DL_GetIniStringValue( char *key, char *DefaultValue )
{
	char *p;
	char *retval;
	
	retval = DefaultValue;
	
	p = DL_GetIniValue( key, NULL );
	if ( p != NULL )
	{
	DL_TrimQuotesInPlace( p );
	retval = p;
	}
	
	return retval;
}
