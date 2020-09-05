/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//  $Log: ast.cc,v $
//  Revision 1.14  2010/09/24 21:28:04  madings
//  *** empty log message ***
//
//  Revision 1.13  2010/03/31 21:23:21  madings
//  *** empty log message ***
//
//  Revision 1.12  2010/02/25 20:37:56  testadit
//  added ability to suppress the stop at the outermost level of the loop,
//  so the loop looks like how CIF likes it.
//
//  Revision 1.11  2010/01/30 00:47:30  testadit
//  *** empty log message ***
//
//  Revision 1.10  2010/01/30 00:28:43  testadit
//  added precomment ability to loop row nodes
//
//  Revision 1.9  2008/08/14 18:46:28  madings
//  *** empty log message ***
//
//  Revision 1.8  2006/03/23 06:20:19  madings
//  *** empty log message ***
//
//  Revision 1.7  2005/11/07 23:30:28  madings
//  *** empty log message ***
//
//  Revision 1.6  2003/02/28 03:16:27  madings
//  Fixed formatNMRSTAR that was failing after the
//  ability to store comments at any level of the
//  AST tree had been taken away.
//
//  Revision 1.5  2002/09/27 22:49:58  madings
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
//  Revision 1.4  2001/12/04 20:58:16  madings
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
//  Revision 1.3  2001/11/13 20:30:28  madings
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
//  Revision 1.2  2001/10/09 19:03:22  madings
//  Modified formatNMRSTAR to return exit codes for errors, and also
//  rolling up some minor changes to the source in terms of formatting.
//
// Revision 1.1.1.1  2001/04/17  10:00:39  madings
//
// Revision 2.9  2000/10/24  21:40:04  madings
// Previous check-in would not work under Linux because g++ and CC
// disagree on what the default is when public: or private: has not been
// explicitly specified.  This version makes it more explicit.
//
// Revision 2.8  2000/03/21  09:53:50  madings
// Bug fix: Some of the constructors for DataValueNode would fail
// to work if you passed in a zero-length string as the starting
// value.  (It would barf because it unconditionally assumed it
// was okay to look at myStrVal[0].)  This is fixed.
//
// Revision 2.7  1999/10/24  06:56:59  madings
// Fixed problem with line numbers being wrong in starlib.
// Bug was this: It was supposed to return the line/col number of the
// parent LoopRowNode when a DataValueNode->get{Line|Col}Num() call is
// made and the NO_POSBITS_DATAVALUENODE define was on.  But it didn't,
// it returned -1.  The problem was that the parsing in the YACC file
// lost the line number info while still building the list of DataValueNodes
// and by the time they got transform()ed into LoopRowNodes, the position
// info was already gone.  To fix this I made a new type "DVNWithPos" that
// is a DataValueNode that always has position info even when
// NO_POSBITS_DATAVALUENODE is defined.  This is the type that now temporarily
// stores the values before they are transformed into LoopRowNodes.  So the
// position info is not lost until after loop transformation now.
//
// Revision 2.6  1999/08/26  06:44:16  madings
// More changes to attempt to trim the memory footprint of this
// program down:  Added the ability to compile the code with
// some of the line-number and column-number information turned off
// in DataValueNodes.  It didn't seem to help any.
//
// Revision 2.5  1999/07/01  20:32:43  informix
// Fixed strCmpInsensitive().  Previously it falsely reported 'equal'
// results when the first string was equal to the second with stuff
// appended:  eg.  s1 = "stringbaseplusmore", s2 = "stringbase".  This
// mistake was because of a transposed greaterthan/lessthan.
//
// Revision 2.4  1999/04/28  02:28:59  madings
// Made the searchByTag and searchByTagValue functions case-insensitive
// for tag-names, datablock names, and saveframe names (but not data values).
//
// Revision 2.3  1999/03/19  00:59:00  madings
// Added setTabFlag/getTabFlag functions to the loops, and altered the parser
// so it remembers the tabulation format and preserves it.
//
// Revision 2.2  1999/03/04  20:32:36  madings
// Added the ability to find some warning-level errors.
// and made the new program called "star_syntax"
//
// Revision 2.1  1999/02/13  00:49:33  madings
// 2 bugfixes for formatting of loops:
//
//    1 - Single dots are now only decimal-point justified when the column
//        has some numbers in it.  If it is all non-numeric, the dot is left
//        justified.
//
//    2 - The length of a semicolon-delimited string is no longer used to
//        calculate the width of the column it is in - this was a bug
//        producing excessive space-padding (hundreds of spaces) for loops which
//        had a long semicolon delimited string in them.
//
// Revision 2.0  1999/01/23  03:34:31  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
//
// Revision 1.16  1999/01/22  23:58:27  madings
// Golly, where should I begin?  Okay, There was a port to linux being
// done in this version, which in turn meant a port to egcs instead of
// making code that can only compile on a CFront style compiler like SGI
// uses.  This port meant a lot of tiny changes all over the place to
// accomodate the differences in the way templates get linked.  Also,
// a bug was discovered in the way rows were deleted in the LoopTableNode's
// erase() operator.  The bug was there all along and it surfaced as part
// of the portation (what was just bad data in SGI was causing coredumps
// in Linux.)  Hopefully now that it compiles under egcs, it will be much
// more portable and I can begin getting it out to the public.
//
// Revision 1.15  1998/12/07  22:53:46  madings
// Checking in multiple error finder for the time being.
// Not done yet but someone needs to check out what I have
// so far...
//
// Revision 1.14  1998/11/20  06:46:42  madings
// Bugfixing.
//
// Revision 1.13  1998/10/08  03:45:29  madings
// Two changes:
//    1 - Most signifigant change is that the library now has
//    doc++ comments in it.
//    2 - Also, changed so that when using the copy constructor,
//    it remembers line numbers and column numbers in the newly
//    copied tree (I hope, this is untested.)
//
// Revision 1.12  1998/09/04  07:02:00  madings
// There were several bug fixes with this version.
//
// However, the erasing probably still needs some work.
//
// Revision 1.11  1998/08/29  08:45:55  madings
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
// Revision 1.10  1998/08/18  01:00:02  madings
// Got rid of the "astoper.cc" and "interface.cc" files and appended
// their contents to "ast.cc".  There was no modular reason for them
// to be seperate.
//
// Revision 1.9  1998/08/14  00:01:13  madings
// Added ability to define which loops are to be output many-rows to the
// line, and the ability to specifcy that a loop's values should be printed
// up against the margin and not indented.
//
// Revision 1.8  1998/07/17  01:41:25  madings
// Two changes related to Unparse() this revision:
//   1 - Added the ability to programtically insert comments with
//           the get/setPreComment functions.
//   2 - Make Unparse() somewhat more sophisticated to handle formatting
// 	  values so they line up with each other.
//
// Revision 1.7  1998/06/02  06:21:58  madings
// Made the yacc code that keeps track of lineNum/colNum a bit more
// sophisticated so that it shows the locations of the beginnings
// of the objects, not the ends of them.  Also gave the base class
// 'ASTnode' a copy constructor that will copy the increasing number
// of variables that are being stored in astnode, like the lineNum/colNum.
//
// Revision 1.6  1998/05/29  06:51:12  madings
// Added the ability to search for all objects of a particular AST type.
// (i.e. all SaveFrameNodes, or all DataItemNodes, etc...)
// See the method called "searchForType()", which was added to do this.
//
// Revision 1.5  1998/05/25  07:15:14  madings
// Added preliminary support for keeping track of line numbers and column
// numbers from the parse.  This way, programs reading the data can give
// the user actual line/column number references to things found in the
// STAR file.
//
// Revision 1.4  1998/05/21  22:20:44  madings
// Made some minor changes from string references to string-by-value
// in a few function templates.
//
// Revision 1.3  1998/05/08  20:27:23  informix
// Made a change to handle the C++ STL instead of gcc's libg++.
// Now all strings are of type 'string' instead of 'String' (notice
// the uppercase/lowercase change.)  Applications using this library
// will have to change also, unfortunately, to use the new string style.
//
// Revision 1.2  1998/02/21  01:12:41  madings
// changed the text of the "Error transforming loop" message to
// make more sense to a human.
//
// Revision 1.1.1.1  1998/02/12  20:59:00  informix
// re-creation of starlib project, without sb_lib stuff.  sb_lib has
// been moved to it's own project.
//
// Revision 1.1.1.1  1997/11/07  01:49:05  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.7  1997/10/25  02:59:24  madings
//  2 Things in this check-in:
//    -1- "Parallel copy".  Addded a new way to do a copy constructor
//        such that it will copy with a parallel link back to the
//        original tree it was copied from.  Any node in either tree
//        can refer to the corresponding node in the new tree.
//        See "parallel.html" for more detail.
//
//    -2- Bug fixes to copy constructors that did not properly deep-copy
//        all the way down into the LoopIter values.
//
// Revision 1.6  1997/10/10  18:34:19  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.5  1997/10/08  20:23:17  madings
// Added "searchForTypeByTag()" and "searchForTypeByTagValue()".
//
// (Also fixed bug where it would not find saveframes.)
//
// Revision 1.4  1997/10/01  22:35:02  bsri
// This version of starlib split main.cc into separate files (which contain
// the transformation functions - one in each file).
//
// Revision 1.2  1997/09/16  22:03:17  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.1.1.1  1997/09/12  22:54:39  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.6  1997/07/30  21:34:26  madings
//check-in before switching to cvs
//
//Revision 1.5  1997/07/11  22:40:03  madings
//Finally seems to work - just waiting for
//new dictionaries.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:57:57  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:43  madings
//I Added the RCS log and ID variables.
//
//
////////////////////////////////////////////////////////////////////////

#define IN_AST_CC

#ifdef IS_GCC
#   pragma implementation "ast.h"
#   pragma implementation "astnode.h"
#endif

#include <iostream>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "ast.h"

ASTnode::ASTnode( void )
{
    parent     = (ASTnode*)NULL;
    peer       = (ASTnode*)NULL;
    bitFields.isDeleting = false;
    bitFields.isCache = false;

#ifdef DEBUG_AST_CONST
    cerr << "DEBUG_AST_CONST: new object's ptr = " << (void*)this <<
	", type = " << myType() << endl;
#endif
}

ASTnode::ASTnode( const ASTnode &copyFromMe )
{
    copyFrom( copyFromMe );
}

void ASTnode::copyFrom( const ASTnode &copyFromMe )
{
    parent     = copyFromMe.parent;
    peer       = copyFromMe.peer;
    bitFields.isDeleting = copyFromMe.bitFields.isDeleting;
    bitFields.isCache = false;
    setLineNum( copyFromMe.getLineNum() );
    setColNum( copyFromMe.getColNum() );
}

void ASTnode::setPeer( const ASTnode *to_this )
{
    peer = UNDO_CONST_PTR(ASTnode, to_this );
}

void ASTnode::setParent( const ASTnode *p )
{
    parent = UNDO_CONST_PTR(ASTnode, p);

    // set to match the line number of the parent if it hasn't been
    // set yet:
    if( p != NULL && getLineNum() < 0 )
    {
	setLineNum( p->getLineNum() );
	setColNum( p->getColNum() );
    }
}

ASTnode * ASTnode::myParallelCopy( void )
{
    return peer;
}

bool ASTnode::getDeleteStatus( void )
{
    return bitFields.isDeleting;
}

List<ASTnode*> * ASTnode::searchByTag( const string & )
{
    // The default behavior if the coder forgets to override
    // this function in a derivative of ASTnode.  Hopefully
    // this never gets executed.  It is just here to get
    // it to compile without complaint for those derivatives
    // of ASTnode for which searchByTag() makes no sense.
    return new List<ASTnode*>;  // return an empty list.
}

List<ASTnode*> * ASTnode::searchByTag( const char *searchFor )
{
    // Just a wrapper around the 'real' searchByTag call,
    // which expects a string object.  Note that since
    // these are virtual functions, you never need to
    // redefine this function for any child of ASTnode.
    // this definition here should handle all uses of
    // the (char*) version of this function, for any
    // subclass of ASTnode.
    List<ASTnode*> *retVal;
    string  *searchString = new string(searchFor);

    retVal = searchByTag( *searchString );

    delete  (searchString);
    return retVal;
}

List<ASTnode*> * ASTnode::searchByTagValue( const string &, const string & )
{
    // The default behavior if the coder forgets to override
    // this function in a derivative of ASTnode.  Hopefully
    // this never gets executed.  It is just here to get
    // it to compile without complaint for those derivatives
    // of ASTnode for which searchByTagValue() makes no sense.
    return new List<ASTnode*> ;  // return an empty list
}

List<ASTnode*> * ASTnode::searchByTagValue( const string &tag,
				     const char   *value )
{
    // Just a wrapper around the first version of this function.
    // ---------------------------------------------------------
    List<ASTnode*> *retVal;
    string  *valueStr = new string(value);

    retVal = searchByTagValue( tag, *valueStr );

    delete  (valueStr);
    return retVal;
}

List<ASTnode*> * ASTnode::searchByTagValue( const char   *tag,
				     const string &value )
{
    // Just a wrapper around the first version of this function.
    // ---------------------------------------------------------
    List<ASTnode*> *retVal;
    string  *tagStr = new string(tag);

    retVal = searchByTagValue( *tagStr, value );

    delete  (tagStr);
    return retVal;
}

List<ASTnode*> * ASTnode::searchByTagValue( const char   *tag,
				      const char   *value )
{
    // Just a wrapper around the first version of this function.
    // ---------------------------------------------------------
    List<ASTnode*> *retVal;
    string  *tagStr = new string(tag);
    string  *valueStr = new string(value);

    retVal = searchByTagValue( *tagStr, *valueStr );

    delete  (valueStr);
    delete  (tagStr);
    return retVal;
}

List<ASTnode*> *ASTnode::searchForTypeByTag(
	  ASTtype  type,      // type to search for.
	  const string &tag         // tag to search for.
	  )
{
    List<ASTnode*> *retVal = searchByTag( tag );

    for( retVal->Reset() ; ! retVal->AtEnd() ; /*void*/ )
    {
	ASTnode *thisNode = retVal->Current();
	while( 	thisNode != (ASTnode*)NULL  &&  ! thisNode->isOfType( type ) )
	    thisNode = thisNode->myParent();
	retVal->MemorizeCurrent();
	if( thisNode == (ASTnode*)NULL )
	{   retVal->RemoveCurrent();
	    retVal->RestoreCurrent();
	}
	else
	{   retVal->AlterCurrent( thisNode );
	    retVal->Next();
	}
    }

    return retVal;
}

List<ASTnode*> *ASTnode::searchForTypeByTag(
	  ASTtype  type,      // type to search for.
	  const char *tag           // tag to search for.
	  )
{
    string tagStr( tag );
    return searchForTypeByTag( type, tagStr );
}

List<ASTnode*> *ASTnode::searchForTypeByTagValue(
	  ASTtype  type,   // type to search for.
	  const string   &tag,   // Look for this tag...
	  const string   &value  // Where it has this value.
	  )
{
    List<ASTnode*> *retVal = searchByTagValue( tag, value );

    for( retVal->Reset() ; ! retVal->AtEnd() ; /*void*/ )
    {
	ASTnode *thisNode = retVal->Current();
	while( 	thisNode != (ASTnode*)NULL  &&  ! thisNode->isOfType( type ) )
	    thisNode = thisNode->myParent();
	retVal->MemorizeCurrent();
	if( thisNode == (ASTnode*)NULL )
	{   retVal->RemoveCurrent();
	    retVal->RestoreCurrent();
	}
	else
	{   retVal->AlterCurrent( thisNode );
	    retVal->Next();
	}
    }

    return retVal;
}

List<ASTnode*> *ASTnode::searchForTypeByTagValue(
	  ASTtype  type,  // type to search for.
	  const string    &tag, // Look for this tag...
	  const char     *value // Where it has this value.
	  )
{
    string valStr( value );
    return searchForTypeByTagValue( type, tag, valStr );
}

List<ASTnode*> *ASTnode::searchForTypeByTagValue(
	  ASTtype  type,  // type to search for.
	  const char     *tag,  // Look for this tag...
	  const string   &value // Where it has this value.
	  )
{
    string tagStr( tag );
    return searchForTypeByTagValue( type, tagStr, value );
}

List<ASTnode*> *ASTnode::searchForTypeByTagValue(
	  ASTtype  type, // type to search for.
	  const char    *tag,  // Look for this tag...
	  const char    *value // Where it has this value.
	  )
{
    string tagStr( tag );
    string valStr( value );
    return searchForTypeByTagValue( type, tagStr, valStr );
}

List<ASTnode*> * ASTnode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*>  *retVal;

    delim;  // get the compiler to shut up about unused arguments.

    // The default behavior if the coder forgets to override
    // this function in a derivative of ASTnode.
    // 
    // The default behavior is to simply return the current
    // node if the type matches.   This is the default behavior
    // that should be appropriate for any leaf node in the tree.
    // To nodes that are not leaves, the algorithm for walking
    // through the children has to be customized for each type
    // of ASTnode.  Thus, only those ASTnode's that are capable
    // of being non-leaf nodes will need to override this function.
    // (All they have to do in that case is call the seachForType()
    // of each of their children.)
    //
    retVal = new List<ASTnode*>;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    return retVal ;
}

bool ASTnode::removeChild( ASTnode *child )
{
    if( unlinkChild( child ) )
    {   DELETE_IF_NOT_ALREADY( child );
	return true;
    }
    else
	return false;
}

bool ASTnode::unlinkChild( ASTnode * )
{
    return false;  // do nothing by default.  Needs to be overridden.
}

bool ASTnode::removeMe( void )
{
    if( myParent() != (ASTnode*)NULL )
	return myParent()->removeChild(this);
    else
	return false;
}

bool ASTnode::unlinkMe( void )
{
    if( myParent() != (ASTnode*)NULL )
    {
	// When the parent deliberately deletes a child, that child turns
	// around and tells the parent to unlink itself.  But if the
	// parent is already in its destructor, then this is a Bad Idea.
	if( myParent()->bitFields.isDeleting )
	{   return true;
	}
        else
	{   return myParent()->unlinkChild(this);
	}
    }
    else
    {   return false;
    }
}

ASTnode::~ASTnode( void )
{
#ifdef DEBUG_AST_DEST
    cerr << "DEBUG_AST_DEST: Removing following object: " << (void*)this <<
	", type = " << myType() << endl;
#endif
    bitFields.isDeleting = true;


    // Unlink me from my parallel peer if I am
    // about to die:
    ASTnode *otherOne = myParallelCopy();
    if( otherOne != NULL )
	otherOne->setPeer( NULL );
    bitFields.isDeleting = false;

    // Set some of my values to obviously bogus data to
    // help with debugging.  (If this node is used after
    // it was deleted (which does not reliably crash all the
    // time), it will be possible to tell in a debugger
    // that the data has been run through this destructor
    // once already.)
    peer = (ASTnode*)0xdeadface;
    parent = (ASTnode*)0xdeadface;
}

HeadingNode::HeadingNode(HeadingNode & h)
{
    (void)copyFrom( h, false );
}

HeadingNode::HeadingNode( bool link, HeadingNode & h)
{
    (void)copyFrom( h, link );
}

