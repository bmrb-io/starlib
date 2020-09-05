/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
/*
//  $Log: template.h,v $
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
 * Revision 1.1.1.1  2001/04/17  10:00:41  madings
 *
 * Revision 2.3  1999/10/24  06:56:59  madings
 * Fixed problem with line numbers being wrong in starlib.
 * Bug was this: It was supposed to return the line/col number of the
 * parent LoopRowNode when a DataValueNode->get{Line|Col}Num() call is
 * made and the NO_POSBITS_DATAVALUENODE define was on.  But it didn't,
 * it returned -1.  The problem was that the parsing in the YACC file
 * lost the line number info while still building the list of DataValueNodes
 * and by the time they got transform()ed into LoopRowNodes, the position
 * info was already gone.  To fix this I made a new type "DVNWithPos" that
 * is a DataValueNode that always has position info even when
 * NO_POSBITS_DATAVALUENODE is defined.  This is the type that now temporarily
 * stores the values before they are transformed into LoopRowNodes.  So the
 * position info is not lost until after loop transformation now.
 *
 * Revision 2.2  1999/01/28  06:07:25  madings
 * Added a template to help the s2nmr program.
 *
 * Revision 2.1  1999/01/23  17:44:55  madings
 * The template.h file used the wrong comment character
 * on the RCS log messages so that after my cvs update it
 * failed to compile (the log message was not commented out).
 * This has been taken care of.
 *
 * Revision 2.0  1999/01/23  03:34:36  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.1  1999/01/22  23:58:31  madings
 * Golly, where should I begin?  Okay, There was a port to linux being
 * done in this version, which in turn meant a port to egcs instead of
 * making code that can only compile on a CFront style compiler like SGI
 * uses.  This port meant a lot of tiny changes all over the place to
 * accomodate the differences in the way templates get linked.  Also,
 * a bug was discovered in the way rows were deleted in the LoopTableNode's
 * erase() operator.  The bug was there all along and it surfaced as part
 * of the portation (what was just bad data in SGI was causing coredumps
 * in Linux.)  Hopefully now that it compiles under egcs, it will be much
 * more portable and I can begin getting it out to the public.
 *
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

*/

////////////////////////////////////////////////////////////////////////

#define IN_TEMPLATE_H
#include "portability.h"

#ifdef NEEDS_EXPLICIT_TEMPLATES

#   include "ast.h"
#   include "list.h"
#   include "astlist.h"
#   include "list.cc"
#   include "astlist.cc"
#   include "scopeTagList.h"
#   include "loopvals.h"
#   include "scanner.h"


TEMPLATE_INSTANTIATE(vector,ASTnode*,vector_ASTnode_ptr)
TEMPLATE_INSTANTIATE(vector,BlockNode *,vector_BlockNode_ptr )
TEMPLATE_INSTANTIATE(vector,DataNameNode *,vector_DataNameNode_ptr )
TEMPLATE_INSTANTIATE(vector,DataNode *,vector_DataNode_ptr )
TEMPLATE_INSTANTIATE(vector,DataValueNode *,vector_DataValueNode_ptr )
TEMPLATE_INSTANTIATE(vector,DVNWithPos *,vector_DVNWithPos_ptr )
TEMPLATE_INSTANTIATE(vector,IterNode *,vector_IterNode_ptr )
TEMPLATE_INSTANTIATE(vector,LoopDefListNode *,vector_LoopDefListNode_ptr )
TEMPLATE_INSTANTIATE(vector,LoopValListNode *,vector_LoopValListNode_ptr )
TEMPLATE_INSTANTIATE(vector,LoopVals *,vector_LoopVals_ptr )
TEMPLATE_INSTANTIATE(vector,int,vector_int)
TEMPLATE_INSTANTIATE(vector,node *,vector_node_ptr )
TEMPLATE_INSTANTIATE(vector,string *,vector_string_ptr )
TEMPLATE_INSTANTIATE(vector,string,vector_string)
TEMPLATE_INSTANTIATE(vector,void *,vector_void_ptr )
TEMPLATE_INSTANTIATE(vector,StarErrEntry,vector_StarErrEntry)

TEMPLATE_INSTANTIATE(List,ASTnode*,List_ASTnode_ptr)
TEMPLATE_INSTANTIATE(List,BlockNode *,List_BlockNode_ptr )
TEMPLATE_INSTANTIATE(List,DataNameNode *,List_DataNameNode_ptr )
TEMPLATE_INSTANTIATE(List,DataNode *,List_DataNode_ptr )
TEMPLATE_INSTANTIATE(List,DataValueNode *,List_DataValueNode_ptr )
TEMPLATE_INSTANTIATE(List,DVNWithPos *,List_DVMWithPos_ptr )
TEMPLATE_INSTANTIATE(List,IterNode *,List_IterNode_ptr )
TEMPLATE_INSTANTIATE(List,LoopDefListNode *,List_LoopDefListNode_ptr )
TEMPLATE_INSTANTIATE(List,LoopValListNode *,List_LoopValListNode_ptr )
TEMPLATE_INSTANTIATE(List,LoopVals *,List_LoopVals_ptr )
TEMPLATE_INSTANTIATE(List,int,List_int)
TEMPLATE_INSTANTIATE(List,char *,List_char_ptr )
TEMPLATE_INSTANTIATE(List,SaveFrameNode *,List_SaveFrameNode_ptr )
TEMPLATE_INSTANTIATE(List,node *,List_node_ptr )
TEMPLATE_INSTANTIATE(List,string *,List_string_ptr )
TEMPLATE_INSTANTIATE(List,string,List_string)
TEMPLATE_INSTANTIATE(List,void *,List_void_ptr )
TEMPLATE_INSTANTIATE(ASTlist,BlockNode *,ASTlist_BlockNode_ptr )
TEMPLATE_INSTANTIATE(ASTlist,DataNameNode *,ASTlist_DataNameNode_ptr )
TEMPLATE_INSTANTIATE(ASTlist,DataNode *,ASTlist_DataNode_ptr )
TEMPLATE_INSTANTIATE(ASTlist,DataValueNode *,ASTlist_DataValueNode_ptr )
TEMPLATE_INSTANTIATE(ASTlist,DVNWithPos *,ASTlist_DVNWithPos_ptr )
TEMPLATE_INSTANTIATE(ASTlist,IterNode *,ASTlist_IterNode_ptr )
TEMPLATE_INSTANTIATE(ASTlist,LoopDefListNode *,ASTlist_LoopDefListNode_ptr )
TEMPLATE_INSTANTIATE(ASTlist,LoopValListNode *,ASTlist_LoopValListNode_ptr )

#endif

#undef IN_TEMPLATE_H
