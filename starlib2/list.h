/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef _LIST_GUARD
#define _LIST_GUARD
//-------------------------- RCS COMMENTS: ----------------------------
//  HISTORY LOG:
//  ------------
/*******************************************************************
//  $Log: list.h,v $
//  Revision 1.6  2005/11/07 23:30:30  madings
//  *** empty log message ***
//
//  Revision 1.5  2002/09/28 00:22:12  madings
//  The std:: in list.h (see previous log message) was a mistake.
//  It won't compile on most compilers.  It was removed.
//
//  Revision 1.4  2002/09/27 22:49:59  madings
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
 * Revision 1.1.1.1  2001/04/17  10:00:41  madings
 *
 * Revision 2.1  2000/10/24  21:40:05  madings
 * Previous check-in would not work under Linux because g++ and CC
 * disagree on what the default is when public: or private: has not been
 * explicitly specified.  This version makes it more explicit.
 *
 * Revision 2.0  1999/01/23  03:34:33  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.9  1999/01/22  23:58:29  madings
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
 * Revision 1.8  1998/12/07  22:53:49  madings
 * Checking in multiple error finder for the time being.
 * Not done yet but someone needs to check out what I have
 * so far...
 *
 * Revision 1.7  1998/10/08  03:45:40  madings
 * Two changes:
 *    1 - Most signifigant change is that the library now has
 *    doc++ comments in it.
 *    2 - Also, changed so that when using the copy constructor,
 *    it remembers line numbers and column numbers in the newly
 *    copied tree (I hope, this is untested.)
 *
 * Revision 1.6  1998/08/29  08:46:07  madings
 * This one was a rather large change.  I haven't finished
 * documenting and testing all of it, so expect another
 * revision or two soon.
 *
 * This revision creates a new way of dealing with loops.  Now
 * the caller can use the loops like 2-D vectors, but with some
 * extra magic behind the scenes to ensure that no insertions or
 * deletions that can leave the tree in an invalid state will
 * be allowed.  (For example, you cannot remove a value from the
 * loop without also removing the tagname for it and all the other
 * values in that column.)
 *
 * Also in this revision is a fix to the previous revision's method
 * of making List and ASTlist be wrappers around STL vector.  The
 * previous version of the library broke when the STL library decided
 * to move the vector to a new location in memory (realloc), and the
 * myCurrent interator became invalid.  Now myCurrent is an integer
 * index offset instead, so it moves with the vector.
 *
 * Also in this revision are some fixes to a longstanding problem that
 * made the library incapable of parsing nested loops unless the nested
 * loops were fully populated.  (In other words if an inner loop for any
 * row was null, because the input file simply has a "stop_" and no
 * data for the inner loop, it used to crash on that.)  Now it works and
 * empty inner loops are represented to the caller as null pointers (so
 * don't blindly follow inner loop pointers that are handed to you - check
 * them first.)
 *
 * Note that this marks my first attempt to make a class that implements
 * the STL vector interface, so I expect to find some bugs to fix in the
 * next revision or two.  There's some strange C++ "magic" going on here
 * to get this to work.
 *
 * Revision 1.5  1998/08/18  01:00:08  madings
 * Got rid of the "astoper.cc" and "interface.cc" files and appended
 * their contents to "ast.cc".  There was no modular reason for them
 * to be seperate.
 *
 * Revision 1.4  1998/06/12  03:36:52  madings
 * Added the InsertAfter() function.
 *
 * Revision 1.3  1998/06/12  02:52:53  madings
 * Added Insert() function to put things in the middle of a list.
 *
 * Revision 1.2  1998/06/09  21:03:10  madings
 * Changed to explicitly include bool.h because "CC" will only
 * include it automatically when doing mips3 or mips4 code,
 * but not mips2 code.  I have no idea why...
 *
 * Revision 1.1.1.1  1998/02/12  20:59:00  informix
 * re-creation of starlib project, without sb_lib stuff.  sb_lib has
 * been moved to it's own project.
 *
 * Revision 1.1.1.1  1997/11/07  01:49:06  madings
 * First check-in of starlib and sb_lib in library form.
 *
// Revision 1.3  1997/09/16  22:03:21  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.2  1997/09/15  22:28:26  madings
// Split list.h into list.h and astlist.h.  DId the same with list.cc
//
// Revision 1.1.1.1  1997/09/12  22:54:40  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.6  1997/07/30  21:34:26  madings
//check-in before switching to cvs
//
//Revision 1.5  1997/07/11  22:40:01  madings
//Finally seems to work - just waiting for
//new dictionaries.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:58:01  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:48  madings
//I Added the RCS log and ID variables.
*******************************************************************/
//
//
//

