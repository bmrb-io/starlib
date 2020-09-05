/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef PARSER_GUARD
#define PARSER_GUARD
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
/*****************************************************************
//  $Log: parser.h,v $
//  Revision 1.5  2009/10/06 21:17:06  madings
//  *** empty log message ***
//
//  Revision 1.4  2008/08/14 18:46:31  madings
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
//  Revision 1.2  2001/10/09 19:03:28  madings
//  Modified formatNMRSTAR to return exit codes for errors, and also
//  rolling up some minor changes to the source in terms of formatting.
//
 * Revision 1.1.1.1  2001/04/17  10:00:41  madings
 *
 * Revision 2.1  2000/10/24  21:40:05  madings
 * Previous check-in would not work under Linux because g++ and CC
 * disagree on what the default is when public: or private: has not been
 * explicitly specified.  This version makes it more explicit.
 *
 * Revision 2.0  1999/01/23  03:34:34  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.2  1998/10/08  03:45:43  madings
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
 * Revision 1.1.1.1  1997/11/07  01:49:05  madings
 * First check-in of starlib and sb_lib in library form.
 *
// Revision 1.2  1997/09/16  22:03:24  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.1.1.1  1997/09/12  22:54:39  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:58:02  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:48  madings
//I Added the RCS log and ID variables.
*****************************************************************/
//
//
////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include "scanner.h" // defines TokenType
#include "ast.h"
#include <fstream>

///
int yyparse();     /// the parse function
void yyerror(char *msg);

#endif
