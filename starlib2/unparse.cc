/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//  $Log: unparse.cc,v $
//  Revision 1.15  2010/09/24 21:28:07  madings
//  *** empty log message ***
//
//  Revision 1.14  2010/07/21 20:40:36  madings
//  *** empty log message ***
//
//  Revision 1.13  2010/02/25 20:40:03  testadit
//  *** empty log message ***
//
//  Revision 1.11  2010/01/30 00:51:13  testadit
//  *** empty log message ***
//
//  Revision 1.10  2010/01/30 00:42:52  testadit
//  *** empty log message ***
//
//  Revision 1.9  2008/08/14 18:46:33  madings
//  *** empty log message ***
//
//  Revision 1.8  2007/09/13 21:43:51  madings
//  Finally fixed the line number count being off with comments.
//  (It used to be the case that comment lines got miscounted, such
//  that a string of comments would throw off the line numbers on all
//  subsequent lines (the line count was calculated as if the comment was
//  only one line long no matter how many lines it really was.))
//
//  Revision 1.7  2006/03/28 17:48:37  madings
//  Fixed bug where the new spacing trim would fail if you never
//  called the routine to set the spacing (in other words, the
//  default setting was bad).
//
//  Revision 1.6  2006/03/23 06:20:20  madings
//  *** empty log message ***
//
//  Revision 1.5  2005/11/07 23:30:30  madings
//  *** empty log message ***
//
//  Revision 1.4  2002/09/27 22:50:00  madings
//  Changes to accomodate newer versions of g++, that are a lot
//  less forgiving about standards violations than earlier
//  versions were.  Specifically what changed is in this list:
//     - Added "typename" keyword to some typedefs.
//     - Added some namespace specifiers (std::).
//     - No longer relies on the "black box" violation of assuming that
//         vector iterators are really pointers (they are in every
//         implementation I've encountered, but supposedly that's
//         not guaranteed by the standard, and the newer g++'s will
//         enforce this.)
//
//  Revision 1.3  2001/12/04 20:58:20  madings
//  Major changes  - see doc/changes.html for details:
//     - setstring / getstring interface at the  loop level
//     - implement #<START-SKIP> ... #<END-SKIP> directives
//     - fix compiler errors related to use of iterators and const
//          on newer versions of gcc.
//     - ValCache usage, with LoopRows containing one string for the
//          whole row instead of seperate data value nodes.
//     - change the type of the *os file stream because the ability to
//          construct from a unix fileno() handle has been depreciated in
//  	newer C++ stream libraries.
//
//  Revision 1.2  2001/11/13 20:30:36  madings
//  This update includes several changes:
//  (See doc/changes.html for the full explanation)
//     1- The previous compilers we had been using had allowed us to get
//         away with some mis-use of the const specifier without flagging
//         it as a warning or error.  That is no longer the case, so we had
//         to alter some of the code's use of "const".
//     2- The STL no longer allows vector iterators to be interchangable
//         with pointers, so we lost the ability to use that feature for
//         users of the library.
//     3- The streams library no longer allows you to make an ofstream out of
//         a pipe or standard output.  Callers of this method must now make
//         'os' be a pointer to an ostream instead of ofstream.
//     4- The newer compilers no longer allow a reference to a temporary store
//         that was created just for the duration of the function call.
//
// Revision 1.1.1.1  2001/04/17  10:00:42  madings
//
// Revision 2.6  2000/10/24  21:40:07  madings
// Previous check-in would not work under Linux because g++ and CC
// disagree on what the default is when public: or private: has not been
// explicitly specified.  This version makes it more explicit.
//
// Revision 2.5  1999/10/18  23:00:20  madings
// Added formatNMRSTAR.cc for the first time.
//
// Revision 2.4  1999/08/26  06:44:17  madings
// More changes to attempt to trim the memory footprint of this
// program down:  Added the ability to compile the code with
// some of the line-number and column-number information turned off
// in DataValueNodes.  It didn't seem to help any.
//
// Revision 2.3  1999/07/02  22:14:45  informix
// Removed dumb code in unparse.cc that for some reason would refuse to indent
// the first value in a loop if the loop was linear and had only 1 column.
// I have no idea what the rationale for that check was supposed to be,
// but it doesn't make any sense.
//
// Revision 2.2  1999/04/28  02:29:02  madings
// Made the searchByTag and searchByTagValue functions case-insensitive
// for tag-names, datablock names, and saveframe names (but not data values).
//
// Revision 2.1  1999/03/19  00:59:01  madings
// Added setTabFlag/getTabFlag functions to the loops, and altered the parser
// so it remembers the tabulation format and preserves it.
//
// Revision 2.0  1999/01/23  03:34:37  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
//
// Revision 1.9  1998/12/21  23:16:24  madings
// Minor formatting modification:  I put an empty line after loop_
// and I shouldn't have, so I took it back out.
//
// Revision 1.8  1998/12/07  22:53:52  madings
// Checking in multiple error finder for the time being.
// Not done yet but someone needs to check out what I have
// so far...
//
// Revision 1.7  1998/11/30  22:39:43  madings
// Modified the spacing in the unparse of saveframes and loops (yet again).
//
// Revision 1.6  1998/11/20  06:46:44  madings
// Bugfixing.
//
// Revision 1.5  1998/08/29  08:46:12  madings
// This one was a rather large change.  I haven't finished
// documenting and testing all of it, so expect another
// revision or two soon.
//
// This revision creates a new way of dealing with loops.  Now
// the caller can use the loops like 2-D vectors, but with some
// extra magic behind the scenes to ensure that no insertions or
// deletions that can leave the tree in an invalid state will
// be allowed.  (For example, you cannot remove a value from the
// loop without also removing the tagname for it and all the other
// values in that column.)
//
// Also in this revision is a fix to the previous revision's method
// of making List and ASTlist be wrappers around STL vector.  The
// previous version of the library broke when the STL library decided
// to move the vector to a new location in memory (realloc), and the
// myCurrent interator became invalid.  Now myCurrent is an integer
// index offset instead, so it moves with the vector.
//
// Also in this revision are some fixes to a longstanding problem that
// made the library incapable of parsing nested loops unless the nested
// loops were fully populated.  (In other words if an inner loop for any
// row was null, because the input file simply has a "stop_" and no
// data for the inner loop, it used to crash on that.)  Now it works and
// empty inner loops are represented to the caller as null pointers (so
// don't blindly follow inner loop pointers that are handed to you - check
// them first.)
//
// Note that this marks my first attempt to make a class that implements
// the STL vector interface, so I expect to find some bugs to fix in the
// next revision or two.  There's some strange C++ "magic" going on here
// to get this to work.
//
// Revision 1.4  1998/08/14  00:01:19  madings
// Added ability to define which loops are to be output many-rows to the
// line, and the ability to specifcy that a loop's values should be printed
// up against the margin and not indented.
//
// Revision 1.3  1998/07/17  01:41:34  madings
// Two changes related to Unparse() this revision:
//   1 - Added the ability to programtically insert comments with
//           the get/setPreComment functions.
//   2 - Make Unparse() somewhat more sophisticated to handle formatting
// 	  values so they line up with each other.
//
// Revision 1.2  1998/05/08  20:27:26  informix
// Made a change to handle the C++ STL instead of gcc's libg++.
// Now all strings are of type 'string' instead of 'String' (notice
// the uppercase/lowercase change.)  Applications using this library
// will have to change also, unfortunately, to use the new string style.
//
// Revision 1.1.1.1  1998/02/12  20:59:00  informix
// re-creation of starlib project, without sb_lib stuff.  sb_lib has
// been moved to it's own project.
//
// Revision 1.3  1998/02/06  01:23:54  madings
// fixed the fix.  (the loop_ was not justified with the indent)
//
// Revision 1.2  1998/02/06  01:14:57  madings
// Changed the unparsing to put a blank line before each loop_ .
//
// Revision 1.1.1.1  1997/11/07  01:49:07  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.3  1997/10/10  18:34:32  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.2  1997/09/15  22:28:31  madings
// Split list.h into list.h and astlist.h.  DId the same with list.cc
//
// Revision 1.1.1.1  1997/09/12  22:54:41  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.5  1997/07/11  22:40:08  madings
//Finally seems to work - just waiting for
//new dictionaries.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:58:00  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:47  madings
//I Added the RCS log and ID variables.
//
//
////////////////////////////////////////////////////////////////////////
#include "ast.h"
#include "list.h"
#include "astlist.h"
#include "portability.h"
#include <fstream>
#include <iostream>
#include <iomanip>