// **********************************************************************
// The List template implements an ordered list of items with a "current" item.
// To declare a list of items of type T use: List<T>, e.g., List<int> intlist;
// **********************************************************************
//
// Member functions
// ================
//
// constructors/destructor
// =======================
// List()               -- constructor: creates a list containing no items
// List(const List & L) -- copy constructor: creates a copy of L
// ~List()              -- destructor: cleans up as necessary
//
// mutators/modifiers
// ==================
// void AddToEnd(T k) -- add k to the end of the list
// void Insert(T k)   -- insert k to list just before Current().
// void InsertAfter(T k)   -- insert k to list just after Current().
// void FreeList()    -- reinitialize the list to be empty, freeing storage
//
// other operations
// ================
// int Length()         -- length of list
// void Reset()         -- reset "current" item to be first item
// void Next()          -- advance current item
// bool AtEnd()         -- is current item past last item?
// T Current()          -- return current item (error if AtEnd)
// bool SeekTo()	-- Find the node that matches the given object,
//                    	      and move 'current' to that spot.  If no
//                    	      such node exists in the list, then current
//                    	      remains where it is, and false is returned.
// List & operator =    -- (assignment)
//
// Note: to iterate through a List L use code like this:
//      L.Reset();
//	while (!L.AtEnd()) {
//	  ...some code involving L.Current()...
//	  L.Next();
//	}
//                                               
// **********************************************************************

#ifdef IS_GCC
#   pragma interface
#endif
#include "portability.h"
#include "port_vector.h"

/**
  * This is the generic list class that everything else is built
  * on top of.  Originally this was a linked list implementation,
  * but that became cumbersome, so now it is a STL vector with
  * some interface functions to envelope this fact.  These interface
  * functions are mostly there to just support backward compatability
  * with older code that uses this class like it was a linked list.
  * <p>
  * <font size=+1>
  * Any of the methods listed here should be considered
  * <em>DEPRECIATED</em> and therefore not to be used by any
  * new programs writted from scratch.
  * </font>
  * <p>
  * For any new software written for this class, please restrict
  * yourself to using this List class as if it were a STL vector
  * class, and manipulate it that way.  
  */
template <class T> class List : private vector<T>
{

  public:
    ///
    List();                  // constructor (default)
    ///
    List(const List<T> & L); // copy constructor
    ///
    List(const vector<T> & L); // another copy constructor

    // mutator/modifier member functions

    ///
    void AddToEnd(T k);
    ///
    void Insert(T k);
    ///
    void InsertAfter(T k);
    ///
    void AddToEnd(List<T> &appendMe );  // concatenate a list on the end.
    ///
    void Insert(List<T> &appendMe );  // Insert a list.
    ///
    void InsertAfter(List<T> &appendMe );  // Insert a list.

    ///
    void FreeList();
    ///
    void RemoveCurrent();
    ///
    void AlterCurrent(T k); //Changes the value of current item to k
    ///
    void MemorizeCurrent(); // remember where the current ptr is.
    ///
    void RestoreCurrent();  // restore the current ptr to what was remembered in memorizeCurrent;.

    // other operations
    ///
    int Length() const;
    ///
    bool Lookup(T k);
    ///
    bool SeekTo( T obj );
    ///
    void Reset();
    ///
    void Next();
    ///
    void Prev();
    ///
    bool AtEnd() const;
    ///
    bool AtStart() const;
    ///
    T Current();
    ///
    T Last() const;
    ///
    List & operator = (const List<T> & dl); // assignment


    // typedefs added to make this look like the vector class:
    ///
    typedef T value_type;
    ///
    typedef value_type* pointer;
    ///
    typedef const value_type* const_pointer;
    ///
    typedef value_type& reference;
    ///
    typedef const value_type& const_reference;
    ///
    typedef size_t size_type;
    ///
    typedef size_t difference_type;

    ///
    typedef typename vector<T>::iterator iterator;
    ///
    typedef typename vector<T>::const_iterator const_iterator;
    ///
    typedef typename vector<T>::reverse_iterator reverse_iterator;
    ///
    typedef typename vector<T>::const_reverse_iterator const_reverse_iterator;

    //@{
	/** <font size=+1>
	  * All of the following work exactly as they do in
	  * the regular STL vector class:
	  * </font>
	  */
	///
	iterator begin() { return vector<T>::begin(); };
	///
	iterator end() { return vector<T>::end(); };
	///
	reverse_iterator rbegin() { return vector<T>::rbegin(); };
	///
	reverse_iterator rend() { return vector<T>::rend(); };
	///
	const_iterator begin() const { return vector<T>::begin(); };
	///
	const_iterator end() const { return vector<T>::end(); };
	///
	size_t  size() const { return vector<T>::size(); };
	///
	bool empty() const { return vector<T>::empty(); };
	///
	T& operator[]( size_type n ) { return (*((vector<T>*)this))[n]; };
	///
	const T& operator[]( size_type n) const { return (*((vector<T>*)this))[n]; };
	///
	T& front() { return vector<T>::front(); };
	///
	const T& back() const { return vector<T>::back(); };
	///
	const T& front() const { return vector<T>::front(); };
	///
	T& back() { return vector<T>::back(); };
    //@}

    //@{
	/** <font size=+1>
	  * All of the following are modified a bit to deal with
	  * the myCurrent() index, but to the outside world they look
	  * like the normal vector operations:
	  * </font>
	  */
	///
	void insert( iterator pos, const T& x );
	///
	void insert( iterator pos, iterator first, iterator last );
	///
	void insert( iterator pos, size_type n, const T& x);
	///
	void erase( iterator pos );
	///
	void erase( iterator first, iterator last );
	///
	void reserve( size_t n );
	///
	size_t capacity( void );
    //@}

  private:
    int  myCurrent;       // idx to "current" item
    int  remember;        // remembers the current position temporarily.
    void Append( List<T> & L); // private operation
};

#endif


