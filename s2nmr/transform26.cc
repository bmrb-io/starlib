///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
//Revision 1.8  2006/08/24 21:25:06  madings
//*** empty log message ***
//
//Revision 1.7  2003/07/24 18:49:13  madings
//Changes to handle 2.1-3.0 mappings.
//
//Revision 1.6  2000/11/18 03:02:38  madings
//*** empty log message ***
//
// Revision 1.5  2000/03/20  09:45:49  madings
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
// Revision 1.4  1999/06/29  19:04:10  zixia
// *** empty log message ***
//
// Revision 1.3  1999/06/18  21:14:35  zixia
// *** empty log message ***
//
// Revision 1.2  1999/06/11  19:01:32  zixia
// *** empty log message ***
//
// Revision 1.1  1999/06/10  17:43:02  zixia
// *** empty log message ***
//
///////////////////////////////////////////////////////////////
#endif
#include "transforms.h"

void save_insert_column(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF)
{  
    
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int key_tag_pos = -1;
   int key_value_pos = -1;
   int key_loop_tag_pos = -1;
   int new_tag_name_pos = -1;
   int new_tag_value_pos = -1;
   int key_loop_tag_newval_pos = -1;
   int allow_new_loop_generation_pos = -1;
   char quote_style;
   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_key_tag" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-26# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
       delete mapLoopMatch;
       return; /* do nothing */
   }
   
   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

   delete mapLoopMatch;

   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
   {
        if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
            key_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
            key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_loop_tag" ) )
            key_loop_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_name" ) )
            new_tag_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_value" ) )
            new_tag_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_loop_tag_newval" ) )
            key_loop_tag_newval_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_allow_new_loop_generation" ) )
            allow_new_loop_generation_pos = i;
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-26# ERROR: input mapping file is missing tag '_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-26# ERROR: input mapping file is missing tag '_key_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_loop_tag_pos < 0  )
   {   (*errStream) << "#transform-26# ERROR: input mapping file is missing tag '_key_loop_name'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_tag_name_pos < 0  )
   {   (*errStream) << "#transform-26# ERROR: input mapping file is missing tag '_new_tag_name'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_tag_value_pos < 0  )
   {   (*errStream) << "#transform-26# ERROR: input mapping file is missing tag '_new_tag_value'." 
                    << endl;
       return; /* do nothing */
   }





   
   // For each row of the loop, change the value.
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {   
       List<ASTnode*>        *matchSF;
       List<ASTnode*>        *matchLoop;
       DataLoopNameListNode  *nameListList;
       LoopTableNode         *valTable;
       LoopNameListNode      *nameList;
       DataLoopNode          *loopPeer;
       int rowCount, colCount;
       
       bool allowNewLoopGen = true;
       if( allow_new_loop_generation_pos >= 0  )
       {
          string loopGenFlag = (*(*mapLoopTbl)[mL_Idx])[allow_new_loop_generation_pos]->myValue();
	  if(   tolower( loopGenFlag.c_str()[0] ) == 'n' ||
                tolower( loopGenFlag.c_str()[0] ) == '0' ||
                tolower( loopGenFlag.c_str()[0] ) == 'f' )
	  {   allowNewLoopGen = false;
	  }
       }
       matchSF = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
                        (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
       
       int sfNum;
       matchLoop = NULL;
       for(  sfNum = 0 ; sfNum < matchSF->size() ; sfNum++ )
       {
	 matchLoop = ((SaveFrameNode *)((*matchSF)[sfNum]))->searchForTypeByTag(
			 ASTnode::DATALOOPNODE,
			 (*(*mapLoopTbl)[mL_Idx])[key_loop_tag_pos]->myValue());

       
	 // If not found, and we are not supposed to make new loops, then skip this one:
	 if( matchLoop->size() == 0 && (!allowNewLoopGen) )
	 {
	    delete matchLoop;
	    continue;
	 }


	 if( matchLoop->size() == 0 ) {
	     SaveFrameNode *curSfMatched = (SaveFrameNode*) ( (*matchSF)[sfNum] );
	     SaveFrameNode *peerOf_curSfMatched = (SaveFrameNode*) ( (*matchSF)[sfNum]->myParallelCopy() );
	     DataLoopNode *dln  = new DataLoopNode("linear");
	     LoopNameListNode *lnln = new LoopNameListNode;
	     lnln->insert(
	         lnln->end(),
	         new DataNameNode( (*(*mapLoopTbl)[mL_Idx])[key_loop_tag_pos]->myValue() ) 
		 );
	     dln->getNames().insert( dln->getNames().end(), lnln );
	     
	     // Give it one dummy row of one item:
	     LoopRowNode *lrn = new LoopRowNode(true);
             if( key_loop_tag_newval_pos >= 0 ) {
		 lrn->insert( lrn->end(),
		              new DataValueNode(
			          *( (*(*mapLoopTbl)[mL_Idx])[key_loop_tag_newval_pos] ) )   );
	     } else {
		 lrn->insert(lrn->end(),
		              new DataValueNode( string("."), DataValueNode::NON ) );
	     }
	     dln->getVals().insert( dln->getVals().end(), lrn );

	     curSfMatched->GiveMyDataList()->insert(
	         curSfMatched->GiveMyDataList()->end(),
		 dln );

	     // Go make its peer exist:
	     DataLoopNode *peerOf_dln = new DataLoopNode( true, *dln );
	     peerOf_curSfMatched->GiveMyDataList()->insert(
	         peerOf_curSfMatched->GiveMyDataList()->end(),
		 peerOf_dln );

	     // Now do the search again, now that the thing is present to be found:
	     matchLoop = ((SaveFrameNode *)((*matchSF)[sfNum]))->searchForTypeByTag(
			     ASTnode::DATALOOPNODE,
			     (*(*mapLoopTbl)[mL_Idx])[key_loop_tag_pos]->myValue());

	 }
	 if( matchLoop->size() != 1){
	     (*errStream)<<"#transform-26# ERROR: only one tag "
		<<(*(*mapLoopTbl)[mL_Idx])[key_loop_tag_pos]->myValue()<<" should be found."<<endl;
	     continue;
	 }
	
	 

	 loopPeer = (DataLoopNode*)(*matchLoop)[0]->myParallelCopy();
	 nameListList = loopPeer->getNamesPtr();
	 nameList = (*nameListList)[0];
	 valTable = (loopPeer)->getValsPtr();
	 rowCount = valTable->size();
	 colCount = (*nameListList)[0]->size();
	 bool isExist = false;
	 
	 //Check if the column name already exists. If it is, no insertion occur.
	 for(int curCol=0; curCol<colCount; curCol++){
	     if( strcmp((*(*mapLoopTbl)[mL_Idx])[new_tag_name_pos]->myValue().c_str(),
	     ((DataNameNode*)((*nameList)[curCol]))->myName().c_str())==0 ){
	       isExist = true;
	     }
	 }
	 
	 if(isExist){
	    continue;
	 }
	 
	 bool doInsert = true; 

	 //Insert the new name for the column, give it the right quote type:
	 DataValueNode *valToInsert;
	 string newValue = (*(*mapLoopTbl)[mL_Idx])[new_tag_value_pos]->myValue() ;
	 short  newDelim = 0;
	 // If the tag starts with "<<", then the value should come from a tag
	 // instead.
	 if( strncmp( newValue.c_str(), "<<", 2 ) == 0 )
	 {
	     SaveFrameNode *sfn = (SaveFrameNode*) ( (*matchSF)[sfNum] );
	     string tagName = newValue.c_str()+2;
	     List<ASTnode*> *matches = sfn->searchByTag(tagName);
	     if( matches->size() > 0 )
	     { if( (*matches)[0]->isOfType( ASTnode::DATAITEMNODE ) )
	       {
		   // FREE tag
		   newValue = ( (DataItemNode*)( (*matches)[0] ) )->myValue();
		   newDelim = (short) ( ( (DataItemNode*)( (*matches)[0] ) )->myDelimType() );
	       } else if( (*matches)[0]->isOfType( ASTnode::DATANAMENODE ) ) {
	           // don't insert the new tag:
		   doInsert = false;
		   // Loop tag matched - only works if it's on the same row as this one -just rename the old to the new:
		   ASTnode *parMatch = (*matches)[0]->myParallelCopy();
		   ( (DataNameNode*) parMatch)->setName( (*(*mapLoopTbl)[mL_Idx])[new_tag_name_pos]->myValue().c_str() );
	       }
	     }
	     else
	     { newValue = string("!@#$%___NO_VALUE");
	     }

	     delete matches;
	 }
	 if( newValue == string("!@#$%___NO_VALUE") )
	     continue;
	 quote_style = FindNeededQuoteStyle( newValue );
	 if( newDelim == 0 )
	 {
	    newDelim = (short) (
		   ( quote_style == '\'' ) ? DataValueNode::SINGLE :
		   ( quote_style == '\"' ) ? DataValueNode::DOUBLE :
		   ( quote_style == ';' )  ? DataValueNode::SEMICOLON :
		   /*default*/               DataValueNode::NON
		 );
	 }
	 valToInsert = new DataValueNode( newValue, (DataValueNode::ValType) newDelim );
	 
	 if( doInsert )
	 {
	     nameList->insert(nameList->end(), 
			      new DataNameNode((*(*mapLoopTbl)[mL_Idx])[new_tag_name_pos]->myValue()),
			      *valToInsert);
	 }
			  
	 
        if( matchLoop != NULL ) delete matchLoop;
      }
      if( matchSF != NULL ) delete matchSF;
      
   }
   
}
