#ifndef TRANSFORMS_H 
#define TRANSFORMS_H

#ifdef BOGUS_DEF_COMMENT_OUT
/*
 * $Log: not supported by cvs2svn $
 * Revision 1.27  2008/08/14 18:44:30  madings
 * *** empty log message ***
 *
 * Revision 1.26  2008/04/16 21:29:21  madings
 * Haven't committed in a while and this is a working version so I should.
 *
 * Revision 1.25  2006/10/27 23:22:06  madings
 * Changes to allow for assigned chemical shift splitting.
 *
 * Revision 1.24  2006/07/14 21:00:35  madings
 * *** empty log message ***
 *
 * Revision 1.23  2006/05/02 21:20:38  madings
 * *** empty log message ***
 *
 * Revision 1.22  2005/11/07 20:21:21  madings
 * *** empty log message ***
 *
 * Revision 1.21  2005/03/18 22:00:30  madings
 * Added flat file creating rule
 *
 * Revision 1.20  2005/03/18 21:07:20  madings
 * Massive update after too long without a checkin
 *
 * Revision 1.19  2005/03/04 23:32:05  madings
 * checking in doc changes and a lot of incremental bugfixes
 * too many to mention (cvs repository was broken and confused
 * for a while so this represents about 5 months of stuff)
 *
 * Revision 1.18  2003/07/24 18:49:13  madings
 * Changes to handle 2.1-3.0 mappings.
 *
 * Revision 1.17  2001/04/13 21:17:07  madings
 * Changes to use the new starlib2.
 * Also discovered an error in transform10 as well.
 *
 * Revision 1.16  2000/11/18 03:02:40  madings
 * *** empty log message ***
 *
 */
#endif

#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "ast.h"
#include "astlist.h"
#include "loopvals.h"
#include "scopeTagList.h"

extern ostream * os;
extern ofstream * errStream;

extern string g_outFname;

extern StarFileNode *AST;
extern StarFileNode *NMR;
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
//This function does transform 5.
void save_remove_tag( StarFileNode *input,
	              StarFileNode *output,
		      BlockNode    *currDicDataBlock,
		      SaveFrameNode *currDicRuleSF );

  ///////////////////////////
 //  save_renamed_tags()  //
///////////////////////////
//This function does transformation 1a). 
void save_renamed_tags(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode* currDicDataBlock,
		       SaveFrameNode *currDicRuleSF );


  ///////////////////////////////
 //  save_tag_to_framecode()  //
///////////////////////////////
//This function does transformation 1b). 
void save_tag_to_framecode(StarFileNode* AST,
			   StarFileNode* NMR,
			   BlockNode* currDicDataBlock,
			   SaveFrameNode *currDicRuleSF );

  /////////////////////////////////
 //  save_keyed_renamed_tags()  //
/////////////////////////////////
//This function does transformation 1c). 
void save_keyed_renamed_tags( StarFileNode* AST,
		              StarFileNode* NMR,
		              BlockNode* currDicDataBlock,
			      SaveFrameNode *currDicRuleSF );

  /////////////////////////////////
 //  save_keyed_renamed_tags_no_block()  //
/////////////////////////////////
//This function does transformation 1c). 
void save_keyed_renamed_tags_no_block( StarFileNode* AST,
		              StarFileNode* NMR,
		              BlockNode* currDicDataBlock,
			      SaveFrameNode *currDicRuleSF );


  ///////////////////////////////
 //  save_tag_to_frame_free() //
///////////////////////////////
//This function does transformation 2a
void save_tag_to_frame_free( StarFileNode* AST,
			     StarFileNode* NMR,
			     BlockNode* currDicDataBlock,
			     SaveFrameNode *currDicRuleSF );

  ///////////////////////////////////////
 //  save_tag_to_frame_loop_uniform() //
///////////////////////////////////////
//This function does transformation 2b
void save_tag_to_frame_loop_uniform( StarFileNode* AST,
			             StarFileNode* NMR,
			             BlockNode* currDicDataBlock,
				     SaveFrameNode *currDicRuleSF );

  //////////////////////////////////////
 //  save_tag_to_frame_loop_unique() //
