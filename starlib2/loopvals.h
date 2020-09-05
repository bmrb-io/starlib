/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef LOOPVALS_H
#define LOOPVALS_H

#include "list.h"
#include "astlist.h"
#include "ast.h"

/************************************************************
  $Log: loopvals.h,v $
  Revision 1.3  2001/12/04 20:58:19  madings
  Major changes  - see doc/changes.html for details:
     - setstring / getstring interface at the  loop level
     - implement #<START-SKIP> ... #<END-SKIP> directives
     - fix compiler errors related to use of iterators and const
          on newer versions of gcc.
     - ValCache usage, with LoopRows containing one string for the
          whole row instead of seperate data value nodes.
     - change the type of the *os file stream because the ability to
          construct from a unix fileno() handle has been depreciated in
  	newer C++ stream libraries.

  Revision 1.2  2001/11/13 20:30:35  madings
  This update includes several changes:
  (See doc/changes.html for the full explanation)
     1- The previous compilers we had been using had allowed us to get
         away with some mis-use of the const specifier without flagging
         it as a warning or error.  That is no longer the case, so we had
         to alter some of the code's use of "const".
     2- The STL no longer allows vector iterators to be interchangable
         with pointers, so we lost the ability to use that feature for
         users of the library.
     3- The streams library no longer allows you to make an ofstream out of
         a pipe or standard output.  Callers of this method must now make
         'os' be a pointer to an ostream instead of ofstream.
     4- The newer compilers no longer allow a reference to a temporary store
         that was created just for the duration of the function call.

 * Revision 1.1.1.1  2001/04/17  10:00:41  madings
 *
 * Revision 2.0  1999/01/23  03:34:34  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.2  1998/10/08  03:45:41  madings
 * Two changes:
 *    1 - Most signifigant change is that the library now has
 *    doc++ comments in it.
 *    2 - Also, changed so that when using the copy constructor,
 *    it remembers line numbers and column numbers in the newly
 *    copied tree (I hope, this is untested.)
 *
 * Revision 1.1.1.1  1998/02/12  20:59:00  informix
 * re-creation of starlib project, without sb_lib stuff.  sb_lib has
 * been moved to it's own project.
 *
 * Revision 1.1.1.1  1997/11/07  01:49:06  madings
 * First check-in of starlib and sb_lib in library form.
 *
// Revision 1.4  1997/10/01  22:35:04  bsri
// This version of starlib split main.cc into separate files (which contain
// the transformation functions - one in each file).
//
// Revision 1.3  1997/09/16  22:03:22  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
************************************************************/
// Origianally, Sridevi's code had this as a struct, but I had
// to change it to a class because it had constructors and
// destructors.  Some compilers let you fuzz the line between
// structs and classes like that, but it is not a portable
/// thing to do    -- Steve.
class LoopVals
{
public:

    ///
    DataValueNode *tag;
    ///
    ASTlist <DataValueNode *> *valList;

    ///
    LoopVals(DataValueNode *_tag) : tag(_tag) , valList( new ASTlist<DataValueNode*> ) {}
    ///
    ~LoopVals() { delete valList; }
};

#endif