extern ostream *os;

int   g_columnPadSpaces = 1;

const string INDENT = "   ";		//indentation string

StarFileNode *g_root = NULL;
int  prevCheckLineNum = 0;

#ifdef DEBUG
static char debugTypeName[][50] =
{
	"ASTNODE",
	"BLOCKNODE",
	"DATABLOCKNODE",
	"DATAFILENODE",
	"DATAHEADINGNODE",
	"DATAITEMNODE",
	"DATALISTNODE",
	"DATALOOPNAMELISTNODE",
	"DATALOOPNODE",
	"DATALOOPVALLISTNODE",
	"DATANAMENODE",
	"DATANODE",
	"DATAVALUENODE",
	"GLOBALBLOCKNODE",
	"GLOBALHEADINGNODE",
	"HEADINGNODE",
	"LOOPROWNODE",
	"LOOPNAMELISTNODE",
	"LOOPTABLENODE",
	"LOOPVALLISTNODE",
	"SAVEFRAMELISTNODE",
	"SAVEFRAMENODE",
	"SAVEHEADINGNODE",
	"STARLISTNODE",
	"STARFILENODE"
};
#endif
////////////////////////
//  isItNumeric()     //
////////////////////////
// A boolean check - true if the string contains nothing but a
// valid numeric string, either as an integer or as
// a floating point (does not handle exponential notation like
// "1.2345e-43", though - but for what we are doing, this is fine.
// We don't really want to format those like the rest of the numbers
// anyway.)
bool isItNumeric( string str )
{
    int  idx;

    // This code simulates the following 'grep' regular expression:
    //     a numeric string is :  [ \t\n]*[+-]?[0-9]*\.?[0-9]*[ \t\n]*

    idx = 0;
    while( idx < str.length() && isspace(str[idx]) )               // [ \t\n]*
	idx++;
    while( idx < str.length() && isspace(str[idx]) )               // [ \t\n]*
	idx++;
    if( idx < str.length() && str[idx] == '+' || str[idx] == '-' ) // [+-]?
	idx++;
    while( idx < str.length() && isdigit(str[idx]) )               // [0-9]*
	idx++;
    if( idx < str.length() && str[idx] == '.' )                    // \.?
	idx++;
    while( idx < str.length() && isdigit(str[idx]) )               // [0-9]*
	idx++;
    while( idx < str.length() && isspace(str[idx]) )               // [ \t\n]*
	idx++;

    // If it is truly a good numeric expression, the above code
    // should have gotten us to the end of the string.  If it did
    // not, then this is not a number.  (notice that this definition
    // counts nullstrings and the single dot '.' as numerics, but
    // for our purposes, this is okay:
    if( idx == str.length() )
	return true;
    else
	return false;
}