//////////////////////////////////////
//This function does transformation 2c
void save_tag_to_frame_loop_unique( StarFileNode* AST,
			            StarFileNode* NMR,
			            BlockNode* currDicDataBlock,
				    SaveFrameNode *currDicRuleSF );

  ///////////////////////////
 //  save_tag_to_value()  //
///////////////////////////
//This function does transformation 3a)
void save_tag_to_value(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode* currDicDataBlock,
		       SaveFrameNode *currDicRuleSF );

  ///////////////////////////
 //  save_value_to_loop() //
///////////////////////////
//This function does transformation 3b)
void save_value_to_loop( StarFileNode *AST,
                         StarFileNode *NMR,
                         BlockNode *currDicDataBlock,
			 SaveFrameNode *currDicRuleSF );

  ////////////////////////////////
 //  save_keyed_tag_to_value() //
////////////////////////////////
//This function does transformation 3c)
void save_keyed_tag_to_value(StarFileNode* AST,
			     StarFileNode* NMR,
			     BlockNode* currDicDataBlock,
			     SaveFrameNode *currDicRuleSF );

  /////////////////////////////////////
 //  save_make_citation_full_tag() //
/////////////////////////////////////
void save_make_citation_full_tag( StarFileNode* AST,
			          StarFileNode* NMR,
			          BlockNode* currDicDataBlock,
			          SaveFrameNode *currDicRuleSF );

  ////////////////////////////////
 //  save_make_new_saveframe() //
////////////////////////////////
void save_make_new_saveframe( StarFileNode* AST,
			      StarFileNode* NMR,
			      BlockNode* currDicDataBlock,
			      SaveFrameNode *currDicRuleSF );

  ////////////////////////////////
 //  save_make_new_tag()     ) //
////////////////////////////////
void save_make_new_tag( StarFileNode* AST,
			StarFileNode* NMR,
			BlockNode* currDicDataBlock,
			SaveFrameNode *currDicRuleSF );

  //////////////////////////////////
 //  save_remove_empty_frames()  //
//////////////////////////////////
void save_remove_empty_frames( StarFileNode *AST,
	                       StarFileNode *NMR,
			       BlockNode    *currDicDataBlock,
			       SaveFrameNode *currDicRuleSF );
  //////////////////////////////////
 //  save_remove_tags_not_in_saveframe()  //
//////////////////////////////////
void save_remove_tags_not_in_saveframe( StarFileNode *AST,
	                       StarFileNode *NMR,
			       BlockNode    *currDicDataBlock,
			       SaveFrameNode *currDicRuleSF );

  /////////////////////////////////////
 //  save_free_all_category_tags()  //
/////////////////////////////////////
void save_free_all_category_tags( StarFileNode *AST,
	                          StarFileNode *NMR,
			          BlockNode    *currDicDataBlock,
				  SaveFrameNode *currDicRuleSF );

  ////////////////////////////////////
 // save_insert_loop_defaults()    //
////////////////////////////////////
void save_insert_loop_defaults( StarFileNode *input, 
	                        StarFileNode *output, 
	            	        BlockNode    *currDicDataBlock,
				SaveFrameNode *currDicRuleSF );

  ///////////////////////////////////////////
 // save_explode_Mol_residure_sequence()  //
///////////////////////////////////////////
void save_explode_Mol_residue_sequence( StarFileNode *AST, 
				        StarFileNode *NMR, 
					BlockNode    *currDicDataBlock,
					SaveFrameNode *currDicRuleSF );


  ////////////////////////////////////
 // save_remove_null_loop_rows()   //
////////////////////////////////////
void save_remove_null_loop_rows( StarFileNode *AST, 
                                 StarFileNode *NMR, 
                                 BlockNode    *currDicDataBlock,
				 SaveFrameNode *currDicRuleSF );
  ////////////////////////////
 // save_remove_null_tags()//
