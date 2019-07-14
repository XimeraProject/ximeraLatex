

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "danalib.h"
#include "xronostoollib.h"


static DL_KeyValue_List *head = NULL;


/*************************************
void XTL_SubVars_Init( void )

Must be called to initialize the variables list

returns nothing
*************************************/
void XTL_SubVars_Init( void )
{
XTL_SubVars_Add( "XTL_SV_QuestionCount", "0", FALSE );
XTL_SubVars_Add( "XTL_SV_HELPFILE", "", FALSE );
XTL_SubVars_Add( "XTL_SV_OUTPUTFILE", "", FALSE );
}



/*************************************
int XTL_SubVars_Add( char *key, char *Value, BOOL CaseSensitive ) )

updates the key with the value

returns 0 on success, -1 is key already exists, other non zero is internal error
*************************************/
int XTL_SubVars_Add( char *key, char *Value, BOOL CaseSensitive )
{
char *p;

p = XTL_SubVars_GetValueChar( key );
if ( p != NULL )
	return -1;

DL_KeyValue_Add( &head, key, Value, CaseSensitive );

return 0;
}


/*************************************
int XTL_SubVars_UpdateChar( char *key, char *Value )

updates the key with the value

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_UpdateChar( char *key, char *Value )
{
return DL_KeyValue_Modify( &head, key, Value );
}


/*************************************
int XTL_SubVars_UpdateInt( char *key, char *Value )

updates the key with the value

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_UpdateInt( char *key, int Value )
{
char buffer[100];

sprintf( buffer, "%d", Value );
return DL_KeyValue_Modify( &head, key, buffer );
}


/*************************************
int XTL_SubVars_GetValueChar( char *key, char *Value )

returns the value associated with the key 

returns NULL if key not found
*************************************/
char *XTL_SubVars_GetValueChar( char *key )
{
return DL_KeyValue_Find( &head, key );
}


/*************************************
int XTL_SubVars_GetValueInt( char *key, char *Value )

returns the value associated with the key 

returns XTL_SUBVARS_INT_VALUE_NOT_FOUND if key not found
*************************************/
int XTL_SubVars_GetValueInt( char *key )
{
char *p;
int retval;

p = DL_KeyValue_Find( &head, key );

if ( p == NULL )
	return XTL_SUBVARS_INT_VALUE_NOT_FOUND;
	
retval = atoi( p );
	
return retval;
}



/*************************************
int XTL_SubVars_IncrementValue( char *key )

Increments the value associated with the key

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_IncrementValue( char *key )
{
int retval;

retval = XTL_SubVars_GetValueInt( key );
if ( retval == XTL_SUBVARS_INT_VALUE_NOT_FOUND )
	return -1;

++retval;

return XTL_SubVars_UpdateInt( key, retval );
}



/*************************************
int XTL_SubVars_DecrementValue( char *key )

decrements the value associated with the key

returns 0 on success, -1 is key not found, other non zero is internal error
*************************************/
int XTL_SubVars_DecrementValue( char *key )
{
int retval;

retval = XTL_SubVars_GetValueInt( key );
if ( retval == XTL_SUBVARS_INT_VALUE_NOT_FOUND )
	return -1;

--retval;

return XTL_SubVars_UpdateInt( key, retval );
}

/*************************************
*************************************/
static char *GetNewBuffer( char *Line, int Increment )
{
	int len;
	char *retval;
	
	if ( Line == NULL )
		return NULL;
	
	len = strlen(Line) + Increment + 1;
	retval = GetMemoryBlock( len );
	
	retval[0] = 0;
	return retval;
}


/*************************************
char *XTL_Substitute( char *line )

Makes all substitutions it can in the line

returns a pointer to a NEW allocated bufffer, 
caller is responsible for calling FreeMemoryBlock()
*************************************/
char *XTL_Substitute( char *line )
{
	char *MyStartLine;
	char *MyEndLine;
	int len, MaxValueLen = -1;
	char *p;
	DL_KeyValue_List *Me;

	MyStartLine = GetNewBuffer( line, 0 );
	strcpy( MyStartLine, line );
	
	/* determine longest string in the substitution variables list */
	for( Me = head; Me != NULL; Me = Me->next )
	{
		len = strlen( Me->value );
		if ( len > MaxValueLen )
			MaxValueLen = len;
		
	}
	
	Me = head; 
	while ( Me != NULL )
	{
		/* do we have a hit */
		p = strstr( MyStartLine, Me->key );
		if ( p == NULL ) /* if not, then move to next variable */
		{
			Me = Me->next;
			continue;
		}
		
		TracePrintf( "XTL_Substitute: Key: %s, Line: %s\n", Me->key, MyStartLine );

		/* get a new buffer we can put stuff into, enough for the original string plus the maximum length value we can have */
		MyEndLine = GetNewBuffer( MyStartLine, MaxValueLen );
		/* copy everything up to the key */
		len = (int)(p - MyStartLine);
		if ( len > 0 )
		{
			strncpy( MyEndLine, MyStartLine, len );
			MyEndLine[len] = 0;
		}
		/* copy in the value related to the key */
		strcat( MyEndLine, Me->value );
		/* skip over the key in the original buffer */
		p+=strlen(Me->key);
		/* copy in the remainder of the line */
		if ( *p != 0 )
			strcat( MyEndLine, p );
		/* free the old buffer */
		p = MyStartLine;
		MyStartLine = MyEndLine;
		FreeMemoryBlock( p );
		MyEndLine = NULL;
	}
	
	return MyStartLine;
}