void setColumnPadSpaces( int columnPadSpaces )
{
   g_columnPadSpaces = columnPadSpaces;
}


void setNMRSTARFormat( StarFileNode *commentMap, StarFileNode *stfn )
{
    // TODO - lift code from validate_server and put it here.
}

void ASTnode::printIndent(int indent)
{
  for (int i = 0; i < indent; i++)
    (*os) << INDENT;
}

void printSkipped( int thisLineNum )
{
    string printMe;
    int    skipLineNum;

    if( g_root )
    {
	printMe = g_root->getSkipTextBetween( prevCheckLineNum+1,
	                                      thisLineNum, skipLineNum );
	if( printMe.length() > 0 )
	{   
#ifdef DEBUG
	    cerr << "unparse: ENCOUNTERED TEXT BLOCK Between "
		<< prevCheckLineNum+1 << " and " << thisLineNum
		<< " (line " <<  skipLineNum << ")" << endl;
#endif
	    (*os) << endl << printMe;
	    prevCheckLineNum = skipLineNum;
	}
	else
	    // This is necessary because some nodes in the tree might
	    // never have had their line numbers filled in if they
	    // were added after parsing, and we don't want the prev
	    // line number to reset to zero when it encounters such
	    // a thing:
	    if( thisLineNum > 0 )
		prevCheckLineNum = thisLineNum;
    }
}