////////////////////////////
void save_remove_null_tags(StarFileNode *AST, 
                           StarFileNode *NMR, 
                           BlockNode    *currDicDataBlock,
			   SaveFrameNode *currDicRuleSF );

  /////////////////////////////
 // save_change_tag_value() //
/////////////////////////////
void save_change_tag_value(StarFileNode *AST, 
                           StarFileNode *NMR, 
                           BlockNode    *currDicDataBlock,
			   SaveFrameNode *currDicRuleSF );

  /////////////////////////////
 // save_tag_to_loop()      //
/////////////////////////////
void save_tag_to_loop(StarFileNode *AST, 
                      StarFileNode *NMR, 
                      BlockNode    *currDicDataBlock,
		      SaveFrameNode *currDicRuleSF );

  ///////////////////////////////////////////////////
 // save_values_to_saveframes_many_to_many()      //
///////////////////////////////////////////////////
void save_values_to_saveframes_many_to_many(
	              StarFileNode *AST, 
                      StarFileNode *NMR, 
                      BlockNode    *currDicDataBlock,
		      SaveFrameNode *currDicRuleSF );


  ///////////////////////////////////////////////////
 // save_parse_tag_to_new_tag()      //
///////////////////////////////////////////////////
void save_parse_tag_to_new_tag(
	              StarFileNode *AST, 
                      StarFileNode *NMR, 
                      BlockNode    *currDicDataBlock,
		      SaveFrameNode *currDicRuleSF );

  ////////////////////
 //  save_order()  //
////////////////////
//This function does re-order transform II-1.
void save_order( StarFileNode *input, 
		 StarFileNode *output,
		 BlockNode    *currDicDataBlock,
		 SaveFrameNode *currDicRuleSF );

  ////////////////////
 //  save_order()  //
////////////////////
//This function does re-order transform II-1-2.
void save_order_and_loops( StarFileNode *input, 
		 StarFileNode *output,
		 BlockNode    *currDicDataBlock,
		 SaveFrameNode *currDicRuleSF );


  ///////////////////////////
 //  save_loops()  //
///////////////////////////
//This function does re-order transform II-2.
void save_loops( StarFileNode *input, 
		 StarFileNode *output,
		 BlockNode    *currDicDataBlock,
		 SaveFrameNode *currDicRuleSF );


  ///////////////////////////
 //  save_loops_test()  //
///////////////////////////
//This is the copy of original transform II-2.
void save_loops_test( StarFileNode *input, 
		 StarFileNode *output,
		 BlockNode    *currDicDataBlock,
		 SaveFrameNode *currDicRuleSF );

 /////////////////////////////
//     parse_split_name()  //
////////////////////////////
//This function parse author names.
void save_parse_split_name( StarFileNode *input, 
                       StarFileNode *output, 
                       BlockNode    *,
		       SaveFrameNode *currDicRuleSF);


 /////////////////////////////////
//    change_data_tag_name()   //
////////////////////////////////
//This function change the root tag name
void save_change_data_tag_name(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF);


 ////////////////////////////
//   remove_global_block  //
///////////////////////////
void save_remove_global_block(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF);

 
 ////////////////////////////////
//   create_saveframe         //
///////////////////////////////
void save_create_saveframe(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF);

 ////////////////////////////////
//    copy_tag_into_saveframe //
///////////////////////////////
void save_copy_tag_into_saveframe(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode*    ,
		       SaveFrameNode *currDicRuleSF);

 ////////////////////////////
//  copy_saveframe        //
///////////////////////////
void save_copy_saveframe(StarFileNode* inStar,
		       StarFileNode* outStar,
		       BlockNode*    ,
		       SaveFrameNode *currDicRuleSF);

 /////////////////////////
//  framecode_to_tag   //
////////////////////////
void save_framecode_to_tag(StarFileNode* inStar,
			   StarFileNode* outStar,
			   BlockNode*,
			   SaveFrameNode *currDicRuleSF);

 //////////////////////////