bool HeadingNode::copyFrom( HeadingNode &fromThis, bool linkWith )
{
    ASTnode::copyFrom( fromThis );
    myHeading = fromThis.myHeading;

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

HeadingNode::HeadingNode(const string & h) : myHeading(h) {}

void HeadingNode::changeName( const string &name) 
{
    myHeading = name;
}

DataListNode::DataListNode(ASTlist<DataNode *> *L) : myKids(L)
{
    myKids->setParent( this );
    myKids->reserve( L->size() );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	myKids->Current()->setParent( this );
}

bool DataListNode::copyFrom( DataListNode &fromThis, bool linkWith )
{
    DataListNode &L    = fromThis;
    DataNode *remember = (L.myKids->AtEnd()) ? 
			(DataNode*)NULL : L.myKids->Current();

    ASTnode::copyFrom( fromThis );
    myKids = new ASTlist<DataNode *>;
    myKids->setParent( this );
    for( L.myKids->Reset() ; ! L.myKids->AtEnd() ; L.myKids->Next() )
    {
	switch( L.myKids->Current()->myType() )
	{
	    case ASTnode::DATAITEMNODE:
		myKids->AddToEnd( new DataItemNode( linkWith, *( (DataItemNode*)L.myKids->Current() ) )  );
		break;
	    case ASTnode::DATALOOPNODE:
		myKids->AddToEnd( new DataLoopNode( linkWith, *( (DataLoopNode*)L.myKids->Current() ) )  );
		break;
	    case ASTnode::SAVEFRAMENODE: 
		myKids->AddToEnd( new SaveFrameNode( linkWith, *( (SaveFrameNode*)L.myKids->Current() ) )  );
		break;
	    default:   // This is here to get "gcc -Wall" to shut up.
		break;
	}
    }
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	myKids->Current()->setParent( this );
    (void)L.myKids->SeekTo( remember );

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

DataListNode::DataListNode(DataListNode &L)
{
    (void)copyFrom( L, false );
}

DataListNode::DataListNode(bool link, DataListNode &L)
{
    (void)copyFrom( L, link );
}

DataListNode::~DataListNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
        if( myKids->Current() != NULL )
 	    DELETE_IF_NOT_ALREADY(myKids->Current());

    if( myKids != NULL )
        delete  (myKids);
    bitFields.isDeleting = false;
}

List<ASTnode*> * DataListNode::searchByTag( const string &searchFor )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *someHits;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	someHits = myKids->Current()->searchByTag( searchFor );
	if( someHits->Length() > 0 )
	    retVal->AddToEnd( *someHits );
	delete  (someHits);
    }

    return retVal;
}

List<ASTnode*> * DataListNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *matches;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	matches = myKids->Current()->searchForType( type, delim );
	if( matches->Length() > 0 )
	    retVal->AddToEnd( *matches );
	delete  (matches);
    }

    return retVal;
}

List<ASTnode*> * DataListNode::searchByTagValue( const string &tag, const string &value )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *someHits;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	someHits = myKids->Current()->searchByTagValue( tag, value );
	if( someHits->Length() > 0 )
	    retVal->AddToEnd( *someHits );
	delete  (someHits);
    }

    return retVal;
}

bool DataListNode::unlinkChild( ASTnode *child )
{
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {   if( myKids->Current() == child )
	{   myKids->RemoveCurrent();
	    return true;
 	}
    }
    return false;
}

int DataListNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of DataNodes in this object:
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	// Loops are weird.  They don't count.
	if( ! myKids->Current()->isOfType( ASTnode::DATALOOPNODE )  )
	{
	    curLen = myKids->Current()->myLongestStr();
	    if( curLen > retVal )
		retVal = curLen;
	}
    }
    return retVal;
}

///////////////////////////////////////////////////////////////////////
//       myType()
// The compiler I was using has trouble with virtual inlined
// functions not behaving like a virtual function should, so I had to
// make this be not inlined for them to work, even though they are
// very simple:
//////////////////////////////////////////////////////////////////////

ASTnode::ASTtype ASTnode::myType( void )
{ return ASTNODE; }
ASTnode::ASTtype BlockNode::myType( void )
{ return BLOCKNODE; }
ASTnode::ASTtype DataBlockNode::myType( void )
{ return DATABLOCKNODE; }
ASTnode::ASTtype DataHeadingNode::myType( void )
{ return DATAHEADINGNODE; }
ASTnode::ASTtype DataItemNode::myType( void )
{ return DATAITEMNODE; }
ASTnode::ASTtype DataListNode::myType( void )
{ return DATALISTNODE; }
ASTnode::ASTtype DataLoopNameListNode::myType( void )
{ return DATALOOPNAMELISTNODE; }
ASTnode::ASTtype DataLoopValListNode::myType( void )
{ return DATALOOPVALLISTNODE; }
ASTnode::ASTtype LoopValListNode::myType( void )
{ return LOOPVALLISTNODE; }
ASTnode::ASTtype DataLoopNode::myType( void )
{ return DATALOOPNODE; }
ASTnode::ASTtype DataNameNode::myType( void )
{ return DATANAMENODE; }
ASTnode::ASTtype DataNode::myType( void )
{ return DATANODE; }
ASTnode::ASTtype DataValueNode::myType( void )
{ return DATAVALUENODE; }
ASTnode::ASTtype GlobalBlockNode::myType( void )
{ return GLOBALBLOCKNODE; }
ASTnode::ASTtype GlobalHeadingNode::myType( void )
{ return GLOBALHEADINGNODE; }
ASTnode::ASTtype HeadingNode::myType( void )
{ return HEADINGNODE; }
ASTnode::ASTtype LoopRowNode::myType( void )
{ return LOOPROWNODE; }
ASTnode::ASTtype LoopNameListNode::myType( void )
{ return LOOPNAMELISTNODE; }
ASTnode::ASTtype LoopTableNode::myType( void )
{ return LOOPTABLENODE; }
ASTnode::ASTtype SaveFrameListNode::myType( void )
{ return SAVEFRAMELISTNODE; }
ASTnode::ASTtype SaveFrameNode::myType( void )
{ return SAVEFRAMENODE; }
ASTnode::ASTtype SaveHeadingNode::myType( void )
{ return SAVEHEADINGNODE; }
ASTnode::ASTtype StarListNode::myType( void )
{ return STARLISTNODE; }
ASTnode::ASTtype StarFileNode::myType( void )
{ return STARFILENODE; }

/////////////////////////////////
//       isOfType()
/////////////////////////////////
// All of the various isOfType() functions have been grouped together here:

bool ASTnode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool HeadingNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool StarFileNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataListNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool BlockNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool StarListNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool GlobalBlockNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataBlockNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataNameNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataValueNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataItemNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool LoopNameListNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataLoopNameListNode ::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataLoopValListNode ::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool LoopValListNode ::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool LoopRowNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool LoopTableNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataLoopNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool GlobalHeadingNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool DataHeadingNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool SaveHeadingNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool SaveFrameListNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}
bool SaveFrameNode::isOfType( ASTtype T )
{
    return NotVirtualIsOfType(T);
}

  //////////////////////////////////////////////////////////////////////////
 /////////////////// NotVirtualIsOfType functions: ////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ASTnode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == ASTNODE);
}
bool HeadingNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == HEADINGNODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool DataNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATANODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool StarFileNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == STARFILENODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool DataListNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATALISTNODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool BlockNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == BLOCKNODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool StarListNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == STARLISTNODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool GlobalBlockNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == GLOBALBLOCKNODE) ? true : ((BlockNode*)this)->NotVirtualIsOfType(T);
}
bool DataBlockNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATABLOCKNODE) ? true : ((BlockNode*)this)->NotVirtualIsOfType(T);
}
bool DataNameNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATANAMENODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool DataValueNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATAVALUENODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool DataItemNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATAITEMNODE) ? true : ((DataNode*)this)->NotVirtualIsOfType(T);
}
bool LoopNameListNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == LOOPNAMELISTNODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool DataLoopNameListNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATALOOPNAMELISTNODE ) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool DataLoopValListNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATALOOPVALLISTNODE ) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool LoopValListNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == LOOPVALLISTNODE ) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool LoopRowNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == LOOPROWNODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool LoopTableNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == LOOPTABLENODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool DataLoopNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATALOOPNODE) ? true : ((DataNode*)this)->NotVirtualIsOfType(T);
}
bool GlobalHeadingNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == GLOBALHEADINGNODE) ? true : ((HeadingNode*)this)->NotVirtualIsOfType(T);
}
bool DataHeadingNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == DATAHEADINGNODE) ? true : ((HeadingNode*)this)->NotVirtualIsOfType(T);
}
bool SaveHeadingNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == SAVEHEADINGNODE) ? true : ((HeadingNode*)this)->NotVirtualIsOfType(T);
}
bool SaveFrameListNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == SAVEFRAMELISTNODE) ? true : ((ASTnode*)this)->NotVirtualIsOfType(T);
}
bool SaveFrameNode::NotVirtualIsOfType( ASTtype T )
{
    return  (T == SAVEFRAMENODE) ? true : ((DataNode*)this)->NotVirtualIsOfType(T);
}
DataListNode::DataListNode()
{
    myKids = new ASTlist<DataNode *>;
    myKids->setParent( this );
}

BlockNode::BlockNode( BlockNode &n )
{
    (void)copyFrom( n, false );
}

BlockNode::BlockNode( bool link, BlockNode &n )
{
    (void)copyFrom( n, link );
}

bool BlockNode::copyFrom( BlockNode &fromThis, bool linkWith )
{
    ASTnode::copyFrom( fromThis );
    myDataList = new DataListNode( linkWith, *(fromThis.myDataList) );
    myHeading = new HeadingNode( linkWith, *(fromThis.myHeading) );
    myHeading->setParent(this);
    myDataList->setParent(this);
    myDataList->reserve( fromThis.myDataList->GiveMyDataList()->size() );

    setPreComment( fromThis.getPreComment() );

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

BlockNode::BlockNode(HeadingNode *h, DataListNode *n) 
:
 myDataList(n), myHeading(h)
{
    myHeading->setParent(this);
    myDataList->setParent(this);
    preComment = string("");
}

BlockNode::BlockNode(const string & name)
{
  myHeading = new HeadingNode(name);
  myDataList = new DataListNode;
  myHeading->setParent(this);
  myDataList->setParent(this);
  preComment = string("");
}

BlockNode::~BlockNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    if( myDataList != NULL )
	DELETE_IF_NOT_ALREADY(myDataList);
    if( myHeading != NULL )
	DELETE_IF_NOT_ALREADY(myHeading);
    bitFields.isDeleting = false;
}

List<ASTnode*> * BlockNode::searchByTag( const string &searchFor )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;
    
    if( myHeading != (ASTnode*)NULL )
    {
	if( strCmpInsensitive( myHeading->myName(), searchFor ) == 0 )
	    retVal->AddToEnd( this );
    }

    if( myDataList != (ASTnode*)NULL )
    {
	foundSome = myDataList->searchByTag( searchFor );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

// Perform a string compare in a case insensitive manner.
// Like the Ansii C 'strcmp()' funciton, it returns -1, 0 ,or +1
// for less than, equal, or greater than, respectively.
int strCmpInsensitive( const string &st1, const string &st2 )
{
    int i;
    int len1 = st1.length();
    int len2 = st2.length();
    int lesserLen = len1 < len2 ? len1 : len2;
    int retVal = 0;

    for( i = 0 ; i < lesserLen ; i++ )
    {
	if( tolower(st1[i]) == tolower(st2[i]) )
	    continue;
	else if( tolower(st1[i]) < tolower(st2[i]) )
	{   retVal = -1;
	    break;
	}
	else // greater than
	{   retVal = 1;
	    break;
	}
    }
    if( retVal == 0 )
    {
	// The parts we compared are equal, but perhaps one string
	// is shorter than the other.  The shorter string is "less than"
	// the longer one.
	if( len1 < len2 )
	    retVal = -1;
        else if( len1 > len2 )
	    retVal = 1;
    }
    return retVal;
}

List<ASTnode*> * BlockNode::searchForType( 
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;
    
    if( isOfType( type ) )
	retVal->AddToEnd( this );

    if( myHeading != (ASTnode*)NULL )
    {
	if( myHeading->isOfType( type ) )
	    retVal->AddToEnd( myHeading );
    }

    if( myDataList != (ASTnode*)NULL )
    {
	foundSome = myDataList->searchForType( type, delim );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

List<ASTnode*> * BlockNode::searchByTagValue( const string &tag, const string &value )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;
    
    if( myDataList != (ASTnode*)NULL )
    {
	foundSome = myDataList->searchByTagValue( tag, value );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

bool BlockNode::unlinkChild( ASTnode *child )
{
    if( myDataList == (DataListNode*)child )
    {	myDataList = (DataListNode*)NULL;
	return true;
    }
    else if( myHeading == (HeadingNode*)child )
    {   myHeading = (HeadingNode*)NULL;
	return true;
    }
    else
	return false;
}

int BlockNode::myLongestStr( void )
{
    int headLength = myHeading->myLongestStr();
    int listLength = myDataList->myLongestStr();

    // return whichever is greater:
    return ( headLength > listLength ) ? headLength : listLength ;
}

StarListNode::StarListNode( StarListNode &L)
{
    (void)copyFrom( L, false );
}

StarListNode::StarListNode( bool link, StarListNode &L)
{
    (void)copyFrom( L, link );
}

bool StarListNode::copyFrom( StarListNode &fromThis, bool linkWith )
{
    StarListNode &L = fromThis;

    BlockNode *remember = (L.myKids->AtEnd()) ? 
			    (BlockNode*)NULL : L.myKids->Current();
    ASTnode::copyFrom( fromThis );
    myKids = new ASTlist<BlockNode *>;
    myKids->setParent(this);
    myKids->reserve(L.myKids->size());
    for(   L.myKids->Reset() ; ! L.myKids->AtEnd() ; L.myKids->Next()  )
    {
	switch( L.myKids->Current()->myType() )
	{
	    case ASTnode::BLOCKNODE:
		myKids->AddToEnd( new BlockNode( linkWith, *(L.myKids->Current()) ) );
		break;
	    case ASTnode::DATABLOCKNODE:
		myKids->AddToEnd( new DataBlockNode( linkWith, *(DataBlockNode*)(L.myKids->Current()) ) );
		break;
	    case ASTnode::GLOBALBLOCKNODE:
		myKids->AddToEnd( new GlobalBlockNode( linkWith, *(GlobalBlockNode*)(L.myKids->Current()) ) );
		break;
	    default:   // Just here to keep gcc -Wall happy.
		break;
	}
    }
    for(   myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next()  )
    {   myKids->Current()->setParent(this);
    }
    (void)L.myKids->SeekTo( remember );

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

StarListNode::StarListNode(ASTlist<BlockNode *> *L) : myKids(L)
{
    myKids->setParent(this);
    for(   myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next()  )
    {   myKids->Current()->setParent(this);
    }
}

StarListNode::StarListNode()
{
    myKids = new ASTlist<BlockNode *>;
    myKids->setParent(this);
    for(   myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next()  )
    {   myKids->Current()->setParent(this);
    }
}

StarListNode::~StarListNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	if( myKids->Current() != NULL )
	    DELETE_IF_NOT_ALREADY(myKids->Current());

    if( myKids != NULL )
	delete  (myKids);
    bitFields.isDeleting = false;
}

List<ASTnode*> * StarListNode::searchByTag( const string &searchFor )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchByTag( searchFor );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

List<ASTnode*> * StarListNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchForType( type, delim );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

List<ASTnode*> * StarListNode::searchByTagValue( const string &tag, const string &value )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchByTagValue( tag, value );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

bool StarListNode::unlinkChild( ASTnode *child )
{
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {   if( myKids->Current() == child )
	{
	    myKids->RemoveCurrent();
	    return true;
	}
    }

    return false;
}

int StarListNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of objects inside me:
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	curLen = myKids->Current()->myLongestStr();
	if( curLen > retVal )
	    retVal = curLen;
    }
    return retVal;
}

List<ASTnode*> * StarFileNode::searchByTag( const string  &searchFor )
{
    return myStarList->searchByTag( searchFor );
}

List<ASTnode*> * StarFileNode::searchByTagValue( const string &tag, const string &value )
{
    return myStarList->searchByTagValue( tag, value );
}

List<ASTnode*> * StarFileNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *matches;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    if( myStarList != NULL )
    {
	matches = myStarList->searchForType( type, delim );
	if( matches->Length() > 0 )
	    retVal->AddToEnd( *matches );
	delete  (matches);
    }

    return retVal;
}

bool StarFileNode::unlinkChild( ASTnode *child )
{
    return myStarList->unlinkChild( child );
}



StarFileNode::StarFileNode(StarFileNode &n)
{
    (void)copyFrom( n, false );
}

StarFileNode::StarFileNode( bool link, StarFileNode &n)
{
    (void)copyFrom( n, link );
}

bool StarFileNode::copyFrom( StarFileNode &fromThis, bool linkWith )
{

    ASTnode::copyFrom( fromThis );
    myStarList = new StarListNode( linkWith, *(fromThis.myStarList) );
    myStarList->setParent( this );

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

StarFileNode::StarFileNode(StarListNode *n) : myStarList(n)
{
    myStarList->setParent( this );
} 

StarFileNode::StarFileNode()
{
  myStarList = new StarListNode;
  myStarList->setParent( this );
  skipTexts.clear();
  skipTextLineNums.clear();
}

StarFileNode::~StarFileNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    if( myStarList != NULL )
	DELETE_IF_NOT_ALREADY(myStarList);
    bitFields.isDeleting = false;
    skipTexts.clear();
    skipTextLineNums.clear();
}

DataNameNode::DataNameNode(const string &str)
    : myStrVal(str)
{}


DataNameNode::DataNameNode(bool link,  DataNameNode &fromThis )
    : myStrVal( fromThis.myStrVal )
{
    if( link )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
}


DataNameNode::~DataNameNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    bitFields.isDeleting = false;
}

DataValueNode::DataValueNode(const char *str) :
    myStrVal(str)
{
    bitFields.delimType = NON;
}
DataValueNode::DataValueNode(const string & str) :
    myStrVal(str)
{
    bitFields.delimType = NON;
}
DataValueNode::DataValueNode(const string & str, ValType initType) :
    myStrVal(str)
{
    bitFields.delimType = initType;
    switch( initType )
    {
	case NON:
	    // do nothing special
	    break;
	case SINGLE:
	    // If there are single-quotes at the start and end, strip them.
	    if( myStrVal.length() > 0 )
	    {
		// Only if the tick marks are at both ends:
		if( myStrVal[0] == '\'' && ( myStrVal[ myStrVal.length() - 1 ] == '\'' ) )
		{
		    erase_str(myStrVal)( 0, 1 );
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1), 1 );
		}
	    }
	    break;
	case DOUBLE:
	    // If there are double-quotes at the start and end, strip them.
	    if( myStrVal.length() > 0 )
	    {
		// Only if the tick marks are at both ends:
		if( myStrVal[0] == '\"' && ( myStrVal[ myStrVal.length() - 1 ] == '\"' ) )
		{
		    erase_str(myStrVal)( 0, 1 );
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1), 1 );
		}
	    }
	    break;
	case SEMICOLON:
	    // If there are semicolons at the start or end, strip them.
	    if( myStrVal.length() > 0 )
	    {
		if( myStrVal[0] == ';' )
		    erase_str(myStrVal)( 0, 1 );
		if( myStrVal[0] == '\n' )
		    erase_str(myStrVal)( 0, 1 );
		if( myStrVal[ myStrVal.length() - 1 ] == '\n' )
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1), 1 );
		if( myStrVal[ myStrVal.length() - 1 ] == ';' )
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1), 1 );
		if( myStrVal[ myStrVal.length() - 1 ] == '\n' )
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1), 1 );
	    }
	    break;
	case FRAMECODE:
	    // If there is a dollar-sign at the start, strip it.
	    if( myStrVal.length() > 0 )
	    {
		if( myStrVal[0] == '$' )
		    erase_str(myStrVal)( 0, 1 );
	    }
	    break;
    }
}
DataValueNode::DataValueNode(const char *str, ValType initType) :
    myStrVal(str)
{
    bitFields.delimType = initType;
    switch( initType )
    {
	case NON:
	    // do nothing special
	    break;
	case SINGLE:
	    // If there are single-quotes at the start and end, strip them.
	    if( myStrVal.length() > 0 )
	    {
		// Only if the tick marks are at both ends:
		if( myStrVal[0] == '\''  && ( myStrVal[ myStrVal.length() - 1 ] == '\'' ) ) 
		{
		    erase_str(myStrVal)( 0, 1 );
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1) , 1 );
		}
	    }
	    break;
	case DOUBLE:
	    // If there are double-quotes at the start and end, strip them.
	    if( myStrVal.length() > 0 )
	    {
		// Only if the tick marks are at both ends:
		if( myStrVal[0] == '\"' &&  ( myStrVal[ myStrVal.length() - 1 ] == '\"' ) ) 
		{
		    erase_str(myStrVal)( 0, 1 );
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1) , 1 );
	        }
	    }
	    break;
	case SEMICOLON:
	    // If there are semicolons at the start or end, strip them.
	    if( myStrVal.length() > 0 )
	    {
		if( myStrVal[0] == ';' )
		    erase_str(myStrVal)( 0, 1 );
		if( myStrVal[0] == '\n' )
		    erase_str(myStrVal)( 0, 1 );
		if( myStrVal[ myStrVal.length() - 1 ] == '\n' )
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1) , 1 );
		if( myStrVal[ myStrVal.length() - 1 ] == ';' )
		    erase_str(myStrVal)( (int)(myStrVal.length() - 1) , 1 );
	    }
	    break;
	case FRAMECODE:
	    // If there is a dollar-sign at the start, strip it.
	    if( myStrVal.length() > 0 )
	    {
		if( myStrVal[0] == '$' )
		    erase_str(myStrVal)( 0, 1 );
	    }
	    break;
    }
}