void ASTnode::printComment( int indent )
{
    char ch;
    int  idx;

    // Don't print if preComment is a nullstring:
    string cmt = getPreComment();
    if( cmt.length() > 0  )
    {
	idx = 0;
	while( (ch = cmt.c_str()[idx++]) != '\0' )
	{
	    (*os) << ch;
	    if( ch == '\n' )  // Indent each new line
	    {
		printIndent( indent );
	    }
	}
	(*os) << '\n';
        printIndent( indent );
    }
}


void StarFileNode::Unparse(int indent)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  g_root = this;
  prevCheckLineNum = this->getLineNum();
  printSkipped( this->getLineNum() );
  printComment( indent );
  myStarList->Unparse(indent);

  g_root = (StarFileNode*) NULL;
  prevCheckLineNum = -1;

#ifdef DEBUGSIZEOF
  cerr << "sizeof(StarFileNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

//****************************************************
//Unparse function to print the kids of StarListNode
//****************************************************
void StarListNode::Unparse(int indent)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  myKids->Reset();
  while (!myKids->AtEnd()) {
    myKids->Current()->Unparse(indent);
    myKids->Next();
  }
#ifdef DEBUGSIZEOF
  cerr << "sizeof(StarListNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void HeadingNode::Unparse(int indent)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  printIndent(indent);
  (*os) << myName() << endl;
#ifdef DEBUGSIZEOF
  cerr << "sizeof(HeadingNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void BlockNode::Unparse(int indent)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  myHeading->Unparse(indent);
  myDataList->Unparse(indent + 1);
