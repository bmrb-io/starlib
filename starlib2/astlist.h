/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef _ASTLIST_GUARD
#define _ASTLIST_GUARD

//  History Log:
//--------------------------------
/*********************************************************************
//  $Log: astlist.h,v $
//  Revision 1.4  2005/11/07 23:30:29  madings
//  *** empty log message ***
//
//  Revision 1.3  2001/12/04 20:58:18  madings
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
//  Revision 1.2  2001/11/13 20:30:34  madings
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
 * Revision 1.1.1.1  2001/04/17  10:00:40  madings
 *
 * Revision 2.0  1999/01/23  03:34:32  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.6  1999/01/22  23:58:28  madings
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
 * Revision 1.5  1998/10/08  03:45:37  madings
 * Two changes:
 *    1 - Most signifigant change is that the library now has
 *    doc++ comments in it.
 *    2 - Also, changed so that when using the copy constructor,
 *    it remembers line numbers and column numbers in the newly
 *    copied tree (I hope, this is untested.)
 *
 * Revision 1.4  1998/08/29  08:46:03  madings
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
 * Revision 1.3  1998/06/12  03:36:49  madings
 * Added the InsertAfter() function.
 *
 * Revision 1.2  1998/06/12  02:52:50  madings
 * Added Insert() function to put things in the middle of a list.
 *
 * Revision 1.1.1.1  1998/02/12  20:59:00  informix
 * re-creation of starlib project, without sb_lib stuff.  sb_lib has
 * been moved to it's own project.
 *
 * Revision 1.1.1.1  1997/11/07  01:49:07  madings
 * First check-in of starlib and sb_lib in library form.
 *
 * Revision 1.2  1997/09/16  22:03:19  madings
 * Changed searchByTag() and searchByTagValue() so that they will
 * return lists instead of returning single hits, and extended
 * them to levels of the star tree above "saveframe" so it is now
 * possible to use them on the whole star file.
 * (Also changed the history log comments of several header files
 * to use slash-star comment style instead of '//' style.)
 *
*********************************************************************/
//--------------------------------

#ifdef IS_GCC
#   pragma interface
#endif
#include "portability.h"
#include "list.h"
#include "astnode.h"

  //--------------------------------
 //  ASTlist - list of ASTnodes    //
//--------------------------------
/**
  *  The purpose of this type is to be a template class just like
  *  the base "List" is, but with the added caveat that it works
  *  on ASTnodes that are part of an AST star file tree.
  *  The idea is that the list itself remembers who it's parent
  *  is, and therefore it can propigate that parent on to any
  *  new AST objects that are attached to the list.
  *
  *  WARNING ! WARNING ! WARNING ! WARNING ! WARNING ! WARNING !
  *  WARNING ! WARNING ! WARNING ! WARNING ! WARNING ! WARNING !
  *  WARNING ! WARNING ! WARNING ! WARNING ! WARNING ! WARNING !
  *  ===========================================================
  *       Although this class is a template that looks like
  *       it should work on any class, it REALLY only works
  *       correctly on classes derived from ASTnode.
  *       It will crash the code (segfault) if you try to use
  *       this list on a type which is not derived from
  *       ASTnode.  Due to a nasty syntax catch-22, I was forced
  *       to make this look like it would operate on any generic
  *       class.  (template <ASTnode*T> would not work).
  */
template <class T> class ASTlist : public List <T>
{
  public:
    
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
      //--------------------------------
     //  constructors //
    //--------------------------------

    ASTlist();                   /// constructor (default)
    ASTlist( ASTnode *parent );  /// constructor (giving the parent node)
    ASTlist(const ASTlist<T> & L);  /// copy constructor
    ASTlist(const List<T> & L);  /// copy constructor for List<>.

      //--------------------------------
     //  myParent()  //
    //--------------------------------
    /** myParent() - gets the parent ASTnode that this list
      * is contained inside.  Returns NULL if this list is
      * not contained inside a parent ASTnode.
      */
    ASTnode *myParent( void ) const;

      //--------------------------------
     //  setParent()  //
    //--------------------------------
    /** setParent() - sets the parent ASTnode that this list is
      *  inside of.  This parent will be propigated to any new
      *  ASTnodes that are added to this ASTlist.
      *  Thus if a user of the AST library gets a hold of one
      *  of the ASTlists that is in the library, that user can
      *  add new items to that list and still retain integrety.
      */
    void setParent( ASTnode *setToThis );

      //--------------------------------
     //  other changes  //
    //--------------------------------
    //@{
    /** <font size=+1>
      * The following functions are identical to their equivilent
      *  functions in List<>, but they have been changed to ensure
      *  that the newly added ASTnodes will have the proper parent
      *  pointer copied to them.
      *  </font>
      */
    //@}

    /// Identical to the STL vector method of the same prototype.
    void insert( iterator pos, const T& x );
    /// Identical to the STL vector method of the same prototype.
    void insert( iterator pos, iterator first, iterator last );
    /// Identical to the STL vector method of the same prototype.
    void insert( iterator pos, size_type n, const T& x);

    /// <em>DEPRECIATED</em>
    void AddToEnd(ASTlist<T> &appendMe);  // concatenate a list to the end.
    void AddToEnd(List<T> &appendMe);  // concatenate a list to the end.
    void AddToEnd( T addMe ); // concatenate one item to the end.
    /// <em>DEPRECIATED</em>
    void Insert(ASTlist<T> &appendMe);  // concatenate a list before current.
    void Insert(List<T> &appendMe);  // concatenate a list before current.
    void Insert( T addMe ); // insert one item before current.
    /// <em>DEPRECIATED</em>
    void InsertAfter(ASTlist<T> &appendMe);  // concatenate a list after cur.
    void InsertAfter(List<T> &appendMe);  // concatenate a list after cur.
    void InsertAfter( T addMe ); // insert one item after current.

    /// <em>DEPRECIATED</em>
    void AlterCurrent(T k); //Changes the value of current item to k

  private:
    ASTnode *parent;
};

#endif