DataValueNode::DataValueNode( DataValueNode &copy )
{
    (void)copyFrom( copy, false );
}

DataValueNode::DataValueNode( bool link, DataValueNode &copy )
{
    (void)copyFrom( copy, link );
}

bool DataValueNode::copyFrom( DataValueNode &fromThis, bool linkWith )
{
#ifdef DEBUG_DVN
    cerr << "DEBUG_DVN: copyFrom ptr = " << &fromThis << endl;
    cerr << "DEBUG_DVN: copyFrom val = " << fromThis.myValue() << endl;
#endif DEBUG_DVN
    ASTnode::copyFrom( fromThis );
    myStrVal = fromThis.myName();
    bitFields.delimType = fromThis.myDelimType();

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

ASTnode *DataValueNode::myParallelCopy( void )
{
    LoopRowNode *parRow;
    LoopRowNode *peerRow;
    int         col, parSize;
    /* If I am inside a LoopRowNode, then go up to
     * the LoopRowNode, over to its peer, and then
     * down to my peer over there, since a direct
     * link won't work with the caching scheme.
     * If I am NOT inside a LoopRowNode, then
     * behave as normal, using the peer pointer.
     */
    if( parent != NULL && parent->isOfType( ASTnode::LOOPROWNODE ) )
    {   parRow = (LoopRowNode*) parent;
	peerRow = (LoopRowNode*) ( parRow->myParallelCopy() ) ;
	if( peerRow != NULL )
	{
	    // Find myself in the cache.  This is pretty silly how
	    // this works.  I query all the possible values that could
	    // be in the cache to see if any of them are me, since I
	    // don't have a primitive for searching where col = "don't
	    // care" and Loop Row Node = foo.
	    // 
	    // Also, I am running under the assumption that if I am
	    // in a LoopRowNode then I *must* be in the cache else the
	    // caller would have never gotten its hands on me, since
	    // DataValueNodes don't *really* exist inside a LoopRowNode.
	    // ---------------------------------------------------------
	    parSize = parRow->size();
	    for( col = 0 ; col < parSize ; col++ )
	    {   if( ValCache::getFromValCache( parRow, col ) == this )
		    break;
	    }
	    if( col < parSize )
	    {   return (*peerRow)[col];
	    }
	    else
	    {   return NULL;
	    }
	}
    }
    else
    {   return peer;
    }
}
void DataValueNode::setValue( const string &newVal )
{
    myStrVal = newVal;
}

void DataValueNode::setValue( const char *newVal )
{
    myStrVal = string( newVal );
}

int DataValueNode::myLongestStr( void )
{
    int retVal = myStrVal.length();

    switch( bitFields.delimType )
    {
	case NON :        retVal += 1;  // One more for a leading space
			  break;
	case DOUBLE :
	case SINGLE :     retVal += 2;
	                  break;
	case FRAMECODE :  retVal += 1;
	                  break;
	case SEMICOLON :  retVal += 4;  // Probably fairly meaningless, really.
			  break;
	default :         break;  // Makes the compiler shut up.
    }
    return retVal;
}

void DataValueNode::changeToAppropriateDelimiter( void )
{
    setDelimType( findAppropriateDelimiterType( myStrVal ) );
}

DataValueNode::ValType DataValueNode::findAppropriateDelimiterType(
	const string &val )
{
    int       length      = val.length();
    bool      has_squote  = false;
    bool      has_dquote  = false;
    bool      has_wspace  = false;
    bool      has_newline = false;
    const char*     strPtr = val.c_str();
    DataValueNode::ValType retVal  = NON;  // return value.  assume 'NON'
					   // by default.


    if( strchr( strPtr, '\'' ) )
	has_squote = true;
    if( strchr( strPtr, '\"' ) )
	has_dquote = true;
    if( strchr( strPtr, ' ' ) || strchr( strPtr, '\t' ) )
	has_wspace = true;
    if( strchr( strPtr, '\n' ) )
	has_newline = true;

    if( has_newline || (has_dquote && has_squote) )
    {   retVal = SEMICOLON; }
    else if( has_dquote )
    {   retVal = SINGLE; }
    else if( has_squote || has_wspace )
    {   retVal = DOUBLE; }
    else
    {   retVal = NON ; }

    return retVal;

}

void DataValueNode::setDelimType( ValType setTo )
{
    bitFields.delimType = setTo;
}

bool DataValueNode::operator==( const string &str )
{
    return  (myStrVal == str) ;
}
bool DataValueNode::operator< (const string &str)
{   return  myStrVal < str ;
}
bool DataValueNode::operator> (const string &str)
{   return  myStrVal > str;
}
bool DataValueNode::operator<=(const string &str)
{   return  myStrVal <= str;
}
bool DataValueNode::operator>=(const string &str)
{   return  myStrVal >= str;
}

bool DataValueNode::operator==( const char *str )
{
    // Just a wrapper around the other version of this function.
    // ---------------------------------------------------------
    bool     retVal;
    string   *tempStr = new string( str );
    retVal = ( operator==(*tempStr) );
    delete  (tempStr);
    return retVal;
}

bool DataValueNode::operator< (const char *str)
{   return  myStrVal < string(str) ;
}
bool DataValueNode::operator> (const char *str)
{   return  myStrVal > string(str);
}
bool DataValueNode::operator<=(const char *str)
{   return  myStrVal <= string(str);
}
bool DataValueNode::operator>=(const char *str)
{   return  myStrVal >= string(str);
}

List<ASTnode*> * DataValueNode::searchForType(
	     ASTtype    type,
	     int        delim
	     )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;

    if( delim < 0 || delim == myDelimType() )  /* compare int to enum is legal, but be careful */
	if( isOfType( type ) )
	    retVal->AddToEnd( this );
    return retVal;
}

DataItemNode::DataItemNode(DataNameNode *name, DataValueNode *value) 
:
 myDataName(name), myDataValue(value)
{
    myDataName->setParent(this);
    myDataValue->setParent(this);
    preComment = string("");
}

///////////////////////////////////////////////////////////////////////////
//Copy constructor for DataItemNode
//////////////////////////////////////////////////////////////////////////
DataItemNode::DataItemNode( DataItemNode &  D)
{
    (void)copyFrom( D, false );
}

DataItemNode::DataItemNode( bool link, DataItemNode & D)
{
    (void)copyFrom( D, link );
}

bool DataItemNode::copyFrom( DataItemNode &fromThis, bool linkWith )
{
    ASTnode::copyFrom( fromThis );
    myDataName = new DataNameNode( linkWith, *(fromThis.myDataName) );
    myDataValue = new DataValueNode( linkWith, *(fromThis.myDataValue) );

    myDataName->setParent(this);
    myDataValue->setParent(this);
    setPreComment( fromThis.getPreComment() );
    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////

DataItemNode::DataItemNode(const string & name, const string & value)
{
  myDataName = new DataNameNode(name);

  DataValueNode *newValue = new DataValueNode( value );

  myDataValue = newValue;
  preComment = string("");

  myDataName->setParent(this);
  myDataValue->setParent(this);
}

DataItemNode::DataItemNode( const string & name,
	                    const string & value,
			    DataValueNode::ValType  type  )
{
  myDataName = new DataNameNode(name);

  DataValueNode *newValue = new DataValueNode( value, type );

  myDataValue = newValue;
  preComment = string("");

  myDataName->setParent(this);
  myDataValue->setParent(this);
}

DataItemNode::~DataItemNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    if( myDataName != NULL )
	DELETE_IF_NOT_ALREADY(myDataName);
    if( myDataValue != NULL )
	DELETE_IF_NOT_ALREADY(myDataValue);
    bitFields.isDeleting = false;
    preComment = string("");

}

void DataItemNode::setDelimType( DataValueNode::ValType newType )
{
    if( myDataValue != (DataValueNode*)NULL )
        myDataValue->setDelimType( newType );
}

void DataItemNode::setValue( const string &val )
{
    if( myDataValue != (DataValueNode*)NULL )
	myDataValue->setValue( val );
}

void DataItemNode::setValue( const char  *val )
{
    if( myDataValue != (DataValueNode*)NULL )
	myDataValue->setValue( val );
}
void DataItemNode::setName( const string &name )
{
    if( myDataName != (DataNameNode*)NULL )
	myDataName->setName( name );
}

void DataItemNode::setName( const char   *name )
{
    if( myDataName != (DataNameNode*)NULL )
	myDataName->setName( name );
}


List<ASTnode*> * DataItemNode::searchByTag( const string &searchFor )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;

    if( myDataName != NULL )
	if( strCmpInsensitive( myDataName->myName(), searchFor ) == 0)
	    retVal->AddToEnd( this );

    return retVal;
}

List<ASTnode*> * DataItemNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *matches;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    if( myDataName != NULL )
    {
	matches = myDataName->searchForType( type, delim  );
	if( matches->Length() > 0 )
	    retVal->AddToEnd( *matches );
	delete  (matches);
    }

    if( myDataValue != NULL )
    {
	matches = myDataValue->searchForType( type, delim );
	if( matches->Length() > 0 )
	    retVal->AddToEnd( *matches );
	delete  (matches);
    }

    return retVal;
}


bool DataItemNode::unlinkChild( ASTnode *child )
{
    if( myDataName == (DataNameNode*)child )
    {   myDataName = (DataNameNode*)NULL;
    }
    else if( myDataValue == (DataValueNode*)child )
    {   myDataValue = (DataValueNode*)NULL;
	return true;
    }
    return false;
}

List<ASTnode*> * DataItemNode::searchByTagValue( const string &tag,
	                                  const string &value )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;

    if( myDataName != NULL )
	if( strCmpInsensitive(myDataName->myName(), tag) == 0 )
	    if( *myDataValue == value )
		retVal->AddToEnd( this );

    return  retVal;
}


LoopNameListNode::LoopNameListNode(ASTlist<DataNameNode *> *L)
  :
  myKids(L) 
{
    myKids->setParent(this);
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	myKids->Current()->setParent(this);
}

LoopNameListNode::LoopNameListNode( vector<DataNameNode *> &L)
{
    vector<DataNameNode *>::iterator  i;

    myKids = new ASTlist<DataNameNode *>;
    myKids->setParent(this);

    for( i = L.begin() ; i != L.end() ; i++ )
        myKids->AddToEnd( new DataNameNode( **i ) );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
        myKids->Current()->setParent(this);

}

LoopNameListNode::LoopNameListNode( LoopDefListNode  &L )
{
    (void)copyFrom( L, false );
}

LoopNameListNode::LoopNameListNode( bool link, LoopDefListNode  &L )
{
    (void)copyFrom( L, link );
}

bool LoopNameListNode::copyFrom( LoopDefListNode &L, bool linkWith )
{
    DataNameNode *remember = (L.myKids->AtEnd()) ? 
			(DataNameNode*)NULL : L.myKids->Current();
    ASTnode::copyFrom( L );
    myKids = new ASTlist<DataNameNode *>;
    myKids->setParent(this);
    myKids->reserve( L.myKids->size() );

    for( L.myKids->Reset() ; ! L.myKids->AtEnd() ; L.myKids->Next() )
        myKids->AddToEnd( new DataNameNode( linkWith, *(L.myKids->Current()) )  );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
        myKids->Current()->setParent(this);

    (void)L.myKids->SeekTo( remember );

    if( linkWith )
    {   setPeer( &L );
	L.setPeer( this );
    }
    return true;
}

LoopNameListNode::LoopNameListNode()
{
  myKids = new ASTlist<DataNameNode *>;
  myKids->setParent(this);
  for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
      myKids->Current()->setParent(this);
}

LoopNameListNode::~LoopNameListNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	if( myKids->Current() != NULL )
	    DELETE_IF_NOT_ALREADY(myKids->Current());

    if( myKids != NULL )
	delete  (myKids);
    bitFields.isDeleting = false;
}

int LoopNameListNode::isInLoop( void )
{
    bool    foundLoop = false;
    int     count = 0;
    ASTnode  *ancestor;

    for(   ancestor = this ;
	   ancestor != NULL && foundLoop == false  ;
	   ancestor = ancestor->myParent() )
    {
	if( ancestor->isOfType( ASTnode::DATALOOPNODE ) )
	{
	    foundLoop = true;
	    break;
	}
	if( ancestor->isOfType( ASTnode::DATALOOPNAMELISTNODE ) )
	{
	    // Find out where in the major list this minor list is:
	    DataLoopNameListNode::iterator  i;
	    DataLoopNameListNode            *parent =
				(DataLoopNameListNode*)ancestor;
	    for(    i = parent->begin(), count = 1 ;
		    i != parent->end() && *i != this ;
		    i++, count++ )
	    { }
	}
    }
    if( ancestor == NULL )
	return 0;
    else
	return count;
}

void LoopNameListNode::insert( iterator pos, const DataNameNode &v )
{
    DataValueNode *dvn = new DataValueNode(string(".") );
    insert( pos, v, *dvn );
}

void LoopNameListNode::insert( iterator pos, DataNameNode *v )
{
    DataValueNode *dvn = new DataValueNode(string(".") );
    insert( pos, v, *dvn );
}

void LoopNameListNode::insert(
	iterator       pos,
	const DataNameNode   &v,
	const DataValueNode  &val )
{
    DataNameNode   *newNode = new DataNameNode( v );
    insert( pos, newNode, val );
}

void LoopNameListNode::insert(
	LoopNameListNode::iterator       pos,
	DataNameNode   *v,
	const DataValueNode  &val )
{
    iterator       i;
    int            rowPos, nest;
    ASTnode        *ancestor;
    DataLoopNode  *ancestorLoop;


    // Need to count: What position is this new tag in the row?
    for(    i = begin() , rowPos = 0 ;
	    i != end() && i != pos;
	    i++, rowPos++ )
    { }

    // Need to find out if I am in a loop, and how many levels
    // nested down I am:
    nest = isInLoop();

    // If I am in a loop, then then I need to iterate over every
    // row at my nesting level and insert the new value into them:
    if( nest )
    {
	// Get the loop I am in (Guaranteed to be there because I already
	// checked via isInLoop() ):
	for(    ancestor = this ;
		! ancestor->isOfType( ASTnode::DATALOOPNODE );
		ancestor = ancestor->myParent() )
	{ }
	ancestorLoop = (DataLoopNode*)ancestor;

	(ancestorLoop->getVals()).insertAtPos( nest-1, rowPos, val );
    }

    myKids->insert( pos, v );
}

LoopNameListNode::iterator LoopNameListNode::iteratorFor( const DataNameNode *ptr )
{
    iterator i;

    for( i = begin() ; i != end() ; i++ )
	if( *i == ptr )
	    break;

    return i;
}
void LoopNameListNode::erase( iterator pos )
{
    iterator       i;
    ASTlist<DataNameNode*>::iterator j;
    int            rowPos, nest, numPos;
    DataNameNode   *deleteMe;
    ASTnode        *ancestor;
    DataLoopNode  *ancestorLoop;


    // Need to count: What position is this new tag in the row?
    for(    i = begin() , rowPos = 0 ;
	    i != end() && i != pos;
	    i++, rowPos++ )
    { }

    // Need to find out if I am in a loop, and how many levels
    // nested down I am:
    nest = isInLoop();

    // If I am in a loop, then then I need to iterate over every
    // row at my nesting level and erase the value:
    if( nest )
    {

	// Get the loop I am in (Guaranteed to be there because I already
	// checked via isInLoop() ):
	for(    ancestor = this ;
		! ancestor->isOfType(ASTnode::DATALOOPNODE);
		ancestor = ancestor->myParent() )
	{ }
	ancestorLoop = (DataLoopNode*)ancestor;

	ancestorLoop->getVals().eraseAtPos( nest-1, rowPos );

	
    }


    // This odd method of deleting is to avoid the automatic
    // re-calling of destructors that would otherwise occur
    // and recurse back to this function itself infinitely:
    // TODO - JUST CHANGED - CHECK THIS HARD:
    deleteMe = *pos; // Have to do this because pos will become
                     // an invalid iterator after the erasure.
    for(    j = myKids->begin() , numPos = 0 ;
	    j != myKids->end() && numPos != rowPos;
	    j++, numPos++ )
    { }
    if( j != myKids->end() )
        myKids->erase( j ); 
    deleteMe->setParent( NULL ); // So it doesn't try to unlinkMe
                            // and thereby end up back in this
			    // function again.
    // Don't double-delete if this erasure is happening because
    // the name was being deleted and we are in the destructor:
    if( ! deleteMe->bitFields.isDeleting )
	DELETE_IF_NOT_ALREADY(deleteMe);

    // If I am now gone entirely, then just get rid of this
    // nesting level altogether and all nesting levels under it:
    if( size() == 0 )
    {
	if( nest )
	{
	    // Was this - broke with new STL:
	    // LoopNameListNode *nameToErase = ancestorLoop->getNames()[nest-1];
	    // ancestorLoop->getNames().erase( nameToErase );

	    ancestorLoop->getNames().erase(
	        ancestorLoop->getNames().begin() + (nest - 1) );
	}
    }
}

void LoopNameListNode::erase( iterator from, iterator to )
{
    iterator       i;
    DataNameNode   *namePtr;
    int            fromNum, toNum, rowPos;
    ASTnode        *ancestor;
    DataValueNode  *ancestorLoop;


    // Need to count: What position is the from and to tag in the row?
    for(    i = begin() , rowPos = 0 ;
	    i != end() ;
	    i++, rowPos++ )
    {
	if( i == from )
	    fromNum = rowPos;
	if( i == to )
	    toNum = rowPos;
    }

    // erase the values one at a time using the above function.
    while( toNum != fromNum )
    {
	namePtr = (*myKids)[fromNum];
	DELETE_IF_NOT_ALREADY(namePtr);
	erase( myKids->begin() + fromNum ); /* was: erase(&namePtr ); */
	toNum--;
    }
}

void LoopTableNode::insertAtPos(
	int            nest,
	int            rowPos,
	const DataValueNode  &val )

{
    LoopTableNode::iterator row;

    // This algorithm is recursive: If this loop is above the
    // nesting level we want then iterate over the rows looking
    // for sub-loops and pass the recursive baton to them.  If
    // this loop *is* the nesting level we want, then insert the
    // value to each row of the loop:

    // Base Case:
    if( nest == 0 )
	for( row = begin() ; row != end() ; row++ )
	{
	    // NOTE: by calling privateInsertElementAt() instead of
	    // just insertElementAt(), I avoid the shoot-self-in-foot
	    // protections that are in the public insert() function.
	    // (They would never let me add a value to a row in a
	    // loop).  I can do this because the LoopRowNode class
	    // considers me its friend.
	    DataValueNode *newNode = new DataValueNode(
	       val.myValue(), val.myDelimType() ) ;
	    (*row)->privateInsertElementAt( rowPos, *newNode);
	    DELETE_IF_NOT_ALREADY(newNode);
	}
    // Recursive Case:
    else
	for( row = begin() ; row != end() ; row++ )
	    if( (*row)->myLoop() )
		(*row)->myLoop()->insertAtPos( nest - 1, rowPos, val );
}
void LoopTableNode::eraseAtPos(
	int                  nest,
	int                  rowPos )

{
    int      rowNum;

    // This algorithm is recursive: If this loop is above the
    // nesting level we want then iterate over the rows looking
    // for sub-loops and pass the recursive baton to them.  If
    // this loop *is* the nesting level we want, then erase the
    // value to each row of the loop:

    // Base Case:
    if( nest == 0 )
    {
	// Must iterate by number because begin/end style
	// iterators become invalid if the last DataValueNode is
	// deleted and causes the whole row to be deleted. 
	// Deleting a row will decrease the size of the myIters
	// vector and possibly move it in memory, invalidating all
	// the iterators we hold on it.  We must go backward
	// because we might end up deleting the rows, where going
	// forward will skip over every other row as things
	// shuffle upward in the vector after each deletion.
	for( rowNum = size()-1 ; rowNum >= 0 ; rowNum-- )
	{
	    ((*this)[rowNum])->privateRemoveElementAt( rowPos );
	}
    }
    // Recursive Case:
    else
	// Must iterate backward, and must use numbers,
	// because deletions are possible here if the column
	// being erased is the last one that exists in the
	// row.  (When the row deletes itself its destructor
	// removes it from my vector, which invalidates any
	// iterators I had on that vector, and re-numbers
	// things (What was element N is now element N-1).
	// This means a forward iteration would skip every
	// other element as they shift backward in the vector
	// underneath me.
	for( rowNum = size()-1 ; rowNum >= 0 ; rowNum-- )
	    if( (*this)[rowNum]->myLoop() )
		(*this)[rowNum]->myLoop()->eraseAtPos( nest-1, rowPos );
}

