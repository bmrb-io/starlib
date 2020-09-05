#include "transforms.h"
///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.7  2000/11/18 03:02:36  madings
*** empty log message ***

// Revision 1.6  1999/07/30  18:22:06  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.5  1999/02/05 01:34:43  madings
Many changes from debug sessions with Eldon:
1 - added new transforms:  transform15.cc and transform16.cc
2 - added 'don't-care' functionality to transform14.cc when
    tag_value is a dot.
3 - Fixed some garbled errStream messages that printed the wrong
    transform # to the log (transform11 printed error messages tagged
    as transform6 for example.)

// Revision 1.4  1999/01/28  06:15:04  madings
// Some bugfixes from Eldon's attempts to use this, as well as a major
// design change:
//
//    Bug fix 1: save_loops was re-creating tags that had been removed
//       earlier in the run.  This has been fixed.
//
//    Bug fix x: after bug fix 1 above, save_loops was leaving behind
//       loops that had no contents.  This has been fixed.
//
//    Massive design change:
//       There is no longer any default order of operations at all.
//       Instead, it uses the new 'save_order_of_operations' saveframe
//       in the mapping dictionary file to decide what rules to run
//       when.  Also, to allow it to have the same rule run more than
//       once at different times from different mapping dictionary
//       saveframes, the name of the saveframe is no longer the rule
//       to run.  Instead the name of the saveframe is irrelevant, and
//       it's the new _Saveframe_category tag that tells the program
//       what rule to run for that saveframe.  This required a small,
//       but repetative change in all the transform rules so that the
//       main.cc program can tell the transforms which saveframe in the
//       mapping file is the one they are supposed to use instead of the
//       transforms trying to look for it themselves.
//
// Revision 1.3  1999/01/27  00:26:50  madings
// Fixed a bug in save_loops that was causing deleted tags to be recreated
// when making a loop.  Also re-arranged the order of operations and adjusted
// the documentation to match this.
//
// Revision 1.2  1998/11/11  04:08:16  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.3  1997/10/21  21:37:23  bsri
//  The function convertToFramecode() was changed to suit to the changes
//  made to the structure of DataValueNode.
//
// Revision 1.2  1997/10/01  22:38:55  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////
//	Auxiliary Transformation Functions
//////////////////////////////////////////////

//////////////////////////
//  tagToVal()
/////////////////////////
string tagToVal(string value)
{
  int length = value.length();
  char *strBuf = (char*)calloc( length +1, sizeof( char ) );
  char *tmpString = strBuf;
  char *startPtr, *endPtr;  // ptr to a new part of tmpString

  strcpy(tmpString, value.c_str() );

  if(   (tmpString[0] =='"' && tmpString[length-1]=='"')
	||
	(tmpString[0] =='\'' && tmpString[length-1]=='\'')   )
    {
      tmpString[length-1] = '\0';
      tmpString = tmpString + 1;
   
    }
  else if(   tmpString[0] == ';'  )
  {
      // Strip the first ';' by moving the str ptr forward.
      startPtr = strchr( tmpString, ';' );
      if( startPtr != NULL )
      {   startPtr++;
	  if( *(startPtr) == '\n' )
	      startPtr++;
      }

      // Trunc the string by cutting off the last ';':
      if( ( endPtr = strrchr( tmpString, ';' ) ) != NULL )
	  *endPtr = '\0';

      if( startPtr != NULL )
      {
	  string retVal(startPtr);
	  free( strBuf );
	  return retVal;
      }
      
  }
  string retVal = string(strBuf);
  free( strBuf );
  return retVal;
}



string removeFirstChar(string value)
{

  int length = value.length();
  char *strBuf = (char*)calloc( length +1, sizeof( char ) );
  char *tmpString = strBuf;
  char *tmpStrPtr = &(tmpString[0]);

  strcpy(tmpStrPtr, value.c_str() );
  tmpString = tmpString + 1;

  string retVal = string(tmpString);
  free( strBuf );
  return retVal;
}



string convertToFramecode(string value)
{

  //value = string("$") + tagToVal(value);
  value = tagToVal(value);
  int length = value.length();
  char *strBuf = (char*)calloc( length +1, sizeof( char ) );
  char *tmpString = strBuf;
  strcpy(tmpString,value.c_str() );
  for(int i=0; tmpString[i] != '\0'; i++)
    if(tmpString[i] == ' ' || tmpString[i] == '\t' || tmpString[i] == '\n' )
      tmpString[i] = '_';

  string retVal = string(strBuf);
  free( strBuf );
  return retVal;
}

// Turn whitespace in the string into underscores:
// ===============================================
string deSpacify( string value )
{
    int length = value.length();
    char *strBuf = (char*)calloc( length +1, sizeof( char ) );
    char *tmpString = strBuf;
    strcpy( tmpString, value.c_str() );
    for(int i=0; tmpString[i] != '\0'; i++)
	if(tmpString[i] == ' ' || tmpString[i] == '\t'  || tmpString[i] == '\n' )
	{   tmpString[i] = '_';
	}

    string retVal = string( strBuf );
    free( strBuf );
    return retVal;
}


  ///////////////////////////////
 // SplitStringByDelimiter()
///////////////////////////////
void SplitStringByDelimiter(
	string        splitMe, // The string to split
	string        delim,   // The delimiter string
	List<string>* valList  // The list to store the results in.
			       // (Must be preinitialized as an empty
			       // list before calling.)
	)
{
    int         subStart = 0;
    int         subEnd = 0;

    valList->FreeList();
    while( subStart >= 0 && subStart < splitMe.length() )
    {
	string      *Str;
	subEnd = splitMe.find( delim, subStart );
	if( subStart < 0 || subStart >= splitMe.length() )
	    subEnd = splitMe.length(); // Not found, so go to end of string.
	Str = new string( splitMe, subStart, (subEnd-subStart) );
	valList->AddToEnd( *Str );
	delete Str;
	subStart = subEnd + delim.length();
    }

    return;

}//end-SplitStringByDelimiter()

  ///////////////////////////////
 // SplitStringByWidth()      //
