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



/*************************************
void DL_KeyValue_Add( DL_KeyValue_List **MyListhead, char *key, char *value, BOOL CaseSensitive )


adds the key value pair to the list

returns nothing
*************************************/
void DL_KeyValue_Add( DL_KeyValue_List **MyListhead, char *key, char *value, BOOL CaseSensitive )
{
DL_KeyValue_List *Me;

if ( MyListhead == NULL )
	ErrorExit( "DL_KeyValue_Add: NULL Key" );

if ( key == NULL )
	ErrorExit( "DL_KeyValue_Add: NULL Key" );

TracePrintf( "DL_KeyValue_Add: Key: %s, Value: %s\n", key, value );

Me = GetMemoryBlock( sizeof(DL_KeyValue_List) );
Me->key = GetMemoryBlock( strlen(key) + 1 );
strcpy( Me->key, key );

Me->CaseSensitive = CaseSensitive;

if ( ! Me->CaseSensitive )
	DL_STRUPR( (unsigned char *) Me->key );

Me->value = GetMemoryBlock( strlen(value) + 1 );
strcpy( Me->value, value );
Me->next = *MyListhead;
*MyListhead = Me;

}


/*************************************
*************************************/
static DL_KeyValue_List *KeyValue_Find( DL_KeyValue_List **MyListhead, char *key, DL_KeyValue_List *prev )
{
DL_KeyValue_List *Me;
char *MyKey;
int val;

if ( MyListhead == NULL )
	ErrorExit( "KeyValue_Find: Bad list head" );

if ( key == NULL )
	ErrorExit( "KeyValue_Find: NULL Key" );

TracePrintf( "KeyValue_Find: Key: %s\n", key );


prev = NULL;

MyKey = GetMemoryBlock( strlen(key) + 1 );
strcpy( MyKey, key );

DL_STRUPR( (unsigned char *) MyKey );

for( Me = *MyListhead; Me != NULL; Me = Me->next )
{
	if ( Me->CaseSensitive )
		val = strcmp( key, Me->key );
	else
		val = strcmp( MyKey, Me->key );

	if ( val == 0 )
		break;

	prev = Me;
}

FreeMemoryBlock( MyKey );
return Me;
}


/*************************************
char *DL_KeyValue_Find( DL_KeyValue_List **MyListhead, char *key )

finds the key if the key exists

returns NULL if key not found, otherwise a pointer to the value
*************************************/
char *DL_KeyValue_Find( DL_KeyValue_List **MyListhead, char *key )
{
DL_KeyValue_List *prev, *Me; 


if ( MyListhead == NULL )
	ErrorExit( "DL_KeyValue_Find: NULL Key" );

if ( key == NULL )
	ErrorExit( "DL_KeyValue_Find: NULL Key" );


TracePrintf( "DL_KeyValue_Find: Key: %s\n", key );


Me = KeyValue_Find( MyListhead, key, prev );
if ( Me == NULL )
	return NULL;

return Me->value;
}


/*************************************
void DL_KeyValue_Delete( DL_KeyValue_List **MyListhead, char *key )

deletes the entry if the key exists

returns nothing
*************************************/
void DL_KeyValue_Delete( DL_KeyValue_List **MyListhead, char *key )
{
DL_KeyValue_List *prev, *Me, *Next; 


if ( MyListhead == NULL )
	ErrorExit( "DL_KeyValue_Delete: Bad list head" );

if ( key == NULL )
	ErrorExit( "DL_KeyValue_Delete: NULL Key" );


TracePrintf( "DL_KeyValue_Delete: Key: %s\n", key );

Me = KeyValue_Find( MyListhead, key, prev );

if ( Me == NULL )
	return;  /* did not exist to be deleted */

Next = Me->next; /* save it */
Me->next = NULL; /* delink it */

if ( prev == NULL )
	*MyListhead = Next;
else
	prev->next = Next;

FreeMemoryBlock( Me->key );
FreeMemoryBlock( Me->value );
FreeMemoryBlock( Me );

}



/*************************************
int DL_KeyValue_Modify( DL_KeyValue_List **MyListhead, char *key, char value )

Modifies the value of gthe key if the key exists

returns 0 on success, -1 is key not found, -2 bad list pointer, other non zero is internal error
*************************************/
int DL_KeyValue_Modify( DL_KeyValue_List **MyListhead, char *key, char *value )
{
	DL_KeyValue_List *Me, *prev;
	

if ( MyListhead == NULL )
	ErrorExit( "DL_KeyValue_Modify: Bad list head" );

if ( key == NULL )
	ErrorExit( "DL_KeyValue_Modify: NULL Key" );

TracePrintf( "DL_KeyValue_Modify: Key: %s, Value: %s\n", key, value );


	Me = KeyValue_Find( MyListhead, key, prev );
	if ( Me == NULL )
		return -1;
	
	if ( strlen(Me->value) < strlen(value) )
	{
		FreeMemoryBlock( Me->value );
		Me->value = GetMemoryBlock( strlen(value) + 1 );
	}
	strcpy( Me->value, value );

	
	return 0;
}




/*************************************
void DL_KeyValue_PurgeAll( DL_KeyValue_List **MyListhead )

deletes the entire KeyValueSet

returns nothing
*************************************/
void DL_KeyValue_PurgeAll( DL_KeyValue_List **MyListhead )
{

if ( MyListhead == NULL )
	ErrorExit( "DL_KeyValue_PurgeAll: Bad list head" );

TracePrintf( "DL_KeyValue_PurgeAll\n" );

if ( *MyListhead != NULL )
{
	DL_KeyValue_Delete( MyListhead, (*MyListhead)->key );
	DL_KeyValue_PurgeAll(MyListhead);
}

}