void LoopNameListNode::reset()
{
  myKids->Reset();
}

List<ASTnode*> * LoopNameListNode::searchByTag( const string &searchFor )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	if( strCmpInsensitive( myKids->Current()->myName(), searchFor ) == 0 )
	    retVal->AddToEnd( myKids->Current() );

    return retVal;
}

List<ASTnode*> * LoopNameListNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *matches;

    if( isOfType( type ) )
	retVal->AddToEnd( this );


    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	matches = myKids->Current()->searchForType( type, delim );
	if( matches->Length() > 0 )
	    retVal->AddToEnd( *matches );
	delete  (matches);
    }

    return retVal;
}


bool LoopNameListNode::unlinkChild( ASTnode *child )
{
    LoopNameListNode::iterator iter;
    for( iter = begin() ; iter != end() ; iter++ )
    {   if( (*iter) == child )
	{   erase( iter );
	    return true;
 	}
    }
    return false;
}


void LoopNameListNode::getNextTag()
{
  if (myKids->AtEnd())
    myKids->Reset();

  myKids->Next();
}

bool LoopNameListNode::atEnd()
{
  return myKids->AtEnd();
}

bool LoopNameListNode::atStart()
{
  return myKids->AtStart();
}

LoopValListNode::LoopValListNode(ASTlist<DVNWithPos *> *L)
  :
  myKids(L) 
{
    myKids->setParent(this);
}

LoopValListNode::LoopValListNode()
{
  myKids = new ASTlist<DVNWithPos *>;
  myKids->setParent(this);
}

LoopValListNode::~LoopValListNode()
{
    bitFields.isDeleting = true;
  unlinkMe();
  if( myKids != NULL )
      delete  (myKids);       // The individual DataValueNodes are not deleted because
                           // now they are part of the transformed structure
    bitFields.isDeleting = false;
}


DataLoopValListNode::Status LoopValListNode::getNextValue(DVNWithPos* & value)
{
  if (myKids->AtEnd())
    return DataLoopValListNode::STOP;

  value = myKids->Current();
  myKids->Next();
  return DataLoopValListNode::CONTINUE;
}

void DataLoopValListNode::debugPrint( void )
{
    ASTlist<LoopValListNode*>::iterator  i;

    for( i = myKids->begin() ; i != myKids->end() ; i++ )
    {
	fprintf( stderr, "\tA valList:\n" );
	(*i)->debugPrint();
    }
}

void LoopValListNode::debugPrint( void )
{
    ASTlist<DVNWithPos*>::iterator  i;

    for( i = myKids->begin() ; i != myKids->end() ; i++ )
    {
	fprintf( stderr, "\t\t%s\n", (*i)->myValue().c_str() );
    }
}


void LoopValListNode::reset()
{
  myKids->Reset();
}

bool DataLoopValListNode::unlinkChild( ASTnode *child )
{
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {   if( myKids->Current() == child )
	{   myKids->RemoveCurrent();
	    return true;
 	}
    }
    return false;
}

bool LoopValListNode::unlinkChild( ASTnode *child )
{
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {   if( myKids->Current() == child )
	{   myKids->RemoveCurrent();
	    return true;
 	}
    }
    return false;
}

int LoopValListNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of values in this object:
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	curLen = myKids->Current()->myLongestStr();
	if( curLen > retVal )
	    retVal = curLen;
    }
    return retVal;
}

DataLoopNameListNode::DataLoopNameListNode(ASTlist<LoopDefListNode *> *L) 
  : 
  myKids(L) 
{
  myKids->setParent(this);
  for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
      myKids->Current()->setParent(this);
}

DataLoopNameListNode::DataLoopNameListNode( DataLoopDefListNode &L)
{
    (void)copyFrom( L, false );
}

DataLoopNameListNode::DataLoopNameListNode( bool link, DataLoopDefListNode &L)
{
    (void)copyFrom( L, link );
}

bool DataLoopNameListNode::copyFrom( DataLoopDefListNode &fromThis, bool linkWith )
{
    LoopNameListNode *remember = (fromThis.myKids->AtEnd()) ? 
			(LoopNameListNode*)NULL : fromThis.myKids->Current();
    ASTnode::copyFrom( fromThis );
    myKids = new ASTlist<LoopNameListNode *>;
    myKids->setParent(this);
    myKids->reserve( fromThis.myKids->size() );

    for( fromThis.myKids->Reset() ; ! fromThis.myKids->AtEnd() ; fromThis.myKids->Next() )
        myKids->AddToEnd( new LoopNameListNode( linkWith, *(fromThis.myKids->Current()) )  );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
        myKids->Current()->setParent(this);

    (void)fromThis.myKids->SeekTo( remember );
    currRow = fromThis.currRow;

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

DataLoopNameListNode::DataLoopNameListNode()
{
  myKids = new ASTlist<LoopNameListNode *>;
  myKids->setParent(this);
  for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
      myKids->Current()->setParent(this);
}


DataLoopNameListNode::~DataLoopNameListNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	if( myKids->Current() != NULL )
	    DELETE_IF_NOT_ALREADY(myKids->Current());

    if( myKids != NULL )
	delete  (myKids);
    bitFields.isDeleting = false;
}

void DataLoopNameListNode::debugPrint( void )
{
    ASTlist<LoopNameListNode*>::iterator i;

    for( i = myKids->begin() ; i != myKids->end() ; i++ )
    {
	fprintf( stderr, "\tList of names:\n" );
	(*i)->debugPrint();
    }
}

void LoopNameListNode::debugPrint( void )
{
    ASTlist<DataNameNode*>::iterator i;

    for( i = myKids->begin() ; i != myKids->end() ; i++ )
    {
	fprintf( stderr, "\t\t%s\n", (*i)->myName().c_str() );
    }
}

bool DataLoopNameListNode::insert(
	iterator               pos,
	vector<DataNameNode*>  &v )
{
    // Let the other overloaded version do all the work:
    LoopNameListNode *newNode = new LoopNameListNode( v );
    return insert( pos, newNode );
}

bool DataLoopNameListNode::insert(
	iterator          pos,
	LoopNameListNode  &v  )
{
    // Let the other overloaded version do all the work:
    return insert( pos, new LoopNameListNode( v ) );
}

bool DataLoopNameListNode::insert(
	iterator          pos,
	LoopNameListNode  *v  )
{
    ASTnode   *myParentLoop;

    // Need to see if we are in DataLoopNode:
    // --------------------------------------
    for(    myParentLoop = this
	    ;
	    myParentLoop != NULL &&
	    !(myParentLoop->isOfType(ASTnode::DATALOOPNODE))
	    ;
	    myParentLoop = myParentLoop->myParent()
	)
    { }
    // If we are in a DataLoopNode:
    // ----------------------------
    if(     myParentLoop!= NULL &&
	    myParentLoop->isOfType(ASTnode::DATALOOPNODE) )
    {
	// If this is not the deepest nesting level, then
	// disallow the insertion:
	if( pos != end() )
	    return false;
	// Ask the associated DataLoopNode to make a new stub
	// nest level:
	( ((DataLoopNode*)myParentLoop)->getVals() ).makeNewNestLevel();
    }
    myKids->insert( pos, v );
    return true;
}

DataLoopNameListNode::iterator DataLoopNameListNode::iteratorFor( const LoopNameListNode *ptr )
{
    iterator i;

    for( i = begin() ; i != end() ; i++ )
	if( *i == ptr )
	    break;

    return i;
}

void DataLoopNameListNode::erase( iterator pos )
{
    DataLoopNameListNode::iterator  i;
    LoopNameListNode::iterator      oneName;
    int                             thisDepth;
    int                             curDepth;
    ASTnode                         *ancestorLoop;

    for(    ancestorLoop = this ;
	    ancestorLoop != NULL && ! ancestorLoop->isOfType( ASTnode::DATALOOPNODE ) ;
	    ancestorLoop = ancestorLoop->myParent()   )
    { }

    // Note, because of the rules of STAR files, I cannot
    // leave an inner loop under a loop level that no longer
    // exists.  Therefore when I delete a row I must delete all
    // the ones after it as well.

    // So I start at the deepest level and begin deleting upward to
    // the current level.  First, find the index number of the
    // position passed.  This is vital because we have to refer to
    // everything by index number not iterator, since the iterators
    // can become invalid every time the vector gets realloced, which
    // could very easily happen when we are deleting things from it:
    for(    i = begin(), thisDepth = 0 ;
	    i != end() && i != pos ;
	    i++, thisDepth++ )
    { }
    assert( i != end() ); // If this fails the caller tried to delete
                          // something that doesn't exist.

    // Now iterate over the list of names backward, so as to delete
    // things depth-first:
    for(    curDepth = size() - 1 ;
	    curDepth >= thisDepth ;
	    curDepth--   )
    {
	// Delete the names one at a time, thereby activating
	// the code I've already written that iterates through
	// and removes the associated columns.  If this is too
	// slow, this can be reimplemented more complicatedly in
	// the future without hurting the interface.
	for(    oneName = (*this)[curDepth]->begin() ;
		oneName != (*this)[curDepth]->end() ;
		oneName++     )
	    (*this)[curDepth]->erase( oneName );
	DELETE_IF_NOT_ALREADY((*this)[curDepth]);
    }
    if( ancestorLoop != NULL )
    {
	((DataLoopNode*)ancestorLoop)->getValsPtr()->truncateNestLevel( thisDepth );
    }
    /* Added 2001-03-28: if I am now empty then delete my parent loop too:
     */
    if( size() == 0 )
    {
	if( myParent() != NULL )
	{
	    DELETE_IF_NOT_ALREADY(myParent());
	}
    }
}

DataLoopNameListNode::Status DataLoopNameListNode::getNextTag(
					     DataLoopValListNode::Status vstat,
					     int & row)
{
    Status stat;
    bool   popUpLevel = false;

    if (myKids->AtEnd())
       return ERROR;

    popUpLevel = false; // default.

    // First, if this is a normal value, then we already determined the right
    // nest level (confusingly called "currRow") the last time this was ran:
    if( vstat == DataLoopValListNode::CONTINUE )
    {
	row = currRow;
	if( myKids->Current()->atStart() )
	    stat = NEW_ITER; // Just beginning a new row of a loop.
	else
	    stat = SAME_ITER; // Same row as last time.
	
	// Increment to the next tagName to get ready for the next
	// call to this function:
	myKids->Current()->getNextTag();

	// If we just went past the end of the names for this nestlevel,
	// then rewind it and drop to the next nest level:
	if( myKids->Current()->atEnd() )
	{
	    myKids->Current()->reset();

	    myKids->Next();
	    currRow++;

	    // But wait, maybe that was the innermost nest level, in
	    // which case we shouldn't have gone any further down:
	    if( myKids->AtEnd() )
		popUpLevel = true;
	}
    }
    // Else, if it is a stop value, then the current nesting level
    // is done and we pop back one nest level:
    else if( vstat == DataLoopValListNode::STOP )
    {
	row = currRow - 1;
	popUpLevel = true;
	stat = STOP;
    }
    else
	stat = ERROR;

    if( popUpLevel )
    {
	// Either a STOP was encountered or the last value of a row
	// at the innermost nesting level was encountered:  Either
	// way we go up a nest level:
	myKids->Prev();
	currRow--;
    }
    
    return stat;
}

void DataLoopNameListNode::reset()
{
  currRow = 0;

  for (myKids->Reset(); !myKids->AtEnd(); myKids->Next())
    myKids->Current()->reset();

  myKids->Reset();
}

List<ASTnode*> * DataLoopNameListNode::searchByTag( const string &searchFor )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchByTag( searchFor );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

List<ASTnode*> * DataLoopNameListNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchForType( type, delim );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

bool DataLoopNameListNode::unlinkChild( ASTnode *child )
{
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {   if( myKids->Current() == child )
	{   myKids->RemoveCurrent();
	    return true;
 	}
    }
    return false;
}

int DataLoopNameListNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of DataNodes in this object:
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	curLen = myKids->Current()->myLongestStr();
	if( curLen > retVal )
	    retVal = curLen;
    }
    return retVal;
}


int LoopNameListNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of DataNodes in this object:
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	curLen = myKids->Current()->myLongestStr();
	if( curLen > retVal )
	    retVal = curLen;
    }
    return retVal;
}


DataLoopValListNode::DataLoopValListNode(ASTlist<LoopValListNode *> *L) 
: 
  myKids(L)
{
    myKids->setParent(this);
}

DataLoopValListNode::~DataLoopValListNode()
{
  bitFields.isDeleting = true;
  unlinkMe();
  for (myKids->Reset(); !myKids->AtEnd(); myKids->Next())
    if( myKids->Current() != NULL )
	DELETE_IF_NOT_ALREADY(myKids->Current());

  if( myKids != NULL )
      delete  (myKids);
  bitFields.isDeleting = false;
}

DataLoopValListNode::Status DataLoopValListNode::getNextValue(DVNWithPos* &value)
{
  if (myKids->AtEnd())
    return END;

  Status stat = myKids->Current()->getNextValue(value);

  if (stat == STOP) {
    myKids->Next();

    if (!myKids->AtEnd())
      myKids->Current()->reset();

    return STOP;
  }
  else
    return CONTINUE;
}

void DataLoopValListNode::reset()
{
  myKids->Reset();
  if (!myKids->AtEnd())
    myKids->Current()->reset();
}

int DataLoopValListNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of DataNodes in this object:
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	curLen = myKids->Current()->myLongestStr();
	if( curLen > retVal )
	    retVal = curLen;
    }
    return retVal;
}

bool LoopRowNode::insert( iterator pos, DataValueNode &v )
{   if( pos.here.lrn != this )
	return false;
    else
	insertElementAt( pos.here.col, v ); 
    return true;
}
/** TODO - document this: give node, erather than copy node
  */
bool LoopRowNode::insert( iterator pos, DataValueNode *v )
{   if( pos.here.lrn != this )
	return false;
    else
	insertElementAt( pos.here.col, *v ); 
    v->setParent(this);
    ValCache::addToValCache( this, pos.here.col, v );
    return true;
}

LoopRowNode::iterator LoopRowNode::iteratorFor( const DataValueNode *ptr )
{
    iterator i;

    for( i = begin() ; i != end() ; i++ )
	if( *i == ptr )
	    break;

    return i;
}

/// erase - Remove the value given.
bool LoopRowNode::erase( iterator pos )
{   if( pos.here.lrn != this )
	return false;
    else if(  pos.here.lrn->myParent() != NULL &&
	      pos.here.lrn->myParent()->myParent() != NULL )
    {
	return false;  // not allowed to remove a value from
		       // a row that is alreaddy attached to
		       // a loop.
    }
    else
	pos.here.lrn->removeElementAt( pos.here.col );
    return true;
}

// commented out - too much work, and nobody uses it:
//   (erases the range [start,finish) (not including finish). )
// bool erase( iterator start, itareator finish );
//@}

LoopRowNode::LoopRowNode(const bool tflag)
{
  bitFields.tFlag = tflag;
  cur = 0;
  innerLoop = NULL;
  myVal = string("");
  myStarts = vector<short>();
}

LoopRowNode::LoopRowNode(const bool tflag, size_t initSize) 
{
  bitFields.tFlag = tflag;
  cur = 0;
  innerLoop = NULL;
  myVal = string("");
  myStarts = vector<short>();
  myStarts.reserve( initSize );
}


