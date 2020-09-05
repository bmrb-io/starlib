/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//  $Log: message.cc,v $
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
//  Revision 1.2  2001/10/09 19:03:28  madings
//  Modified formatNMRSTAR to return exit codes for errors, and also
//  rolling up some minor changes to the source in terms of formatting.
//
// Revision 1.1.1.1  2001/04/17  10:00:41  madings
//
// Revision 2.0  1999/01/23  03:34:34  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
//
// Revision 1.4  1999/01/22  23:58:29  madings
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
// Revision 1.3  1998/12/07  22:53:49  madings
// Checking in multiple error finder for the time being.
// Not done yet but someone needs to check out what I have
// so far...
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
// Revision 1.1.1.1  1997/09/12  22:54:40  madings
// Restarting CVS tree because I lost the CVS dir.
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
static char *rcs_ident = "$Id: message.cc,v 1.3 2001/12/04 20:58:19 madings Exp $";
#include <string.h>
#include "portability.h"
#include <stdlib.h>
#include "message.h"
#include "scanner.h"
#include "ast.h"
#include "gram.tab.h"

#ifdef UNCOMMENT_MESSAGE_CC
//*******************************************************************
//Parser error handling function which prints an error message when
//the bison-generated parser detects a syntax error in the input.
//This function is called by yyerror(). The line and column numbers
//of the error are also printed out.
//*******************************************************************
void parser_error(char *mesg, line, col, char *tokentext)
{
  fprintf(stderr, " %s: on token '%s' (line %d, column %d)\n",
	  errmsg, tokentext, line, col );
  return;
}
	
#endif

//*******************************************************************
//Scanner error handling function. 
//*******************************************************************
void scanner_error(int linenum, int colnum, string mesg)
{
  cerr << linenum << ":" <<  colnum;
  cerr << " ERROR " << mesg << endl;
  return;
}

//*******************************************************************
//Scanner warning function.
//*******************************************************************
void scanner_warning(int linenum, int colnum, string mesg)
{
  cerr << linenum << ":" <<  colnum;
  cerr << " WARNING " << mesg << endl;
  return;
}
