/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//  $Log: list.cc,v $
//  Revision 1.5  2008/08/14 18:46:31  madings
//  *** empty log message ***
//
//  Revision 1.4  2005/11/07 23:30:30  madings
//  *** empty log message ***
//
//  Revision 1.3  2001/12/04 20:58:19  madings
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
//  Revision 1.2  2001/11/13 20:30:35  madings
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
// Revision 2.3  2000/10/24  21:40:05  madings
// Previous check-in would not work under Linux because g++ and CC
// disagree on what the default is when public: or private: has not been
// explicitly specified.  This version makes it more explicit.
//
// Revision 2.2  1999/01/29  23:03:23  madings
// Fixed bug in copy cnstructor for list so that it uses array notation
// instead of iterators.
//
// Revision 2.1  1999/01/29  22:17:33  madings
// Changed the comparasins that said " i < L.end()" where i is an iterator.
// iterators don't technically have "less than" operators, so the only
// legal check is really "!=".
//
// Revision 2.0  1999/01/23  03:34:33  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
//
// Revision 1.10  1999/01/22  23:58:28  madings
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
// Revision 1.9  1998/12/07  22:53:48  madings
// Checking in multiple error finder for the time being.
// Not done yet but someone needs to check out what I have
// so far...
//
// Revision 1.8  1998/08/29  08:46:06  madings
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
// Revision 1.7  1998/08/18  01:00:07  madings
// Got rid of the "astoper.cc" and "interface.cc" files and appended
// their contents to "ast.cc".  There was no modular reason for them
// to be seperate.
//
// Revision 1.6  1998/06/15  18:40:51  madings
// fixed bug in the way current was updated for InsertAfter()
//
// Revision 1.5  1998/06/15  18:33:30  madings
// I changed the InsertAfter() functions to move the current
// pointer properly.
//
// Revision 1.4  1998/06/12  03:36:51  madings
// Added the InsertAfter() function.
//
// Revision 1.3  1998/06/12  02:52:51  madings
// Added Insert() function to put things in the middle of a list.
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
// Revision 1.1.1.1  1997/11/07  01:49:05  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.4  1997/10/25  02:59:28  madings
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
// Revision 1.3  1997/09/16  22:03:21  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.2  1997/09/15  22:28:25  madings
// Split list.h into list.h and astlist.h.  DId the same with list.cc
//
// Revision 1.1.1.1  1997/09/12  22:54:39  madings
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
//Revision 1.2  1997/06/04  18:52:45  madings
//I Added the RCS log and ID variables.
//
//
////////////////////////////////////////////////////////////////////////

#ifdef IS_GCC
#   pragma implementation 
#endif
#include "list.h"
#include <assert.h>
#include <fstream> // for definition of NULL
#include <stdlib.h>
#include "portability.h"

// Implementation of List template using a linked list with a header node.
// See list.h for documentation.

// **********************************************************************
// List constructor
//   initialize the list to be empty
// **********************************************************************
template<class T> List<T>::List()
{
  myCurrent = size(); // size() = last index + 1, i.e. the index of "end()".
  remember = size();
}


// **********************************************************************
// List copy constructor
// **********************************************************************
template<class T> List<T>::List(const List<T> & L)

// postcondition: list is a copy of L
{
    int i;

    myCurrent = size();
    remember = size();
    for( i = 0 ; i < L.size() ; i++ )
    {
	insert( end(), L[i] );
    }
    myCurrent = size();
    remember = size();
}

// **********************************************************************
// List copy a vector constructor
// **********************************************************************
template<class T> List<T>::List(const vector<T> & L)

// postcondition: list is a copy of L
{
    int i;

    myCurrent = size();
    remember = size();
    for( i = 0 ; i < L.size() ; i++ )
    {
	insert( end(), L[i] );
    }
    myCurrent = size();
    remember = size();
}

// **********************************************************************
// List assignment
// **********************************************************************
template<class T> List<T> & List<T>::operator = (const List & L)
// postcondition: list is a copy of L
{
    if (this != & L)                      // watch aliasing
    {
	(*this) = L;
    }
    return * this;
}