bool LoopRowCol::operator==( const LoopRowCol compareTo ) const 
{
    size_t numCols = lrn->size();
    if( lrn == compareTo.lrn )
    {
	if( col == compareTo.col )
	    return true;
	else if( col == -1 || compareTo.col == -1 )
	    return true;
	else if( col >= numCols && compareTo.col >= numCols )
	    return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////
//Copy constructor for LoopRowNode
/////////////////////////////////////////////////////////////////
LoopRowNode::LoopRowNode(IterNode& N)
{
    (void)copyFrom( N, false );
}

LoopRowNode::LoopRowNode( bool link, IterNode& N)
{
    (void)copyFrom( N, link );
}

LoopRowNode::LoopRowNode( vector<DataValueNode*> &v, const bool tflag )
{
    short  i;

    bitFields.tFlag = tflag;
    innerLoop = NULL;
    myVal = string("");
    myStarts = vector<short>();
    myStarts.reserve( v.size() );

    for( i = 0 ; i < v.size() ;  i++ )
	insertElementAt( i, *(v[i]) );
    DELETE_IF_NOT_ALREADY(v[i]);

    cur = 0;
}

bool LoopRowNode::copyFrom( IterNode &fromThis, bool linkWith )
{
    short         col;
    DataValueNode *dvn, *dvnNewCopy;


    LoopRowNode &N = fromThis;

    ASTnode::copyFrom( fromThis );
    bitFields.tFlag = N.bitFields.tFlag;

    innerLoop = NULL;
    myVal = N.myVal;
    myStarts = N.myStarts;

    preComment = N.preComment;

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }

    // If anything exists in the cache for the old LoopRow, copy
    // it for the new LoopRow.  This is needed because there might
    // be altered values for the old LoopRow that are only represented
    // via its cache.
    for( col = 0 ; col < fromThis.size() ; col++ )
    {
	dvn = ValCache::getFromValCache( &(fromThis), col );
	if( dvn != NULL )
	{
	    dvnNewCopy = new DataValueNode( linkWith, *dvn );
	    dvnNewCopy->setParent(this);
	    ValCache::addToValCache( this, col, dvnNewCopy );
	}
    }
    if( fromThis.innerLoop != NULL )
    {   innerLoop = new LoopTableNode( linkWith, *(fromThis.innerLoop) );
	innerLoop->setParent(this);
    }

    cur = N.cur;

    return true;
}
///////////////////////////////////////////////////////////////////

LoopRowNode::~LoopRowNode()
{
  int i;
  bitFields.isDeleting = true;

  if( parent != NULL )
      for( i = 0 ; i < myStarts.size() ; i++ )
	  ValCache::removeFromValCache( this, i );

  if( innerLoop != NULL )
      DELETE_IF_NOT_ALREADY(innerLoop);

  unlinkMe();
  myStarts.clear();
  myVal = string("");
  bitFields.isDeleting = false;
}

DataValueNode *LoopRowNode::generateVal( short pos )
{
    DataValueNode *retVal;
    char          delimChar;


    delimChar = myVal.c_str()[ myStarts[pos] ];
    retVal = new DataValueNode(
		    myVal.substr(
				myStarts[pos]+1,
			        ( ( pos < myStarts.size()-1 ) ?
			            myStarts[pos+1] : myVal.length() )
				       - (myStarts[pos]+1)
			      ),
		    ( delimChar == (char)DataValueNode::SINGLE ) ?
			DataValueNode::SINGLE :
		    ( delimChar == (char)DataValueNode::DOUBLE ) ?
			DataValueNode::DOUBLE :
		    ( delimChar == (char)DataValueNode::SEMICOLON ) ?
			DataValueNode::SEMICOLON :
		    ( delimChar == (char)DataValueNode::FRAMECODE ) ?
			DataValueNode::FRAMECODE :
			DataValueNode::NON  // default or '\001
		);
    retVal->setParent( this );
    retVal->setLineNum( getLineNum() );
    retVal->setColNum( getColNum() );
    return retVal;
}

DataValueNode *&LoopRowNode::elementAt( short pos )
{
    // First see if it it already cached:
    DataValueNode *&retVal = ValCache::getFromValCache( this, pos );
    DataValueNode *retValPtr;

    // Second, if that doesn't work, make it anew:
    if( retVal == NULL )
    {
	retValPtr = generateVal( pos );
	ValCache::addToValCache( this, pos, retValPtr );
	// Now get the actual one out of the cache:
	retVal = ValCache::getFromValCache( this, pos );
    }
    return retVal;
}

void LoopRowNode::debugDump( void )
{
    short i;

    cerr << "DEBUGDUMP of LOOPROWNODE:" << endl;
    cerr << "   this = " << this << ", I have " << myStarts.size() << " elements."
	<< " innerLoop = " << innerLoop <<endl;
    cerr << "   My val is: ";
    for( i = 0 ; i < myVal.length() ; i++ )
    {
	if( myVal[i] < 26 )
	    cerr << "^" << (char)( 'A' + myVal[i] - 1 );
	else
	    cerr << myVal[i];
    }
    cerr << "   MyStarts array: ";
    for( i = 0 ; i < myStarts.size() ; i++ )
    {
	cerr << "[" << i << "]=" << myStarts[i] << ", ";
    }
    cerr << endl;
}

void LoopRowNode::insertElementAt( short pos, DataValueNode &v)
{
    internalInsertElementAt( pos, v, false );
}
void LoopRowNode::privateInsertElementAt( short pos, DataValueNode &v)
{
    internalInsertElementAt( pos, v, true );
}
void LoopRowNode::internalInsertElementAt( short pos, DataValueNode &v,
	                                   bool priv )
{
    vector<short>::iterator iPos;
    string  insertStr;
    short   offset, i , delim;
    short   oldsize;
    char    tmp[2];
    bool    done;

    if( ! priv && isInLoop() )
	return;

    insertStr = string("");
    delim = v.myDelimType();

    // For some reason the basic_string::append(int,char) method
    // has inverted parameters from the standard in the C++ libs
    // on one of our platforms, so I can't portably use it.  That
    // is why this is done with a temp char* string.
    tmp[0] = (char)delim;
    tmp[1] = '\0';
    insertStr.append( tmp );

    insertStr.append( v.myStrVal );

    // First ,insert the value into the string:
    offset = insertStr.length();
    myVal.insert( ( pos < myStarts.size() ) ? myStarts[pos] : myVal.length(),
	          insertStr );

    // Add one more value to the myStarts array, to ensure its
    // size has the right number of elements now.
    // NOTE: If this ends up being an insert at the end of the row, then
    // these dummy values will actually be left in place and used.
    // else they will be clobbered down below.  (So I guess they aren't
    // entirely 'dummy' values.)

    if( myStarts.size() == 0 ) // empty row being started:
    {
	myStarts.insert( myStarts.end(), 0 );
    }
    else
    {
	myStarts.insert( myStarts.end(), myVal.length() -  offset );
    }

    // Shift all values after pos up one posiiton, and add offset to them.
    // The myStarts entry for the new value will be the same as the old one
    // that used to be in that spot, because that's the cahracter position
    // at which the new value was inserted.
    for( i = myStarts.size() - 1; i > pos ; i-- )
    {
	myStarts[i] = myStarts[i-1] + offset ;
	ValCache::moveInValCache( this, i-1, this, i );
    }

} 

string LoopRowNode::stringAt( const short pos )
{
    DataValueNode *dvn;

    dvn = elementAt( pos );
    if( dvn != NULL )
	return dvn->myValue();
    else
	return string("");
}

void LoopRowNode::insertStringAt( const short pos, const string &val )
{
    DataValueNode dvn( val );
    dvn.changeToAppropriateDelimiter();
    privateInsertElementAt( pos, dvn );
}

void LoopRowNode::setStringAt( const short pos, const string &val )
{
    DataValueNode dvn( val );
    dvn.changeToAppropriateDelimiter();
    privateSetElementAt( pos, dvn );
}

void LoopRowNode::removeElementAt( short pos )
{
    internalRemoveElementAt( pos, false );
}

void LoopRowNode::privateRemoveElementAt( short pos )
{
    internalRemoveElementAt( pos, true );
}

void LoopRowNode::internalRemoveElementAt( short pos, bool internalCall )
{
    int   offset;
    int   i;

    if( isInLoop() && !internalCall )
	return;


    offset = ( (pos < myStarts.size() - 1) ? myStarts[pos+1] : myVal.length() ) 
	         - myStarts[pos];
    erase_str(myVal)( myStarts[pos], offset );
    ValCache::removeFromValCache( this, pos );
    for( i = pos ; i < myStarts.size() - 1 ; i++ )
    {
	myStarts[i] = myStarts[i+1] - offset;
	ValCache::moveInValCache( this, i+1, this, i );
    }
    myStarts.erase( myStarts.end()-1 );

    /* If that was the last element in this row, and this row is in a
     * loop, then remove this row entirely, getting rid of it from its
     * parent too.
     */
    if( isInLoop() && myStarts.size() == 0 )
    {
	DELETE_IF_NOT_ALREADY(this);
    }

    return;
}


void LoopRowNode::setElementAt( short pos, DataValueNode &val )
{
    DataValueNode *dvn;
    privateSetElementAt( pos, val );

    // Change the val cache too, if it's there:
    dvn = ValCache::getFromValCache( this, pos );
    if( dvn != NULL )
    {
	dvn->setValue( val.myValue() );
	dvn->setDelimType( val.myDelimType() );
    }

}

// Some static members to instantiate:
map<const LoopRowCol, DataValueNode*, LoopRowCol> ValCache::cache;

void ValCache::flushValCache(void)
{
    // First, set the loop's big string value to match the
    // values of the things in the cache, second get rid
    // of the things in the cache.
    map<const LoopRowCol, DataValueNode*, LoopRowCol>::iterator cur;
    DataValueNode *dvn;

    for( cur = ValCache::cache.begin() ; cur != ValCache::cache.end() ; cur++ )
    {
	 dvn = (*cur).second;
	 if( dvn != NULL )
	 {
	     (*cur).first.lrn->privateSetElementAt( (*cur).first.col, *dvn );
	 }
	 DELETE_IF_NOT_ALREADY(dvn);
    }
    ValCache::cache.clear();
}

void LoopRowNode::flushValCache(void)
{
    // First, set the loop's big string value to match the
    // values of the things in the cache, second get rid
    // of the things in the cache.
    short         i;
    DataValueNode *dvn;

    for( i = 0 ; i < size() ; i++ )
    {
	 dvn = ValCache::getFromValCache( this, i );
	 if( dvn != NULL )
	 {
	     privateSetElementAt( i, *dvn );
	     ValCache::removeFromValCache( this, i );
	 }
	 DELETE_IF_NOT_ALREADY(dvn);
    }
}

void LoopRowNode::privateSetElementAt( short pos, DataValueNode &val )
{
    string insertStr = string("");
    int    offset;
    int    idx;
    short  delim;
    char   tmpStr[2];

    delim = val.myDelimType();
    if( delim == DataValueNode::SINGLE )
    {
	tmpStr[0] = (char)DataValueNode::SINGLE ; tmpStr[1] = '\0';
        insertStr.append( tmpStr );
	insertStr.append( val.myValue() );
    }
    else if( delim == DataValueNode::DOUBLE )
    {
	tmpStr[0] = (char)DataValueNode::DOUBLE ; tmpStr[1] = '\0';
        insertStr.append( tmpStr );
	insertStr.append( val.myValue() );
    }
    else if( delim == DataValueNode::SEMICOLON )
    {
	tmpStr[0] = (char)DataValueNode::SEMICOLON ; tmpStr[1] = '\0';
        insertStr.append( tmpStr );
	insertStr.append( val.myValue() );
    }
    else if( delim == DataValueNode::FRAMECODE )
    {
	tmpStr[0] = (char)DataValueNode::FRAMECODE ; tmpStr[1] = '\0';
        insertStr.append( tmpStr ); 
	insertStr.append( val.myValue() );
    }
    else // NON or Dont-care
    {
	tmpStr[0] = (char)DataValueNode::NON ; tmpStr[1] = '\0';
        insertStr.append( tmpStr );
	insertStr.append( val.myValue() );
    }

    // offset = difference between the new length and the old length:
    offset = insertStr.length() - (
	        (  ( pos < myStarts.size() -1 ) ?
		       myStarts[pos+1] : myVal.length() )
		- myStarts[pos] ) ;
    erase_str(myVal)( myStarts[pos], ( ( pos < myStarts.size() - 1 ) ?
	                              myStarts[pos+1] : myVal.length() )
	                        - myStarts[pos]  );
    myVal.insert( myStarts[pos], insertStr );

    // Shift the remaining starting indeces forward to match the
    // change in length of this value: (shifts backward if offset
    // is calculated as a negative number, meaning the new string
    // is shorter than the old.
    for( idx = pos + 1 ; idx < myStarts.size() ; idx++ )
    {
	myStarts[idx] = myStarts[idx] + offset;
    }

}

void LoopRowNode::setMyLoop( LoopTableNode *tbl )
{
    innerLoop = tbl;
    innerLoop->setParent( (ASTnode*)this );
}

int LoopRowNode::isInLoop( void )
{
    bool    foundLoop = false;
    int     count = 0;
    ASTnode  *ancestor;

    for(   ancestor = this ;
	   ancestor != NULL && foundLoop == false  ;
	   ancestor = ancestor->myParent() )
    {
	if( ancestor->isOfType( ASTnode::DATALOOPNODE ) )
	    foundLoop = true;
	if( ancestor->isOfType( ASTnode::LOOPTABLENODE ) )
	    count++;  // Every LoopNode encountered on the way up
	              // the parent is another level of nesting.
    }
    if( ancestor == NULL )
	return 0;
    else
	return count;
}

void LoopRowNode::addValue(DVNWithPos *value)
{
  privateInsertElementAt( myStarts.size(), *value );
  value->setParent(this);
}

void LoopRowNode::addInnerLoopValue(const DataLoopNameListNode::Status dstat,
				 const int row,
				 const int currRow,
				 DVNWithPos *value)
{
  if (!innerLoop)
    if (bitFields.tFlag)
      innerLoop = new LoopTableNode(string("tabulate"));
    else
      innerLoop = new LoopTableNode(string("linear"));

  innerLoop->setParent(this);

  if( dstat != DataLoopNameListNode::STOP )
      innerLoop->addValue(dstat, row, currRow, value);
}

bool LoopRowNode::unlinkChild( ASTnode *child )
{
    short  i;
    if( innerLoop == child )
    {   innerLoop = (LoopTableNode*)NULL;
	return true;
    }
    /* If it's in the cache, take it out of the cache: */
    for( i = 0 ; i < myStarts.size() ; i++ )
    {   if( parent != NULL )
	{
	    if( ValCache::getFromValCache( this, i ) == child )
	    {   privateRemoveElementAt( i );
		return true;
	    }
	}
    }
    return false; // pessamistic default.
}


List <ASTnode*> *LoopRowNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    int            i;
    DataValueNode  *nextMatch;
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *matches;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    if( innerLoop != NULL )
    {
	matches = innerLoop->searchForType( type, delim );
	if( matches->Length() > 0 )
	    retVal->AddToEnd( *matches );
	delete  (matches);
    }

    /* Check to see if any of my own values are of the right type: */
    for( i = 0 ; i < myStarts.size() ; i++ )
    {
	bool generated = false ;
	/* get my i-th value: */
	nextMatch = ValCache::getFromValCache( this, i );
	if( nextMatch == NULL )
	{
	    nextMatch = generateVal( i ); /* generate the DVN, but don't cache it */
	    generated = true;
	}
	/* If my i-th value matches, add it to the list: */
	if( type == ASTnode::DATAVALUENODE && nextMatch->myDelimType() == delim )
	{
	    retVal->AddToEnd( nextMatch );
	    ValCache::addToValCache( this, i, nextMatch );
	}
	else if( generated ) /* If a value was generated and didn't match */
	    DELETE_IF_NOT_ALREADY(nextMatch);
    }
    return retVal;
}

int LoopRowNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;
    int i;
    DataValueNode *dvn;

    // Return the longest of the 'myLongestStr()'s from the
    // list of DataNodes in this object:
    for( i = 0 ; i < myStarts.size() ; i++ )
    {
	dvn = generateVal( i );
	dvn->setParent( NULL );
	curLen = dvn->myLongestStr();
	DELETE_IF_NOT_ALREADY(dvn);
	if( curLen > retVal )  retVal = curLen;
    }
    return retVal;

}


///////////////////////////////////////////////////////////////////////////
//Copy constructor for LoopTableNode Node
//////////////////////////////////////////////////////////////////////////

LoopTableNode::LoopTableNode(LoopTableNode& I)
{
    (void)copyFrom( I, false );
}

LoopTableNode::LoopTableNode( bool link, LoopTableNode& I)
{
    (void)copyFrom( I, link );
}

bool LoopTableNode::copyFrom( LoopIter &fromThis, bool linkWith )
{
    ASTnode::copyFrom( fromThis );
    bitFields.tFlag = fromThis.bitFields.tFlag;
    bitFields.indFlag = fromThis.bitFields.indFlag;
    rowsPerLine = fromThis.rowsPerLine;

    setPreComment( fromThis.getPreComment() );
    LoopRowNode *remember = (fromThis.myIters.AtEnd()) ? 
				(LoopRowNode*)NULL : fromThis.myIters.Current();
    myIters.FreeList();
    myIters.setParent(this);
    myIters.reserve( fromThis.myIters.size() );
    for( fromThis.myIters.Reset() ; ! fromThis.myIters.AtEnd() ; fromThis.myIters.Next() )
	myIters.AddToEnd( new LoopRowNode( linkWith, *(fromThis.myIters.Current()) )   );
    for( myIters.Reset() ; ! myIters.AtEnd() ; myIters.Next() )
	myIters.Current()->setParent(this);
    (void)fromThis.myIters.SeekTo( remember );

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////


LoopTableNode::LoopTableNode(const string & tflag, bool indentFl, int rowsPerLn )
{
  myIters.setParent(this);
  if (tflag == "tabulate")
    bitFields.tFlag = true;
  else
    bitFields.tFlag = false;
  bitFields.indFlag = indentFl;
  rowsPerLine = rowsPerLn;
  preComment = string("");
}

LoopTableNode::LoopTableNode(bool tflag )
{
  myIters.setParent(this);
  bitFields.tFlag = tflag;
  bitFields.indFlag = true;
  rowsPerLine = 1;
  preComment = string("");
}

LoopTableNode::~LoopTableNode()
{
  bitFields.isDeleting = true;
  unlinkMe();
  flushValCache();
  for (myIters.Reset(); !myIters.AtEnd(); myIters.Next())
    if( myIters.Current() != NULL )
	DELETE_IF_NOT_ALREADY(myIters.Current());
  myIters.erase( myIters.begin(), myIters.end() );
  bitFields.isDeleting = false;
}


List<int> *LoopTableNode::checkValueAlignment(void)
{
    List<int>   *retVal = new List<int>();
    List<int>   *recurseRetVal;
    LoopRowNode *lpRow;
    int         rowIdx, valIdx, recIdx;
    int         lnum;  // line number

    for( rowIdx = 0 ; rowIdx < myIters.size() ; rowIdx++ )
    {
	lnum = -1;
	if( bitFields.tFlag ) // Don't bother checking the values in a linear loop.
	{
	    for( valIdx = 0 ; valIdx < (*myIters[rowIdx]).size() ; valIdx++ )
	    {
		if( lnum == -1 )
		{
		    lnum = (*myIters[rowIdx])[valIdx]->getLineNum();
		}
		else if( (*myIters[rowIdx])[valIdx]->myDelimType() ==
			     DataValueNode::SEMICOLON )
		{
		    // This is a semicolon delimited string, give up on
		    // trying to examine this row, assume it is good. 
		    break;
		}
		else if( lnum != (*myIters[rowIdx])[valIdx]->getLineNum() )
		{
		    retVal->insert( retVal->end(), lnum );
		    break;  // Once one misalignment is found on a row,
		            // don't bother with the rest of the row.
		}
	    }
	}
	// If there is an inner loop, then check it recursively,
	// and append the results to this retVal.
	if( myIters[rowIdx]->myLoop() != NULL )
	{
	    recurseRetVal =
		myIters[rowIdx]->myLoop()->checkValueAlignment();

	    for( recIdx = 0 ; recIdx < recurseRetVal->size() ; recIdx++ )
		retVal->insert( retVal->end(), (*recurseRetVal)[recIdx] );

	    delete  (recurseRetVal);
	}
    }
    return retVal;
}


bool LoopTableNode::insert(
	iterator    pos,
	vector<DataValueNode*> &v,
	const bool  tflag )
{
    LoopRowNode  *newRow;
    if( v.size() != getNumCols() )
    {
	return false;
    }

    newRow = new LoopRowNode( v , tflag );
    myIters.insert( pos, newRow );

    // If I am not at the deepest level and I have no
    // inner loop, make an empty inner loop so that there
    // will be a "stop_" printed:
    if( newRow->innerLoop == NULL )
    {
	if( getMyDepth() < getMaxDepth() )
	{
	    newRow->innerLoop = new LoopTableNode( tflag );
	    newRow->innerLoop->setParent( newRow );
	}
    }
    return true;
}

bool LoopTableNode::insert( iterator pos, LoopRowNode &v )
{
    return insert( pos, new LoopRowNode( v ) );
}

bool LoopTableNode::insert( iterator pos, LoopRowNode *v )
{
    if( parent && v->size() != getNumCols() )
    {
	return false;
    }

    myIters.insert( pos, v );

    // If I am not at the deepest level and I have no
    // inner loop, make an empty inner loop so that there
    // will be a "stop_" printed:
    if( parent && v->innerLoop == NULL )
    {
	if( getMyDepth() < getMaxDepth() )
	{
	    v->innerLoop = new LoopTableNode( v->bitFields.tFlag );
	    v->innerLoop->setParent( v );
	}
    }
    return true;
}

bool LoopTableNode::insert( iterator pos, LoopTableNode &t )
{
    return insert( pos, new LoopTableNode( t ) );
}

bool LoopTableNode::insert( iterator pos, LoopTableNode *t )
{
    LoopRowNode  *newRow;
    iterator     i;

    for( i = t->begin() ; i != t->end() ; i++ )
    {
	if( (*i)->size() != getNumCols() )
	    return false;
	newRow = new LoopRowNode( **i );
        myIters.insert( pos, newRow );

	// If I am not at the deepest level and I have no
	// inner loop, make an empty inner loop so that there
	// will be a "stop_" printed:
	if( parent && newRow->innerLoop == NULL )
	{
	    if( getMyDepth() < getMaxDepth() )
	    {
		newRow->innerLoop = new LoopTableNode(newRow->bitFields.tFlag);
		newRow->innerLoop->setParent( newRow );
	    }
	}
    }
    return true;
}

LoopTableNode::iterator LoopTableNode::iteratorFor( const LoopRowNode *ptr )
{
    iterator i;

    for( i = begin() ; i != end() ; i++ )
	if( *i == ptr )
	    break;

    return i;
}

void LoopTableNode::erase( iterator pos )
{
    iterator i;

    DELETE_IF_NOT_ALREADY(*pos); // deletes the LoopRowNode, who's destructor
                 // will remove itself from me.
}

void LoopTableNode::erase( iterator from, iterator to )
{
    iterator i;

    // delete the things being pointed at too:
    for( i = from ; i != to ; i++ )
    {
	DELETE_IF_NOT_ALREADY(*i);  // deletes the LoopRowNode, who's destructor will
	            // remove itself from me.
    }
}

string LoopTableNode::getString( int row, int col )
{
    LoopRowNode *lrn;
    if( row >= myIters.size() || row < 0 )
	return string("");
    lrn = myIters[row];
    if( col >= lrn->size() || col < 0 )
	return string("");
    return lrn->stringAt(col);
}
void LoopTableNode::setString( int row, int col, const string &val )
{
    LoopRowNode *lrn;
    if( row >= myIters.size() || row < 0 )
	return;
    lrn = myIters[row];
    if( col >= lrn->size() || col < 0 )
	return;
    lrn->setStringAt( col, val );
    return;
}

void LoopTableNode::truncateNestLevel( int depth )
{
    iterator   i;
    int        rowNum;

    // If it's talking about me: then commit suicide:
    if( depth == 0 )
    {
	// Only commit suicide if I am nested down in.
	// If I am at the outermost loop level and my
	// parent is a DataLoopNode, then don't delete me
	// because I was never really "new"ed in the first
	// place, I was instantiated as a real object inside
	// the DataLoopNode, not as a pointer.
	if( myParent()->isOfType( ASTnode::DATALOOPNODE ) )
	{
	    // Must iterate backward, and must use numbers not
	    // iterators, since iterators can be invalidated by
	    // the delete operations on the vector we are doing.
	    for( rowNum = size()-1 ; rowNum >= 0 ; rowNum-- )
	    {
		DELETE_IF_NOT_ALREADY(((*this)[rowNum]));
	    }
	}
	else
	{
	    unlinkMe();
	    parent = NULL;
	    DELETE_IF_NOT_ALREADY(this);
	}
    }
    // Otherwise recursively drop a level:
    else
    {
	// Must iterate backward because there is a chance
	// that these truncations will delete things from
	// my list, and therefore invalidate my iterators.
	// (Which is also why I must use an integer iteration
	// rather than a real iterator):
	for( rowNum = size()-1 ; rowNum >= 0  ; rowNum-- )
	{
	    if( (*this)[rowNum]->myLoop() != NULL )
		(*this)[rowNum]->myLoop()->truncateNestLevel(depth-1);
	}
    }
}

void LoopTableNode::setTabFlag( bool setTo )
{
    int  i;
    bitFields.tFlag = setTo;

    // Change all the children's flags too:
    for( i = 0 ; i < size() ; i++ )
	(*this)[i]->setTabFlag( setTo );
}

bool LoopTableNode::getTabFlag( void )
{
    return bitFields.tFlag;
}

int LoopTableNode::getNumCols( void )
{
    ASTnode  *ancestor;
    int      retVal = 0;
    int      nestLevel;

    // Get the DataLoopNode that I am inside of:
    for(   ancestor = this , nestLevel = -1;
	   ancestor != NULL  && ! ancestor->isOfType( ASTnode::DATALOOPNODE ) ;
	   ancestor = ancestor->myParent()  )
    { 
	if( ancestor->isOfType( ASTnode::LOOPTABLENODE ) )
	    nestLevel++;
    }
    if( ancestor == NULL )  // Not contained inside a DataLoopNode
	return 0;
    if( nestLevel < 0 ) //  weird should never happen case.
	return 0;

    return  (((DataLoopNode*)ancestor)->getNames())[nestLevel]->size() ;
}

int LoopTableNode::getMyDepth( void )
{
    ASTnode *ancestor;
    int     retVal;

    for(    ancestor = this , retVal = -1;
	    ancestor != NULL && ! ancestor->isOfType(ASTnode::DATALOOPNODE) ;
	    ancestor = ancestor->myParent()  )
    {
	if(   ancestor->isOfType(ASTnode::LOOPTABLENODE)  )
	    retVal++;
    }

    return retVal;
}

void LoopTableNode::makeNewNestLevel( void )
{
    iterator i;

    for( i = begin() ; i != end() ; i++ )
	if( (*i)->innerLoop == NULL )
	{
	    (*i)->innerLoop = new LoopTableNode( bitFields.tFlag );
	    (*i)->innerLoop->setParent( (*i) );
	}
	else
	    (*i)->innerLoop->makeNewNestLevel();
}

int LoopTableNode::getMaxDepth( void )
{
    ASTnode *ancestor;
    int     retVal;

    for(    ancestor = this ;
	    ancestor != NULL && ! ancestor->isOfType(ASTnode::DATALOOPNODE) ;
	    ancestor = ancestor->myParent()  )
    { }
    if( ancestor->isOfType(ASTnode::DATALOOPNODE) )
	retVal = ((DataLoopNode*)ancestor)->getMaxDepth();
    else
	retVal = -1;

    return retVal;
}

void LoopTableNode::addValue(const DataLoopNameListNode::Status dstat,
			const int row,
			const int currRow,
			DVNWithPos *value)
{
  LoopRowNode *lastIter = myIters.Last();

  if( row == currRow && dstat != DataLoopNameListNode::STOP )
    if (dstat == DataLoopNameListNode::SAME_ITER)
      lastIter->addValue(value);
    else {
      lastIter = new LoopRowNode(bitFields.tFlag);
      lastIter->setParent(this);
      lastIter->addValue(value);
      lastIter->setLineNum( value->getLineNum() );
      lastIter->setColNum( value->getColNum() );
      myIters.AddToEnd(lastIter);
    }
  else
  {
    lastIter->addInnerLoopValue(dstat, row, currRow + 1, value);
  }
}
/* end of COMMENTING_OUT */

bool LoopTableNode::unlinkChild( ASTnode *child )
{
    bool retVal = false;
    for( myIters.Reset() ; ! myIters.AtEnd() ; myIters.Next() )
    {   if( myIters.Current() == child )
	{   myIters.RemoveCurrent();
	    retVal = true;
	    break;
 	}
    }
    // Never delete myself here.  A loop table with zero rows is
    // legal because it is needed at nested levels.
    return retVal;
}

List<ASTnode*> * LoopTableNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *matches;

    // WARNING:   AN UGLY HACK FOR SPEED PURPOSES HERE:
    //     Over 50% of the data in a typical STAR file is
    //     contained inside loops.  To sift down through
    //     all the values in the loop when we already know
    //     it is impossible to find a match of the kind we
    //     are looking for is a huge time waster.  So here,
    //     we truncate the search at this point if we know that
    //     the type being searched for is one that cannot exist
    //     inside a loop.  I consider this an ugly hack because
    //     it requires that this code has knowlege of how the
    //     entire tree is laid out.  This means the code is
    //     built on the fragile assumption that the tree structure
    //     used in this library won't change.  Theoretically,
    //     this search-truncation could have been done at
    //     every level of the tree for even more speed, but then
    //     it would have been a lot of fragile assumptions built
    //     on top of fragile assumptions.
    //
    switch( type )
    {
	case    ASTNODE :
	case    DATAVALUENODE :
	case	LOOPROWNODE :
	case	LOOPTABLENODE :

		    if( isOfType( type ) )
			retVal->AddToEnd( this );

		    for( myIters.Reset() ; ! myIters.AtEnd() ; myIters.Next() )
		    {
			matches = myIters.Current()->searchForType( type, delim );
			if( matches->Length() > 0 )
			    retVal->AddToEnd( *matches );
			delete  (matches);
		    }

		    break;
	default:
		    {} /* If it isn't the right type, Don't waste time iterating
			  through the loop - just truncate the search here. */
    }
    return retVal;
}

int LoopTableNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of DataNodes in this object:
    for( myIters.Reset() ; ! myIters.AtEnd() ; myIters.Next() )
    {
	curLen = myIters.Current()->myLongestStr();
	if( curLen > retVal )
	    retVal = curLen;
    }
    return retVal;
}

void LoopTableNode::calcPrintSizes(
      List<int> **presizes,
      List<int> **postsizes,
      List<int> **allNonQuoteds
      )
{
    LoopRowNode  *curRow;
    int       numCols = 0;
    int       curCol = 0;
    string    curStr;
    int       curPre;
    int       curPost;
    int       curIdx;
    int       dotIdx;
    char      *tmpStr;

    if( myIters.Length() > 0 )
    {
	myIters.Reset();
	if( myIters.Current() != NULL )
	{
	    numCols = myIters.Current()->size();
	}
    }
    *presizes = new List<int>;
    *postsizes= new List<int>;
    *allNonQuoteds= new List<int>;
    
    // Make the starting stub column info for each column:
    for( curCol = 0 ; curCol < numCols ; curCol++ )
    {
	(*presizes)->AddToEnd( 0 );
	(*postsizes)->AddToEnd( -1 );
	(*allNonQuoteds)->AddToEnd( 1 );
    }
    if( numCols == 0 )
	return;  // give up - nothing here;

    // For each row in the table:
    for( myIters.Reset() ; ! myIters.AtEnd() ; myIters.Next() )
    {
	short           col;
	DataValueNode   *dvn;

	curRow = myIters.Current();
	(*presizes)->Reset();
	(*postsizes)->Reset();
	(*allNonQuoteds)->Reset();
	// For each value in this row:
	for( col = 0 ; col < curRow->size() ; col++ )
	{
	    dvn = curRow->generateVal( col );
	    curStr = dvn->myValue();
	    curPre = dvn->myLongestStr();

	    curPost = -1;
	    int thisColHasDelimits = ! (int)( (*allNonQuoteds)->Current() ) ;
	    // The numeric check:  Is it even eligable to be a number?
	    if( dvn->myDelimType() == DataValueNode::NON )
	    {
		tmpStr = strdup( curStr.c_str() );
		// Two things in one - checking for valid syntax and
		// finding the decimal point.
		// -------------------------------------------------
	
		curIdx = 0;
		dotIdx = -1;
		// Consume leading whitespace:
		while( tmpStr[curIdx] == ' ' || tmpStr[curIdx] == '\t' )
		    curIdx++;
		// Consume optional pos/neg sign:
		if( tmpStr[curIdx] == '-' || tmpStr[curIdx] == '+' )
		    curIdx++;
		// Now a string of digits ended by a dot or whitespace or
		// the end of the string.  Anything else is an indicator
		// that this is not a true numeric value:
		while( isdigit( tmpStr[curIdx] ) )
		    curIdx++;
		curPre = curIdx+1;
		if( tmpStr[curIdx] == '.' )
		{   dotIdx = curIdx;
		    curIdx++;
		}
		curPost = 0;
		while( isdigit( tmpStr[curIdx] ) )
		{
		    curPost++; // count digits after the dot.
		    curIdx++;
		}
		while( isspace( tmpStr[curIdx] ) )  // trailing whitespace
		    curIdx++;

		// Okay, so the check is: if it stopped because it hit the
		// end of the string - the string was fully numeric, else
		// it had other garbage in it.  If the string was nothing
		// but a dot, then calc the size as if it were non-numeric.
		// (This has the effect that if the only 'numerics' in a
		// column are really dots, then the dots get printed left-
		// justified, but if any 'real' numerics exist, then the dots
		// get printed decimal-point justified.)
		if( tmpStr[curIdx] != '\0' || ( strcmp( tmpStr, "." ) == 0 ) )
		{
		    // Not numeric after all, so reset to the values
		    // for text strings:
		    curPost = -1;
	            curPre = dvn->myLongestStr();
		}
		free(tmpStr);
	    }
	    else if( dvn->myDelimType() == DataValueNode::SEMICOLON )
	    {
		curPre = 0; // If this is a semicolon delimited string, size doesn't matter.
		thisColHasDelimits = 1;
	    }
	    else
	    {
		thisColHasDelimits = 1;
	    }

	    if( (*presizes)->Current() < curPre )
		(*presizes)->AlterCurrent( curPre );
	    
	    if( (*postsizes)->Current() < curPost )
		(*postsizes)->AlterCurrent( curPost );

	    (*allNonQuoteds)->AlterCurrent( ! thisColHasDelimits );
	    
	    (*presizes)->Next();
	    (*postsizes)->Next();
	    (*allNonQuoteds)->Next();

	    DELETE_IF_NOT_ALREADY(dvn);
	}
    }
}

void ValCache::addToValCache(
	LoopRowNode *lrn, short col, DataValueNode *val )
{
    LoopRowCol lrc;

    lrc.lrn = lrn;
    lrc.col = col;
    cache[ lrc ] = val;
    val->setIsCache( true );
}

void ValCache::moveInValCache( LoopRowNode *lrnFrom, short colFrom,
	             LoopRowNode *lrnTo, short colTo )
{
    LoopRowCol lrcFrom;
    LoopRowCol lrcTo;
    DataValueNode *valToMove, *valToClober;

    /*If the old To-value exists in the cache, remove it from the cache:*/
    if( getFromValCache( lrnTo, colTo ) != NULL )
	removeFromValCache( lrnTo, colTo );

    /* Get the Value to move, if it is not in the cache, then abort,
     * having effectively replaced the old to-value with the new
     * from-value (which is non existant, so deleting the to-value
     * from cache and stopping at that is actually the right thing
     * to do [replaces the to-value with nothing]).
     */
    valToMove = getFromValCache( lrnFrom, colFrom );
    if( valToMove == NULL )
	return;
    else
    {
	lrcFrom.lrn = lrnFrom;
	lrcFrom.col = colFrom;
	lrcTo.lrn = lrnTo;
	lrcTo.col = colTo;
	cache[ lrcTo ] = valToMove;
	cache.erase( lrcFrom );
    }
}

DataValueNode *&ValCache::getFromValCache( LoopRowNode *lrn, short col )
{
    static DataValueNode *nullToRef = NULL;
    LoopRowCol lrc;
    lrc.lrn = lrn;
    lrc.col = col;

    nullToRef = NULL; // Just in case some smart-alek changed it on us, which
                    // is technically possible since it is passed back
		    // as a reference.  I only pass it back as a reference
		    // because I want to pass back the VALID values as
		    // references to the pointer in the stack, and I can't
		    // do that unless I pass back NULL results as references
		    // to pointers as well.

    DataValueNode *&retVal1 = (  *( ValCache::cache.find( lrc ) )  ).second;

    // C++ STL routines return end iterators to indicate failures,
    // since there is no "null" in iterator parlance.
    if( retVal1 == ValCache::cache.end()->second )
    {
	DataValueNode *&retVal2 = nullToRef;
	return retVal2;
    }
    return retVal1;
}

void ValCache::removeFromValCache( LoopRowNode *lrn, short col )
{
    LoopRowCol lrc;
    
    lrc.lrn = lrn;
    lrc.col = col;
    
    ValCache::cache.erase( lrc );
}

void ValCache::debugDump( void )
{
    int idx;
    map<const LoopRowCol, DataValueNode*, LoopRowCol>::iterator cur;
    cerr << "DebugDump ValCache: Cache has " << ValCache::cache.size() << " entries."
	<< endl << "===========================================" << endl;
    for(    idx = 0 , cur = ValCache::cache.begin() ;
	    cur != ValCache::cache.end() ;
	    idx++, cur++ )
    {

	cerr << idx << "\tlrn=[" << (*cur).first.lrn << "], col="
	    << (*cur).first.col << ", dvn=[" << (*cur).second
	    << "]:" << (int) (((*cur).second)->myDelimType())
	    << ((*cur).second)->myValue() << endl;
    }
}

void LoopTableNode::flushValCache( void )
{
    iterator     i;

    for( i = begin() ; i != end() ; i++ )
    {
	(*(*i)).flushValCache();
    }
    ValCache::clear();
}

DataLoopNode::DataLoopNode(DataLoopNameListNode *d, DataLoopValListNode *l,
			   const string &tflag)
: 
  myDefList(d),
  myValList(tflag)
{
  if (transform(l) != StarFileNode::OK)
    cerr << "Starlib error: # of values in loop does not match # of names."
	 << endl;
  DELETE_IF_NOT_ALREADY(l);

  if( myDefList != NULL )
      myDefList->setParent(this);
  myValList.setParent(this);
  showStop = true;
}

DataLoopNode::DataLoopNode(const string &tflag) : myValList(tflag)
{
  myDefList = new DataLoopNameListNode;
  showStop = true;

  if( myDefList != NULL )
      myDefList->setParent(this);
  myValList.setParent(this);
  preComment = string("");
}

DataLoopNode::~DataLoopNode()
{
    bitFields.isDeleting = true;
    unlinkMe();

    if( myDefList != NULL )
    {
        // small recursive delete loop being avoided here.
	// DELETE_IF_NOT_ALREADY(myDefList);
    }
    bitFields.isDeleting = false;
    showStop = true;
}

void DataLoopNode::reset()
{
  myValList.reset();
}

void DataLoopNode::setVals( LoopTableNode &L )
{
    myValList.setTabFlag( L.getTabFlag() );
    myValList.setParent(this);
    // Clear the whole old list out:
    myValList.erase( myValList.begin(), myValList.end() );
    myValList.reserve( L.size() );

    int i;

    // Copy the pointers over to this table - note this is
    // a POINTER copy, not an instance copy.  So we need
    // to be VERY careful when disposing of the L object
    // so as not to make its destructor end up deleting the
    // LoopRowNodes that have just been linked to in here.
    for( i = 0 ; i < L.size() ; i++ )
    {
	L.setParent(this);
	myValList.insert( myValList.end(), L[i] );
    }

}

///////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////

DataLoopNode::DataLoopNode( DataLoopNode& D) : myValList( D.myValList )
{
    (void)copyFrom( D, false );
}

DataLoopNode::DataLoopNode( bool link, DataLoopNode& D) : myValList( link, D.myValList )
{
    (void)copyFrom( D, link );
}

bool DataLoopNode::copyFrom( DataLoopNode &fromThis, bool linkWith )
{

    ASTnode::copyFrom( fromThis );
    myDefList = new DataLoopNameListNode( linkWith, *(fromThis.myDefList));
    showStop = fromThis.showStop;

    if( myDefList != NULL )
      myDefList->setParent(this);

    setPreComment( fromThis.getPreComment() );
    myValList.copyFrom( fromThis.myValList, linkWith );
    myValList.setParent(this);

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

int DataLoopNode::getMaxDepth( void )
{
    return myDefList->size() - 1;
}

////////////////////////////////////////////////////////////////////////

List<ASTnode*> * DataLoopNode::searchByTag( const string &searchFor )
{
    if( myDefList == NULL )
	return new List<ASTnode*>;  // return empty list
    else
	return  myDefList->searchByTag( searchFor );
}

List<ASTnode*> * DataLoopNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *matches;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    if( myDefList != NULL )
    {
	matches = myDefList->searchForType( type, delim );
	if( matches->Length() > 0 )
	    retVal->AddToEnd( *matches );
	delete  (matches);
    }

    matches = myValList.searchForType( type, delim );
    if( matches->Length() > 0 )
	retVal->AddToEnd( *matches );
    delete  (matches);

    return retVal;
}

bool DataLoopNode::unlinkChild( ASTnode *child )
{
    if( myDefList == child )
    {   myDefList = (DataLoopNameListNode*)NULL;
	return true;
    }
    else if( &myValList == child )
    {   
	// TODO: Fix this.
	return false;  // Not sure how to go about deleting
	               // something that is not a pointer.
	               // myValList is a real instantiation,
	               // not a pointer.  Unfortunately, its
	               // type, LoopTableNode, has no 'clearall'
	               // method, so this would be ugly.
    }
    else
	return false;
}

List<ASTnode*> * DataLoopNode::searchByTagValue( const string &tag,
	                                  const string &value )
{
    int                   nest = -1;
    int                   col  = -1;
    int                   row;
    bool                  generated;
    LoopRowNode           *curRow;
    DataValueNode         *curVal;
    List<ASTnode*>        *retVal = new List<ASTnode*>;


    tagPositionDeep( tag, &nest, &col );


    if( nest >= 0 )
    {
	// TODO: DELETE THIS NEXT PART IF AND WHEN NESTED
	// LOOPS ARE IMPLEMENTED FOR THIS FUNCTION!!!!!
	// ----------------------------------------------
	if( nest < 1 )
	{
	    // THIS ONLY WORKS ON NON-NESTED LOOPS!!!  I NEED TO
	    // MAKE THIS MORE SOPHISTICATED WHEN I HAVE THE TIME!
	    // --------------------------------------------------

	    for( row = 0 ; row < myValList.size() ; row++ )
	    {
	        curVal = ValCache::getFromValCache( myValList[row], col );
		generated = false;
		if( curVal == NULL )
		{   curVal = myValList[row]->generateVal( col );
		    generated = true;
		}


		if( *curVal == value )
		{   retVal->AddToEnd( curVal );
		    if( generated )
			ValCache::addToValCache( myValList[row], col, curVal );
		}
		else
		{   if( generated )
			DELETE_IF_NOT_ALREADY(curVal);
		}
	    }
	}
    }
    return retVal;
}


StarFileNode::Status DataLoopNode::transform(DataLoopValListNode *v)
{
  DVNWithPos *value;
  DVNWithPos *prevValue;
  DataLoopNameListNode::Status dstat;
  DataLoopValListNode::Status vstat;
  int row;

  v->reset();
  myDefList->reset();
  prevValue = NULL;

  while ((vstat = v->getNextValue(value)) != DataLoopValListNode::END)
  {
    dstat = myDefList->getNextTag(vstat, row);
    if( prevValue != NULL )
        DELETE_IF_NOT_ALREADY(prevValue); 
    prevValue = value;
    if (dstat == DataLoopNameListNode::ERROR)
      return StarFileNode::ERROR;

    if( row >= 0 ) // row is negative only when the very last STOP
	           // for the DataLoopNode has been encountered,
		   // the negative value indicating that the nestlevel
		   // has popped all the way back past the starting point.
    {
	myValList.addValue(dstat, row, 0, value);
    }
  }
  return StarFileNode::OK;
}
/* end of COMMENTING_OUT */

SaveFrameListNode::SaveFrameListNode(ASTlist<DataNode *> *L) : myKids(L)
{
    myKids->setParent(this);
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
        myKids->Current()->setParent(this);
}

