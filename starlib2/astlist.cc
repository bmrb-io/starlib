/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
////////////////////////////////// RCS LOG ////////////////////////////////
// $Log: astlist.cc,v $
// Revision 1.5  2008/08/14 18:46:31  madings
// *** empty log message ***
//
// Revision 1.4  2005/11/07 23:30:29  madings
// *** empty log message ***
//
// Revision 1.3  2001/12/04 20:58:18  madings
// Major changes  - see doc/changes.html for details:
//    - setstring / getstring interface at the  loop level
//    - implement #<START-SKIP> ... #<END-SKIP> directives
//    - fix compiler errors related to use of iterators and const
//         on newer versions of gcc.
//    - ValCache usage, with LoopRows containing one string for the
//         whole row instead of seperate data value nodes.
//    - change the type of the *os file stream because the ability to
//         construct from a unix fileno() handle has been depreciated in
// 	newer C++ stream libraries.
//
// Revision 1.2  2001/11/13 20:30:34  madings
// This update includes several changes:
// (See doc/changes.html for the full explanation)
//    1- The previous compilers we had been using had allowed us to get
//        away with some mis-use of the const specifier without flagging
//        it as a warning or error.  That is no longer the case, so we had
//        to alter some of the code's use of "const".
//    2- The STL no longer allows vector iterators to be interchangable
//        with pointers, so we lost the ability to use that feature for
//        users of the library.
//    3- The streams library no longer allows you to make an ofstream out of
//        a pipe or standard output.  Callers of this method must now make
//        'os' be a pointer to an ostream instead of ofstream.
//    4- The newer compilers no longer allow a reference to a temporary store
//        that was created just for the duration of the function call.
//
// Revision 1.1.1.1  2001/04/17  10:00:40  madings
//
// Revision 2.0  1999/01/23  03:34:32  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
//
// Revision 1.7  1999/01/22  23:58:28  madings
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
// Revision 1.6  1998/10/08  03:45:36  madings
// Two changes:
//    1 - Most signifigant change is that the library now has
//    doc++ comments in it.
//    2 - Also, changed so that when using the copy constructor,
//    it remembers line numbers and column numbers in the newly
//    copied tree (I hope, this is untested.)
//
// Revision 1.5  1998/08/29  08:46:01  madings
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
// Revision 1.4  1998/06/12  03:36:48  madings
// Added the InsertAfter() function.
//
// Revision 1.3  1998/06/12  02:52:48  madings
// Added Insert() function to put things in the middle of a list.
//
// Revision 1.2  1998/06/01  21:15:01  madings
// Changed FlattenNestedLoop so it returns a List<> instead of an ASTlist<>.
// ASTlist<> should NEVER, EVER, EVER be used to hold a list that is outside
// the normal star tree.  (ASTlist<> changes the parent pointers of the
// nodes you add to it, so if you use it to hold pointers to nodes in a
// star tree, you change the parent pointers of those nodes.)
//
// Revision 1.1.1.1  1998/02/12  20:59:00  informix
// re-creation of starlib project, without sb_lib stuff.  sb_lib has
// been moved to it's own project.
//
// Revision 1.1.1.1  1997/11/07  01:49:07  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.2  1997/09/16  22:03:18  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.1  1997/09/15  22:28:22  madings
// Split list.h into list.h and astlist.h.  DId the same with list.cc
//
///////////////////////////////////////////////////////////////////////////

#ifdef IS_GCC
#   pragma implementation
#endif
#include "astlist.h"
#include <fstream>
#include <stdlib.h>

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//                  Functions for ASTlist<>
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

///////////////////////////////
//     Constructors:
///////////////////////////////

// Default constructor:
// ====================
template< class T > ASTlist<T>::ASTlist( void )
: List<T>()
{
    parent = (ASTnode*)NULL;
}

// Constructor with parent paremeter given:
// ========================================
template< class T > ASTlist<T>::ASTlist( ASTnode *par )
: List<T>()
{
    parent = par;
}

// Copy constructor:
// =================
template< class T > ASTlist<T>::ASTlist( const ASTlist<T> & L )
: List<T>()
{
    parent = L.myParent();
}
template< class T > ASTlist<T>::ASTlist( const List<T> & L )
: List<T>()
{
    parent = (ASTnode*)NULL;
}

// myParent():
// ===========
template< class T> ASTnode * ASTlist<T>::myParent( void ) const
{
    return  parent;
}

// setParent():
// ============
template< class T> void ASTlist<T>::setParent( ASTnode *par )
{
    parent = par;
}

// AddToEnd():
// ===========
template< class T> void ASTlist<T>::AddToEnd( T addMe )
{
    assert( addMe != NULL );
    ((ASTnode*)addMe)->setParent( parent );
    List<T>::AddToEnd( addMe );
}