///////////////////////////////
void SplitStringByWidth(
	string splitMe,          // The string to split
	int    width,            // The width to split on.
	List<string>* valList    // The list to store the results in.
	                         // Must start out as an empty list );
	)
{
    int         subStart;
    int         totLength = splitMe.length();

    valList->FreeList();
    for( subStart = 0 ; subStart < totLength ; subStart += width )
    {
	string      *Str;
	Str = new string( splitMe, subStart, width );
	valList->AddToEnd( *Str );
	delete Str;
    }

    return;
} //end-SplitStringByWidth()

  //////////////////////////////////
 //  FindNeededQuoteStyle()
//////////////////////////////////
char FindNeededQuoteStyle( string searchMe )
{
    int       length      = searchMe.length();
    bool      has_leading_underscore = false;
    bool      has_squote  = false;
    bool      has_dquote  = false;
    bool      has_wspace  = false;
    bool      has_newline = false;
    bool      is_null_length = false;
    char*     tmpString   = new char[length+1];
    char      retVal      = ' ';   // return value.  assume 'space'
                                   // by default.

    strcpy(tmpString, searchMe.c_str() );

    if( strlen(tmpString) == 0 ) {
        is_null_length = true;
    } else {
	if( tmpString[0] == '_' )
	    has_leading_underscore = true;
	if( strchr( tmpString, '\'' ) )
	    has_squote = true;
	if( strchr( tmpString, '\"' ) )
	    has_dquote = true;
	if( strchr( tmpString, ' ' ) || strchr( tmpString, '\t' ) )
	    has_wspace = true;
	if( strchr( tmpString, '\n' ) )
	    has_newline = true;
    }


    if( has_newline || (has_dquote && has_squote) )
    {   retVal = ';'; }
    else if( has_dquote )
    {   retVal = '\''; }
    else if( has_squote || has_wspace || is_null_length )
    {   retVal = '\"'; }
    else if( has_leading_underscore ) // if it is a tagname, then quote if it its not already quoted.
    {   retVal = '\"'; }
    else
    {   retVal = ' ' ; }

    return retVal;

} //end-FindNeededQuoteStyle()

  /////////////////////////////////////
 //   SearchAndReplace()
/////////////////////////////////////
void SearchAndReplace(
	string &changeMe,
	const string &findMe,
	const string &putMe
	)
{
    int  foundStart = 0, foundLength = findMe.length();

    while( foundStart >= 0 )
    {
	foundStart = changeMe.find( findMe, foundStart );
	if( foundStart >=0  )
	{
	    // Ditch the old string:
	    erase_str(changeMe)( foundStart, foundLength );
	    // Insert the new string in its place:
	    changeMe.insert( foundStart, putMe );
	    // Skip over the replaced string, to avoid infinite
	    // recursion if putMe just so happened to have
	    // findMe inside of it:
	    foundStart+= foundLength;
	}
    }
} //end-SearchAndReplace()

  /////////////////////////////////////
 //    InsertNeededQuoteStyle()
//////////////////////////////////////
bool InsertNeededQuoteStyle( string  &changeMe )
{
    char    quoteType = FindNeededQuoteStyle( changeMe );
    string  beforeString, afterString;  // stuff to prepend/append to string.

    if( quoteType == '\'' )
    {   beforeString = afterString = "\'";
    }
    else if( quoteType == '\"' )
    {   beforeString = afterString = "\"";
    }
    else if( quoteType == ';' )
    {   beforeString = "\n;\n";
        afterString  = "\n;";
    }
    else
    {   beforeString = afterString = "";
    }

    changeMe = beforeString + changeMe + afterString;

    if( beforeString != "" )  // If something changed.
	return true;
    else
	return false;

} //end-InsertNeededQuoteStyle()

  ////////////////////////////////
 //  ListAllTagsInDataNode()   //
////////////////////////////////
void ListAllTagsInDataNode( DataNode &node, ostream &dumpHere, string msgSoFar )
{
    ASTlist <DataNode*>*nodeList;
    List <DataNameNode*>*nameList;
    List <DataValueNode*>*valList;

    switch( node.myType() )
    {
	// Terminal case: A tag was found:
	case DataNode::DATAITEMNODE:
	    dumpHere << msgSoFar << node.myName() << endl;
	    break;

	case DataNode::DATALOOPNODE:
	    msgSoFar += "in loop: ";
	    // /////////////////////////////////////////////////////
	    // Warning - does not work correctly on nested loops:
	    // /////////////////////////////////////////////////////
	    ((DataLoopNode*)&node)->FlattenNestedLoop( nameList, valList );
	    for( nameList->Reset() ; ! nameList->AtEnd() ; nameList->Next() )
	        dumpHere << msgSoFar << nameList->Current()->myName() << endl;
	    break;

	// For save frames, just recursively call myself on each datanode in the list.
	case DataNode::SAVEFRAMENODE:
	    msgSoFar += "in saveframe ";
	    msgSoFar += node.myName();
	    msgSoFar += ": ";
	    nodeList = ((SaveFrameNode*)&node)->GiveMyDataList();
	    for(  nodeList->Reset() ; ! nodeList->AtEnd() ; nodeList->Next()  )
		ListAllTagsInDataNode( *(nodeList->Current()), dumpHere, msgSoFar );
	    break;

        // Dummy default to get gcc -Wall to shut up:
	default: 
	    break;
    } //End-switch
}