SaveFrameListNode::SaveFrameListNode()
{
  myKids = new ASTlist<DataNode *>;
  myKids->setParent(this);
  for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
      myKids->Current()->setParent(this);
}

SaveFrameListNode::SaveFrameListNode( SaveFrameListNode &L )
{
    (void)copyFrom( L, false );
}

SaveFrameListNode::SaveFrameListNode( bool link, SaveFrameListNode &L )
{
    (void)copyFrom( L, link );
}

bool SaveFrameListNode::copyFrom( SaveFrameListNode &fromThis, bool linkWith )
{
    SaveFrameListNode &L = fromThis;

    ASTnode::copyFrom( fromThis );
    DataNode *remember = (L.myKids->AtEnd()) ? 
			(DataNode*)NULL : L.myKids->Current();
    myKids = new ASTlist<DataNode *>;
    myKids->setParent(this);
    myKids->reserve( L.myKids->size() );
    for( L.myKids->Reset() ; ! L.myKids->AtEnd() ; L.myKids->Next() )
    {
	switch( L.myKids->Current()->myType() )
	{
	    case DataNode::DATAITEMNODE:
		myKids->AddToEnd( new DataItemNode( linkWith, *( (DataItemNode*)L.myKids->Current() ) )  );
		break;
	    case DataNode::DATALOOPNODE:
		myKids->AddToEnd( new DataLoopNode( linkWith, *( (DataLoopNode*)L.myKids->Current() ) )  );
		break;
	    case DataNode::SAVEFRAMENODE:
		myKids->AddToEnd( new SaveFrameNode( linkWith, *( (SaveFrameNode*)L.myKids->Current() ) )  );
		break;
	    default:   // gets gcc -Wall to shut up.
		break;
	}
    }
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
        myKids->Current()->setParent(this);

    (void)L.myKids->SeekTo( remember );

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

SaveFrameListNode::~SaveFrameListNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
	if( myKids->Current() != NULL )
	    DELETE_IF_NOT_ALREADY(myKids->Current());

    if( myKids != NULL )
	delete  (myKids);
    bitFields.isDeleting = false;
}

List<ASTnode*> * SaveFrameListNode::searchByTag( const string &searchFor )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchByTag( searchFor );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }
    return retVal;
}

List<ASTnode*> * SaveFrameListNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchForType( type, delim );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }
    return retVal;
}

bool SaveFrameListNode::unlinkChild( ASTnode *child )
{
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {   if( myKids->Current() == child )
	{   myKids->RemoveCurrent();
	    return true;
 	}
    }
    return false;
}

List<ASTnode*> * SaveFrameListNode::searchByTagValue( const string &tag,
	                                       const string &value )
{
    List<ASTnode*> *retVal = new List<ASTnode*>;
    List<ASTnode*> *foundSome;

    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	foundSome = myKids->Current()->searchByTagValue( tag, value );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }
    return retVal;
}

int SaveFrameListNode::myLongestStr( void )
{
    int retVal = 0;
    int curLen = 0;

    // Return the longest of the 'myLongestStr()'s from the
    // list of DataNodes in this object:
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	// Loops don't count - they're weird.
	if( ! myKids->Current()->isOfType( ASTnode::DATALOOPNODE )  )
	{
	    curLen = myKids->Current()->myLongestStr();
	    if( curLen > retVal )
		retVal = curLen;
	}
    }
    return retVal;
}



SaveFrameNode::SaveFrameNode(SaveHeadingNode *h, SaveFrameListNode *l)
: 
  myHeading(h), mySaveFrameList(l)
{
    if( myHeading != NULL)
	myHeading->setParent(this);
    if( mySaveFrameList != NULL)
	mySaveFrameList->setParent(this);
    preComment = string("");
}

SaveFrameNode::SaveFrameNode(const string & name)
{
  myHeading = new SaveHeadingNode(name);
  mySaveFrameList = new SaveFrameListNode;

  if( myHeading != NULL)
      myHeading->setParent(this);
  if( mySaveFrameList != NULL)
      mySaveFrameList->setParent(this);
  preComment = string("");
}

SaveFrameNode::SaveFrameNode( SaveFrameNode &S )
{
    (void)copyFrom( S, false );
}

SaveFrameNode::SaveFrameNode( bool link, SaveFrameNode &S )
{
    (void)copyFrom( S, link );
}

bool SaveFrameNode::copyFrom( SaveFrameNode &fromThis, bool linkWith )
{
    ASTnode::copyFrom( fromThis );
    mySaveFrameList = new SaveFrameListNode( linkWith, *(fromThis.mySaveFrameList) );
    myHeading = new SaveHeadingNode( fromThis.myHeading->myName() );
    if( linkWith )
    {
	myHeading->setPeer( fromThis.myHeading );
	fromThis.myHeading->setPeer( myHeading );
    }

    if( myHeading != NULL)
	myHeading->setParent(this);
    if( mySaveFrameList != NULL)
	mySaveFrameList->setParent(this);

    setPreComment(fromThis.getPreComment());

    if( linkWith )
    {   setPeer( &fromThis );
	fromThis.setPeer( this );
    }
    return true;
}

SaveFrameNode::~SaveFrameNode()
{
    bitFields.isDeleting = true;
    unlinkMe();
    if( myHeading != NULL )
	DELETE_IF_NOT_ALREADY(myHeading);
    if( mySaveFrameList != NULL )
	DELETE_IF_NOT_ALREADY(mySaveFrameList);
    bitFields.isDeleting = false;
    preComment = string("");
}