#ifdef DEBUGSIZEOF
  cerr << "sizeof(BlockNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void DataListNode::Unparse(int indent)
{
  int  longest;
  ASTnode::ASTtype  prevType, curType;

#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  printSkipped( this->getLineNum() );

  prevType = ASTnode::DATAITEMNODE;
  longest = myLongestStr();
  myKids->Reset();
  while (!myKids->AtEnd()) {
    curType = myKids->Current()->myType();
    if( curType  == ASTnode::DATALOOPNODE  &&
	prevType != ASTnode::DATALOOPNODE  )
    {
	(*os) << endl;
    }
    prevType = curType;
    myKids->Current()->Unparse(indent, longest);
    myKids->Next();
  }
#ifdef DEBUGSIZEOF
  cerr << "sizeof(DataListNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void DataItemNode::Unparse(int indent, int padsize)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  if( padsize > 0 )
  {
     myDataName->Unparse(indent, padsize);
  }
  else
  {
      myDataName->Unparse(indent);
  }
  (*os) << "  ";
  myDataValue->Unparse(indent);
  (*os) << endl;

  // This is a kluge for something used in NMRstar files:
  // One future possible improvement could be to make Unparse()
  // be capable of being configured by the caller to change its
  // pretty-printing style.  Right now no such feature exists,
  // so the output that NMR-star needs is the output that it produces:
  if( myDataName->myName() == "_Saveframe_category" )
      (*os) << endl;
#ifdef DEBUGSIZEOF
  cerr << "sizeof(DataItemNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void DataValueNode::Unparse(int indent, int presize, int postsize, bool allNonQuoted)
{
    string tmpStr;
    int    dotIdx;
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
    printSkipped( this->getLineNum() );
    printComment( indent );
    // for debugging line num calculation: (*os)<<"("<<this->getLineNum()<<":"<<this->getColNum()<<":";

    switch( bitFields.delimType )
    {
	// Note: We must use the C-String version of these in order to
	// have the stream formatting work because the string library
	// is not very good that came with this compiler and it doesn't
	// behave with iostream very nicely.
	case NON:
	    if( ! allNonQuoted )
	    {
		(*os) << " " ;  // nonquoted values get an extra space to line
				// up better with quoted values, when there's
				// a mixture of both in the column.
	    }
	    if( postsize >= 0 && isItNumeric( myStrVal ) )
	    {
		tmpStr = "";
		// Find the dot:
		for(    dotIdx = 0 ;
			dotIdx < myStrVal.length() && myStrVal[dotIdx] != '.' ;
			dotIdx++    )
		{   tmpStr+= myStrVal[dotIdx];
		}
		// Print the stuff up to but not including the dot
		(*os) << resetiosflags(ios::left)
		      << setiosflags(ios::right)
		      << setw(presize-1)  // -1 to compensate for extra space
		                          // that all NONquoted values get.
		      << tmpStr.c_str()
		      << resetiosflags(ios::right)
		      << setiosflags(ios::left) ;

		// If anything still left, print the dot and the fractional
		if( dotIdx < myStrVal.length() )
		{
		    tmpStr = "";
		    for( /*void-init*/ ; dotIdx < myStrVal.length() ; dotIdx++ )
		    {   tmpStr+= myStrVal[dotIdx];
		    }
		    (*os) << setiosflags(ios::left) << setw(postsize+1) <<
			tmpStr.c_str();
		}
		else // This value has no fractional part, but others in this
		     // column do, so pad with spaces to match it up.
		{
		    if( postsize > 0 )
			(*os) << setw(postsize) << " ";
		    if( postsize >= 1 )  // also another space for the dot
			(*os) << " ";
		}
	    }
	    else  // This is string - format left justified normally:
	    {
		// format to presize - 1 (minus-one is to compensate for
		// the space that is printed before each NONquoted value:)
		if( presize > 0 )
		    (*os) << setiosflags(ios::left) << setw(presize-1);
		(*os) << myStrVal.c_str();

		// If other values in this column were numeric with
		// stuff after the decimal place, then space out to
		// match up with those other values:
		if( postsize >= 0 )
		    (*os) << setw(postsize) << " ";
		if( postsize >=1 ) // one more space for the decimal point
		    (*os) << " ";
	    }
	    break;
	case SINGLE:
	    tmpStr = "'";
	    tmpStr += myStrVal;
	    tmpStr += "'";
	    if( presize > 0 )
		(*os) << setiosflags(ios::left) << setw(presize);
	    (*os) << tmpStr.c_str();
	    break;
	case DOUBLE:
	    tmpStr = "\"";
	    tmpStr += myStrVal;
	    tmpStr += "\"";
	    if( presize > 0 )
		(*os) << setiosflags(ios::left) << setw(presize);
	    (*os) << tmpStr.c_str();
	    break;
        case SEMICOLON:
	    (*os) << "\n;" ;

	    // Change: only output a blank line if the value does not
	    // start with a semicolon.  If the value starts with a 
	    // semicolon, then breaking it onto another line would
	    // terminate the value the next time it's parsed:
	    // i.e.:
	    //  ;;;;value
	    //  ;
	    //  gets turned into:
	    //  ;
	    //  ;;;value
	    //  ;
	    //  which is wrong.
	    if( myStrVal.length() > 0 && myStrVal[0] == ';' )
	        (*os) << "" ; // output nothing if the value starts with ';' 
	    else 
	        (*os) << "\n" ; // output eoln before the value.
	    (*os) << myStrVal.c_str();

	    // If it doesn't end with a newline, print one anyway
	    // to ensure the delimiting semicolon ends up at the
	    // start of a line (not at the end of the last line of
	    // the string value):
	    if( myStrVal.c_str()[ strlen( myStrVal.c_str() ) - 1 ] != '\n' )
		(*os) << "\n" ;

	    (*os) << ";" ;
	    break;
	case FRAMECODE:
	    tmpStr = "$";
	    tmpStr += myStrVal;
	    if( presize > 0 )
		(*os) << setiosflags(ios::left) << setw(presize);
	    (*os) << tmpStr.c_str();
	    break;
    }
    // If this was not a nonquoted value, but OTHER values in this
    // column were numeric nonquoted values, then space out to
    // match up with to the "postsize" of the other values in this
    // column (plus 1 for the dot):
    if( bitFields.delimType != NON  &&  postsize >= 0)
    {
	(*os) << setw(postsize) << " ";
	if( postsize >= 1 )
	    (*os) << " "; // one more space for the decimal point
    }
    // for debugging line num calculation: (*os)<<")";
#ifdef DEBUGSIZEOF
  cerr << "sizeof(DataValueNode)"<<" = " << sizeof(*this)<<endl;
#endif

}

void DataNameNode::Unparse(int indent, int padsize)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  printIndent(indent);
  // This string class doesn't behave nice with stream, so we have to
  // use it as a C-String instead to get the following formatting to work:
  (*os) << setiosflags( ios::left) << setw(padsize) << myStrVal.c_str();

#ifdef DEBUGSIZEOF
  cerr << "sizeof(DataNameNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void LoopNameListNode::Unparse(int indent)
{   Unparse( indent, false );
}
void LoopNameListNode::Unparse(int indent, bool isEmpty)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  if( isEmpty )
  {  (*os) <<"#### ";
  }
  printComment( indent );
  printIndent(indent);
  (*os) << "loop_" << endl;

  for (myKids->Reset(); !myKids->AtEnd(); myKids->Next())
  {
    if( isEmpty )
    {  (*os) <<"#### ";
    }
    myKids->Current()->Unparse(indent + 1);
    if( myKids->Current()->myName() == "_Saveframe_category" )
    {
	(*os) << endl;
    }
    (*os) << endl;
  }
#ifdef DEBUGSIZEOF
  cerr << "sizeof(LoopNameListNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void LoopValListNode::Unparse( int indent )
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
    printSkipped( this->getLineNum() );
    printComment( indent );
    return;  // do-nothing for now.
#ifdef DEBUGSIZEOF
  cerr << "sizeof(LoopValListNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void DataLoopNameListNode::Unparse(int indent)
{
    Unparse(indent, false);
}
void DataLoopNameListNode::Unparse(int indent, bool isEmpty)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  for (myKids->Reset(); !myKids->AtEnd(); myKids->Next())
    myKids->Current()->Unparse(indent++, isEmpty );
#ifdef DEBUGSIZEOF
  cerr << "sizeof(DataLoopNameListNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void DataLoopValListNode::Unparse( int indent)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
    printSkipped( this->getLineNum() );
    printComment( indent );
    return; // do-nothing for now.
#ifdef DEBUGSIZEOF
  cerr << "sizeof(DataLoopValListNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void LoopTableNode::Unparse(int indent)
{
  List<int>  *presizes;
  List<int>  *postsizes;
  List<int> *allNonQuoteds;
  int        rowcnt;
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << "LoopTableNode, this="  << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
   // It is possible for there to be a LoopTableNode with
   // nothing in it in the case of nested loops:
  if( size() == 0 )
      return;
  printSkipped( this->getLineNum() );
  printComment(  (bitFields.indFlag) ? indent : 0 );

  calcPrintSizes( &presizes, &postsizes, &allNonQuoteds );

  for(  myIters.Reset(), rowcnt = 0;
	!myIters.AtEnd();
	myIters.Next(), rowcnt++  )
  {
      if( rowcnt % rowsPerLine == 0  || rowcnt == 0 )
      {
	  (*os) << endl;
	  printIndent( bitFields.indFlag ? indent : 0 );
      }
      else
	  (*os) << " " ;  // extra space between rows on same line.
      myIters.Current()->Unparse(indent, presizes, postsizes, allNonQuoteds );
  }

  (*os) << endl;

  // If this is the outermost level of loop, pad an empty line:
  if(     myParent() != NULL && 
	  myParent()->isOfType( ASTnode::DATALOOPNODE )  )
  {
      if( this->size() == 0 )
      {   (*os) << "#### ";
      }
      (*os) << endl;
  }
  if( this->size() == 0 )
  {   (*os) << "#### ";
  }
  printIndent( (indent > 0 && bitFields.indFlag) ? (indent-1) : 0 ); // outdent "stop_"

  // If this is the outermost level of loop, pad an empty line:
  // and maybe don't print the "stop_"
  if(     myParent()!= NULL &&
	  myParent()->isOfType( ASTnode::DATALOOPNODE )  )
  {
      if( ((DataLoopNode*)myParent())->getShowStop() ) {
  	  (*os) << "stop_";
      } else { // still print a blank line so the count of lines is the same either way.
  	  (*os) << endl;
      }
      (*os) << endl;
  } else {
      (*os) << "stop_";
  }

  delete presizes;
  delete postsizes;
#ifdef DEBUGSIZEOF
  cerr << "sizeof(LoopTableNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void LoopRowNode::Unparse(
	int        indent,
	List<int>  *presizes,
	List<int>  *postsizes,
	List<int> *allNonQuoteds)
{
    short          col;
    DataValueNode  *dvn;

#ifdef DEBUG
    if( peer != NULL && peer->myParallelCopy() != this )
    {   cerr << "ERROR: Peer mismatch: ";  }
    cerr << "LoopRowNode, this= " << ":" << this << ":" << peer << ":" << parent << "\n";
    debugDump();
#endif
    printSkipped( this->getLineNum() );
    printComment( indent );

    if( presizes )
      presizes->Reset();
    if( postsizes )
      postsizes->Reset();
    if( allNonQuoteds )
      allNonQuoteds->Reset();
    for ( col = 0 ; col < myStarts.size() ; col++ )
    {
	dvn = generateVal( col );
	if( bitFields.tFlag )            // Tabulate
	{
	    // was: (*os) << INDENT;
	    if( presizes && postsizes )
	    {
		dvn->Unparse( indent, presizes->Current(), postsizes->Current(), allNonQuoteds->Current() );
		for( int numPad = 0 ; numPad < g_columnPadSpaces ; numPad++ )
		{ (*os) << " ";
		}
	    }
	    else  // no info given about column widths, so wing it:
	    {
		dvn->Unparse(indent);
		(*os) << "\t";
	    }
	}
	else                     // Linear
	{
	    if( presizes && postsizes )  // If column info given:
	    {
		dvn->Unparse( indent, presizes->Current(), postsizes->Current(), allNonQuoteds);
		(*os) << " ";
	    }
	    else // no info given about widths, so wing it:
	    {
		dvn->Unparse(indent);
	    }
	    (*os) << endl;
	    printIndent(indent);
	}
	delete dvn;
	if( presizes )
	    presizes->Next();
	if( postsizes )
	    postsizes->Next();
	if( allNonQuoteds )
	    allNonQuoteds->Next();
    }

    if( innerLoop != NULL )
	innerLoop->Unparse(indent + 1);
#ifdef DEBUGSIZEOF
    cerr << "sizeof(LoopRowNode)"<<" = " << sizeof(*this)<<endl;
#endif
}


void DataLoopNode::Unparse(int indent)
{
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
    cerr << "DataLoopNode, this= " << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  if( myValList.size() == 0 )
  {   (*os) << endl;
      (*os) << "#### ###################################" << endl;
      (*os) << "####  The starlib library has commented " << endl;
      (*os) << "####  ths loop out because it contains  " << endl;
      (*os) << "####  no data values, which is illegal  " << endl;
      (*os) << "####  syntax in STAR.                   " << endl;
      (*os) << "#### ###################################" << endl;
  }
  myDefList->Unparse(indent, (myValList.size() <= 0) );
  myValList.Unparse(indent + 1);
#ifdef DEBUGSIZEOF
  cerr << "sizeof(DataLoopNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void SaveFrameNode::Unparse(int indent, int padsize)
{
  if( indent > 0 )
      indent--;     // outdent saveframes one level.
                    // (This is not astheticly pleasing
		    // for other star files, but it is
		    // used for NMR-star files.)

  (*os) << endl;
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif
  printSkipped( this->getLineNum() );
  printComment( indent );
  myHeading->Unparse(indent);
  mySaveFrameList->Unparse(indent + 1);

  printIndent(indent);
  (*os) << endl << "save_" << endl << endl;
#ifdef DEBUGSIZEOF
  cerr << "sizeof(SaveFrameNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

void SaveFrameListNode::Unparse(int indent)
{
  int longest;
  ASTnode::ASTtype  prevType, curType;
#ifdef DEBUG
   if( peer != NULL && peer->myParallelCopy() != this )
   {   cerr << "ERROR: Peer mismatch: ";  }
   cerr << debugTypeName[(int)(myType())] << ":" << this << ":" << peer << ":" << parent << "\n";
#endif

  printSkipped( this->getLineNum() );
  printComment( indent );

  prevType = ASTnode::DATAITEMNODE;
  longest = myLongestStr();
  myKids->Reset();
  while (!myKids->AtEnd()) {
    curType = myKids->Current()->myType();
    if( curType  == ASTnode::DATALOOPNODE  &&
	prevType != ASTnode::DATALOOPNODE  )
    {
	(*os) << endl;
    }
    prevType = curType;
    myKids->Current()->Unparse(indent, longest);
    if( myKids->Current()->isOfType( ASTnode::DATALOOPNODE ) )
    {   myKids->Next();
	if( ! myKids->AtEnd() )
	    (*os) << endl;
    }
    else
    {   myKids->Next();
    }
  }
#ifdef DEBUGSIZEOF
  cerr << "sizeof(SaveFrameListNode)"<<" = " << sizeof(*this)<<endl;
#endif
}

