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

#include "xronosfmt.h"

typedef struct _taglist {
	struct _taglist *next;
	char *tag;
} TAGLIST;


typedef struct _XronosOption {
	char OptionName[50];
	char OptionValue[200];
} XronosOption;


typedef struct _tagdata {
	char name[50];
	char headername[50];
	TAGLIST *head;
} TAGDATA;

typedef struct _driverfile {
	struct _driverfile *next;
	BOOL IsTagLine;
	TAGLIST *head;
} DriverFileEntry;

static DriverFileEntry *DriverFileHead, *DriverFileTail;
static TAGLIST *DriverFileTailLast;
BOOL DriverFileAppend = TRUE;  /* assume a new entry we can just append to the end of the file and not that we are changing an entry and have to rewrite the entire file */
static TAGLIST *AppendEntry;


static char RollupFileName[XTL_MAX_FILENAME_SIZE + 1];  /* used for the rollup of the file info to drive the process */

static int LineNo = 0;
static char ParseBuffer[XIO_MAX_LINE +1];   /* a temp buffer to be used within a function and never holding context over a call to another function */

static int BucketCount = 0;

static TAGDATA UnknownTags = {"", "Unknown Tags:\t", NULL};
static TAGDATA Buckets[] = {
	{"Topic", "Topic Type:   \t", NULL },
	{"Type",  "Question Type:\t", NULL },
	{"Ans",   "Answer Type:  \t", NULL },
	{"Sub",   "Sub Types:    \t", NULL },
	{"File",  "File Number:  \t", NULL },
{"", "", NULL }};  /* end marker, do not use or remove it is how we find the end of the array duriing loops */

	
XronosOption XronosOptions[] = {
	{"Path", "" },
	{"MaxQuestions", "" },
	{"", "" } /* used to mark end of array, a zero length name implies end during various processing */
};

	
/*************************************
*
*************************************/
static void DeleteTagListEntry( TAGLIST *Entry )
{
	if ( Entry == NULL )
		return;
	
	if ( Entry->tag != NULL )
	{
		FreeMemoryBlock(Entry->tag);
		Entry->tag = NULL;
	}
	
	FreeMemoryBlock(Entry);
	
}



/*************************************
*
*************************************/
static void DeleteTagList( TAGLIST **List )
{

TAGLIST *Entry;	

	if ( List == NULL || *List == NULL )
		return;

	while ( (*List) != NULL )
	{
		Entry = *List;
		(*List) = Entry->next;
		DeleteTagListEntry( Entry );
	}
	
}



/*************************************
*
*************************************/
static TAGLIST *BuildTagListEntry( char *tag )
{
	TAGLIST *Entry;
	
	Entry = GetMemoryBlock( sizeof(TAGLIST) );
	Entry->tag = GetMemoryBlock( strlen(tag) + 1 );
	strcpy( Entry->tag, tag );
	Entry->next = NULL;
	
	return Entry;
}


/*************************************
*
*************************************/
static void AddDriverFileLine( char *line )
{
	TAGLIST *Entry;
	
	if ( line == NULL || *line == 0 )
		return;
	
	Entry = BuildTagListEntry( line );
	if ( DriverFileTail->head == NULL )
	{
		/*
		TracePrintf( "AddDriverFileLine setting head to %p for line %s\n", Entry, line );
		*/
		DriverFileTail->head = Entry;
	}
	else
	{
		DriverFileTailLast->next = Entry;
	}
	
	DriverFileTailLast = Entry;
	
}



/*************************************
*
*************************************/
static void DeleteDriverFileEntry( DriverFileEntry *Entry )
{
	TAGLIST *next;
	
	if ( Entry == NULL )
		return;
	
	while ( Entry->head != NULL )
	{
		next = Entry->head->next;
		FreeMemoryBlock(Entry->head);
		Entry->head = next;
	}
	
	FreeMemoryBlock(Entry);
}




/*************************************
*
*************************************/
static void DeleteDriverFileList( void )
{
	DriverFileEntry *next;
	
	while ( DriverFileHead != NULL )
	{
		next = DriverFileHead->next;
		DeleteDriverFileEntry( DriverFileHead );
		DriverFileHead = next;
	}
	
	DriverFileTail = DriverFileHead;
	
}




