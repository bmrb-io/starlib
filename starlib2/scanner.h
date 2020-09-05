/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef SCANNER_GUARD
#define SCANNER_GUARD
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
/**************************************************************
//  $Log: scanner.h,v $
//  Revision 1.6  2010/06/30 19:37:42  madings
//  *** empty log message ***
//
//  Revision 1.5  2009/10/12 18:13:10  madings
//  *** empty log message ***
//
//  Revision 1.4  2008/08/14 18:46:32  madings
//  *** empty log message ***
//
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
 * Revision 2.4  2000/10/24  21:40:06  madings
 * Previous check-in would not work under Linux because g++ and CC
 * disagree on what the default is when public: or private: has not been
 * explicitly specified.  This version makes it more explicit.
 *
 * Revision 2.3  1999/04/28  02:29:01  madings
 * Made the searchByTag and searchByTagValue functions case-insensitive
 * for tag-names, datablock names, and saveframe names (but not data values).
 *
 * Revision 2.2  1999/03/04  20:54:11  madings
 * Added WARN_BASE_ID to the enum.
 *
 * Revision 2.1  1999/03/04  20:32:43  madings
 * Added the ability to find some warning-level errors.
 * and made the new program called "star_syntax"
 *
 * Revision 2.0  1999/01/23  03:34:35  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.5  1999/01/22  23:58:30  madings
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
 * Revision 1.4  1998/12/19  06:54:00  madings
 * Added multiple error handing (recovery) and caller-controlled
 * error handling.
 *
 * Revision 1.3  1998/10/08  03:45:44  madings
 * Two changes:
 *    1 - Most signifigant change is that the library now has
 *    doc++ comments in it.
 *    2 - Also, changed so that when using the copy constructor,
 *    it remembers line numbers and column numbers in the newly
 *    copied tree (I hope, this is untested.)
 *
 * Revision 1.2  1998/05/08  20:27:25  informix
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
// Revision 1.2  1997/09/16  22:03:25  madings
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
//Revision 1.3  1997/06/04  18:58:02  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:49  madings
//I Added the RCS log and ID variables.
**************************************************************/
//
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <fstream>
#include "portability.h"
#include "port_vector.h"
#include "ast.h"

///
extern FILE *yyin;  /// the file from which the scanner reads
int yylex();        // the scan function

/// The type of the value associated with each token
struct TokenType {
  ///
  int lineNum;
  ///
  int colNum;
  ///
  string *strVal;
};

/// The ID number for the kinds of errors there are:
enum StarErrID
{
    /** Some unknown error for which no explicit error handling
      * code was written in the library.
      */
    STAR_UNKNOWN_ERR,

    /** Expected a global_ or data_, but got something else. */
    EXP_GLOBAL_OR_DATA,

    /** Inside a global_ or data_ -  Expected a DataNode
      * (tag/value, loop_ or saveframe), but instead got
      * something else.
      */
    EXP_DATANODE,

    /** Inside a saveframe - Expected a loop_ or tag/value,
      * but got something else instead:
      */
    EXP_ITEM_OR_LOOP,

    // ---- Above this point, put all the error-level IDs.
    // ---- Below this point, put all the warning-level IDs. (>500)
    WARN_BASE_ID = 500,

    /** Warning level error - keyword in data value - possibly non
      * terminated semicolon string.
      */
    WARN_KEYWORD_IN_VALUE, // Makes the next values all above 500.

    /** Warning level error - number of values on a line of text does
      * not match the number of names in the taglist.
      */
    WARN_LOOP_VALUES_MISALIGNED
};

/// The struct that holds a single error message from the parser:
struct StarErrEntry {
    /// Identifier number for the type of error.  For a text description, see errStr below.
    StarErrID errID;
    /// line of the input text where the error was reported:
    int lineNum;
    /// column of the input text where the error was reported:
    int colNum;
    /// string of the token where the error was reported:
    char tokenStr[1024];
    /// string of text describing the error in English:
    char errStr[4096];
};

/** The collection of all the error messages that were built up in the parsing.
  * This vector is populated by the Star library when you run yyparse().
  * This vector does not need to be deleted by the program using Starlib.
  * The vector will disappear cleanly on its own when it goes out of scope
  * because it uses no pointers.
  */
extern vector<StarErrEntry> starErrReport;

#endif
