/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//  HISTORY LOG:
//  ------------
//  $Log: convert.cc,v $
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
// Revision 2.0  1999/01/23  03:34:33  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
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
// Revision 1.1.1.1  1997/11/07  01:49:07  madings
// First check-in of starlib and sb_lib in library form.
//
// Revision 1.1.1.1  1997/09/12  22:54:41  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:57:57  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:44  madings
//I Added the RCS log and ID variables.
//
//
////////////////////////////////////////////////////////////////////////

#include "interface.h"

ostream & os = cout;

main()
{
  StarFileNode *zinc = new StarFileNode;

  zinc->AddDataBlock(string("global_"));
  zinc->AddSaveFrame(string("save_depositor_information"));
  zinc->AddSaveFrameDataItem(string("_primary_full_name"), 
			     string("evil dave argentar"));
  zinc->AddSaveFrameDataItem(string("_primary_mailing_address"), 
			     string("420 henry mall\nmadison, wi 53706\n"));
  
  zinc->AddSaveFrame(string("save_molecules_studied"));
  zinc->AddSaveFrameLoop();

  zinc->AddSaveFrameLoopDataName(string("_seq_nonst_label"));
  zinc->AddSaveFrameLoopDataName(string("_space_22"));
  zinc->AddSaveFrameLoopDataName(string("_seq_nonst_residue"));
  zinc->AddSaveFrameLoopDataName(string("_seq_nonst_standard_residue"));
  zinc->AddSaveFrameLoopDataName(string("_seq_nonst_describe"));

  zinc->AddSaveFrameLoopDataValue(string("$ornithine"));
  zinc->AddSaveFrameLoopDataValue(string("."));
  zinc->AddSaveFrameLoopDataValue(string("ornithine"));
  zinc->AddSaveFrameLoopDataValue(string("ornithine"));
  zinc->AddSaveFrameLoopDataValue(string("standard ornithine residue"));

  zinc->AddSaveFrameLoopDataValue(string("$p-tyr"));
  zinc->AddSaveFrameLoopDataValue(string("."));
  zinc->AddSaveFrameLoopDataValue(string("p-tyr"));
  zinc->AddSaveFrameLoopDataValue(string("phosphotyrosine"));
  zinc->AddSaveFrameLoopDataValue(string("O-phosphorylated tyrosine"));

  zinc->AddSaveFrameDataItem(string("_primary_mailing_address"), 
			     string("420 henry mall\nmadison, wi 53706\n"));  

  zinc->Unparse();
}
