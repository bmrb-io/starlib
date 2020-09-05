///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
///////////////////////////////////////////////////////////////
#endif  
#include "transforms.h"

void save_insert_value_from_lookup_B( StarFileNode* inStar,
		                      StarFileNode* outStar,
		                      BlockNode*,
		                      SaveFrameNode *currDicRuleSF )
{  
    
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int static_value_pos = -1;
   int look_up_where_pos = -1;
   int retrieve_value_pos = -1;
   int new_tag_name_pos = -1;

   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_look_for_this_value" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-45# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
       delete mapLoopMatch;
       return; /* do nothing */
   }
   
   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

   delete mapLoopMatch;

   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
   {
        if( (* ((*mapNames)[0]) )[i]->myName()==string( "_look_for_this_value" ) )
            static_value_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_look_up_where_that_value_matches_this_tag" ) )
            look_up_where_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_retrieve_value_of_this_tag_on_that_same_row" ) )
            retrieve_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_name" ) )
            new_tag_name_pos = i;
   }
   if( static_value_pos < 0  )
   {   (*errStream) << "#transform-45# ERROR: input mapping file is missing tag '_look_for_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( look_up_where_pos < 0  )
   {   (*errStream) << "#transform-45# ERROR: input mapping file is missing tag '_look_up_where_that_value_matches_this_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( retrieve_value_pos < 0  )
   {   (*errStream) << "#transform-45# ERROR: input mapping file is missing tag '_retrieve_value_of_this_tag_on_that_same_row'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_tag_name_pos < 0  )
   {   (*errStream) << "#transform-45# ERROR: input mapping file is missing tag '_new_tag_name'." 
                    << endl;
       return; /* do nothing */
   }

   
   // For each row of the dictionary loop:
   // ------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {
       string static_value =
           (*((*mapLoopTbl)[mL_Idx]))[static_value_pos]->myValue();
       string look_tag_name =
           (*((*mapLoopTbl)[mL_Idx]))[look_up_where_pos]->myValue();
       string retrieve_value_name =
           (*((*mapLoopTbl)[mL_Idx]))[retrieve_value_pos]->myValue();
       string new_tag_name =
           (*((*mapLoopTbl)[mL_Idx]))[new_tag_name_pos]->myValue();

       List<ASTnode*> *matches = inStar->searchByTagValue( look_tag_name, static_value );

       int matchIdx;
       string retrievedValue = string(".");
       DataValueNode::ValType retrievedDelim = DataValueNode::NON;
       for( matchIdx = 0 ; matchIdx < matches->size() ; matchIdx++ )
       {
           ASTnode *match = (*matches)[matchIdx];
	   if( match->isOfType( ASTnode::DATAITEMNODE ) )
	   {
	       List<ASTnode*> *matches2 = match->myParent()->searchByTag( retrieve_value_name );
	       if( matches2 && matches2->size() > 0 )
	       {
		   if( (*matches2)[0]->isOfType( ASTnode::DATAITEMNODE ) )
		   {
		       retrievedValue = ( (DataItemNode*)( (*matches2)[0] ) )->myValue();
		       retrievedDelim = ( (DataItemNode*)( (*matches2)[0] ) )->myDelimType();
		   }
	       }
	       delete matches2;
	   }
	   else // It's a data value node inside a loop.
	   {
	       ASTnode *par = NULL;
	       LoopRowNode *lrn = NULL;
	       for(  par = match ;
	             par != NULL && (! (par->isOfType(ASTnode::DATALOOPNODE)) ) ;
	             par = par->myParent() )
	       {   if( par->isOfType(ASTnode::LOOPROWNODE) )
	           {   lrn = (LoopRowNode*)(par);
		   }
	       }
	       if( lrn != NULL )
	       {
		   int depth, col;
		   if( par != NULL )
		   {   ((DataLoopNode*)par)->tagPositionDeep( retrieve_value_name, &depth, &col );
		       if( col > -1 )
		       {
			   // Take the value from the same row on which the look_tag was found::
			   retrievedValue = (*lrn)[ col ]->myValue();
			   retrievedDelim = (*lrn)[ col ]->myDelimType();
		       }
		   }
	       }
	   }

	   // Now make the new tag in the same saveframe:
	   ASTnode *parentSF = NULL;
	   for( parentSF = match ;
	        parentSF != NULL && (! parentSF->isOfType(ASTnode::SAVEFRAMENODE) ) ;
		parentSF = parentSF->myParent() )
	   { /*void body*/ }
	   if( parentSF != NULL )
	   {   parentSF = parentSF->myParallelCopy();
	       if( parentSF != NULL )
	       {   ((SaveFrameNode*)parentSF)->GiveMyDataList()->insert(
	               ((SaveFrameNode*)parentSF)->GiveMyDataList()->end(),
		       new DataItemNode( new_tag_name, retrievedValue, retrievedDelim ) );
	       }
	   }
       }
       delete matches;
   }
}