// change_tag_value     //
/////////////////////////
void save_change_tag_value_II(StarFileNode* inStar,
			   StarFileNode* outStar,
			   BlockNode*,
			   SaveFrameNode *currDicRuleSF);


 /////////////////////////
//insert_column        //
////////////////////////
void save_insert_column(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF);

 /////////////////////////
//insert_ordinal        //
////////////////////////
void save_insert_ordinal(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF);

 //////////////////////////
// save_insert_loop     //
/////////////////////////
void save_insert_loop(   StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF);

 /////////////////////////
//rename_saveframe     //
////////////////////////
void save_rename_saveframe(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF);


 //////////////////////////////////////////////
//save_check_chemical_shift_nomenclature    //
/////////////////////////////////////////////
void save_check_chemical_shift_nomenclature(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF);


 /////////////////////////////////////////
//calculate_residue_count              //
////////////////////////////////////////
void save_calculate_residue_count( StarFileNode *inStar, 
                     StarFileNode *outStar, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF );


  //////////////////////////////////////
// change_database_name              //
//////////////////////////////////////
void save_change_database_name(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF);


 //////////////////////////////////
//convert_C_to_K                //
/////////////////////////////////
void save_convert_C_to_K(StarFileNode* inStar,
		         StarFileNode* outStar,
		         BlockNode*,
		         SaveFrameNode *currDicRuleSF);

 /////////////////////////////////////
//assign_citation_tag              //
////////////////////////////////////
void save_assign_citation_type(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF);


 /////////////////////////////////
//make_new_tag_II              //
////////////////////////////////
void save_make_new_tag_II(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);


 /////////////////////////////////////
//save_insert_loop_defaults_II     //
////////////////////////////////////
void save_insert_loop_defaults_II( StarFileNode *input, 
                     StarFileNode  *output, 
                     BlockNode     *currDicDataBlock,
		     SaveFrameNode *currDicRuleSF );


 ////////////////////////////////////////////
//save_create_enzyme_commission_number    //
///////////////////////////////////////////
void save_create_enzyme_commission_number( StarFileNode *inStar, 
                                           StarFileNode *outStar, 
                                           BlockNode    *,
		                           SaveFrameNode *currDicRuleSF );


 //////////////////////////////////////////
//save_copy_saveframe                   //
/////////////////////////////////////////
void save_copy_saveframe_content(StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);

  //////////////////////////////////////////
 //save_remove_saveframes                //
/////////////////////////////////////////
void save_remove_saveframes( StarFileNode *inStar, 
                             StarFileNode *outStar, 
                             BlockNode    *,
			     SaveFrameNode *currDicRuleSF);

  //////////////////////////////////////////
 //save_remove_saveframes                //
/////////////////////////////////////////
void save_rename_saveframes( StarFileNode *inStar, 
                             StarFileNode *outStar, 
                             BlockNode    *,
			     SaveFrameNode *currDicRuleSF);


  //////////////////////////////////////////
 //save_remove_version_2_1_tags          //
//////////////////////////////////////////
void save_remove_version_2_1_tags( StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *,
			           SaveFrameNode *currDicRuleSF);
  //////////////////////////////////////////
 //save_remove_version_3_0_tags          //
//////////////////////////////////////////
void save_remove_version_3_0_tags( StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *,
			           SaveFrameNode *currDicRuleSF);
  //////////////////////////////////////////
 //save_generic_value_mapper_A          //
//////////////////////////////////////////
void save_generic_value_mapper_A(  StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *,
			           SaveFrameNode *currDicRuleSF);

  //////////////////////////////////////////
 //save_generic_value_mapper_A          //
//////////////////////////////////////////
void save_invert_generic_value_mapper_A(  StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *bNode,
			           SaveFrameNode *currDicRuleSF);

  //////////////////////////////////
 //    save_insert_nmrstr_3_IDs  //
//////////////////////////////////
void save_insert_nmrstr_3_IDs( StarFileNode *input,
	                       StarFileNode *output,
		               BlockNode    *blk,
		               SaveFrameNode *currDicRuleSF);

  /////////////////////////////////////////
 //    save_insert_value_from_lookup_A  //
