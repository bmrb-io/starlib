/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef TRANSFORMS_H 
#define TRANSFORMS_H

#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "portability.h"
#include "ast.h"
#include "scopeTagList.h"
#include "list.h"
#include "astlist.h"
#include "loopvals.h"

///
extern ostream * os;

///
extern StarFileNode *AST;
///
extern StarFileNode *NMR;
///
extern StarFileNode *DIC;

//The following file contains a list of functions. Each of these function performs
//a specific STAR to NMR transformation. Each function takes as input pointers
//to two STAR AST's -- both of which are identical to begin with-- then, depending
//on the nature of the transformation the NMR tree is modified. Additionally,
//each function takes in a pointer to a datablock of the dictionary. If the
//dictionary consists of multiple datablocks then these functions will have to called
//repeatedly for each data block (as shown in main()).

  /////////////////////////
 //  save_remove_tag()  //
/////////////////////////
///This function does transform 5.
void save_remove_tag( StarFileNode *input,
	              StarFileNode *output,
		      BlockNode    *currDicDataBlock );

  ///////////////////////////
 //  save_renamed_tags()  //
///////////////////////////
///This function does transformation 1a). 
void save_renamed_tags(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode* currDicDataBlock);


  ///////////////////////////////
 //  save_tag_to_framecode()  //
///////////////////////////////
///This function does transformation 1b). 
void save_tag_to_framecode(StarFileNode* AST,
			   StarFileNode* NMR,
			   BlockNode* currDicDataBlock);

  /////////////////////////////////
 //  save_keyed_renamed_tags()  //
/////////////////////////////////
///This function does transformation 1c). 
void save_keyed_renamed_tags( StarFileNode* AST,
		              StarFileNode* NMR,
		              BlockNode* currDicDataBlock );


  ///////////////////////////////
 //  save_all_tag_to_scope()  //
///////////////////////////////
///This function does transformation 2
void save_all_tag_to_scope(StarFileNode* AST,
			   StarFileNode* NMR,
			   BlockNode* currDicDataBlock);

  ///////////////////////////
 //  save_tag_to_value()  //
///////////////////////////
///This function does transformation 3a)
void save_tag_to_value(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode* currDicDataBlock);

  ///////////////////////////
 //  save_value_to_loop() //
///////////////////////////
///This function does transformation 3b)
void save_value_to_loop( StarFileNode *AST,
                         StarFileNode *NMR,
                         BlockNode *currDicDataBlock );

  ////////////////////////////////
 //  save_keyed_tag_to_value() //
////////////////////////////////
///This function does transformation 3c)
void save_keyed_tag_to_value(StarFileNode* AST,
			     StarFileNode* NMR,
			     BlockNode* currDicDataBlock);

  ////////////////////
 //  save_order()  //
////////////////////
///This function does re-order transform II-1.
void save_order( StarFileNode *input, 
		 StarFileNode *output,
		 BlockNode    *currDicDataBlock );


  ///////////////////////////
 //  save_loops()  //
///////////////////////////
///This function does re-order transform II-2.
void save_loops( StarFileNode *input, 
		 StarFileNode *output,
		 BlockNode    *currDicDataBlock );


////////////////////////////////////////////////
//	Auxiliary Transformation Functions
////////////////////////////////////////////////

  //////////////////
 //  tagToVal()  //
//////////////////
//Very often it is the case that tagged values appear as data values---meaning that they
//appear within double quotes. This function simply strips off the delimiting double quotes
///from the strings
string tagToVal(string value);


  //////////////////////////
 //  removeFirstChar()   //
//////////////////////////
//As in the case in transformations 3a,c, often the leading underscore has to be removed
//since the tag becomes a value. This function can remove the first character
///of the string
string removeFirstChar(string value);


  ////////////////////////////
 //  convertToFrameCode()  //
////////////////////////////
// Converts a value to a string suitable for a framecode.
// (It replaces whitespace with underscores, and prepends
/// a "$" to the string.)
string convertToFramecode(string value);

  ////////////////////
 //  deSpacify()   //
////////////////////
/// Returns the string with whitespace turned into underscores.
string deSpacify( string value );

////////////////////////////////////////////////////////////////////////
// These two functions split up a given string into a list of strings.
// SplitStringByDelimiter splits the string on the delimiter char given,
// while SplitStringByWidth splits the string every N characters, where
// N is the width passed in:
/////////////////////////////////////////////////////////////////////////
void SplitStringByDelimitter(
	string splitMe,          // The string to split
	string delim,            // The delimiter string to split on.
	                         //    (This is a regular expression)
	List<string>* valList   // The list to store the results in.
	                         // Must start out as an empty list
	);
///
void SplitStringByWidth(
	string splitMe,          // The string to split
	int    width,            // The width to split on.
	List<string>* valList   // The list to store the results in.
	                         // Must start out as an empty list );
	);

  //////////////////////////////////
 //  FindNeededQuoteStyle()
//////////////////////////////////
    //  Finds out what style of starfile quotation is
    //  needed for a given string object.  The possibilities
    //  are semicolon, singlequote, doublequote, or none.  The
    //  possible values will be returned in a 'char' containing
    ///  either (") or (') or (;) or a space for 'none'.
char FindNeededQuoteStyle( string searchMe );

  /////////////////////////////////////
 //    InsertNeededQuoteStyle()
//////////////////////////////////////
    //  Calls FindNeededQuoteStyle(), and then inserts the proper
    //  type of quoting into the string according to the result.
    //  returns: true if it needed to make a change, or false if
    ///  no change was made.
bool InsertNeededQuoteStyle( string  &changeMe );

/////////////////////////////////////////////////////////////////////////
// List all tags that are in a DataNode, or in a DataNode inside
// that DataNode, and so on (it's a recusive function).  Returns
// the number of tags that were printed.
//////////////////////////////////////////////////////////////////////////
void ListAllTagsInDataNode( DataNode &node,    // DataNode to start the search at.
	                    ostream &dumpHere, // ouptut stream to list to.
			    string msgSoFar ); // call with a nullstring.

#endif
