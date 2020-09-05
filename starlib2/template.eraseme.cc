/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//  $Log: template.eraseme.cc,v $
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
// Revision 1.1.1.1  2001/04/17  10:00:41  madings
//
// Revision 1.5  1998/08/29  08:46:11  madings
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
// Revision 1.4  1998/07/17  01:41:33  madings
// Two changes related to Unparse() this revision:
//   1 - Added the ability to programtically insert comments with
//           the get/setPreComment functions.
//   2 - Make Unparse() somewhat more sophisticated to handle formatting
// 	  values so they line up with each other.
//
// Revision 1.3  1998/05/08  20:27:26  informix
// Made a change to handle the C++ STL instead of gcc's libg++.
// Now all strings are of type 'string' instead of 'String' (notice
// the uppercase/lowercase change.)  Applications using this library
// will have to change also, unfortunately, to use the new string style.
//
// Revision 1.2  1998/02/24  05:50:31  informix
// added List<void*> to the template.cc file.
//
// Revision 1.1.1.1  1998/02/12  20:59:00  informix
// re-creation of starlib project, without sb_lib stuff.  sb_lib has
// been moved to it's own project.
//
// Revision 1.1.1.1  1997/11/07  01:49:06  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.3  1997/09/16  22:03:26  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.2  1997/09/15  22:28:30  madings
// Split list.h into list.h and astlist.h.  DId the same with list.cc
//
// Revision 1.1.1.1  1997/09/12  22:54:40  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.5  1997/07/11  22:40:07  madings
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

#define IN_TEMPLATE_CC
#include "portability.h"

#ifdef NEEDS_EXPLICIT_TEMPLATES

#   include "ast.h"
#   include "list.h"
#   include "astlist.h"
#   include "list.cc"
#   include "astlist.cc"
#   include "scopeTagList.h"
#   include "loopvals.h"

template class     List<ASTnode*>;
template class     vector<ASTnode*>;
template class     List<DataNode *>;
template class     vector<DataNode *>;
template class     ASTlist<DataNode *>;
template class     vector<DataNode *>;
template class     List<BlockNode *>;
template class     vector<BlockNode *>;
template class     ASTlist<BlockNode *>;
template class     vector<BlockNode *>;
template class     List<DataNameNode *>;
template class     vector<DataNameNode *>;
template class     ASTlist<DataNameNode *>;
template class     vector<DataNameNode *>;
template class     List<DataValueNode *>;
template class     vector<DataValueNode *>;
template class     ASTlist<DataValueNode *>;
template class     vector<DataValueNode *>;
template class     List<LoopDefListNode *>;
template class     vector<LoopDefListNode *>;
template class     ASTlist<LoopDefListNode *>;
template class     vector<LoopDefListNode *>;
template class     List<LoopValListNode *>;
template class     vector<LoopValListNode *>;
template class     ASTlist<LoopValListNode *>;
template class     vector<LoopValListNode *>;
template class     List<IterNode *>;
template class     vector<IterNode *>;
template class     ASTlist<IterNode *>;
template class     vector<IterNode *>;
template class     List<string>;
template class     vector<string>;
template class     List<string *>;
template class     vector<string *>;
template class     List<int>;
template class     vector<int>;
template class     List<node *>;
template class     vector<node *>;
template class     List<LoopVals *>;
template class     vector<LoopVals *>;
template class     List<void *>;
template class     vector<void *>;

#endif

#undef IN_TEMPLATE_CC