// Insert():
// ===========
template< class T> void ASTlist<T>::Insert( T addMe )
{
    assert( addMe != NULL );
    ((ASTnode*)addMe)->setParent( parent );
    List<T>::Insert( addMe );
}

// InsertAfter():
// ===========
template< class T> void ASTlist<T>::InsertAfter( T addMe )
{
    assert( addMe != NULL );
    ((ASTnode*)addMe)->setParent( parent );
    List<T>::InsertAfter( addMe );
}

// AddToEnd():
// ===========
template< class T> void ASTlist<T>::AddToEnd( ASTlist<T> &appendMe )
{
    appendMe.MemorizeCurrent();
    for( appendMe.Reset() ; !appendMe.AtEnd() ; appendMe.Next() )
    {
	AddToEnd( appendMe.Current() );
    }
    appendMe.RestoreCurrent();
}

// Insert():
// ===========
template< class T> void ASTlist<T>::Insert( ASTlist<T> &appendMe )
{
    appendMe.MemorizeCurrent();
    for( appendMe.Reset() ; !appendMe.AtEnd() ; appendMe.Next() )
    {
	((ASTnode*)appendMe.Current())->setParent( parent );
    }
    List<T>::Insert( appendMe );
    appendMe.RestoreCurrent();
}

// InsertAfter():
// ===========
template< class T> void ASTlist<T>::InsertAfter( ASTlist<T> &appendMe )
{
    appendMe.MemorizeCurrent();
    for( appendMe.Reset() ; !appendMe.AtEnd() ; appendMe.Next() )
    {
	((ASTnode*)appendMe.Current())->setParent( parent );
    }
    List<T>::InsertAfter( appendMe );
    appendMe.RestoreCurrent();
}

// AddToEnd():
// ===========
template< class T> void ASTlist<T>::AddToEnd( List<T> &appendMe )
{
    appendMe.MemorizeCurrent();
    for( appendMe.Reset() ; !appendMe.AtEnd() ; appendMe.Next() )
    {
	((ASTnode*)appendMe.Current())->setParent( parent );
    }
    appendMe.RestoreCurrent();
}

// Insert():
// ===========
template< class T> void ASTlist<T>::Insert( List<T> &appendMe )
{
    appendMe.MemorizeCurrent();
    for( appendMe.Reset() ; !appendMe.AtEnd() ; appendMe.Next() )
    {
	AddToEnd( appendMe.Current() );
    }
    List<T>::Insert( appendMe );
    appendMe.RestoreCurrent();
}

// InsertAfter():
// ===========
template< class T> void ASTlist<T>::InsertAfter( List<T> &appendMe )
{
    appendMe.MemorizeCurrent();
    for( appendMe.Reset() ; !appendMe.AtEnd() ; appendMe.Next() )
    {
	AddToEnd( appendMe.Current() );
    }
    List<T>::InsertAfter( appendMe );
    appendMe.RestoreCurrent();
}

// AlterCurrent():
// ===============
template< class T> void ASTlist<T>::AlterCurrent( T setToMe )
{
    ((ASTnode*)setToMe)->setParent( parent );
    List<T>::AlterCurrent( setToMe );
}

// *******************************************************************
// insert from the list classes:
//    need to augment these to update the parent pointer.
// *******************************************************************
template<class T>  void ASTlist<T>::insert(
	iterator pos, iterator first, iterator last )
{
    iterator iter;

    for(   iter = first ;
	   iter != last ;
	   iter++  );
    {  ((ASTnode*)*iter)->setParent( parent );
    }
    List<T>::insert( pos, first, last );
}
template<class T> void ASTlist<T>::insert(
	iterator pos, const T& x )
{
    ((ASTnode*)x)->setParent( parent );
    List<T>::insert( pos, x );
}
template<class T>  void ASTlist<T>::insert(
	iterator pos, size_type n, const T& x)
{
    ((ASTnode*)x)->setParent( parent );
    List<T>::insert( pos, x );
}

// This part is Ugly.  It is here because the g++ compiler is deficient
// in the way it deals with template classes.  I must instantiate
// the classes in *this* source file, and no other.  But that means
// I must also include some other header files so that the classes
// I am instantiating for (like SaveFrameNode for example) are recognized
// by the compiler.  I consider this ugly because this module is *supposed*
// to be more 'low-level' than that, so it makes little sense to include
// the higher-level header file here.

// This was once a separate header file:

#define IN_TEMPLATE_H
#include "portability.h"

#ifdef NEEDS_EXPLICIT_TEMPLATES

#   include "ast.h"
#   include "list.h"
#   include "astlist.h"
#   include "scopeTagList.h"
#   include "loopvals.h"

#endif

#undef IN_TEMPLATE_H
