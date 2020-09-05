/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//  $Log: scopeTagList.cc,v $
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
// Revision 2.0  1999/01/23  03:34:35  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
//
// Revision 1.3  1999/01/22  23:58:30  madings
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
// Revision 1.2  1998/05/08  20:27:25  informix
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
// Revision 1.1.1.1  1997/09/12  22:54:39  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.5  1997/07/30  21:34:26  madings
//check-in before switching to cvs
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:57:59  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:46  madings
//I Added the RCS log and ID variables.
//
//
////////////////////////////////////////////////////////////////////////
#ifdef IS_GCC
#   pragma implementation
#endif

#include "scopeTagList.h"


ScopeTagList::ScopeTagList()
{
  L = new List <node*>;
}


//This function is  inserts a saveframe,tagname pair into the structure
 
void ScopeTagList::Insert(string old_saveframe, string tagName)
{
  bool insertComplete  = false;
  L->Reset();
  while(!insertComplete && !L->AtEnd())
    {
      if(L->Current()->old_saveframe == old_saveframe)
	{
	  L->Current()->tagList->AddToEnd(tagName);
	  insertComplete = true;
	}
      L->Next();
    }

  if(!insertComplete)
    {
      node* tmpNode = new node;
      tmpNode->old_saveframe = old_saveframe;
      tmpNode->tagList->AddToEnd(tagName);
      L->AddToEnd(tmpNode);
    }
  return;
} 


//Given a saveframe name, this returns the list of tags associated with this name
List<string>* ScopeTagList:: Lookup(string old_saveframe)
{
  bool found = false;
  List<string>* rtnList = NULL;
  L->Reset();
  while(!L->AtEnd() && !found)
    {
      if(L->Current()->old_saveframe == old_saveframe)
	{
	  rtnList = new List<string>;
	  (*rtnList) = *(L->Current()->tagList);
	  found = true;
	}
      L->Next();
    }
  return rtnList;
}

