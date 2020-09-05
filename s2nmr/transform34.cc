//////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
// Revision 1.2  1999/07/30  18:22:08  june
// june, 7-30-99
// add transform1d.cc
//
//Revision 1.1  1999/06/18 21:15:06  zixia
//*** empty log message ***
//
#endif
//////////////////////////////////////////////////////
#include "transforms.h"

void save_make_new_tag_II(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF)
{   
    List<ASTnode*>        *mapLoopMatch;
    DataLoopNameListNode  *mapNames;
    LoopTableNode         *mapLoopTbl;
    int                   mL_Idx; /* map loop index */
    int                   saveframe_name_pos;
    int                   new_tag_pos;
    int                   new_value_pos;

    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_saveframe_name" ) );
    if( mapLoopMatch->size() < 0 )
    {
	(*errStream) << "#transform-34# ERROR: no loop tag called '_saveframe_name' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;

    saveframe_name_pos = -1;
    new_tag_pos = -1;
    new_value_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_saveframe_name" ) )
	    saveframe_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag" ) )
	    new_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_value" ) )
	    new_value_pos = i;
    }
    if( saveframe_name_pos < 0  )
    {   (*errStream) << "#transform-34# ERROR: input mapping file is missing tag '_saveframe_name'." << endl;
	return; /* do nothing */
    }
    if( new_tag_pos < 0  )
    {   (*errStream) << "#transform-34# ERROR: input mapping file is missing tag '_new_tag'." << endl;
	return; /* do nothing */
    }
    if( new_value_pos < 0  )
    {   (*errStream) << "#transform-34# ERROR: input mapping file is missing tag '_new_value'." << endl;
	return; /* do nothing */
    }

    // For each row of the map loop, make a new tag in outStar:
    // --------------------------------------------------------------
    for( mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
    {
	List<ASTnode*> *matchSF;
	matchSF = AST->searchForType( ASTnode::SAVEFRAMENODE); 
	
	for( int i = 0 ;  i < matchSF->size(); i++ ){  
	       
	   if(  strcmp( ((SaveFrameNode*)(*matchSF)[i])->myName().c_str(), 
		    (*(*mapLoopTbl)[mL_Idx])[saveframe_name_pos]->myValue().c_str())==0 ){ 
            
	    
	       SaveFrameNode *inputSaveFrame = (SaveFrameNode*)( (*matchSF)[i] );
	       SaveFrameNode *outputSaveFrame = (SaveFrameNode*) inputSaveFrame->myParallelCopy();
	       
	       if( outputSaveFrame == NULL ){
	               (*errStream) << "#transform34# ERROR: Saveframe '" 
		                    <<inputSaveFrame->myName()
		                    <<"' has no peer in the output star file." 
		                    <<endl;
		       
		       delete matchSF;
		       return;
	       }
	       

	       outputSaveFrame->GiveMyDataList()->insert(
		    outputSaveFrame->GiveMyDataList()->end(),
		    new DataItemNode( 
			(*(*mapLoopTbl)[mL_Idx])[new_tag_pos]->myValue(),
			(*(*mapLoopTbl)[mL_Idx])[new_value_pos]->myValue(),
			(*(*mapLoopTbl)[mL_Idx])[new_value_pos]->myDelimType() ) );
	   }
	}
	delete matchSF;
    } //end-for
}

