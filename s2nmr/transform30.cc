///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.7  2000/11/18 03:02:38  madings
*** empty log message ***

// Revision 1.6  2000/11/18  02:26:30  madings
// Fixed a problem that was common to several of the files (apparently
// all the ones edited by 'june', although it's hard to tell if this is
// her doing or not.)  The problem is that there were several places in
// the code where something like this had been done:
//     char *strPtr1, *strPtr2;
//     ...
//     /* strPtr points at some string right now */.
//     strPtr2 = new char [ strlen(strPtr1) ];  /* error: needs a +1 */
//     strcpy( strPtr2, strPtr1 );
// The problem with this, of course. is that strlen() gives the length
// of the string NOT including the null terminator, so the above code
// doesn't allocate enough space for strPtr2 to store the terminator.
// The result is that when the strcpy() is called, a zero-byte (null
// terminator) overwrites whatever was in the next byte just after strPtr2.
// Depending on what that next byte actually is, this might corrupt the
// value, or might not if it was supposed to be a zero anyway.  I suspect
// that this is what was causing some erratic segfaults we were seeing
// in this program - where seemingly irrelevant changes to the input data
// would 'fix' crashes.  (Even though the data changed had nothing to do with
// the part of the code that crashed.)  I suspect that the overwritten byte
// was part of the data the C-library heap uses internally to keep track of
// how much memory a variable on the heap is taking up (so it knows how much
// to free later when free() is called).  When 'free()' was being called, the
// wrong amount of memory was being freed, and thus the heap was a jumbled
// mess after that.)
//
// Revision 1.5  2000/03/23  01:26:32  madings
// Added transform30's ability to map nomenclature down to one-letter codes
// via a mapping loop in the map file.  Also fixed a bug in transform17, when
// the tag to get the saveframe's name from is null it didn't work before.
//
// Revision 1.4  2000/03/20  09:45:49  madings
// Weekend fixes to s2nmr, to respond to some of Jurgen's comments:
// Note, the old s2nmr program is under the name "/bmrb/bin/s2nmr-old", just
// in case you discover a major bug and want to get the old version back.
//
// 1 - The max number allowed for a saveframe's order sequence used to be
// 999.  This was too small.  It has been increased to 9999999, and documented
// acccordingly in specs.html
//
// 2 - Documented that only one middle initial is recognized when
// parsing names from strings.
//
// 3 - Numerous places in the code that created new values did so without
// the proper delimiter type  (For example, it might put a multi-line
// value into a quoted string ("...") instead of a semicolon string.)
// Jurgen noticed this on the citation title, but when I looked for it
// in the code, I found that particular mistake was common throughout the
// newer rules that came after transform # 17.  This has been fixed.
//
// 4 - rule 19, change data_tag_name, did not work as expected:
// I looked at the code and this was simply a case of bad documentation.
// What this rule is meant to do is look for the _BMRB_accession_number
// tag somewhere in the file and use its value as the name of the data_
// tag.  That's all it does.  If a _BMRB_accession_number tag is not there,
// it fails to do anything.  The documentation has been changed to properly
// describe this.
//
// 5 - "rule 22 is unclear to me" - I have re-written explanation of this
// rule, and fixed a bug in the code that refused to operate on multiple
// target saveframes.  (If the key/value pair designated more than one
// output saveframe, it would fail.)
//
// 6 - "rule 25 change_tag_value has backward documenation".  Looked at
// the code to see what it does, and the code is really buggy.  A redo
// from scratch would be quicker than trying to fix it.  The documentation
// isn't backward - the code is.  It uses the wrong variables for things,
// but not in a consistant manner, so I can't just quickly swap things
// around.  Also, we need to define what should be the correct behaviour
// when multiple saveframes are found by either of the two key_tag/value
// searches.  If two input saveframes are found, and two output saveframes
// are found, then is the correct behaviour to copy input 1 to output 1 and
// input 2 to output 2?  Or is the correct behaviour to copy in a 'cross
// product' fashion and copy input 1 to both outputs 1 and 2, and copy
// input 2 to both outputs 1 and 2?  Issues of possible name clashes need
// to be considered too with this.  In light of this, I'm going to put off
// working on this rule untill I am sure it is worth the time, and this
// ambigious behaviour is defined better.  I have to be careful not to spend
// too much time on this program.
//
// 7 - Fixed the broken condition where output_sf_name_tag is null (.) in
// values_to_saveframes_many_to_many.
//
// 8 - Repaired a few typos here and there.
//
// CVS----------------------------------------------------------------------
//
// Revision 1.3  1999/07/13  16:12:04  zixia
// *** empty log message ***
//
// Revision 1.2  1999/06/11  19:01:32  zixia
// *** empty log message ***
//
// Revision 1.1  1999/06/10  17:43:49  zixia
// *** empty log message ***
//
*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_calculate_residue_count( StarFileNode *inStar, 
                     StarFileNode *outStar, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF )
{
   DataNode* currDicSaveFrame = currDicRuleSF;
   int labelMapIdx;

   if(!currDicSaveFrame){//Return if the target saveframe is not found
      return;
   }

   List<ASTnode*>        *matchSF;
   List<ASTnode*>        *matchTag;
   List<ASTnode*>        *matchLoop;
   LoopTableNode         *valTable;
   DataLoopNameListNode *nameListList;
   DataItemNode          *dataItemPeer;
   char                  *value = new char[1000]; 
   int rowCount, colCount;
   int Residue_label_pos = -1;
   int Residue_seq_code_pos = -1;
 
   List<ASTnode*>        *matchSF2;
   List<ASTnode*>        *matchLoop2;
   SaveFrameNode         *SFPeer;
   List<DataNode*>       *sf_insides;
   
   LoopTableNode	*labelMapTbl = NULL;
   int			labelFromCol;
   int			labelToCol;
   // If it exists, find the loop that contains the
   // mapping into one-letter nomenclature for the
   // sequence string:
   matchLoop = currDicRuleSF->searchForTypeByTag( 
			ASTnode::DATALOOPNODE, "_Residue_label_from" );
   if( matchLoop->size() < 1 )
   {
       labelMapTbl = NULL;
   }
   else
   {
       DataLoopNode *lp = ( (DataLoopNode*)( (*matchLoop)[0] ) );
       labelMapTbl = lp->getValsPtr();
       string col0, col1;
       col0 = ( *(lp->getNames() [0] ) )[0] -> myName();
       col1 = ( *(lp->getNames() [0] ) )[1] -> myName();
       if( strCmpInsensitive( col0, string("_Residue_label_from") ) == 0 )
       {   labelFromCol = 0;
	   labelToCol = 1;
       }
       else
       {   labelToCol = 0;
	   labelFromCol = 1;
       }
   }

   //Find the saveframe which contains (_Saveframe_category, monomeric_polymer) 
   matchSF = inStar->searchForTypeByTagValue(
	             ASTnode::SAVEFRAMENODE,
		     string("_Saveframe_category"),
		     string("monomeric_polymer") );

   if(matchSF->size()< 1){
      (*errStream)<<"#transform-30# ERROR: No saveframes contain tag and value pair(_Saveframe_category, monomeric_polymer" <<endl;
      delete matchSF;
      return; /* do nothing */
   }

   int matchIdx;
   for( matchIdx = 0 ; matchIdx < matchSF->size() ; matchIdx++ )
   {
       matchTag = (*matchSF)[matchIdx]->searchForTypeByTag(ASTnode::DATAITEMNODE, string("_Residue_count") );
       //1. Calculate the value for _Residue_count
       //Find the tag _Residue_count
       if(matchTag->size()!= 1){
	  (*errStream)<<"#transform-30# ERROR: only one tag with name \"_Residue_count\""
		      <<" should be found."<<endl;
	  delete matchSF;
	  delete matchTag;
	  
	  return; /* do nothing */
       }
       
       dataItemPeer = (DataItemNode* )(*matchTag)[0]->myParallelCopy(); // original line
	      
       //Find the loop with tag _Residue_seq_code
       matchLoop = (*matchSF)[matchIdx]->searchForTypeByTag( ASTnode::DATALOOPNODE, string("_Residue_seq_code") );  

       if(matchLoop->size()!= 1){
	   (*errStream) <<"#transform-30# ERROR: only one loop with tag \"_Residue_seq_code\" should be found."<<endl;
	   delete matchSF;
	   delete matchTag;
	   delete matchLoop;
	   
	   return; /* do nothing */
       }

       nameListList = ((DataLoopNode*)(*matchLoop)[0])->getNamesPtr();
       valTable     = ((DataLoopNode*)(*matchLoop)[0])->getValsPtr();
       rowCount = valTable->size();
       colCount = (*nameListList)[0]->size();
       
       sprintf(value, "%d", rowCount);
       
       dataItemPeer->setValue(value); 

       //2. Assign value for _Mol_residue_sequence
       delete matchTag;
       matchTag = (*matchSF)[matchIdx]->searchForTypeByTag(ASTnode::DATAITEMNODE, string("_Mol_residue_sequence") );
       
       if(matchTag->size()>1){
	  (*errStream) <<"#transform-30# ERROR: only one tag with name \"_Mol_residue_sequence\" should be found."<<endl;
	  delete matchSF;
	  delete matchTag;
	  delete matchLoop;
	  return; /* do nothing */
       }else if( matchTag->size() == 0 ) {
	   // Doesn't exist, so make a new one, insert it, and use that.
	   // Put it in a list of size 1, to mimic the behaviour of searchForTypeByTag.
	   // (This condition was added after the rest of the code was written.)
	   delete matchTag;
           matchTag = new List<ASTnode*>;
	   matchTag->AddToEnd( new DataItemNode( string("_Mol_residue_sequence"),
		                                 string("?"),
					         DataValueNode::SEMICOLON ) );
	   (  (SaveFrameNode*)( (*matchSF)[matchIdx] )  ) ->
	           GiveMyDataList()->AddToEnd( (DataItemNode*)( (*matchTag)[0] ) );
	   // Don't forget to insert it into the peer too in order to
	   // make it match up:
	   SaveFrameNode* peerSF =  (SaveFrameNode*)
	       ( (  (SaveFrameNode*)( (*matchSF)[matchIdx] )  ) -> myParallelCopy() );
	   DataItemNode *peerDataItem = new DataItemNode(
			  ( (DataItemNode*)( (*matchTag)[0] ) )->myName(),
			  ( (DataItemNode*)( (*matchTag)[0] ) )->myValue(),
			  ( (DataItemNode*)( (*matchTag)[0] ) )->myDelimType()
			  );
	   // Now make them link up (don't try this at home - icky starlib
	   // bypassing).
	   peerDataItem->setPeer( (*matchTag)[0] );
	   (*matchTag)[0] ->setPeer( peerDataItem );

	   peerSF->GiveMyDataList()->AddToEnd( peerDataItem );

       }
      
       //If the value for _Mol_residue_sequence is already assigned, do nothing.
       if(    strcmp( ((DataItemNode *)(*matchTag)[0])->myValue().c_str(), "?") == 0
	   || strcmp( ((DataItemNode *)(*matchTag)[0])->myValue().c_str(), ".") == 0 
	   || strcmp( ((DataItemNode *)(*matchTag)[0])->myValue().c_str(), "") == 0 
	   || strcmp( ((DataItemNode *)(*matchTag)[0])->myValue().c_str(), "*") == 0 )
       {

	 dataItemPeer = (DataItemNode *)(*matchTag)[0]->myParallelCopy();
      
	 //Find the value for _Mol_residue_sequence.
	 for(int i=0; i<colCount; i++){
	      if( (*((*nameListList)[0]) )[i]->myName() == string("_Residue_label") ){
		  Residue_label_pos = i;
	      }
	      else if( (*((*nameListList)[0]) )[i]->myName() == string("_Residue_seq_code") ){
		  Residue_seq_code_pos = i;
	      }
	 }

	 if( Residue_label_pos < 0 ){
	      (*errStream) <<"#transform-30# ERROR: input data file is missing tag '_Residue_label'."
			   <<endl;
	      return; /* do nothing */
	 }
	 if( Residue_seq_code_pos < 0 ){
	      (*errStream) <<"#transform-30# ERROR: input data file is missing tag '_Residue_seq_code'."
			   <<endl;
	      return; /* do nothing */
	 }
       
	 //Clean the value.
	 strcpy(value, "");
       
	 for(int curRow=0; curRow<rowCount; curRow++){
	   if(curRow%20 == 0 && curRow != 0 ){
	      strcat(value,  "\n");
	   }

	 
	   if( (*(*valTable)[curRow])[Residue_label_pos]->myDelimType() == DataValueNode::FRAMECODE ){
	       strcat(value, "X");
	   }else{
	       // If no mapping table specified, then map the values as-is,
	       // else convert them via the mapping table into one-letter codes:
	       if( labelMapTbl == NULL )
		   strcat(value, (*(*valTable)[curRow])[Residue_label_pos]->myValue().c_str() );
	       else
	       {
		   // Look for the value in the "from" column, if
		   // it is found, output the mapping.
		   for(   labelMapIdx = 0 ;
			  labelMapIdx < labelMapTbl->size() ;
			  labelMapIdx ++ )
		   {
		        if  ( 0 == strCmpInsensitive(
				       (*(*valTable)[curRow])[Residue_label_pos]->myValue() ,
				       (* (*labelMapTbl)[ labelMapIdx ] )[ labelFromCol ]->myValue()  )
		 	    )
			{
			    strcat( value,
				    (* (*labelMapTbl)[ labelMapIdx ] )[ labelToCol ]->myValue().c_str()  );
			    break;
			}
			// Check to see if it is already matching the valid
			// name in the to column - if so, copy it:
			else if ( 0 == strCmpInsensitive(
				       (*(*valTable)[curRow])[Residue_label_pos]->myValue() ,
				       (* (*labelMapTbl)[ labelMapIdx ] )[ labelToCol ]->myValue()  )
		 	        )
			{
			    strcat( value,
				    (* (*labelMapTbl)[ labelMapIdx ] )[ labelToCol ]->myValue().c_str()  );
			    break;
			}
		   }
		   // If it got to the end without seeing a match, then
		   // this means it was neither in the "to", nor the "from"
		   // columns, so it is invalid - use a hardcoded "X" for
		   // this spot in the sequence:
		   if( labelMapIdx >= labelMapTbl->size() )
		   {   strcat( value, "X" );
		   }
	       }
	   }
	 }
       
	 dataItemPeer->setDelimType(DataValueNode::SEMICOLON);
	 dataItemPeer->setValue(value);
       }else{
	  delete matchSF;
	  delete matchTag;
	  delete matchLoop;
	  return; /* do nothing */  
       }
       
       //3. Create a loop in the saveframe which contains (_Saveframe_category, polymer_residue).
       DataLoopNode         *newLoop = NULL;
       DataLoopNameListNode *nameListList2 = NULL;
       LoopTableNode        *valTable2 = NULL;
       LoopNameListNode     *nameList2 = NULL;
       LoopRowNode          *newRow = NULL;
	   
       //Clean the value.
       strcpy(value, "");
       
       for(int row = 0; row<rowCount; row++){
	   
	  if( (*(*valTable)[row])[Residue_label_pos]->myDelimType() == DataValueNode::FRAMECODE ){
	       
	       strcpy(value, (*(*valTable)[row])[Residue_label_pos]->myValue().c_str() );
	       
	       //Find the corresponding saveframe which contains the pair (_Abbreviation_common, value).
	       matchSF2 = inStar->searchForTypeByTagValue(
				  ASTnode::SAVEFRAMENODE,
				  string("_Abbreviation_common"),
				  value);
	       
	       if(matchSF2->size()< 1){
		  (*errStream)<<"#transform-30# ERROR: No saveframes contain tag and value pair(_Abbreviation_common, "
			      <<value <<endl;
		  delete matchSF;
		  delete matchTag;
		  delete matchLoop;
		  delete matchSF2;
		     
		  return; 
	       }
	       
	       int matchIdx2;
	       for( matchIdx2 = 0 ; matchIdx2 < matchSF2->size() ; matchIdx++ )
	       {
		   SFPeer = (SaveFrameNode *)(*matchSF2)[matchIdx2]->myParallelCopy();
		   sf_insides = SFPeer->GiveMyDataList();
		  
		   if(SFPeer == NULL){
			(*errStream)<<"#transform-30# ERROR::no parallel copy for SFPeer"<<endl;
			delete matchSF;
			delete matchTag;
			delete matchLoop;
			delete matchSF2;
			return; 
		   }
		  
		   //Check if the loop already exists.
		   matchLoop2 = SFPeer->searchForTypeByTag(ASTnode::DATALOOPNODE, string("_Residue_seq_code") );

		   if(matchLoop2->size() == 1){
		   //The loop exist. Thus insert one row.
		       nameListList2 = ((DataLoopNode*)(*matchLoop2)[0])->getNamesPtr();
		       valTable2     = ((DataLoopNode*)(*matchLoop2)[0])->getValsPtr();

		       newRow = new LoopRowNode(true);
		
		       newRow->insert(newRow->end(), 
			  new DataValueNode((*(*valTable)[row])[Residue_label_pos]->myValue(), DataValueNode::FRAMECODE ) );
					  
		       newRow->insert(newRow->end(), 
				  new DataValueNode((*(*valTable)[row])[Residue_seq_code_pos]->myValue(),
						    (*(*valTable)[row])[Residue_seq_code_pos]->myDelimType() ) );
		   
		       valTable2->insert(valTable2->end(), newRow);
		   
		   }else if(matchLoop2->size() == 0 ){
		   //The loop does not exist. First, create the new loop, then insert a new row.
			newLoop = new DataLoopNode(string("tabulate") );
			nameListList2 = newLoop->getNamesPtr();
			valTable2 = newLoop->getValsPtr();
		      
			//Populate the list of tag names for the loop.
			nameListList2->insert(nameListList2->end(), new LoopNameListNode() );
			nameList2 = (*nameListList2)[0];

			nameList2->insert(nameList2->end(), new DataNameNode("_Mol_label") );
			nameList2->insert(nameList2->end(), new DataNameNode("_Residue_seq_code") );

			newRow = new LoopRowNode(true);
		
			newRow->insert(newRow->end(), 
			  new DataValueNode((*(*valTable)[row])[Residue_label_pos]->myValue(), DataValueNode::FRAMECODE ) );
					  
			newRow->insert(newRow->end(), 
				  new DataValueNode((*(*valTable)[row])[Residue_seq_code_pos]->myValue()) );
		   
			valTable2->insert(valTable2->end(), newRow);
			sf_insides->insert(sf_insides->end(), newLoop);
		   }

		   delete matchLoop2;
		   delete matchSF2;
	       }
	       
	  }
       }
   }
   
      
   delete matchSF;
   delete matchTag;
   delete matchLoop;
}