/////////////////////////////////////////
void save_insert_value_from_lookup_A( StarFileNode* inStar,
		                      StarFileNode* outStar,
		                      BlockNode*,
		                      SaveFrameNode *currDicRuleSF );

  /////////////////////////////////////////
 //    save_insert_value_from_lookup_B  //
/////////////////////////////////////////
void save_insert_value_from_lookup_B( StarFileNode* inStar,
		                      StarFileNode* outStar,
		                      BlockNode*,
		                      SaveFrameNode *currDicRuleSF );

  /////////////////////////////////////////
 //    save_remove_sf_if_missing_tag    //
/////////////////////////////////////////
void save_remove_sf_if_missing_tag(
                         StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);

  /////////////////////////////////////////
 //    save_remove_sf_if_missing_tag    //
/////////////////////////////////////////
void save_split_lines_to_loop(
                         StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);

  //////////////////////////////////////////////
 // save_explode_Mol_residue_seq_dict3_0()   //
//////////////////////////////////////////////
void save_explode_Mol_residue_seq_dict3_0( StarFileNode *AST, 
				        StarFileNode *NMR, 
					BlockNode    *currDicDataBlock,
					SaveFrameNode *currDicRuleSF );

  //////////////////////////////////////////
 //save_make_framecodes_agree            //
//////////////////////////////////////////
void save_make_framecodes_agree(  StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *bNode,
			           SaveFrameNode *currDicRuleSF);

  //////////////////////////////////////////
 //save_free_looped_tags                 //
//////////////////////////////////////////
void save_free_looped_tags(        StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *bNode,
			           SaveFrameNode *currDicRuleSF);

void save_make_submission_date_3(  StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *,
				   SaveFrameNode *dictEntry );

void save_delete_invalid_tag_syntax( StarFileNode *AST, 
				     StarFileNode *NMR, 
				     BlockNode    *currDicDataBlock,
				     SaveFrameNode *currDicRuleSF );

void save_remove_duplicate_rows(     StarFileNode *AST, 
				     StarFileNode *NMR, 
				     BlockNode    *currDicDataBlock,
				     SaveFrameNode *currDicRuleSF );

void save_remove_null_and_question_tags(
                         StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);

void save_free_looped_tags_by_value(
                         StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);

void save_free_and_rename_looped_tags(  StarFileNode *inStar, 
                                        StarFileNode *outStar, 
                                        BlockNode    *bNode,
			                SaveFrameNode *currDicRuleSF );


void save_default_if_not_exist( StarFileNode *input, 
                                StarFileNode *output, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF);


void save_change_tag_delim(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF) ;


void save_remove_header_comments(    StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF);

void save_decouple_saveframes_3_to_2(
                         StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);

void save_merge_saveframes_2_to_3(
                         StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF);

void save_rows_concatenated_to_free(   StarFileNode *input,
				       StarFileNode *output,
				       BlockNode    *blk,
				       SaveFrameNode *currDicRuleSF);

void save_flat_file_name_if_not_exist( StarFileNode *input, 
                                StarFileNode *output, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF);

void save_collapse_3_to_2( StarFileNode *input, 
                           StarFileNode *output, 
                           BlockNode    *,
			   SaveFrameNode *currDicRuleSF);

void save_expand_2_to_3( StarFileNode *input, 
                           StarFileNode *output, 
                           BlockNode    *,
			   SaveFrameNode *currDicRuleSF);

void save_move_PDBX_poly_seq_to_assembly(
                           StarFileNode *input, 
                           StarFileNode *output, 
                           BlockNode    *,
			   SaveFrameNode *currDicRuleSF);

void save_join_ambiguity_codes(  StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF);

void save_merge_loop_rows_to_text(  StarFileNode *inStar, 
                                    StarFileNode *outStar, 
                                    BlockNode    *,
		                    SaveFrameNode *currDicRuleSF);