/*************************************
*
*************************************/
static void AddDriverFileEntry( void )
{
	DriverFileEntry *Entry;
	
	Entry = GetMemoryBlock( sizeof(DriverFileEntry) );
	Entry->IsTagLine = FALSE;
	Entry->next = NULL;
	Entry->head = NULL;

/*
	TracePrintf( "in - AddDriverFileEntry adding block %p", Entry );
	
*/
	
	if ( DriverFileHead == NULL )
	{
		DriverFileHead = Entry;

	}
	else
	{
		DriverFileTail->next = Entry;
	}
	
	DriverFileTail = Entry;
	DriverFileTailLast = DriverFileTail->head;

/*	
	TracePrintf( "out - AddDriverFileEntry Head %p, tail: %p", (void *) DriverFileHead, (void *) DriverFileTail );
	
*/

}






/*************************************
*  Handle each individual tag
*************************************/
static void HandleTag( char *line )
{
	char *p;
	int i;
	TAGLIST *TagList;
	
	if ( line == NULL || *line == 0 ) /* skip empty crap */
		return;
	
	TracePrintf( "Found tag %s\n", line );
	
	
	strcpy( ParseBuffer, line );

	/* strip off any trailing braces in case user used them to group items */
	p = FindEndOfString( ParseBuffer );  /* last character in string */
	
	if ( *p == '{' || *p == '}' || *p == '[' || *p == ']' )
		*p = 0;

	
	TagList = BuildTagListEntry( ParseBuffer );
	
	/* bust it at the @ so we can identify the bucket for processing counts */
	p = strstr( ParseBuffer, "@" );
	if ( p != NULL ) /* if we have an @ then truncate the buffer there for pattern matching */
		*p = 0;
	
	for ( i = 0; strlen(Buckets[i].name) > 0; ++i )
	{
		if ( strcmp( Buckets[i].name, ParseBuffer ) == 0 )
		{
			TagList->next = Buckets[i].head;
			Buckets[i].head = TagList;
			TracePrintf( "HandleTag - i: %d, TagList %p, head: %p, taglist->next: %p\n", i, (void *) TagList, (void *) Buckets[i].head, (void *) TagList->next );
			break;
		}
	}
	
	if ( i >= BucketCount )
	{
		TagList->next = UnknownTags.head;
		UnknownTags.head = TagList;
	}
		
}



/*************************************
*  Parse the Tags on the tag line we found
*************************************/
static void ParseTags( char *line )
{
char *p;
int Count = 1;

	TracePrintf( "BuildHeader - processing tags: %s\n", line );

	p = XTL_FindClosingCurlyBrace( line, &Count );
	if ( p != 0 )
		*p = 0;  /* kill tags at close brace to simplify processing */	
	
	p = strtok( line, "," );
	while ( p != NULL )
	{
	p = TrimWhitespace( p );
	if ( *p != 0 )  /* we actually have a string */
	{
	/* strip off any Leading braces in case user used them to group items */
		
		if ( *p == '{' || *p == '}' || *p == '[' || *p == ']' )
			++p;
		
		HandleTag( p );
	}
		
	p = strtok( NULL, "," );
	}

	DoTrace( "Done Parsing Tags" );
}




/*************************************
*  Walk the xronosfmtoptions  array and find the one we want.
*************************************/
static XronosOption *FindXronosfmtOption( char *name )
{
	int i;
	
/* 	
TracePrintf( "FindXronosfmtOption - looking for %s\n", name );
*/

	for ( i = 0; strlen(XronosOptions[i].OptionName) > 0; i++ )
	{

/*
TracePrintf( "FindXronosfmtOption - test match with %s\n", XronosOptions[i].OptionName );
*/

		if ( strcmp( XronosOptions[i].OptionName, name ) == 0 ) /* matches */
		{
			/*
			DoTrace("FindXronosfmtOption - found it")
			*/
			return &XronosOptions[i];
		}
	}
	/*
	DoTrace("FindXronosfmtOption - did NOT find it")
	*/
	return NULL;
}



