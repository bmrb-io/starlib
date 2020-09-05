/////////////////////////////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//  $Log: not supported by cvs2svn $
// Revision 1.2  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
//  Revision 1.1  1999/06/10 17:42:26  zixia
//  *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////////////////
#endif
#include "transforms.h"

void save_copy_saveframe(StarFileNode* inStar,
		       StarFileNode* outStar,
		       BlockNode*    ,
		       SaveFrameNode *currDicRuleSF)
{
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int	key_tag_pos = -1;  
   int  key_value_pos = -1;

   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_key_tag" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-23# ERROR: no loop tag called '_input_key_tag' in mapping file." << endl;
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
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-23# ERROR: input mapping file is missing tag '_key_tag'." << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-23# ERROR: input mapping file is missing tag '_key_value'." << endl;
       return; /* do nothing */
   }

   // For each row of the loop, move saveframe.
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {  
       List<ASTnode*> *matchSF;
       ASTnode * par;
       
       //Find SAVEFRAME by tag and value pair
       if( strcmp( (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue().c_str(), ".") != 0 ){ 
       
	   matchSF = AST->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
                        (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
       
       }else{//Find SAVEFRAME by key tag only
       
	   matchSF = AST->searchForTypeByTag(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue());
       }
       
       for(int i=0; i<matchSF->size(); i++){   
	  par = (*matchSF)[i]; 
	  
          SaveFrameNode *newSaveFrame = new SaveFrameNode( *(SaveFrameNode*)par );

          // Insert the newly made saveframe at the end of the
          // output star file:
       
          List<ASTnode*> *outputBlocks = outStar->searchForType( ASTnode::BLOCKNODE );
          BlockNode *lastOutputBlock = (BlockNode*)( (*outputBlocks)[outputBlocks->size() - 1] );
          delete outputBlocks;
          lastOutputBlock->GiveMyDataList()->insert( lastOutputBlock->GiveMyDataList()->end(), newSaveFrame );  
       }
       
       delete matchSF; 
   }
   delete mapNames;
   delete mapLoopTbl;
    
}