// *****************
// MemorizeCurrent()
// *****************
template<class T> void List<T>::MemorizeCurrent()
{
    remember = myCurrent;
}

// *****************
// RestoreCurrent()
// *****************
template<class T> void List<T>::RestoreCurrent()
{
    myCurrent = remember;
}


// **********************************************************************
// Freelist
//    reinitialize the list to be empty, freeing storage
// **********************************************************************
template<class T> void List<T>::FreeList()
{
    erase( begin(), end() );
    myCurrent = size();
}

// **********************************************************************
// AddToEnd
//    Add the given item k to the end of the list
// **********************************************************************
template<class T> void List<T>::AddToEnd(T k)
{
    push_back( k );
}

// **********************************************************************
// AddToEnd (second version)
//    Add the given list of items to the end of this list.
// **********************************************************************
template<class T> void List<T>::AddToEnd( List<T> &appendMe )
{
    insert( end(), appendMe.begin(), appendMe.end() );
}

// **********************************************************************
// Insert
//    Inserts just before current.
// **********************************************************************
template<class T> void List<T>::Insert(T k)
{
    insert( ( begin() + myCurrent ), k );
    myCurrent++;
}

// **********************************************************************
// Insert (second version)
//    Insert the given list of items to the list.
// **********************************************************************
template<class T> void List<T>::Insert( List<T> &appendMe )
{
    insert( ( begin() + myCurrent ), appendMe.begin(), appendMe.end() );
    myCurrent++;
}

// **********************************************************************
// InsertAfter
//    Inserts just after current.
// **********************************************************************
template<class T> void List<T>::InsertAfter(T k)
{
    insert( (begin() + myCurrent + 1), k );
    myCurrent++;
}

// **********************************************************************
// InsertAfter (second version)
//    Insert the given list of items to the list, just after the current.
// **********************************************************************
template<class T> void List<T>::InsertAfter( List<T> &appendMe )
{
    insert( (begin() + myCurrent + 1), appendMe.begin(), appendMe.end() );
    myCurrent++;
}

// ********************************************************************
// AlterCurrent
// Changes the value of the current item
// *******************************************************************
template <class T> void List<T>::AlterCurrent(T k)
{
  assert(myCurrent < size() );  // else it's past the end.
  (*this)[myCurrent] = k;
}



// *********************************************************************
// RemoveCurrent
//  removes the current item from the list
// *********************************************************************
template <class T> void List<T>::RemoveCurrent()
{

    assert(size() != 0); //can't remove from an empty list
    erase( begin()+myCurrent );
}


// **********************************************************************
// Prev
//   Go back one item
// **********************************************************************
template<class T> void List<T>::Prev()
{
    if( myCurrent != 0 )
	myCurrent--;
}


// **********************************************************************
// AtStart
//   return true iff current item points to the first item
// **********************************************************************
template<class T> bool List<T>::AtStart() const
{
    return (myCurrent == 0 );
}


// **********************************************************************
// Length
//   return the length of the list
// **********************************************************************
template<class T> int List<T>::Length() const
{
    return( size() );
}

// **********************************************************************
// Lookup
// returns true if found else false.  Does not move Current.
// *********************************************************************
template <class T> bool List<T>::Lookup(T k) 
{
    iterator cur;

    for( cur = begin() ; cur != end() ; cur++ )
	if( *cur == k )
	    return  true;
    return false;
}

// *****************************************************************
// SeekTo
// Scan the list until the current ptr is set to the object given.
// If the object given is not in the list, the current remains
// unchanged, and false is returned, else true is returned.
// ******************************************************************
template<class T> bool List<T>::SeekTo( T obj )
{
    iterator cur;
    int      idx;

    for( cur = begin(), idx = 0 ; cur != end() ; cur++, idx++ )
    {   if( *cur == obj )
	{   myCurrent = idx;
	    return true;
	}
    }
    return false;
}