/*************************************
*  we have all the options loaded, some may need to be cleaned up
*************************************/
static void XronosfmtOptionsCleanup( void )
{
	XronosOption *ctxt;
	char *p, *q;
	
	/* handle "Path" 
	if Path has quotes strip them off
	trim the whitespace 
	and put it back into the OptionValue all cleaned up
	*/
	
	ctxt = FindXronosfmtOption( "Path" );
	if ( ctxt != NULL && ctxt->OptionValue != NULL && strlen( ctxt->OptionValue) > 0 ) /* if the value exists */
	{
		p = ctxt->OptionValue;
		if ( *p == '.' && *(p+1) == '/' )
		{
			strcpy( ParseBuffer, CurrentDirectory );
			strcat(ParseBuffer, ++p );
		}
		else
			strcpy( ParseBuffer, ctxt->OptionValue);
		
		p = FindEndOfString( ParseBuffer ); /* last character */
		if ( *p != '/' ) /* ensure path ends in a directory separator */
		{
			strcat( ParseBuffer, "/" );
		}			
		
		strcpy( ctxt->OptionValue, ParseBuffer );
		TracePrintf( "XronosfmtOptionsCleanup - set Path to %s\n", ctxt->OptionValue );
	}
	
}



/*************************************
*  Handle line with xronosfmt control options
*************************************/
static void HandleSingleXronosfmtTag( char *line )
{
	XronosOption *ctxt;
	char *p, *q;


	if ( line == NULL || *line == 0 )
			return;
		
	TracePrintf( "In - HandleSingleXronosfmtTag - with: %s\n", line );

	/* first break it into 2 pieces, before and after the equals */
	q = line;
	p = strstr( line, "=" );
	if ( p == NULL )
	{
		printf( "!!!! Xronosfmt Option %s has no equals sign -  skipping it\n", q );
		return;
	}
	
	*p = 0;
	++p;
	if ( p != NULL && *p != 0 )  /* clean upp the value */
		p = TrimWhitespace( p );

	q = TrimWhitespace( q ); /* clean up the option name */
	if ( q == NULL || *q == 0 )
		return;  /* nothing to match, we REALLY should never get here but better to trap for it than blow up with seg fault below */  

	TracePrintf( "In - HandleSingleXronosfmtTag - looking for option name: %s\n", q );


	/* find the option if we can */	
	ctxt = FindXronosfmtOption( q );
	if ( ctxt != NULL ) /* got it */
	{
		if ( strlen(p) >= sizeof(ctxt->OptionValue) )
			ErrorExit( "xronosfmt Option is too long" );
		
		if ( p != NULL && *p != 0 )
		{
			strcpy( ctxt->OptionValue, p );
			DL_TrimQuotesInPlace( ctxt->OptionValue );
			
			TracePrintf( "xronosfmt option %s has value %s\n", ctxt->OptionName, ctxt->OptionValue );
		}
		else
			TracePrintf( "xronosfmt option %s has NULL pointer for data\n", ctxt->OptionName );
	}
else	
	{
		printf( "xronosfmt option %s is unknown, skipping it\n", q );
	}
	
	DoTrace( "out - HandleSingleXronosfmtTag" );
}



/*************************************
*  Handle line with xronosfmt control options
*************************************/
static void ParseXronosfmtTags( char *line )
{
char *p, *q;
char *MyBuffer;  /* use a temp buffer */
int Count = 1;

	TracePrintf( "BuildHeader - processing ParseXronosfmtTags: %s\n", line );

	p = TrimWhitespace( line );
	if ( p == NULL || *p == 0 )  /* effectively an empty line */
		return;

		
	MyBuffer = GetMemoryBlock( strlen(line) + 1);
	strcpy( MyBuffer, line );
	q = MyBuffer;
	while ( (p = strstr(q, ";")) != NULL )
	{
		if ( p != NULL )
		{
	TracePrintf( "ParseXronosfmtTags - looking at: %s\n", q );

			*p = 0;
			++p;
		}

		q = TrimWhitespace( q ); /* clean it up */
		HandleSingleXronosfmtTag( q ); /* process it */
		
		q = p; /* setup for the next one */
	}
	
	FreeMemoryBlock( MyBuffer );
	DoTrace( "Done Parsing ParseXronosfmtTags" );
}