void save_join_via_sprintf(  StarFileNode *inStar, 
                             StarFileNode *outStar, 
                             BlockNode    *,
		             SaveFrameNode *currDicRuleSF);

void save_split_ambiguity_codes(  StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF);

void save_strip_chem_comp_from_ligand_sf_names(
                                StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF);

void save_generate_residue_mol_labels( StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF );

void save_calculate_data_row_counts( StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF );

void save_insert_default_isotope_numbers(
                     StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF );

void save_unparse_to_tagvalue(
                     StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF );

void save_swap_ref_tags_if_needed(
		     StarFileNode *input, 
		     StarFileNode *output, 
		     BlockNode    *,
		     SaveFrameNode *currDicRuleSF);

////////////////////////////////////////////////
//	Auxiliary Transformation Functions
////////////////////////////////////////////////

  //////////////////
 //  tagToVal()  //
//////////////////
// Very often it is the case that tagged values appear as data values---meaning that they
// appear within double quotes. This function simply strips off the delimiting double quotes
// from the strings
string tagToVal(string value);


  //////////////////////////
 //  removeFirstChar()   //
//////////////////////////
//As in the case in transformations 3a,c, often the leading underscore has to be removed
//since the tag becomes a value. This function can remove the first character
//of the string
string removeFirstChar(string value);


  ////////////////////////////
 //  convertToFrameCode()  //
////////////////////////////
// Converts a value to a string suitable for a framecode.
// (It replaces whitespace with underscores, and prepends
// a "$" to the string.)
string convertToFramecode(string value);

  ////////////////////
 //  deSpacify()   //
////////////////////
// Returns the string with whitespace turned into underscores.
string deSpacify( string value );

////////////////////////////////////////////////////////////////////////
// These two functions split up a given string into a list of strings.
// SplitStringByDelimiter splits the string on the delimiter char given,
// while SplitStringByWidth splits the string every N characters, where
// N is the width passed in:
////////////////////////////////////////////////////////////////////////
void SplitStringByDelimiter(
	string splitMe,          // The string to split
	string delim,            // The delimiter string to split on.
	List<string>* valList    // The list to store the results in.
	                         // Must start out as an empty list
	);
void SplitStringByWidth(
	string splitMe,          // The string to split
	int    width,            // The width to split on.
	List<string>* valList    // The list to store the results in.
	                         // Must start out as an empty list );
	);

  /////////////////////////////////////
 //   SearchAndReplace()
/////////////////////////////////////
    // Searches for the given string and replaces all
    // occurances of it with the putMe string.
void SearchAndReplace(
	string &changeMe,      // This is the string to be affected
	const string &findMe,  // This is the substring to be replaced.
	const string &putMe    // This is the substring to put in its place.
	);

  //////////////////////////////////
 //  FindNeededQuoteStyle()
//////////////////////////////////
    //  Finds out what style of starfile quotation is
    //  needed for a given string object.  The possibilities
    //  are semicolon, singlequote, doublequote, or none.  The
    //  possible values will be returned in a 'char' containing
    //  either (") or (') or (;) or a space for 'none'.
char FindNeededQuoteStyle( string searchMe );

  /////////////////////////////////////
 //    InsertNeededQuoteStyle()
//////////////////////////////////////
    //  Calls FindNeededQuoteStyle(), and then inserts the proper
    //  type of quoting into the string according to the result.
    //  returns: true if it needed to make a change, or false if
    //  no change was made.
bool InsertNeededQuoteStyle( string  &changeMe );

/////////////////////////////////////////////////////////////////////////
// List all tags that are in a DataNode, or in a DataNode inside
// that DataNode, and so on (it's a recusive function).  Returns
// the number of tags that were printed.
/////////////////////////////////////////////////////////////////////////
void ListAllTagsInDataNode( DataNode &node,    // DataNode to start the search at.
	                    ostream &dumpHere, // ouptut stream to list to.
			    string msgSoFar ); // call with a nullstring.

void eraseme1(void);

#endif