// **********************************************************************
// Reset
//   reset the "current" item to be the first one
// **********************************************************************
template<class T> void List<T>::Reset()
{
  myCurrent = 0;
}

// **********************************************************************
// Next
//   advance the "current" item
// **********************************************************************
template<class T> void List<T>::Next()
{
    if( myCurrent < size() )
	myCurrent++;
}

// **********************************************************************
// AtEnd
//   return true iff current item is past last item
// **********************************************************************
template<class T> bool List<T>::AtEnd() const
{
  return ( size() == myCurrent );
}

// **********************************************************************
// Current
//   return current item (error if AtEnd)
// **********************************************************************
template<class T> T List<T>::Current()
{
    if( myCurrent > size() )
	myCurrent = size();   // This is end();
    if( myCurrent < 0 )
	myCurrent = 0;        // This is begin();
    return( (*this)[myCurrent] );
}

// **********************************************************************
// Last
//   return last item (NULL if empty)
// **********************************************************************
template<class T> T List<T>::Last() const
{
  if (size() == 0)
    return NULL;
  else
    return( *(end() - 1) );
}

// **********************************************************************
// Append
//    Given: list L
//    Do:    add all items in L to the end of this list
// **********************************************************************
template<class T> void List<T>::Append( List<T> & L)
{
    insert( end(), L.begin(), L.end() );
}

// *******************************************************************
// insert and erase from the vector classes:
//    These call the vector insert/erase operation, and update the
//    myCurrent if need be:
// *******************************************************************
template<class T>  void List<T>::insert(
	iterator pos, const T& x )
{
    // This has been recently changed, so check it thoroughly:
    // Once it was this:
    //    if( &((*this)[myCurrent]) != end() )
    //        if( &((*this)[myCurrent]) >= pos )
    if( myCurrent < size() )
	if( begin() + myCurrent >= pos )
	    myCurrent++;
    vector<T>::insert( pos, x );
}
template<class T>  void List<T>::insert(
	iterator pos, iterator first, iterator last )
{
    iterator iter;
    int      count;

//    if( &((*this)[myCurrent]) != end() )
//	if( &((*this)[myCurrent]) >= pos )
    if( myCurrent < size() )
	if( begin() + myCurrent >= pos )
	{
	    for(   iter = first, count = 0 ;
		   iter != last ;
		   iter++, count++   )
	    { }
	    myCurrent += count;
	}
    vector<T>::insert( pos, first, last );
}
template<class T>  void List<T>::insert(
	iterator pos, size_type n, const T& x)
{
//    if( &((*this)[myCurrent]) != end() )
//	if( &((*this)[myCurrent]) >= pos )
    if( myCurrent <= size() )
	if( begin() + myCurrent >= pos )
	    myCurrent++;
    vector<T>::insert( pos, x );
}

template<class T>  void List<T>::erase(
	iterator pos )
{
//    if( &((*this)[myCurrent]) != end() )
//	if( &((*this)[myCurrent]) > pos )
    if( myCurrent < size() )
	if( begin() + myCurrent > pos )
	    myCurrent--;
    vector<T>::erase( pos );
}

template<class T>  void List<T>::erase(
	iterator first, iterator last )
{
    iterator iter;
    int      count;

//    if( &((*this)[myCurrent]) != end() )
//	if( &((*this)[myCurrent]) > first )
    if( myCurrent < size() )
	if( begin() + myCurrent > first )
	{
	    for(   iter = first, count = 0 ;
		   iter != last  && iter != end() ;
		   iter++, count++ )
	    {}
	    myCurrent -= count;
	    if( myCurrent < 0 )  // (myCurrent was in the deleted range and
		                 // was thus decremented too far)
		myCurrent = 0;
	}
    vector<T>::erase( first, last );
}

template<class T>  void List<T>::reserve( size_type n )
{
    vector<T>::reserve( n );
}

template<class T>  size_t List<T>::capacity( void )
{
    return vector<T>::capacity();
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

#undef IN_TEMPLATE_H