/*************************************
*  Handle each line looking for interesting stuff
*************************************/
static int HandleLine( char *line )
{
char *p, *comment;
char *TagSearchString = "\\tagged{";
char *XronosfmtSearchString = "\\xronosfmt-options:";

	comment = XTL_FindComment( line );  /* needs to be inside the comment */
	if ( comment == NULL ) /* no comment? not interested */
		return 0;
	
	p = strstr( comment, TagSearchString );
	if ( p != NULL )
	{
		DoTrace( "Found tags String" );
		p += strlen(TagSearchString);
		ParseTags( p );
	}

	p = strstr( comment, XronosfmtSearchString );
	if ( p != NULL )
	{
		DoTrace( "Found Xronosfmt option String" );
		p += strlen(XronosfmtSearchString);
		ParseXronosfmtTags( p );
	}

	return 0;
}



/*************************************
*  Main routine to build header structure
*************************************/
static char *FindHeaderTag( char *BucketName )
{
	int i;
	TAGLIST *Entry;

	DoTrace( "In FindTag" );
	
	for ( i = 0; strlen(Buckets[i].name) > 0; i++ )
	{
		if ( strcmp(BucketName, Buckets[i].name) == 0 )
		{
			TracePrintf( "FindTag - hit on %s\n", Buckets[i].name );
			
			Entry = Buckets[i].head;
			if ( Entry == NULL )
			{
				DoTrace("Out FindTag - empty list" );
				return NULL;
			}
			TracePrintf( "Out FindTag - tag val is %s\n", Entry->tag );
			
			return Entry->tag;
		}
		
	}
	
DoTrace("Out FindTag - no bucket" );
return NULL;
}


/*************************************
*  Main routine to build header structure
*************************************/
static int BuildOutputFilenameFromHeader( char *outfilename )
{
	XronosOption *ctxt;
	int retval = 0, i;
	char path[1000];
	char pathtemp[1000];
	char *TopicTag;
	char *TypeTag;
	char *FileTag;
	char *p, *q;
	
	DoTrace( "In BuildOutputFilenameFromHeader" );
	*outfilename = 0;  /* we use a bunch offf strcat calls, best to ensure the buffer is clear at start */
	path[0] = 0;  /* we use a bunch offf strcat calls, best to ensure the buffer is clear at start */
	
	/* build the file name here */
	TopicTag = FindHeaderTag( "Topic" );
	if ( TopicTag == NULL || strlen( TopicTag ) < 1 )
	{
		puts( "!!!!!  Missing Topic Tag from Tag line" );
		retval = 1;
	}
	
	TypeTag = FindHeaderTag( "Type" );
	if ( TypeTag == NULL || strlen( TypeTag ) < 1 )
	{
		puts( "!!!!!  Missing Type Tag from Tag line" );
		retval = 2;
	}
	
	FileTag = FindHeaderTag( "File" );
	if ( FileTag == NULL || strlen( FileTag ) < 1 )
	{
		puts( "!!!!!  Missing File Tag from Tag line" );
		retval = 3;
	}
	
	/* build the file name here */
	if ( retval == 0 )  /* still good */
	{
		DoTrace( "In BuildOutputFilenameFromHeader - building filename" );
		TracePrintf( "Initial tags - Topic: %s, Type: %s, File %s\n", TopicTag, TypeTag, FileTag );
		
		p = strstr(TopicTag, "@" );
		if ( p != NULL )
			TopicTag = p + 1;  /* move to after the @ */
		
		p = strstr(TypeTag, "@" );
		if ( p != NULL )
			TypeTag = p + 1;  /* move to after the @ */
		
		p = strstr(FileTag, "@" );
		if ( p != NULL )
			FileTag = p + 1;  /* move to after the @ */
		
		TracePrintf( "Final/Adjusted tags - Topic: %s, Type: %s, File %s\n", TopicTag, TypeTag, FileTag );
		

			/* build the path up first */
		p = GetXronosfmtOutputPath();
		if ( p != NULL && *p != 0 )
			strcpy( path, p );  /* use command line value as VALUE since you specified */
		else
		{  /* no command line override, figure it out */
			strcpy( path, "./" );  /* default to current directory */
		}

		/* 	if path is current directory we assume debug 
			and do not do input file or the subdirectory by topic 
			If not current directory, we do topic and input file processing
			NOTE: specify the current directory on the command line as other 
			than ./ and we ASSUME you do not mean current directory processing 
			as you tried hard to hide the current directory issue 
		*/
		if ( strcmp( path, "./" ) != 0 ) 
		{
			if ( strlen( TopicTag ) > 0 )
			{
				strcat( path, TopicTag );
				
			/* start the rollupfilename with the path and topic */
				strcpy( RollupFileName, path );

			/* terminate the file path & topic to signify a subdirectory */
				strcat( path, "/" );
			}
		}

		/* if path is relative, make full path */
		if ( path[0] == '.' && path[1] == '/' )
		{
			strcpy( pathtemp, CurrentDirectory );
			strcat( pathtemp, &path[1] );
			strcpy( path, pathtemp );
		}
		
		
	/* OK now the file name gets added */
		strcpy( outfilename, path );
		strcat( outfilename, TopicTag );
		strcat( outfilename, "-");
		strcat( outfilename, TypeTag );
		strcat( outfilename, "-");
		strcat( outfilename, FileTag );
		
		strcat( outfilename, ".tex" );  /* add .tex file extension to "normal" file */
		
		/* the rollup filename */
		if ( strlen(RollupFileName) > 0 )
		{
			strcat( RollupFileName, "-Input.tex");
			TracePrintf( "The rollup file for the input driver is: %s\n", RollupFileName );
		}
		
	}

	DoTrace( "out BuildOutputFilenameFromHeader" );

	return retval;
}

