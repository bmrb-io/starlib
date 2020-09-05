///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
// Revision 1.4  1999/06/29  19:04:10  zixia
// *** empty log message ***
//
// Revision 1.3  1999/06/18  21:14:35  zixia
// *** empty log message ***
//
// Revision 1.2  1999/06/11  19:01:32  zixia
// *** empty log message ***
//
// Revision 1.1  1999/06/10  17:42:51  zixia
// *** empty log message ***
//
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_change_tag_value_II(StarFileNode* inStar,
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
   int new_value_key_tag_pos = -1;
   int new_value_key_value_pos = -1;
   int new_value_tag_name_pos = -1;
   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_key_tag" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-25# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
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
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_value_key_tag" ) )
            new_value_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_value_key_value" ) )
            new_value_key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_value_tag_name" ) )
            new_value_tag_name_pos = i;
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-25# ERROR: input mapping file is missing tag '_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-25# ERROR: input mapping file is missing tag '_key_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( tag_name_pos < 0  )
   {   (*errStream) << "#transform-25# ERROR: input mapping file is missing tag '_tag_name'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_value_key_tag_pos < 0  )
   {   (*errStream) << "#transform-25# ERROR: input mapping file is missing tag '_new_value_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_value_key_value_pos < 0  )
   {   (*errStream) << "#transform-25# ERROR: input mapping file is missing tag '_new_value_key_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_value_tag_name_pos < 0  )
   {   (*errStream) << "#transform-25# ERROR: input mapping file is missing tag '_new_value_tag_name'." 
                    << endl;
       return; /* do nothing */
   }


   // For each row of the loop, change the value.
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {   
       List<ASTnode*> *matchSF;
       List<ASTnode*> *matchTag;
       List<ASTnode*> *matchSF2;
       List<ASTnode*> *matchTag2;
       
       char value[200];
       
       
       matchSF = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
                        (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
       
       if( matchSF->size() != 1){
	   (*errStream)<<"#transform-25# ERROR: only one saveframe with tag "
              <<(*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue()<<" and value "
	      <<(*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue()<<" should be found."<<endl;
	   delete matchSF;
	   return;
       }

       matchTag = (*matchSF)[0]->searchByTag(
	               (*(*mapLoopTbl)[mL_Idx])[tag_name_pos]->myValue());

       
       if( matchTag->size() != 1){
	   (*errStream)<<"#transform-25# ERROR: only one tag "
              <<(*(*mapLoopTbl)[mL_Idx])[tag_name_pos]->myValue()<<" should be found."<<endl;
	   delete matchSF;
	   delete matchTag;
	   return;
       }
       
       matchSF2 = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[new_value_key_tag_pos]->myValue(),
                        (*(*mapLoopTbl)[mL_Idx])[new_value_key_value_pos]->myValue() );
       
       if( matchSF2->size() != 1){
	   (*errStream)<<"#transform-25# ERROR: only one saveframe with tag "
              <<(*(*mapLoopTbl)[mL_Idx])[new_value_key_tag_pos]->myValue()<<" should be found."<<endl;
	   delete matchSF;
	   delete matchTag;
	   delete matchSF2;
	   return;
       }
       
       matchTag2 = ((SaveFrameNode *)((*matchSF2)[0]))->searchByTag(
	               (*(*mapLoopTbl)[mL_Idx])[new_value_tag_name_pos]->myValue());

      
       if( matchTag2->size() != 1){
	   (*errStream)<<"#transform-25# ERROR: only one tag "
              <<(*(*mapLoopTbl)[mL_Idx])[new_value_tag_name_pos]->myValue()<<" should be found."<<endl;
	   delete matchSF;
	   delete matchTag;
	   delete matchSF2;
	   delete matchTag2;
	   return;
       }
       strcpy(value, (( (DataItemNode*)((*matchTag2)[0]) )->myValue()).c_str() );
       
       //Check the _key_tag is in loop
       ASTnode *cur_par = (*matchTag)[0]->myParent();
       while(cur_par != NULL && !cur_par->isOfType(ASTnode::DATALOOPNODE) )
	   cur_par = cur_par->myParent();
       
       
       if(cur_par !=NULL && cur_par->isOfType(ASTnode::DATALOOPNODE)){//loop tag
          DataLoopNode         *dataLoopPeer = (DataLoopNode*)cur_par->myParallelCopy();
	  DataLoopNameListNode *nameListList = dataLoopPeer->getNamesPtr();
	  LoopTableNode        *valTable     = dataLoopPeer->getValsPtr();
	  int in_loop_pos = -1;

	  for(int j=0; j<(*nameListList)[0]->size(); j++){
                if ( strcmp( (* ((*nameListList)[0]) )[j]->myName().c_str(),
			(*(*mapLoopTbl)[mL_Idx])[tag_name_pos]->myValue().c_str() ) == 0 ){
                     in_loop_pos = j;
		}
	  }

	  if( in_loop_pos < 0  )
          {   (*errStream) << "#transform-25# ERROR: input mapping file is missing tag '_key_tag' in the loop." 
                    << endl;
               continue; // do nothing 
          }
	  
	  //For each row, change the value
	  for(int k=0; k<1 ; k++){
             (*(*valTable)[k])[in_loop_pos]->setDelimType((((DataItemNode*)((*matchTag2)[0]))->myDelimType()));
             (*(*valTable)[k])[in_loop_pos]->setValue(value);
	  }
	   
       }else{//non-loop tag
         ASTnode *dataItemPeer = (*matchTag)[0]->myParallelCopy();
       
         ( (DataItemNode*)dataItemPeer )->setDelimType((( (DataItemNode*)((*matchTag2)[0]) )->myDelimType())  );
         ( (DataItemNode*)dataItemPeer )->setValue(value);
       }

       delete matchSF;
       delete matchTag;
       delete matchSF2;
       delete matchTag2;
   }
}
