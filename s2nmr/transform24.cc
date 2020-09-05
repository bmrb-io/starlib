//////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
//Revision 1.6  2006/06/26 21:47:01  madings
//*** empty log message ***
//
//Revision 1.5  2000/11/18 03:02:38  madings
//*** empty log message ***
//
// Revision 1.4  2000/11/18  02:26:29  madings
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
// Revision 1.3  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
//Revision 1.2  1999/06/11 19:01:32  zixia
//*** empty log message ***
//
// Revision 1.1  1999/06/10  17:42:40  zixia
// *** empty log message ***
//
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"
#include "string.h"

void save_framecode_to_tag(StarFileNode* inStar,
			   StarFileNode* outStar,
			   BlockNode*,
			   SaveFrameNode *currDicRuleSF)
{
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int key_tag_pos = -1;
   int key_value_pos = -1;
   int tag_name_pos = -1;

   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_key_tag" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-24# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
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
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_tag_name" ) )
            tag_name_pos = i;
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-24# ERROR: input mapping file is missing tag '_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-24# ERROR: input mapping file is missing tag '_key_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( tag_name_pos < 0  )
   {   (*errStream) << "#transform-24# ERROR: input mapping file is missing tag '_tag_name'." 
                    << endl;
       return; /* do nothing */
   }
   
   // For each row of the loop, change the value.
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {   
       List<ASTnode*>        *matchSF;
       List<ASTnode*>        *matchTag;
       DataLoopNameListNode  *nameListList;
       LoopTableNode         *valTable;
       DataLoopNode          *peerLoop;
       ASTnode               *dataItemPeer;
       ASTnode               *cur_par;
       int rowCount, colCount;
       int curRow, curCol;
       int target_tag_pos;
       
       char value[200];
       
       //Find the saveframe contains a pair of (tag, value).       
       matchSF = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
                        (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
       
       
       if( matchSF->size() < 1){
	   (*errStream)<<"#transform-24# ERROR: no saveframes with tag "
              <<(*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue()<<" were found."<<endl;
	   delete matchSF;
	   continue;
       }

       int matchNum = 0;

       for( matchNum = 0 ; matchNum < matchSF->size() ; matchNum++ ) {

	   //Within the saveframe, find the tag
	   matchTag = ((SaveFrameNode *)((*matchSF)[matchNum]))->searchByTag(
			   (*(*mapLoopTbl)[mL_Idx])[tag_name_pos]->myValue());

	  
	   if( matchTag->size() != 1 ){
	       (*errStream)<<"#transform-24# ERROR: exactly one tag "
		  <<(*(*mapLoopTbl)[mL_Idx])[tag_name_pos]->myValue()<<
		  " should be found, but " << matchTag->size() <<" were found."<<endl;
	       
	       matchNum = 99999;
	       
	       break;
	   }

	   cur_par = (*matchTag)[0]->myParent();
	   
	   while(cur_par != NULL 
		   && !cur_par->isOfType(ASTnode::DATALOOPNODE) 
		   && !cur_par->isOfType(ASTnode::SAVEFRAMENODE) )
	       cur_par = cur_par->myParent();
	  
	   //If the tag is non-loop tag.
	   if( cur_par->isOfType(ASTnode::SAVEFRAMENODE) ) {
	       strcpy(value, (( (DataItemNode*)((*matchTag)[0]) )->myValue()).c_str() );
	    
	   
	       for(int i=0; i<strlen(value); i++){
		  if( value[i] == '_' ){
		      value[i] = ' ';
		  }
	       }
	  
	       dataItemPeer = (*matchTag)[0]->myParallelCopy();
	   
	       int qStyle= FindNeededQuoteStyle( value );
	       if( qStyle == '\'' ) 
	       {   ( (DataItemNode*)dataItemPeer )->setDelimType(DataValueNode::SINGLE); }
	       else if( qStyle == '\"' )  
	       {   ( (DataItemNode*)dataItemPeer )->setDelimType(DataValueNode::DOUBLE); }
	       else if(  qStyle == ';' )  
	       {   ( (DataItemNode*)dataItemPeer )->setDelimType(DataValueNode::SEMICOLON); }

	       ( (DataItemNode*)dataItemPeer )->setValue(value);

	   }else if(cur_par->isOfType(ASTnode::DATALOOPNODE) ){
	       //If the tag is a loop tag.
	       //Find the corresponding loop in the output NMR-STAR file.
	       peerLoop = (DataLoopNode*)cur_par->myParallelCopy(); 
	       nameListList = peerLoop->getNamesPtr();
	       valTable     = peerLoop->getValsPtr();
	       
	       //The number of rows in the loop.
	       rowCount = valTable->size();
	       //Size of the first row of names(the outermost loop level).
	       colCount = (*nameListList)[0]->size();
		
	       for(int i=0; i<colCount; i++){
		   if( strcmp( (*((*nameListList)[0]) )[i]->myName().c_str(), 
			    (*(*mapLoopTbl)[mL_Idx])[tag_name_pos]->myValue().c_str() )==0 )
		     target_tag_pos = i;
	       }

	       if( target_tag_pos < 0 ){
		   (*errStream) <<"#transform-24# ERROR: input data file is missing tag "
				<<(*(*mapLoopTbl)[mL_Idx])[tag_name_pos]->myValue()
				<<endl;
		   break; /* do nothing */
	       }

	       for(curRow = 0; curRow<rowCount; curRow++){
		   if( (*(*valTable)[curRow])[target_tag_pos]->myDelimType() == DataValueNode::FRAMECODE ){
			(*(*valTable)[curRow])[target_tag_pos]->setDelimType(DataValueNode::NON);
		   }
		   strcpy(value, (*(*valTable)[curRow])[target_tag_pos]->myValue().c_str() );

		   for(int i=0; i<strlen(value); i++){
		      if( value[i] == '_' ){
			  value[i] = ' ';
		      }
		   }
		   int qStyle= FindNeededQuoteStyle( value );
		   if( qStyle == '\'' ) 
		   {   (*(*valTable)[curRow])[target_tag_pos]->setDelimType(DataValueNode::SINGLE ); }
		   else if( qStyle == '\"' )  
		   {   (*(*valTable)[curRow])[target_tag_pos]->setDelimType(DataValueNode::DOUBLE ); }
		   else if(  qStyle == ';' )  
		   {   (*(*valTable)[curRow])[target_tag_pos]->setDelimType(DataValueNode::SEMICOLON ); }
		   (*(*valTable)[curRow])[target_tag_pos]->setValue(value);
	       }
	   }
	   if( matchTag ) delete matchTag;
       }
       if( matchSF ) delete matchSF;
       
   }
}