/*************************************
*
*************************************/
void ProcessDriverFile( void )
{
	int i;
	int TagCount = 0;
	TAGLIST *Entry;
	DriverFileEntry *DriverEntry;
	TAGLIST *DriverList;


	
	DoTrace( "in - ProcessDriverFile" );
	sprintf( ParseBuffer, "\\tagged{ " );
	
	/* build the tagged line for the driver file */
	for ( i = 0; strlen(Buckets[i].name) > 0; ++i )
	{
		for ( Entry = Buckets[i].head; Entry != NULL; Entry = Entry->next )
		{
			if ( Entry->tag != NULL )
			{
				if ( TagCount > 0 )
					strcat( ParseBuffer, ", " );
				
				strcat( ParseBuffer, Entry->tag );
				++TagCount;
			}

		}
	}
	
/* handle unknown tags here */
	for ( Entry = UnknownTags.head; Entry != NULL; Entry = Entry->next )
	{
		if ( Entry->tag != NULL )
			strcat( ParseBuffer, Entry->tag );
	}

	strcat( ParseBuffer, "}{\n" );
	strcat( ParseBuffer, "\t\\select@Question{" );
	strcat( ParseBuffer, OutputFileName );
	strcat( ParseBuffer, "}\n}\n\n" );
	AppendEntry = BuildTagListEntry( ParseBuffer );

	/*	
	TracePrintf( "ProcessDriverFile: DriverFileHead %p\n", (void *) DriverFileHead );
	*/
	
	for ( DriverEntry = DriverFileHead; DriverEntry != NULL; DriverEntry = DriverEntry->next )
	{
		/*
		TracePrintf( "ProcessDriverFile: DriverFileEntry %p\n", (void *) DriverEntry );
		*/
	
		for ( DriverList = DriverEntry->head; DriverList != NULL; DriverList = DriverList->next )
		{
			/*
			TracePrintf( "ProcessDriverFile: DriverList %p\n", (void *) DriverList );
			*/
			
			if ( DriverList->tag != NULL )
			{
				if( strstr( DriverList->tag, OutputFileName ) != NULL ) /* file was already processed, force rewrite */
				{
					/*
					TracePrintf( "ProcessDriverFile: Found it! DriverList %p, DriverFileEntry %p\n", (void *) DriverList, (void *) DriverEntry );
					*/
			
					DriverFileAppend = FALSE;	/* can no longer append, must rewrite */
					DeleteTagList( &(DriverEntry->head) );
					DriverEntry->head = AppendEntry;
					break;
				}
			}
		}
	}



}