List<ASTnode*> * SaveFrameNode::searchByTag( const string &searchFor )
{
    List <ASTnode*> *retVal = new List<ASTnode*>;
    List <ASTnode*> *foundSome;

    if( strCmpInsensitive( myHeading->myName(), searchFor ) == 0)
	retVal->AddToEnd( this );

    if( mySaveFrameList != (ASTnode*)NULL )
    {
	foundSome = mySaveFrameList->searchByTag( searchFor );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

List<ASTnode*> * SaveFrameNode::searchForType(
	ASTtype   type,       // The type to search for.
	int       delim
	)
{
    List <ASTnode*> *retVal = new List<ASTnode*>;
    List <ASTnode*> *foundSome;

    if( isOfType( type ) )
	retVal->AddToEnd( this );

    if( myHeading != NULL )
    {
	foundSome = myHeading->searchForType( type, delim );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }
    if( mySaveFrameList != (ASTnode*)NULL )
    {
	foundSome = mySaveFrameList->searchForType( type, delim );
	if( foundSome->Length() > 0 )
	    retVal->AddToEnd( *foundSome );
	delete  (foundSome);
    }

    return retVal;
}

bool SaveFrameNode::unlinkChild( ASTnode *child )
{
    if( myHeading == (SaveHeadingNode*)child )
    {   myHeading = (SaveHeadingNode*)NULL;
	return true;
    }
    else if( mySaveFrameList == (SaveFrameListNode*)child )
    {   mySaveFrameList = (SaveFrameListNode*)NULL;
	return true;
    }
    else
	return false;
}

List<ASTnode*> * SaveFrameNode::searchByTagValue( const string &tag,
	                                   const string &value )
{
    if( mySaveFrameList != (ASTnode*)NULL )
	return  mySaveFrameList->searchByTagValue( tag, value );
    else
	return new List<ASTnode*>;
}

void SaveFrameNode::changeName(const string &name)
{
    myHeading->changeName( name );
}

SaveHeadingNode::SaveHeadingNode(const string & myHeading) 
: 
  HeadingNode(myHeading)
{}

SaveHeadingNode::SaveHeadingNode( SaveHeadingNode &N)
    : HeadingNode( N )
{}

SaveHeadingNode::SaveHeadingNode( bool link, SaveHeadingNode &N)
    : HeadingNode( link, N )
{}


DataHeadingNode::DataHeadingNode(const string & myHeading) 
: 
  HeadingNode(myHeading)
{}

DataHeadingNode::DataHeadingNode( DataHeadingNode &N)
    : HeadingNode( N )
{}

DataHeadingNode::DataHeadingNode( bool link, DataHeadingNode &N)
    : HeadingNode( link, N )
{}

GlobalHeadingNode::GlobalHeadingNode(const string & myHeading) 
: 
  HeadingNode(myHeading)
{}

GlobalHeadingNode::GlobalHeadingNode( GlobalHeadingNode &N)
    : HeadingNode( N )
{}

GlobalHeadingNode::GlobalHeadingNode( bool link, GlobalHeadingNode &N)
    : HeadingNode( link, N )
{}


DataBlockNode::DataBlockNode(const string & myHeading)
: 
  BlockNode(myHeading)
{}

DataBlockNode::DataBlockNode(HeadingNode *h, DataListNode *n) 
: 
  BlockNode(h, n)
{}

DataBlockNode::DataBlockNode(DataBlockNode &n)
    : BlockNode( n )
{}

DataBlockNode::DataBlockNode( bool link, DataBlockNode &n)
    : BlockNode( link, n )
{}

void DataBlockNode::setName( const string & newName ) 
{
  ASTnode *prevHeadingPeer = myHeading->myParallelCopy();

  if(  newName.substr(0,5) == string("data_") ) {
    delete myHeading;
    myHeading = new DataHeadingNode( newName );
  } else {
    delete myHeading;
    myHeading = new DataHeadingNode( string("data_")+newName );
  }
  myHeading->setParent(this);
  myHeading->setPeer(prevHeadingPeer);
}

GlobalBlockNode::GlobalBlockNode(HeadingNode *h, DataListNode *n) 
    : BlockNode(h, n)
{}

GlobalBlockNode::GlobalBlockNode(GlobalBlockNode &n)
    : BlockNode( n )
{}

GlobalBlockNode::GlobalBlockNode( bool link, GlobalBlockNode &n)
    : BlockNode( link, n )
{}



////////////////////////////////////////////////////////////////////
////////    Here begins the text that used to be in the     ////////
////////    file called "astoper.cc".  I have left in the   ////////
////////    RCS log for the sake of posterity, but it is    ////////
////////    no longer updated on each check-in.  Only the   ////////
////////    main RCS log at the top of this file is updated ////////
////////    now.  There was absolutely zero logical reason  ////////
////////    to split up these routines the way they were    ////////
////////    being split into different files.   Splitting   ////////
////////    things up is good, but in this case they were   ////////
////////    only split based on who was coding which pieces ////////
////////    rather than on any modular logical breakdown.   ////////
////////////////////////////////////////////////////////////////////

//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//
// Revision 1.4  1998/06/01  21:15:03  madings
// Changed FlattenNestedLoop so it returns a List<> instead of an ASTlist<>.
// ASTlist<> should NEVER, EVER, EVER be used to hold a list that is outside
// the normal star tree.  (ASTlist<> changes the parent pointers of the
// nodes you add to it, so if you use it to hold pointers to nodes in a
// star tree, you change the parent pointers of those nodes.)
//
// Revision 1.3  1998/06/01  18:58:30  madings
// Changed FlattenNestedLoops() so that it returns a list of pointers
// to the names and values that are actually in the star tree, as
// opposed to generating new copies of those nodes and making a list
// of the new copies.
//
// Revision 1.2  1998/05/08  20:27:24  informix
// Made a change to handle the C++ STL instead of gcc's libg++.
// Now all strings are of type 'string' instead of 'String' (notice
// the uppercase/lowercase change.)  Applications using this library
// will have to change also, unfortunately, to use the new string style.
//
// Revision 1.1.1.1  1998/02/12  20:59:00  informix
// re-creation of starlib project, without sb_lib stuff.  sb_lib has
// been moved to it's own project.
//
// Revision 1.1.1.1  1997/11/07  01:49:07  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.3  1997/10/10  18:34:22  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.2  1997/10/01  22:35:03  bsri
// This version of starlib split main.cc into separate files (which contain
// the transformation functions - one in each file).
//
// Revision 1.1.1.1  1997/09/12  22:54:40  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.6  1997/07/30  21:34:26  madings
//check-in before switching to cvs
//
//Revision 1.5  1997/07/11  22:40:03  madings
//Finally seems to work - just waiting for
//new dictionaries.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:57:57  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:44  madings
//I Added the RCS log and ID variables.
//
//
////////////////////////////////////////////////////////////////////////


//#############################################################################
//#############################################################################
//#############################################################################
//Given the name of a  data item in a SAVEFRAME object this
//function returns a pointer to the data item which can ONLY be
//a loop or simple data item 


DataNode*  SaveFrameNode::ReturnSaveFrameDataNode(string dataItemName)
{
  return (mySaveFrameList->ReturnSaveFrameDataNode(dataItemName));
}


DataNode*  SaveFrameListNode::ReturnSaveFrameDataNode(string dataItemName)
{
  myKids->Reset();
  
  while(!myKids->AtEnd())
    {
      if (myKids->Current()->myName() == dataItemName)
	  return (myKids->Current());

      myKids->Next();
    }
  return NULL;
}

DataNode* SaveFrameNode::ReturnSaveFrameDeepLook(  string dataItemName, int *nest, int *col )
{
    // Defaults:
    (*nest) = -1;
    (*col) = -1;

    return mySaveFrameList->ReturnSaveFrameDeepLook( dataItemName, nest, col );
}

DataNode* SaveFrameListNode::ReturnSaveFrameDeepLook( string dataItemName, int *nest, int *col )
{
    myKids->Reset(); 
    while(!myKids->AtEnd())
    {
	//                      ITEM TYPE:
	if( myKids->Current()->myType() == DataNode::DATAITEMNODE )
	{
	    if (myKids->Current()->myName() == dataItemName)
		return myKids->Current();
	}
	//                      LOOP TYPE:
	else if( myKids->Current()->myType() == DataNode::DATALOOPNODE )
	{
	    ( (DataLoopNode*)myKids->Current() )->tagPositionDeep( dataItemName, nest, col );

	    if( (*nest) >= 0 )
		return myKids->Current();
	}

	myKids->Next();
    }
    return NULL;
}


//#############################################################################
//#############################################################################
//#############################################################################


//The functions below returns a pointer to a DataNode in a specified
//datablock and a specified data item name in the data block.
//Note this item could be a simple data item, a saveframe object
//or a loop.NULL is returned if the item is not present.


DataNode* StarFileNode::  ReturnDataBlockDataNode(
						  string datablockName, 
						  string dataitemName
						  )
{
  return (myStarList->ReturnDataBlockDataNode(datablockName, dataitemName));
}




DataNode* StarListNode::  ReturnDataBlockDataNode(
						  string datablockName,
						  string dataitemName
						  )
{

//Checks to see if there is a match in the data block
  myKids->Reset();
  while(!myKids->AtEnd())
    {
      if(myKids->Current()->myName() == datablockName)
	return (myKids->Current()->ReturnDataBlockDataNode(dataitemName));
      myKids->Next();
    }
  return NULL;
}



DataNode* BlockNode:: ReturnDataBlockDataNode(string dataitemName)
{
  return (myDataList->ReturnDataBlockDataNode(dataitemName));
}



DataNode* DataListNode:: ReturnDataBlockDataNode(string dataitemName)
  {
    myKids->Reset();
    while(!myKids->AtEnd())
      {
	if( (myKids->Current()->myName() == dataitemName))
	  return (myKids->Current());
	myKids->Next();
      }
    return NULL;
  }

void DataLoopNode::tagPositionDeep( string tagName, int *nestLevel, int *column )
{
    myDefList->tagPositionDeep( tagName, nestLevel, column ) ;
}

void DataLoopNameListNode::tagPositionDeep( string tagName, int *nestLevel, int *column )
{
    int pos = -1;
    for( myKids->Reset() , (*nestLevel) = 0 ; ! myKids->AtEnd() ; myKids->Next() , (*nestLevel)++ )
    {
	pos = myKids->Current()->tagPosition( tagName );

	if( pos >= 0)
	    break;
    }
    if( pos >= 0 )
    {   (*column) = pos;
	return;
    }
    else
    {   (*column) = -1;
	(*nestLevel) = -1;
	return;
    }
}

string StarFileNode::getSkipTextBetween( int line1, int line2, int &retLine )
{
    int i, sz;
    string retVal = string("");
    retLine = -1;

    sz = skipTextLineNums.size();
    for( i = 0 ; i < sz ; i++ )
    {   if(   skipTextLineNums[i] >= line1  &&
	      skipTextLineNums[i] <= line2   )
	{   retVal = skipTexts[i];
	    retLine = skipTextLineNums[i];
	    break;
	}
    }
    return retVal;
}
//############################################################################
//############################################################################
//############################################################################
//These set of functions will perform remove a specific saveframe
//from a specified data block. No action will be taken if either
//does not exist in the tree.

void  StarFileNode::  RemoveSaveFrame(
				      string datablockName, 
				      string saveframeName
				      )
{
  myStarList->RemoveSaveFrame(datablockName, saveframeName);
}



void  StarListNode::RemoveSaveFrame(
				    string datablockName, 
				    string saveframeName
				    )
{
  //Checks to see if there is a match in the data block
  myKids->Reset();
  bool done = false;
  while(!myKids->AtEnd() && !done)
    {
      if(myKids->Current()->myName() == datablockName)
	{
	  myKids->Current()->RemoveSaveFrame(saveframeName);
	  done = true;
	}
      else
	myKids->Next();
    }
}


void BlockNode::RemoveSaveFrame(string saveframeName)
{
  
    myDataList->RemoveSaveFrame(saveframeName);
}


void  DataListNode:: RemoveSaveFrame(string saveframeName)
  {
    myKids->Reset();
    bool done = false;
    while(!myKids->AtEnd())
      {
	  if( (myKids->Current()->myName() == saveframeName) && DataNode::SAVEFRAMENODE ==myKids->Current()->myType() && !done)
	  {
	    myKids->RemoveCurrent();
	    done = true;
	  }
	else
	  myKids->Next();
      }
  }
//########################################################################
//########################################################################
//########################################################################
//These functions will add a given data item to a specified saveframe
void SaveFrameNode::AddItemToSaveFrame(DataNode* S)
{
  mySaveFrameList->AddItemToSaveFrame(S);
}


void SaveFrameListNode::AddItemToSaveFrame(DataNode* S)
{
  S->setParent(this);
  myKids->AddToEnd(S);
}


//#########################################################################
//#########################################################################
//#########################################################################
//These functions will add a given data item to a specified datablock

void StarFileNode::AddSaveFrameToDataBlock(
					   string datablockName, 
					   DataNode* newNode
					   )
{
   
   myStarList->AddSaveFrameToDataBlock(datablockName,newNode);
}


void StarListNode::AddSaveFrameToDataBlock(
					   string datablockName, 
					   DataNode* newNode
					   )
{

  myKids->Reset();
  while(!myKids->AtEnd())
    {
      if(myKids->Current()->myName() == datablockName)
	{
	 
	  myKids->Current()->AddSaveFrameToDataBlock(newNode);
	  
	  return;
	}
      myKids->Next();
    }

}


void  BlockNode:: AddSaveFrameToDataBlock(DataNode* newNode)
{
   
  myDataList->AddSaveFrameToDataBlock(newNode);
}


void DataListNode:: AddSaveFrameToDataBlock(DataNode* newNode)
{

  newNode->setParent( this );
  myKids->AddToEnd(newNode);
  
}
//############################################################################
//############################################################################
//############################################################################
//The following function returns a pointer to the list of dataitems
//contained in a datablock


ASTlist<DataNode*>* BlockNode::GiveMyDataList()
{
  return(myDataList->GiveMyDataList()); 
}


ASTlist<DataNode*>* DataListNode::GiveMyDataList()
{
  return (myKids);
}



//#############################################################################
//#############################################################################
//#############################################################################

//The following function returns a pointer to the list of dataitems
//contained in a saveframe

ASTlist<DataNode*>* SaveFrameNode::GiveMyDataList()
{
  return(mySaveFrameList->GiveMyDataList());
}


ASTlist<DataNode*>* SaveFrameListNode::GiveMyDataList()
{
  return (myKids);
}
//############################################################################
//###########################################################################
//##########################################################################
//The following functions give a pointer to the list of datablocks in the AST
ASTlist<BlockNode*>* StarFileNode::GiveMyDataBlockList()
{
  return (myStarList->GiveMyDataBlockList());
}


ASTlist<BlockNode*>* StarListNode::GiveMyDataBlockList()
{
  return myKids;
}

//##########################################################################
//##########################################################################
//##########################################################################
//The following functions are responsible for flattening nested
//loop structures into linear lists of tags and values

void DataLoopNode::FlattenNestedLoop(
				     List<DataNameNode*> * &L,
				     List<DataValueNode*>* &M
				     )
				    
{
  L = new List<DataNameNode *>;
  M = new List<DataValueNode*>;

  myDefList->FlattenNestedLoop(L);
  myValList.FlattenNestedLoop(M);
}


void DataLoopNameListNode::FlattenNestedLoop(List<DataNameNode*>*  L)
{
  
  
  myKids->Reset();
  while(!myKids->AtEnd())
   {
      myKids->Current()->FlattenNestedLoop(L);
      myKids->Next();
    }
}


void LoopNameListNode::FlattenNestedLoop(List<DataNameNode*>* L)
{
  myKids->Reset();
  while(!myKids->AtEnd())
    {
      DataNameNode* tempNode = myKids->Current();
      L->AddToEnd(tempNode);
      myKids->Next();
    }
}

void LoopTableNode::FlattenNestedLoop(List<DataValueNode*>* M)
{
  myIters.Reset();
  while(!myIters.AtEnd())
    {
      myIters.Current()->FlattenNestedLoop(M);
      myIters.Next();
    }
}

void LoopRowNode::FlattenNestedLoop(List<DataValueNode*>* M)
{
  short i;
  cur = 0;
  for( i = 0 ; i < myStarts.size() ; i++ )
  {
      DataValueNode* tempNode = elementAt( i );
      M->AddToEnd(tempNode);
  }
  if(innerLoop)
    innerLoop->FlattenNestedLoop(M);
}
////////////////////////////////////////////////
//For part 2
///////////////////////////////////////////////
void LoopRowNode::reset()
{
  cur=0;
  if (innerLoop != NULL)
    innerLoop->reset();
}
/* end of COMMENTING_OUT */

DataLoopValListNode::Status LoopRowNode::returnNextLoopElement(DataValueNode* & v)
{
  if( cur<size() )
  {   v = (*this)[cur];
      cur++;
      return DataLoopValListNode::CONTINUE;
  }

  if (innerLoop)
      return innerLoop->returnNextLoopElement(v);
  else
      return DataLoopValListNode::END;
}
/* end of COMMENTING_OUT */
DataValueNode* LoopRowNode::returnLoopValue(unsigned position)
{
  unsigned i = 0;

  for (cur=0 ; cur < size() ; cur++)
    if (cur == position)
      return (*this)[cur];

  return NULL;
}
/* end of COMMENTING_OUT */

void LoopTableNode::reset()
{
  for (myIters.Reset(); !myIters.AtEnd(); myIters.Next())
    myIters.Current()->reset();
  myIters.Reset();
}

DataLoopValListNode::Status LoopTableNode::returnNextLoopElement(DataValueNode* & v)
{
  // If we are at the end the first time, we will return STOP. This is done
  // by the last statement in this function. Otherwise, we have already
  // returned STOP once, now we have to return END.
  // The distinction between STOP and END is the following :
  // STOP means that some loop has completed, not necessarily this one, it may
  // be some inner loop. That information is reported to the caller.
  // END means that this loop and all its children do not have any more data.

  if (myIters.AtEnd())
    return DataLoopValListNode::END;

  while (!myIters.AtEnd()) {

    DataLoopValListNode::Status retVal = 
      myIters.Current()->returnNextLoopElement(v);

    if (retVal == DataLoopValListNode::STOP)
      return DataLoopValListNode::STOP;

    if (retVal == DataLoopValListNode::END)
      myIters.Next();
    else
      return DataLoopValListNode::CONTINUE;
  }    

  return DataLoopValListNode::STOP;
}

ASTlist<DataValueNode *> * LoopTableNode::returnLoopValues(unsigned position)
{
  ASTlist<DataValueNode *> *retVal = new ASTlist<DataValueNode *>;

  retVal->setParent(this);

  for (myIters.Reset(); !myIters.AtEnd(); myIters.Next())
    retVal->AddToEnd(myIters.Current()->returnLoopValue(position));

  return retVal;
}

DataLoopValListNode::Status DataLoopNode::returnNextLoopElement(
						    DataValueNode* & v)
{
  return myValList.returnNextLoopElement(v);
}

ASTlist<DataValueNode *> * DataLoopNode::returnLoopValues(const string & tagName)
{
  // Caution : This function only searches in the first level of loop
  int tagPos;

  if ((tagPos = myDefList->tagPosition(tagName)) == -1)
    // Not found
    return NULL;

  return myValList.returnLoopValues(tagPos);
}

int DataLoopNameListNode::tagPosition(const string & tagName)
{
  // Caution : This function only searches in the first level of loop
  myKids->Reset();
  return myKids->Current()->tagPosition(tagName);
}

int LoopNameListNode::tagPosition(const string & tagName)
{
  int i = 0;
  for (myKids->Reset(); !myKids->AtEnd(); myKids->Next(), i++)
  {
    if( strCmpInsensitive( tagName, myKids->Current()->myName() ) == 0 )
    {
      return i;
    }
  }

  // Not found
  return -1;
}

ASTlist<DataValueNode *> * DataItemNode::returnLoopValues(const string & tagName)
{
  if (tagName != myName())
    return NULL;

  ASTlist<DataValueNode *> *retVal = new ASTlist<DataValueNode *>;
  retVal->setParent( myDataValue->myParent() );
  retVal->AddToEnd(myDataValue);

  return retVal;
}

ASTlist<DataValueNode *> * SaveFrameNode::returnLoopValues(const string & )
{
  return NULL;
}



StarFileNode::Status DataLoopNameListNode::RemoveDataName( const string & name,
	                                                  int   *nest,
							  int   *ele   )
{
    if( nest )
	*nest = 0;
    // For each LoopNameListNode in this DataLoopDefListNode:
    for(  myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next()  )
    {
	if( nest )
	    (*nest)++;
	// Try to remove the name from the list.  If it was found and
	// removed, then we are done, get out:
	if( myKids->Current()->RemoveDataName( name, ele ) == StarFileNode::OK )
	    return StarFileNode::OK;
    }
    if( nest )
	*nest = 0;
    return StarFileNode::ERROR;
}

StarFileNode::Status LoopNameListNode::RemoveDataName(const string & name,
	                                             int          * ele )
{
    if( ele )
	*ele = 0;
    for( myKids->Reset() ; ! myKids->AtEnd() ; myKids->Next() )
    {
	if( ele )
	    (*ele)++;
	if( myKids->Current()->myName() == name )
	{
	    myKids->RemoveCurrent();
	    return StarFileNode::OK;  // found it - okay, we're done
	}
    }
    if( ele )
	*ele = 0;
    return StarFileNode::ERROR;
}

StarFileNode::Status  DataLoopNode::ChangeName(
	const string &oldName,
	const string &newName
	)
{
    return myDefList->ChangeName( oldName, newName );
}

StarFileNode::Status  DataLoopNode::RemoveColumn( const string name )
{
    int  nestLevel, columnNum;
    StarFileNode::Status   retVal;

    retVal = myDefList->RemoveDataName( name, &nestLevel, &columnNum );

    if( retVal == StarFileNode::OK )
	myValList.RemoveColumnValues( nestLevel, columnNum );

    return retVal;
}
/* end of COMMENTING_OUT */


//   These next two functions form a recursive pair.  Watch their
//   intertwining carefully.  LoopTableNode's RemoveColumnValues() calls
//   IterLoop's RemoveColumnValues() for each row of the loop.  If
//   IterLoop's RemoveColumnValues() realizes that it is not at
//   the base case of recursion yet, then it calls the
//   RemoveColumnValues() of its inner LoopTableNode.  (Which will call
//   RemoveColumnValues() for each of its rows and so on.)

StarFileNode::Status  LoopRowNode::RemoveColumnValues(
	int          nest,
	int          ele )
{
    StarFileNode::Status  retVal = StarFileNode::OK;

    // Base case: delete the element from the list.
    if( nest == 0 )
    {
	// Count off until we have reached the number of the
	// element we want to chuck away:
	for( cur=0 ; cur < size() && ele > 1 ; cur++ , --ele )
	{ /*void-body*/ }

	if( cur < size() )  // Chuck it
	{
	    privateRemoveElementAt(cur);
	}
	else
	    retVal = StarFileNode::ERROR;
    }
    else // Recursive case - call it for my inner loop.
    {
	retVal = innerLoop->RemoveColumnValues( nest, ele );
    }

    return retVal;
}

StarFileNode::Status  LoopTableNode::RemoveColumnValues(
	int          nest,
	int          ele )
{
    int  rowNum;
    StarFileNode::Status  retVal = StarFileNode::ERROR;
    // Count backward because this could result in removal
    // of rows:
    for( rowNum = size()-1 ; rowNum >= 0 ; rowNum-- )
    {
	if( (*this)[rowNum]->RemoveColumnValues( nest-1, ele ) == StarFileNode::OK )
	    retVal = StarFileNode::OK ;
    }

    return retVal;
}

StarFileNode::Status  DataLoopNameListNode::ChangeName(
	const string &oldName,
	const string &newName
	)
{
    StarFileNode::Status retVal = StarFileNode::ERROR;
    for(   myKids->Reset() ;
	   ! myKids->AtEnd() && retVal == StarFileNode::ERROR ;
	   myKids->Next()   )
    {
	retVal = myKids->Current()->ChangeName( oldName, newName );
    }
    return retVal;
}

StarFileNode::Status LoopNameListNode::ChangeName(
	const string &oldName,
	const string &newName
	)
{
    StarFileNode::Status retVal = StarFileNode::ERROR;
    for(   myKids->Reset() ;
	   ! myKids->AtEnd() && retVal == StarFileNode::ERROR ;
	   myKids->Next()   )
    {
	if( myKids->Current()->myName() == oldName )
	{
	    myKids->Current()->setName( newName );
	    retVal = StarFileNode::OK;
	}
    }
    return retVal;
}



////////////////////////////////////////////////////////////////////
////////    Here begins the text that used to be in the     ////////
////////    file called "interface.cc".  I have left in the ////////
////////    RCS log for the sake of posterity, but it is    ////////
////////    no longer updated on each check-in.  Only the   ////////
////////    main RCS log at the top of this file is updated ////////
////////    now.  There was absolutely zero logical reason  ////////
////////    to split up these routines the way they were    ////////
////////    being split into different files.   Splitting   ////////
////////    things up is good, but in this case they were   ////////
////////    only split based on who was coding which pieces ////////
////////    rather than on any modular logical breakdown.   ////////
////////////////////////////////////////////////////////////////////

//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//
// Revision 1.2  1998/05/08  20:27:24  informix
// Made a change to handle the C++ STL instead of gcc's libg++.
// Now all strings are of type 'string' instead of 'String' (notice
// the uppercase/lowercase change.)  Applications using this library
// will have to change also, unfortunately, to use the new string style.
//
// Revision 1.1.1.1  1998/02/12  20:59:00  informix
// re-creation of starlib project, without sb_lib stuff.  sb_lib has
// been moved to it's own project.
//
// Revision 1.1.1.1  1997/11/07  01:49:06  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.2  1997/10/10  18:34:23  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.1.1.1  1997/09/12  22:54:40  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.6  1997/07/30  21:34:26  madings
//check-in before switching to cvs
//
//Revision 1.5  1997/07/11  22:40:04  madings
//Finally seems to work - just waiting for
//new dictionaries.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:57:58  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:44  madings
//I Added the RCS log and ID variables.
//
//
////////////////////////////////////////////////////////////////////////

StarFileNode::Status StarFileNode::AddDataBlock(const string & name)
{
  return myStarList->AddDataBlock(name);
}

StarFileNode::Status StarFileNode::AddSaveFrame(const string & name)
{
  return myStarList->AddSaveFrame(name);
}

StarFileNode::Status StarFileNode:: AddSaveFrameDataItem( const string & name,
	                                                  const string & value,
							  DataValueNode::ValType type)
{
  return myStarList->AddSaveFrameDataItem(name, value, type);
}

StarFileNode::Status StarFileNode::AddSaveFrameLoop()
{
  return myStarList->AddSaveFrameLoop();
}

StarFileNode::Status StarFileNode::AddSaveFrameLoopDataName(const string & name)
{
  return myStarList->AddSaveFrameLoopDataName(name);
}

StarFileNode::Status StarFileNode::AddSaveFrameLoopDataValue( const string & value,
						              DataValueNode::ValType type )
{
  return myStarList->AddSaveFrameLoopDataValue( value, type );
}

StarFileNode::Status StarListNode::AddDataBlock(const string & name)
{
  DataBlockNode *db = new DataBlockNode(name);
  db->setParent(this);
  myKids->AddToEnd(db);
  return StarFileNode::OK;
}

StarFileNode::Status StarListNode::AddSaveFrame(const string & name)
{
  BlockNode *db = myKids->Last();

  if (db == NULL)
    return StarFileNode::ERROR;
  else
    return db->AddSaveFrame(name);
}

StarFileNode::Status StarListNode::AddSaveFrameDataItem(const string & name, 
							const string & value,
							DataValueNode::ValType type)
{
  BlockNode *db = myKids->Last();

  if (db == NULL)
    return StarFileNode::ERROR;
  else
    return db->AddSaveFrameDataItem(name, value, type);
}

StarFileNode::Status StarListNode::AddSaveFrameLoop()
{
  BlockNode *db = myKids->Last();

  if (db == NULL)
    return StarFileNode::ERROR;
  else
    return db->AddSaveFrameLoop();
}

StarFileNode::Status StarListNode::AddSaveFrameLoopDataName(const string & name)
{
  BlockNode *db = myKids->Last();

  if (db == NULL)
    return StarFileNode::ERROR;
  else
    return db->AddSaveFrameLoopDataName(name);

}

StarFileNode::Status StarListNode::AddSaveFrameLoopDataValue( const string & value,
						              DataValueNode::ValType type )
{
  BlockNode *db = myKids->Last();

  if (db == NULL)
    return StarFileNode::ERROR;
  else
    return db->AddSaveFrameLoopDataValue(value, type);
}

StarFileNode::Status BlockNode::AddSaveFrame(const string & name)
{
  return myDataList->AddSaveFrame(name);
}

StarFileNode::Status BlockNode::AddSaveFrameDataItem(const string & name,
						     const string & value,
						     DataValueNode::ValType type )
{
  return myDataList->AddSaveFrameDataItem(name, value, type);
}

StarFileNode::Status BlockNode::AddSaveFrameLoop()
{
  return myDataList->AddSaveFrameLoop();
}

StarFileNode::Status BlockNode::AddSaveFrameLoopDataName(const string & name)
{
  return myDataList->AddSaveFrameLoopDataName(name);
}

StarFileNode::Status BlockNode::AddSaveFrameLoopDataValue( const string & name,
						           DataValueNode::ValType type )
{
  return myDataList->AddSaveFrameLoopDataValue(name, type);
}

StarFileNode::Status DataListNode::AddSaveFrame(const string & name)
{
  SaveFrameNode *sf = new SaveFrameNode(name);
  sf->setParent(this);
  myKids->AddToEnd(sf);
  return StarFileNode::OK;
}

StarFileNode::Status DataListNode::AddSaveFrameDataItem(const string & name, 
							const string & value,
							DataValueNode::ValType type)
{
  DataNode *dn = myKids->Last();

  if (!dn || dn->myType() != DataNode::SAVEFRAMENODE)
    return StarFileNode::ERROR;
  else
    return ((SaveFrameNode *)dn)->AddDataItem(name, value, type);
}

StarFileNode::Status DataListNode::AddSaveFrameLoop()
{
  DataNode *dn = myKids->Last();

  if (!dn || dn->myType() != DataNode::SAVEFRAMENODE)
    return StarFileNode::ERROR;
  else
    return ((SaveFrameNode *)dn)->AddLoop();
}

StarFileNode::Status DataListNode::AddSaveFrameLoopDataName(const string & name)
{
  DataNode *dn = myKids->Last();

  if (!dn || dn->myType() != DataNode::SAVEFRAMENODE)
    return StarFileNode::ERROR;
  else
    return ((SaveFrameNode *)dn)->AddLoopDataName(name);
}
StarFileNode::Status DataListNode::AddSaveFrameLoopDataValue( const string & name,
						              DataValueNode::ValType type )
{
  DataNode *dn = myKids->Last();

  if (!dn || dn->myType() != DataNode::SAVEFRAMENODE)
    return StarFileNode::ERROR;
  else
    return ((SaveFrameNode *)dn)->AddLoopDataValue(name, type);
}

StarFileNode::Status SaveFrameNode::AddDataItem(const string & name, 
						const string & value,
						DataValueNode::ValType type)
{
  return mySaveFrameList->AddDataItem(name, value, type);
}

StarFileNode::Status SaveFrameNode::AddLoop(const string &tflag)
{
  return mySaveFrameList->AddLoop(tflag);
}

StarFileNode::Status SaveFrameNode::AddLoopDataName(const string & name)
{
  return mySaveFrameList->AddLoopDataName(name);
}

StarFileNode::Status SaveFrameNode::AddLoopDataValue( const string & value,
					              DataValueNode::ValType type )
{
  return mySaveFrameList->AddLoopDataValue(value, type);
}
  
StarFileNode::Status SaveFrameListNode::AddDataItem(const string & name, 
						    const string & value,
						    DataValueNode::ValType type)
{
  DataNode *dn = new DataItemNode(name, value, type);
  dn->setParent(this);
  myKids->AddToEnd(dn);
  return StarFileNode::OK;
}

StarFileNode::Status SaveFrameListNode::AddLoop(const string &tflag)
{
  DataNode *dn = new DataLoopNode(tflag);
  dn->setParent(this);
  myKids->AddToEnd(dn);
  return StarFileNode::OK;
}

StarFileNode::Status SaveFrameListNode::AddLoopDataName(const string & name)
{
  DataNode *dn = myKids->Last();

  if (!dn || dn->myType() != DataNode::DATALOOPNODE)
    return StarFileNode::ERROR;
  else
    return ((DataLoopNode *)dn)->AddDataName(name);
}

StarFileNode::Status SaveFrameListNode::AddLoopDataValue( const string & value,
					                  DataValueNode::ValType type )
{
  DataNode *dn = myKids->Last();

  if (!dn || dn->myType() != DataNode::DATALOOPNODE)
    return StarFileNode::ERROR;
  else
    return ((DataLoopNode *)dn)->AddDataValue(value, type);
}
  
StarFileNode::Status DataLoopNode::AddDataName(const string & name)
{
  if (myDefList->AddDataName(name) == StarFileNode::ERROR)
    return StarFileNode::ERROR;

  myDefList->reset(); // This is needed so that when AddDataValue is done,
  // we can scan from the beginning of the name list to figure correct position

  return StarFileNode::OK;
}

StarFileNode::Status DataLoopNode::AddDataValue( const string & value,
					         DataValueNode::ValType type )
{
  int row;

  const DataLoopNameListNode::Status dstat = 
    myDefList->getNextTag(DataLoopValListNode::CONTINUE, row);

  if (row != 0 || (dstat != DataLoopNameListNode::NEW_ITER && 
		   dstat != DataLoopNameListNode::SAME_ITER))
    return StarFileNode::ERROR;

  DVNWithPos *newValue = new DVNWithPos( value, type );
  myValList.addValue(dstat, row, 0u, newValue );

  return StarFileNode::OK;
}

StarFileNode::Status DataLoopNameListNode::AddDataName(const string & name)
{
  // The list already has been allocated, now have to check if it is empty
  if (myKids->Length() == 0) {
    myKids->AddToEnd(new LoopNameListNode);
    myKids->Last()->setParent(this);
    myKids->Reset();
  }
  return myKids->Current()->AddDataName(name);
}

StarFileNode::Status LoopNameListNode::AddDataName(const string & name)
{
  myKids->AddToEnd(new DataNameNode(name));
  return StarFileNode::OK;
}

// -------------------
// globalSetTabFlag()
// -------------------
//  This is an ugly hack to get around the fact that I cannot call
//  LoopTableNode::setTabFlag() directly from inside LoopRowNode.
//  (I can't because at that point in the file, LoopTableNode is
//  still an incomplete class defnition and its methods have not been
//  declared yet.  But LoopTableNode needs to call LoopRowNode's methods
//  too, so no matter which comes first in the file it won't work.  This
//  global function gets around the problem.)
//
void globalSetTabFlag( LoopTableNode *tbl, bool value )
{
    tbl->setTabFlag(value);
}

#undef IN_AST_CC
