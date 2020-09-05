///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
// Revision 1.2  1999/07/30  18:22:08  june
// june, 7-30-99
// add transform1d.cc
//
//Revision 1.1  1999/06/18 21:14:35  zixia
//*** empty log message ***
//
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_convert_C_to_K(StarFileNode* inStar,
		         StarFileNode* outStar,
		         BlockNode*,
		         SaveFrameNode *currDicRuleSF)
{   
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int key_tag_pos   = -1;
   int key_value_pos = -1;
   int type_tag_pos  = -1;
   int value_tag_pos = -1;
   int unit_tag_pos  = -1;
   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_key_tag" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-32# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
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
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_type_tag" ) )
            type_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_value_tag" ) )
            value_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_unit_tag" ) )
            unit_tag_pos = i;
   }
   
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-32# ERROR: input mapping file is missing tag '_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-32# ERROR: input mapping file is missing tag '_key_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( type_tag_pos < 0  )
   {   (*errStream) << "#transform-32# ERROR: input mapping file is missing tag '_type_tag'." 
                    << endl;
       return; /* do nothing */
   }
     if( value_tag_pos < 0  )
   {   (*errStream) << "#transform-32# ERROR: input mapping file is missing tag '_value_tag'." 
                    << endl;
       return; /* do nothing */
   }
      if( unit_tag_pos < 0  )
   {   (*errStream) << "#transform-32# ERROR: input mapping file is missing tag '_unit_tag'." 
                    << endl;
       return; /* do nothing */
   }
   
   
   // For each row of the loop, change the database name. 
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   { 
      List<ASTnode*>   *matchSF;
      
      matchSF = inStar->searchForTypeByTagValue(
	                 ASTnode::SAVEFRAMENODE,
			 (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
			 (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );

      
      //For each match, find the tag _type_tag 
      for(int i=0; i<matchSF->size(); i++){
	  List<ASTnode*>       *matchLoop;
          DataLoopNode         *loopPeer;
	  DataLoopNameListNode *nameListList;
	  LoopTableNode        *valTable;
	  LoopNameListNode     *nameList;
	  char charvalue[200] = "";
	  int  intvalue = 0;
	  int type_tag_inloop_pos = -1;
	  int value_tag_inloop_pos = -1;
	  int unit_tag_inloop_pos = -1;
	  int rowCount, colCount;
	  int curRow, curCol;
	  
	  //Find the dataLoopNode contains _type_tag.
	  matchLoop = ((SaveFrameNode *)(*matchSF)[i])->searchForTypeByTag(
		       ASTnode::DATALOOPNODE,
		       (*(*mapLoopTbl)[mL_Idx])[type_tag_pos]->myValue() );

	  if(matchLoop->size() != 1 ){
              (*errStream)<<"#transform-32# ERROR: only one loop with tag "
		          <<(*(*mapLoopTbl)[mL_Idx])[type_tag_pos]->myValue() 
			  <<" should be found."
			  <<endl;
	      delete matchSF;
	      return;
	  }
	  
          loopPeer = (DataLoopNode*)(*matchLoop)[0]->myParallelCopy();   

	  if(loopPeer == NULL){
             (*errStream)<<"#transform-32# ERROR: no loop peer is found."<<endl;
	     return;
	  }
          
	  nameListList = loopPeer->getNamesPtr();
	  nameList     = (*nameListList)[0];
	  valTable     = loopPeer->getValsPtr();
	  rowCount     = valTable->size();
	  colCount     = nameList->size();
	  
          for(int j=0; j<nameList->size(); j++){
             if( strcmp( (*nameList)[j]->myName().c_str(), 
		     (*(*mapLoopTbl)[mL_Idx])[type_tag_pos]->myValue().c_str() ) == 0 )
		 type_tag_inloop_pos = j;
	     else if( strcmp( (*nameList)[j]->myName().c_str(), 
		     (*(*mapLoopTbl)[mL_Idx])[value_tag_pos]->myValue().c_str() ) == 0 )
		 value_tag_inloop_pos = j;
	     else if( strcmp( (*nameList)[j]->myName().c_str(), 
		     (*(*mapLoopTbl)[mL_Idx])[unit_tag_pos]->myValue().c_str() ) == 0 )
		 unit_tag_inloop_pos = j;
	  }
	  
	  if( type_tag_inloop_pos < 0){
	   (*errStream)<<"#transform-32# ERROR: input file is missing tag "
	                <<(*(*mapLoopTbl)[mL_Idx])[type_tag_pos]->myValue()
			<<"."
			<<endl;
			return;
	  }
          if( value_tag_inloop_pos < 0){
	   (*errStream)<<"#transform-32# ERROR: input file is missing tag "
	                <<(*(*mapLoopTbl)[mL_Idx])[value_tag_pos]->myValue()
			<<"."
			<<endl;
			return;
	  }
	  if( unit_tag_inloop_pos < 0){
	   (*errStream)<<"#transform-32# ERROR: input file is missing tag "
	                <<(*(*mapLoopTbl)[mL_Idx])[unit_tag_pos]->myValue()
			<<"."
			<<endl;
			return;
	  }

	  for(curRow=0; curRow<rowCount; curRow++){
              //If the value of type_tag is 'temperature' and the unit_tag is 'C',
	      //then add 273 to the old value of value_tag and change the unit_tag from 'C' to 'K'.
	      if( strcasecmp( (*(*valTable)[curRow])[type_tag_inloop_pos]->myValue().c_str(), "temperature") == 0
		  && strcasecmp( (*(*valTable)[curRow])[unit_tag_inloop_pos]->myValue().c_str(), "C") == 0 ){
                 
		  strcpy(charvalue, (*(*valTable)[curRow])[value_tag_inloop_pos]->myValue().c_str() );
		  intvalue =  atoi(charvalue);
		  intvalue = intvalue + 273;

		  sprintf(charvalue, "%d", intvalue);
		  
		  (*(*valTable)[curRow])[unit_tag_inloop_pos]->setValue(string("K"));
		  (*(*valTable)[curRow])[value_tag_inloop_pos]->setValue(charvalue);
		  
	      }
	  }
	  delete matchLoop;
      }
   
      delete matchSF;
   }
}