/*************************************
*
*************************************/
void WriteDriverFile( char *DriverFileName )
{
	FILE *DriverFile;
	DriverFileEntry *Entry;
	TAGLIST *DriverList;
	
	if ( DriverFileName == NULL || *DriverFileName == 0 )
		return;
	
	if ( DriverFileAppend )
	{
		DoTrace( "Opening driver file for append" );
		DriverFile = fopen( DriverFileName, "a+" );  /* open for append */
		if ( DriverFile == NULL ) /* currently no driver file */
		{		
			ErrorExit( "Failed to append to driver file" );
		}

		if ( AppendEntry != NULL )
			fprintf( DriverFile, "%s", AppendEntry->tag );

	}
	else
	{
		DoTrace( "Opening driver file for rewrite" );
		DriverFile = fopen( DriverFileName, "w+" );
		if ( DriverFile == NULL ) /* currently no driver file */
		{		
			ErrorExit( "Failed to rewrite driver file" );
		}
	
		for ( Entry = DriverFileHead; Entry != NULL; Entry = Entry->next )
		{
			/*
			TracePrintf( "WriteDriverFile: DriverFileEntry %p\n", (void *) Entry );
			*/
		
			for ( DriverList = Entry->head; DriverList != NULL; DriverList = DriverList->next )
			{
				/*
				TracePrintf( "WriteDriverFile: DriverList %p\n", (void *) DriverList );
				*/
				
				if ( DriverList->tag != NULL )
					fprintf( DriverFile, "%s", DriverList->tag );
			}
		}
	}
	
	fclose( DriverFile );	
}


