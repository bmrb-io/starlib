//This is the program for insert ordinal column. 
//Created 1999/07/19, zhijun
//It could insert a new column with sequence or constant numbers as values  
//Works for multiple saveframe 
#ifdef BOGUS_DEF_COMMENT_OUT
/* 
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2003/07/24 18:49:13  madings
 * Changes to handle 2.1-3.0 mappings.
 *
 * Revision 1.3  2001/02/13 23:00:43  jurgen
 * Elaborated and corrected error messages
 *
 * Revision 1.2  2000/11/18 03:02:38  madings
 * *** empty log message ***
 *
 */
#endif


#include "transforms.h"

void save_insert_ordinal(StarFileNode* inStar,
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
   int new_tag_value_start_pos = -1;
   int new_tag_value_interval_pos = -1;
   int new_tag_value_change_pos = -1;
   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_key_tag" ) );
   if( mapLoopMatch->size() <= 0 )
   {
       (*errStream) << "#transform-26a# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
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
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_value_start" ) )
            new_tag_value_start_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_value_interval" ) )
            new_tag_value_interval_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_value_change" ) )
            new_tag_value_change_pos = i;
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-26a# ERROR: input mapping file is missing tag '_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-26a# ERROR: input mapping file is missing tag '_key_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_loop_tag_pos < 0  )
   {   (*errStream) << "#transform-26a# ERROR: input mapping file is missing tag '_key_loop_name'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_tag_name_pos < 0  )
   {   (*errStream) << "#transform-26a# ERROR: input mapping file is missing tag '_new_tag_name'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_tag_value_start_pos < 0  )
   {   (*errStream) << "#transform-26a# ERROR: input mapping file is missing tag '_new_tag_value_start'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_tag_value_interval_pos < 0  )
   {   (*errStream) << "#transform-26a# ERROR: input mapping file is missing tag '_new_tag_value_interval'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_tag_value_change_pos < 0  )
   {   (*errStream) << "#transform-26a# ERROR: input mapping file is missing tag '_new_tag_value_change'." 
                    << endl;
       return; /* do nothing */
   }


   
   // For each row of the loop, add ordinal to loops of saveframes.
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {   
       List<ASTnode*>        *matchSF;
       
       matchSF = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
                        (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
       
       
       if( matchSF->size() <=0){
	   (*errStream)<<"#transform-26a# ERROR:no saveframe with  tag/value of "
              <<(*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue()<<"/"
	     <<(*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue()
	     <<"is found."<<endl;
	   delete matchSF;
	   continue;
       }
       
       //for each matching saveframe
       for( int currSF =0; currSF < matchSF->size(); currSF++)
	 {
	   List<ASTnode*>        *matchLoop;
	   DataLoopNameListNode  *nameListList;
	   LoopTableNode         *valTable;
	   LoopNameListNode      *nameList;
	   DataLoopNode          *loopPeer;
	   int rowCount, colCount;

	   matchLoop = ((SaveFrameNode *)((*matchSF)[currSF]))
	     ->searchForTypeByTag(
	               ASTnode::DATALOOPNODE,
	               (*(*mapLoopTbl)[mL_Idx])[key_loop_tag_pos]->myValue());

	   if( matchLoop->size() < 1){ /* JFD elaboration of error message */
	                               /* SLM: changed from != 1 to < 1, because
				        * in a temporary state the file will
					* sometimes have duplicate copies of
					* the key tags during s2nmr's run, and
					* in that case it's safe to ignore the
					* extra hits.
					*/
	     (*errStream)   <<"#transform-26a# ERROR: "
                            <<matchLoop->size()
                            <<" tag(s) found whereas one tag with name: "
			    <<(*(*mapLoopTbl)[mL_Idx])[key_loop_tag_pos]->myValue()
                            <<" should have been found (saveframe:"
                            <<((SaveFrameNode *)((*matchSF)[currSF]))->myName()
                            <<")."<<endl;
	     delete matchLoop;
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

       //insert a new column of new_tag_name with values "."
       nameList->insert(nameList->begin(), 
			    new DataNameNode((*(*mapLoopTbl)[mL_Idx])[new_tag_name_pos]->myValue()), *new DataValueNode("."));
       
       
       //insert values        
       int valToInsert;
       int startVal = (atoi) (((*(*mapLoopTbl)[mL_Idx])[new_tag_value_start_pos]->myValue()).c_str());
       int interval =
       (atoi)(((*(*mapLoopTbl)[mL_Idx])[new_tag_value_interval_pos]->myValue()).c_str());

       char tmpArr[80];
       DataValueNode *currVal;
       //for each row in loop
       for( int currRow=0; currRow<rowCount; currRow++)
	 {
	   if((*(*mapLoopTbl)[mL_Idx])[new_tag_value_change_pos]->myValue() ==
	      string ("increase"))
	     valToInsert = startVal + interval*currRow;

	   else if((*(*mapLoopTbl)[mL_Idx])[new_tag_value_change_pos]->myValue
		   () ==  string ("decrease"))
	     valToInsert = startVal - interval*currRow;

	   else if((*(*mapLoopTbl)[mL_Idx])[new_tag_value_change_pos]->myValue
		   () == string ("constant"))
	     valToInsert = startVal;

	   sprintf(tmpArr, "%d", valToInsert);
	 
	   //set the value in table as the seq. value
	   currVal = (*((*valTable)[currRow]))[0];
	   currVal->setValue(tmpArr);
	 }
        
       delete matchLoop;
	 }
       delete matchSF;
      
   }
   
}





