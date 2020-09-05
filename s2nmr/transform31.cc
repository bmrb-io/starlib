///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
// Revision 1.2  1999/07/30  18:22:08  june
// june, 7-30-99
// add transform1d.cc
//
//Revision 1.1  1999/06/10 17:44:00  zixia
//*** empty log message ***
//
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_change_database_name(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF)
{  
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int key_tag_pos = -1;
   int value_pos = -1;
   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_key_tag" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-31# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
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
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_value" ) )
            value_pos = i;
   }
   
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-31# ERROR: input mapping file is missing tag '_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( value_pos < 0  )
   {   (*errStream) << "#transform-31# ERROR: input mapping file is missing tag '_value'." 
                    << endl;
       return; /* do nothing */
   }
   
   // For each row of the loop, change the database name. 
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   { 
      List<ASTnode*>   *matchTag;
      ASTnode          *cur_par;
      DataItemNode     *dataItemPeer;
      DataLoopNode     *loopPeer;
      char   value[200]="";
      
      matchTag = inStar->searchByTag( (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue() );

      for(int i=0; i<matchTag->size(); i++){
	  
          cur_par = (*matchTag)[i]->myParent();
	  
	  while( cur_par != NULL && !cur_par->isOfType(ASTnode::DATALOOPNODE) )
		  cur_par = cur_par->myParent();
	  
	  if( cur_par->isOfType(ASTnode::DATALOOPNODE) ){//A loop tag.
              DataLoopNameListNode  *nameListList;
	      LoopTableNode         *valTable;
	      int target_tag_pos = -1;
	      int rowCount, colCount, curRow, curCol;
	      
	      loopPeer = (DataLoopNode*)cur_par->myParallelCopy();
	     
              nameListList = loopPeer->getNamesPtr();
	      valTable     = loopPeer->getValsPtr();
	      
	      //The number of columns in the loop.
              rowCount = valTable->size();
              //Size of the first row of names(the outermost loop level).
              colCount = (*nameListList)[0]->size();

	      for(int j=0; j<colCount; j++){
                   if( strcmp( (*((*nameListList)[0]) )[j]->myName().c_str(),
			   (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue().c_str())==0  )
                      target_tag_pos = j;
              }
      
              if( target_tag_pos < 0 ){
                     (*errStream) <<"#transform-31# ERROR: input data file is missing tag "
			          <<(*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue()
                       <<endl;
                      continue; /* do nothing */
              }

	      //For each row of the loop, apply the changing of name.
	      for(curRow=0; curRow<rowCount; curRow++){
		 //Check if the value of the tag contains 'PDB'. If yes, change the value into
		 //'PDB'. If no, do nothing.
                 if(strstr((*(*valTable)[curRow])[target_tag_pos]->myValue().c_str(),
			 (*(*mapLoopTbl)[mL_Idx])[value_pos]->myValue().c_str() ) != NULL ){

		     (*(*valTable)[curRow])[target_tag_pos]->setDelimType(DataValueNode::NON);
		     
		     (*(*valTable)[curRow])[target_tag_pos]->setValue(
			     (*(*mapLoopTbl)[mL_Idx])[value_pos]->myValue());
		 }
			 
	      }

	  }else{//A DataItemNode 
             dataItemPeer=(DataItemNode*)(*matchTag)[i]->myParallelCopy();
	     strcpy(value, dataItemPeer->myValue().c_str());
	     if( strstr(value, (*(*mapLoopTbl)[mL_Idx])[value_pos]->myValue().c_str() ) != NULL){
		dataItemPeer->setDelimType(DataValueNode::NON); 
                dataItemPeer->setValue((*(*mapLoopTbl)[mL_Idx])[value_pos]->myValue());
	     }
	  }

      }

      delete matchTag;
   }
}