/*************************************
*
*************************************/
void ReadDriverFile( char *DriverFileName )
{
	FILE *DriverFile;
	char readbuf[XIO_MAX_LINE + 1];
	
	
	if ( DriverFileName == NULL || *DriverFileName == 0 )
		return;
	
	/* initialized the list to start with the crap at the top of the file */
	DriverFile = fopen( DriverFileName, "r" );
	if ( DriverFile == NULL ) /* currently no driver file */
	{	
		DriverFile = fopen( DriverFileName, "w+" );
		if ( DriverFile == NULL ) /* currently no driver file */
			ErrorExit( "Can not open the Driver/Input file for either read or write" );
			
		fprintf( DriverFile, "%s", "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n" );
		fprintf( DriverFile, "%s", "%%%%%%%%%%%%%%%%%%% 			Header Contents				%%%%%%%%%%%%%%%%%%%\n" );
		fprintf( DriverFile, "%s",  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n" );
		fprintf( DriverFile, "%s",  " \n" );
		fprintf( DriverFile, "%s",  "%Debug line. to activate this check, put \\Verbosetrue at the start of a file calling this.\n" );
		sprintf( readbuf, "\\ifVerbose{Input File Called: %s}\\fi\n", DriverFileName );
		fprintf( DriverFile, "%s",  readbuf );
		fprintf( DriverFile, "%s",  " \n" );
		fprintf( DriverFile, "%s",  " \n" );
		fprintf( DriverFile, "%s",  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n" );
		fprintf( DriverFile, "%s",  "%%%%%%%%%%%%%%%%%%% 			File Contents				%%%%%%%%%%%%%%%%%%%\n" );
		fprintf( DriverFile, "%s",  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n" );
		fprintf( DriverFile, "%s",  " \n" );

		fclose( DriverFile );

		return;  
	}
		
	AddDriverFileEntry();
	
	while( fgets( readbuf, XIO_MAX_LINE, DriverFile ) != NULL )	
	{
		if ( strstr( readbuf, "\\tagged{" ) != NULL )
			{
			AddDriverFileEntry();
			DriverFileTail->IsTagLine = TRUE;
			}
			
		AddDriverFileLine( readbuf );	
	}
	
	fclose( DriverFile );
}





/*************************************
*  routine to write header structure
*************************************/
int WriteOutputHeader( FILE *Out )
{
	int i;
	TAGLIST *Entry;
		
	/* NOTE: it takes two % to get one to display so %% in the format string is % in the file*/
	fputs( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", Out );
	fputs( "%%%%%%%%%%%%%%%%%%%%%%%%%   TAGS   %%%%%%%%%%%%%%%%%%%%%%%%%\n", Out );

	for ( i = 0; strlen(Buckets[i].name) > 0; ++i )
	{
		fprintf( Out, "%%%%%%%%\t%s", Buckets[i].headername );
/*
		TracePrintf( "WriteOutputHeader - i: %d, head: %p\n", i, (void *) Buckets[i].head );
*/

		for ( Entry = Buckets[i].head; Entry != NULL; Entry = Entry->next )
		{
/*
			TracePrintf( "WriteOutputHeader - i: %d, Entry %p, head: %p, Entry->tag: %p\n", 
				i, (void *) Entry, (void *) Buckets[i].head, (void *) Entry->tag );
*/
			if ( Entry->tag != NULL )
				fprintf( Out, "%s, ", Entry->tag );
		}
		fprintf( Out, "\n" );
	}
	
/* handle unknown tags here */
	fprintf( Out, "%%%%%%%%\t%s", UnknownTags.headername );
	for ( Entry = UnknownTags.head; Entry != NULL; Entry = Entry->next )
	{
		if ( Entry->tag != NULL )
			fprintf( Out, "%s", Entry->tag );
	}
	fprintf( Out, "\n" );
	
	
	fputs( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", Out );

/* DANA - DEAD?
	if ( QuestionsOnly )
	{		
		fprintf( Out, "\\ifquestionCount\\addtocounter{Total@Question}{%d}\n\\else\n", GetActualQuestionCount() );
	}
*/	
	/*  update the driver/rollup file if it exists */
	if ( strlen( RollupFileName ) > 0 )
	{
		printf( "Updating driver file %s\n", RollupFileName );
		ReadDriverFile(RollupFileName);
		ProcessDriverFile();
		WriteDriverFile( RollupFileName );
	}
	
/* clean up lists */
DoTrace( "Before taglist clean up" );
for ( i = 0; strlen(Buckets[i].name) > 0; ++i )
	DeleteTagList(&(Buckets[i].head));

DoTrace( "Before unknown taglist clean up" );

DeleteTagList(&(UnknownTags.head));
DoTrace( "After taglist clean up" );
	
DeleteDriverFileList();
DoTrace( "After DeleteDriverFileList clean up" );


	return 0;
}



/*************************************
*  Main routine to build header structure
*************************************/
void BuildOutputHeader( BOOL TagsRequired )
{
int done = 0;
int status;
int i;


/* init the max count of buckets */
for ( i = 0; strlen(Buckets[i].name) > 0; ++i )
	++BucketCount;
	
	
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
		status = HandleLine( LineBuffer );
		if ( status != 0 )  /* if not zero then either we have an error OR we reached max questions, so we are done */
			done = TRUE;
			
		if ( status > 0 )  /* positive values imply error */
			ErrorExit( "Handle Line blew up" );
		}
	}

XronosfmtOptionsCleanup(); /* must run before we build the output file name */
DoTrace( "After XronosfmtOptionsCleanup clean up" );


i = BuildOutputFilenameFromHeader( OutputFileName );
if ( i != 0 )
{
	if ( TagsRequired )
		ErrorExit( "Missing Tags, can not generate the output file name" );
	else
		strcpy( OutputFileName, "xronosfmt.out.tex" );
}

if ( strlen(OutputFileName) > XTL_MAX_FILENAME_SIZE )
	ErrorExit( "Generated OutputFileName is too long" );


printf( "Processed %d lines for BuildOutputHeader\n", LineNo );

}


/*************************************
*  allow access to xronosfmt option values
*************************************/
char *XronosfmtOptionGet( char *name )
{
	XronosOption *ctxt;

	ctxt = FindXronosfmtOption( name );
	if ( ctxt != NULL && strlen( ctxt->OptionValue ) > 0 ) /* got a valid one */
	{
		return ctxt->OptionValue;
	}
	
return NULL;
}

