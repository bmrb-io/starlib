///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
//Revision 1.6  2008/04/16 21:29:17  madings
//Haven't committed in a while and this is a working version so I should.
//
//Revision 1.5  2006/11/14 23:12:45  madings
//*** empty log message ***
//
//Revision 1.4  2006/10/27 23:22:06  madings
//Changes to allow for assigned chemical shift splitting.
//
//Revision 1.3  2006/06/26 22:45:18  madings
//*** empty log message ***
//
//Revision 1.2  2006/03/23 19:49:09  madings
//*** empty log message ***
//
//Revision 1.1  2003/07/24 18:49:13  madings
//Changes to handle 2.1-3.0 mappings.
//
///////////////////////////////////////////////////////////////
#endif  
#include "transforms.h"

SaveFrameNode *getParentSF( ASTnode *in );

void save_insert_value_from_lookup_A( StarFileNode* inStar,
		                      StarFileNode* outStar,
		                      BlockNode*,
		                      SaveFrameNode *currDicRuleSF )
{  
    
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int get_the_value_pos = -1;
   int look_up_where_pos = -1;
   int this_tag_on_lookup_row_pos = -1;
   int has_this_value_pos = -1;
   int retrieve_value_pos = -1;
   int new_tag_name_pos = -1;

   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_get_the_value_for_this_tag" ) );
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
        if( (* ((*mapNames)[0]) )[i]->myName()==string( "_get_the_value_for_this_tag" ) )
            get_the_value_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_look_up_where_that_value_matches_this_tag" ) )
            look_up_where_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_if_this_tag_on_the_lookup_row" ) ) // optional tag
            this_tag_on_lookup_row_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_has_this_value" ) ) // optional tag
            has_this_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_retrieve_value_of_this_tag_on_that_same_row" ) )
            retrieve_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_name" ) )
            new_tag_name_pos = i;
   }
   if( get_the_value_pos < 0  )
   {   (*errStream) << "#transform-45# ERROR: input mapping file is missing tag '_get_the_value_for_this_tag'." 
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
       string start_tag_name =
           (*((*mapLoopTbl)[mL_Idx]))[get_the_value_pos]->myValue();
       string look_tag_name =
           (*((*mapLoopTbl)[mL_Idx]))[look_up_where_pos]->myValue();
       string this_tag_on_lookup_row = // optional
           ( this_tag_on_lookup_row_pos >= 0  ?  (*((*mapLoopTbl)[mL_Idx]))[this_tag_on_lookup_row_pos]->myValue() : string("*") );
       string has_this_value =  //optional
           ( has_this_value_pos >= 0   ?  (*((*mapLoopTbl)[mL_Idx]))[has_this_value_pos]->myValue()  : string("*") );
       string retrieve_value_name =
           (*((*mapLoopTbl)[mL_Idx]))[retrieve_value_pos]->myValue();
       string new_tag_name =
           (*((*mapLoopTbl)[mL_Idx]))[new_tag_name_pos]->myValue();

       List<ASTnode*> *startTagMatches = inStar->searchByTag(start_tag_name);

       // Was this - now moved down inside the loop:
       //  DataValueNode *prevDVN = new DataValueNode(".");
       //  string prevValToLookup = string("");

       int startTagMatchesIdx;
       for( startTagMatchesIdx = 0;
            startTagMatchesIdx < startTagMatches->size();
            startTagMatchesIdx++ )
       {
           ASTnode *curStartTagMatch = (*startTagMatches)[startTagMatchesIdx];

	   // startTagValues is a list pointing to the matching values
	   // from within the input tree - if curStartMatch is looped,
	   // it's going to have many values, else just one value:
	   List<ASTnode*>  *startTagValues = new List<ASTnode*>;
	   List<DataValueNode*>  *newTagValues = new List<DataValueNode*>;
           DataValueNode *prevDVN = new DataValueNode(".");
           string prevValToLookup = string("");
	   if( curStartTagMatch->isOfType( ASTnode::DATAITEMNODE ) )
	   {   startTagValues->insert( startTagValues->end(),
			((DataItemNode*)curStartTagMatch) );
	   }
	   else if( curStartTagMatch->isOfType( ASTnode::DATANAMENODE ) )
	   {   
	       // Sloppy hardcoding based on the assumption that
	       // a name in a loop is exactly three children deep:
	       // DataLoopnode...
	       //   DataLoopNameListNode...
	       //     LoopNameListNode...
	       //       DataNameNode
	       DataLoopNode *dln = (DataLoopNode*)
	                   curStartTagMatch->myParent()->myParent()->myParent();
	       int row, col, depth;
	       dln->tagPositionDeep(
	                      ((DataNameNode*)curStartTagMatch)->myName(),
	                      &depth,
			      &col );
	       for( row = 0 ; row < dln->getValsPtr()->size() ; row++ )
	       {
		   startTagValues->insert( startTagValues->end(),
			(*((*( dln->getValsPtr() ))[row]))[col] );
	       }
	   }

	   // Okay now we have a list of values for the start tag.
	   // For each one of them, do the lookup:
	   int startTagValuesIdx;
	   for( startTagValuesIdx = 0;
	        startTagValuesIdx < startTagValues->size();
		startTagValuesIdx++  )
	   {

	       ASTnode *cur = (*startTagValues)[startTagValuesIdx];
	       string curValToLookup = string("");
	       if( cur->isOfType(ASTnode::DATAITEMNODE) )
	           curValToLookup = ((DataItemNode*)cur)->myValue();
	       else if( cur->isOfType(ASTnode::DATAVALUENODE) )
	           curValToLookup = ((DataValueNode*)cur)->myValue();
	       List<ASTnode*> *matches = inStar->searchByTagValue(
	           look_tag_name, curValToLookup );

	       // If it's the same as the previous value, just re-use the previous
	       // value without doing the expensive lookup:
	       if( curValToLookup == prevValToLookup )
	       {
		   newTagValues->insert( newTagValues->end(),
				     new DataValueNode( *prevDVN )
				     );
	           continue;
	       }
	       prevValToLookup = curValToLookup;

	       // If the extra secondary conditional tags were used, then further trim the list
	       // of hits down to only the ones that match the seeked-for-value:
	       if( matches->size() > 0 && this_tag_on_lookup_row != string("*") )
	       {
		   // Loop counts backward because there will be deletions occurring:
	           for( int matchRow = matches->size() - 1 ; matchRow >= 0 ; --matchRow ) 
		   {
		       bool deleteThis = false;
		       ASTnode *thisHit = (*matches)[matchRow];
		       // Only matters if this is a loop match, otherwise this the additional condition
		       // is meaningless:
		       if( thisHit->isOfType( ASTnode::DATAVALUENODE ) )
		       {
		           // Walk up to the row, and to the entire loop:
			   LoopRowNode *matchRow = NULL;
			   DataLoopNode *matchLoop = NULL;
			   while( thisHit != NULL ) {
			       if( thisHit->isOfType( ASTnode::LOOPROWNODE ) )
			       {
			           matchRow = (LoopRowNode*) thisHit;
			       } else if( thisHit->isOfType( ASTnode::DATALOOPNODE ) ) 
			       {
			           matchLoop = (DataLoopNode*) thisHit;
			       }
			       thisHit = thisHit->myParent();
			   }
			   // Find if the tag name in question is present in the loop, and if so get its value on this row:
		           int col = -1;
			   int nest = -1;
		           matchLoop->tagPositionDeep( this_tag_on_lookup_row, &nest, &col );
			   if( col >= 0 )
			   {
			       string checkVal = (*matchRow)[col]->myValue(); 
			       if( has_this_value != string("*") && has_this_value !=  checkVal ) {
			          deleteThis = true;
			       }
			   }
		       }
		       if( deleteThis ) {
		           matches->erase( matches->begin() + matchRow );
		       }
		   }
	       }

	       if( matches->size() > 0 )
	       {
		   // If multiple such hits exist, this rule doesn't
		   // understand what that means, so just take the
		   // first hit we find:
		   // Change:  Actually, if multiple hits exist, then
		   // take the hit from the same saveframe, if such a hit
		   // exists.
		   ASTnode *usedMatch = (*matches)[0]; // default to the first one:
		   if( matches->size() > 1 )
		   {
		       for( int i = 0 ; i < matches->size() ; i++ )
		       {
			   if( getParentSF( (*matches)[i] ) == getParentSF( cur ) )
			   {
			        usedMatch = (*matches)[i];
				break;
			   }
		       }
		   }
		   if( usedMatch->isOfType( ASTnode::DATAITEMNODE ) )
		   {
		       ASTnode *par;
		       List<ASTnode*> *retrievedItem = NULL;
		       for(  par = usedMatch ;
		             par != NULL
			         && ! par->isOfType( ASTnode::SAVEFRAMENODE )
			         && ! par->isOfType( ASTnode::BLOCKNODE ) ;
                             par = par->myParent()   )
		       { }
		       retrievedItem = par->searchByTag( retrieve_value_name );
		       if( retrievedItem->size() > 0 &&
		           (*retrievedItem)[0]->isOfType(ASTnode::DATAITEMNODE)
			 )
		       {
			   prevDVN = new DataValueNode(
					      ((DataItemNode*)((*retrievedItem)[0]))->myValue(),
					      ((DataItemNode*)((*retrievedItem)[0]))->myDelimType()
					      );
		           newTagValues->insert(newTagValues->end(), prevDVN );
		       }
		       else
		       {
			   prevDVN  = new DataValueNode( string("?"), DataValueNode::NON );
		           newTagValues->insert(newTagValues->end(), prevDVN );
		       }
		       delete retrievedItem;
		   }
		   else if( usedMatch->isOfType( ASTnode::DATAVALUENODE ) )
		   {
		       ASTnode *par;
		       for(  par = usedMatch ;
		             par != NULL
			         && ! par->isOfType( ASTnode::LOOPROWNODE );
                             par = par->myParent()   )
		       { }
		       if( par == NULL ) continue;
		       LoopRowNode *hitRow = (LoopRowNode*)par;

		       for(  par = hitRow ;
		             par != NULL
			         && ! par->isOfType( ASTnode::DATALOOPNODE );
                             par = par->myParent()   )
		       { }
		       if( par == NULL ) continue;
		       DataLoopNode *hitLoop = (DataLoopNode*)par;

		       int nest, col;
		       hitLoop->tagPositionDeep( retrieve_value_name,
		                                 &nest, &col );
		       if( col >= 0 )
		       {
			   prevDVN = new DataValueNode( (*hitRow)[col]->myValue(), (*hitRow)[col]->myDelimType());
			   newTagValues->insert( newTagValues->end(), prevDVN );
		       }
		       else
		       {
			   prevDVN = new DataValueNode(
					     string("?"),
					     DataValueNode::NON
					     );
			   newTagValues->insert( newTagValues->end(), prevDVN );
		       }
		   }
	       }
	       else
	       {
		   prevDVN = new DataValueNode( string("?"), DataValueNode::NON);
		   newTagValues->insert( newTagValues->end(), prevDVN );
               }

	       if( matches != NULL ) delete matches;

	   }


	   // Whew!  Okay, now we have the list of tags we started from,
	   // and the list of values we are going to be using for the new
	   // tags.  Now it's just a matter of making the new tags next
	   // to the tags we started from:

	   for( startTagValuesIdx = 0;
	        startTagValuesIdx < startTagValues->size();
		startTagValuesIdx++  )
	   {
               ASTnode *curStartTagValue = 
	           (*startTagValues)[startTagValuesIdx];
	       if( curStartTagValue->isOfType(ASTnode::DATAITEMNODE) )
	       {
		   ASTnode *par;
		   for(  par = curStartTagValue ;
			 par != NULL
			     && ! par->isOfType( ASTnode::SAVEFRAMENODE )
			     && ! par->isOfType( ASTnode::BLOCKNODE ) ;
			 par = par->myParent()   )
		   { }
		   if( par != NULL && par->isOfType(ASTnode::SAVEFRAMENODE) )
		   {
		       SaveFrameNode * sfn = (SaveFrameNode*)(par->myParallelCopy());
		       if( sfn != NULL )
		       {   sfn->GiveMyDataList()->insert(
			       sfn->GiveMyDataList()->end(),
			       new DataItemNode( 
				   new_tag_name,
				   (*newTagValues)[startTagValuesIdx]->myValue() , 
				   (*newTagValues)[startTagValuesIdx]->myDelimType() ) );

		           delete (*newTagValues)[startTagValuesIdx] ;
		       }
		   }
	       }
	       else if( curStartTagValue->isOfType(ASTnode::DATAVALUENODE) )
	       {
		   ASTnode *par;
		   for(  par = curStartTagValue ;
			 par != NULL
			     && ! par->isOfType( ASTnode::LOOPROWNODE );
			 par = par->myParent()   )
		   { }
		   if( par == NULL ) continue;
		   LoopRowNode *hitRow = (LoopRowNode*)(par->myParallelCopy());

		   for(  par = hitRow ;
			 par != NULL
			     && ! par->isOfType( ASTnode::DATALOOPNODE );
			 par = par->myParent()   )
		   { }
		   if( par == NULL ) continue;
		   DataLoopNode *hitLoop = (DataLoopNode*)par;

		   int nest, col;
		   hitLoop->tagPositionDeep( new_tag_name,
					     &nest, &col );
	           if( col < 0 )
		   {
		       LoopNameListNode *lnln = (*( hitLoop->getNamesPtr() ))[0];
		       lnln->insert( lnln->end(), new DataNameNode( new_tag_name ) );
		       col = lnln->size() - 1;
		   }
		   // Suppress overwriting if overwriting a good value with a dummy:
		   if(   (  (*newTagValues)[startTagValuesIdx]->myValue() == string("?") ||
		         (  (*newTagValues)[startTagValuesIdx]->myValue() == string(".") ) )
			   &&
			 (  (*hitRow)[col]->myValue() != string(".") &&
			    (*hitRow)[col]->myValue() != string("?")  )  ) {
		       // Do nothing.
		   } else {
		       (*hitRow)[col]->setValue( (*newTagValues)[startTagValuesIdx]->myValue() );
		       (*hitRow)[col]->setDelimType( (*newTagValues)[startTagValuesIdx]->myDelimType() );
		   }
		   delete (*newTagValues)[startTagValuesIdx];
	       }
	   }

	   if( startTagValues != NULL ) delete startTagValues;
	   if( newTagValues != NULL ) delete newTagValues;

       }
       if( startTagMatches != NULL ) delete startTagMatches;
   }
}
SaveFrameNode *getParentSF( ASTnode *in )
{
    ASTnode *cur;

    for( cur = in ; cur != NULL ; cur = cur->myParent() )
    {
        if( cur->isOfType( ASTnode::SAVEFRAMENODE) )
	{
	    return (SaveFrameNode*) cur;
	}
    }
    return NULL;
}
