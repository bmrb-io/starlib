/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
/*************************************************************
//  $Log: interface.h,v $
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
 * Revision 2.0  1999/01/23  03:34:33  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.3  1998/10/08  03:45:39  madings
 * Two changes:
 *    1 - Most signifigant change is that the library now has
 *    doc++ comments in it.
 *    2 - Also, changed so that when using the copy constructor,
 *    it remembers line numbers and column numbers in the newly
 *    copied tree (I hope, this is untested.)
 *
 * Revision 1.2  1998/05/08  20:27:24  informix
 * Made a change to handle the C++ STL instead of gcc's libg++.
 * Now all strings are of type 'string' instead of 'String' (notice
 * the uppercase/lowercase change.)  Applications using this library
 * will have to change also, unfortunately, to use the new string style.
 *
 * Revision 1.1.1.1  1998/02/12  20:59:00  informix
 * re-creation of starlib project, without sb_lib stuff.  sb_lib has
 * been moved to it's own project.
 *
 * Revision 1.1.1.1  1997/11/07  01:49:06  madings
 * First check-in of starlib and sb_lib in library form.
 *
// Revision 1.3  1997/10/10  18:34:24  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.2  1997/09/16  22:03:20  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.1.1.1  1997/09/12  22:54:40  madings
// Restarting CVS tree because I lost the CVS dir.
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
*************************************************************/
//
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"

///
class StarListNode;

class ASTnode
{ 
 public:
  ///
  virtual void Unparse(int indent = 0) = 0;

  ///
  protected :
  void printIndent(int indent);
};

///
class StarFileNode: public ASTnode
{
 public:
  ///
  enum Status { OK, ERROR };

  ///
  StarFileNode(StarListNode *n);
  ///
  StarFileNode();

  ///
  virtual void Unparse(int indent=0);

  ///
  StarFileNode::Status AddDataBlock(const string & name);
  ///
  StarFileNode::Status AddSaveFrame(const string & name);
  ///
  StarFileNode::Status AddSaveFrameDataItem( const string & name,
	                                     const string & value,
					     DataValueNode::ValType type);
  ///
  StarFileNode::Status AddSaveFrameLoop();
  ///
  StarFileNode::Status AddSaveFrameLoopDataName(const string & name);
  ///
  StarFileNode::Status AddSaveFrameLoopDataValue( const string & value,
						  DataValueNode::ValType type );
  
 private:
  ///
  StarListNode *myStarList;
};

