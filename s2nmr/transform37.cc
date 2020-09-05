///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.1  1999/07/13  16:12:05  zixia
// *** empty log message ***
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_copy_saveframe_content(StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF)
{
    List<ASTnode*>        *mapLoopMatch;
    DataLoopNameListNode  *mapNames;
    LoopTableNode         *mapLoopTbl;
    int                   mL_Idx; /* map loop index */
    string                tagname;
    int                   source_saveframe_name_pos;
    int                   key_tag_pos;
    int                   key_value_pos;
    
    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_source_saveframe_name" ) );
    if( mapLoopMatch->size() < 0 )
    {
	(*errStream) << "#transform-37# ERROR: no loop tag called '_source_saveframe_name' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;

    source_saveframe_name_pos = -1;
    key_tag_pos = -1;
    key_value_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_source_saveframe_name" ) )
	    source_saveframe_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
    }
    if( source_saveframe_name_pos < 0  )
    {   (*errStream) << "#transform-37# ERROR: input mapping file is missing tag '_source_saveframe_name'." << endl;
	return; /* do nothing */
    }
    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-37# ERROR: input mapping file is missing tag '_key_tag'." << endl;
	return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-37# ERROR: input mapping file is missing tag '_key_value'." << endl;
	return; /* do nothing */
    }

    // For each row, do the transformation:
    // -----------------------------------------------
    for( mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
    {
	//Find all the saveframe node.
	List<ASTnode*> *matchSF = inStar->searchForType( ASTnode::SAVEFRAMENODE );

	// For each matched saveframe, get each matching value:
	for( int sf_i = 0 ; sf_i < matchSF->size() ; sf_i++ )
	{

            //Find the saveframe having the name of _source_saveframe_name.
	    if( strcmp( ((SaveFrameNode*)((*matchSF)[sf_i]))->myName().c_str(), 
		        (*(*mapLoopTbl)[mL_Idx])[source_saveframe_name_pos]->myValue().c_str() ) == 0 ){
	   

               string tagName  = (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue();
               string tagValue = (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue();  
	       
	       SaveFrameNode * newSaveFrame = new SaveFrameNode( *((SaveFrameNode*)((*matchSF)[sf_i])) );
	      
	       DataItemNode *newItem = new DataItemNode(tagName, tagValue);
	       newSaveFrame->GiveMyDataList()->insert( newSaveFrame->GiveMyDataList()->end(), newItem );
	       // Insert the newly made saveframe at the end of the
               // output star file:
       
              List<ASTnode*> *outputBlocks = outStar->searchForType( ASTnode::BLOCKNODE );
              BlockNode *lastOutputBlock = (BlockNode*)( (*outputBlocks)[outputBlocks->size() - 1] );
              delete outputBlocks;
              lastOutputBlock->GiveMyDataList()->insert( lastOutputBlock->GiveMyDataList()->end(), newSaveFrame );
	      break;
	    }
	}
	delete matchSF;
    } //end-for
}

