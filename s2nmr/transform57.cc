///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_free_looped_tags_by_value( StarFileNode *inStar, 
                                     StarFileNode *outStar, 
                                     BlockNode    *,
		  		     SaveFrameNode *dictEntry )
{
    string            tag_to_change = string( "" );
    List<ASTnode*>    *loopMatch;
    int               lCur;
    DataLoopNode      *curLoop;
    LoopNameListNode  *curNames;
    int               nameIdx;
    LoopTableNode     *curValues;
    DataNameNode      *nameNode;
    DataValueNode     *valNode;
    DataItemNode      *newItem;
    DataLoopNode      *peerLoop;
    LoopNameListNode  *peerNames;
    List<ASTnode*>    *peerMatch;
    int               peerMatchCount;
    ASTnode           *curSF;
    ASTnode           *peerSF;
    
    int               dictIdx;
    List<ASTnode*>    *dictLoops;
    LoopTableNode     *dictTbl;


    // example rule:
    //
    // loop_
    //    _key_tag
    //    _key_value
    //    _looped_tag_to_free
    //    _new_free_tag_name

    int key_tag_pos = -1;
    int key_value_pos = -1;
    int looped_tag_to_free_pos = -1;
    int new_free_tag_name_pos = -1;

    dictLoops = dictEntry->searchForType( ASTnode::DATALOOPNODE );

    if( dictLoops->size() >= 1 )
    {
       dictTbl = ( (DataLoopNode*)  ( (*dictLoops)[0]) )->getValsPtr();
       curNames = ( ( (DataLoopNode*) (*dictLoops)[0] )->getNames() )[0];
    }
    else
       return;

    for( int nameIdx = 0 ; nameIdx < curNames->size() ; nameIdx++ )
    {
	if( (*curNames)[nameIdx]->myName()      == string("_key_tag") )
	    key_tag_pos = nameIdx;
	else if( (*curNames)[nameIdx]->myName() == string("_key_value") )
	    key_value_pos = nameIdx;
	else if( (*curNames)[nameIdx]->myName() == string("_looped_tag_to_free") )
	    looped_tag_to_free_pos = nameIdx;
	else if( (*curNames)[nameIdx]->myName() == string("_new_free_tag_name") )
	    new_free_tag_name_pos = nameIdx;
    }
    if( key_tag_pos < 0 ) {
        (*errStream) << "#transform57# ERROR: missing mapping file tag: _key_tag"<<endl;
	return;
    }
    else if( key_value_pos < 0 ) {
        (*errStream) << "#transform57# ERROR: missing mapping file tag: _key_value"<<endl;
	return;
    }
    else if( looped_tag_to_free_pos < 0 ) {
        (*errStream) << "#transform57# ERROR: missing mapping file tag: _looped_tag_to_free"<<endl;
	return;
    }
    else if( new_free_tag_name_pos < 0 ) {
        (*errStream) << "#transform57# ERROR: missing mapping file tag: _new_free_tag_name"<<endl;
	return;
    }


    
    for( dictIdx = 0 ; dictIdx < dictTbl->size() ; dictIdx++ )
    {
	string key_tag            = (*((*dictTbl)[dictIdx]))[key_tag_pos]->myValue();
	string key_value          = (*((*dictTbl)[dictIdx]))[key_value_pos]->myValue();
	string looped_tag_to_free = (*((*dictTbl)[dictIdx]))[looped_tag_to_free_pos]->myValue();
	string new_free_tag_name  = (*((*dictTbl)[dictIdx]))[new_free_tag_name_pos]->myValue();



	// get a match of all the places with that tag/value pair mentioned in the key
	// for this rule:

	loopMatch = inStar->searchByTagValue( key_tag, key_value );
	for( int matchIdx = 0 ; matchIdx < loopMatch->size() ; matchIdx++ )
	{
	    DataValueNode *newValue = NULL;
	    // if it's a free tag match:
	    if( (*loopMatch)[matchIdx]->isOfType(ASTnode::DATAITEMNODE) ) 
	    {
		ASTnode *curNode = (DataItemNode*) (*loopMatch)[matchIdx];
		while( curNode != NULL && ! curNode->isOfType(ASTnode::SAVEFRAMENODE) )
		   curNode = curNode->myParent();
		if( curNode )
		{
		    SaveFrameNode *sfMatch = (SaveFrameNode *)curNode;
		    List<ASTnode*> *valMatches = sfMatch->searchForTypeByTag( ASTnode::DATAITEMNODE, looped_tag_to_free );
		    if( valMatches->size() > 0 )
		    {
			SaveFrameNode *sfMatchPar = (SaveFrameNode*) (sfMatch->myParallelCopy());
			if( sfMatchPar )
			{
		            sfMatchPar->GiveMyDataList()->insert( 
		                sfMatchPar->GiveMyDataList()->end(), 
				new DataItemNode( new_free_tag_name,  
			              ( (DataItemNode*) (*valMatches)[0] ) -> myValue(),
			              ( (DataItemNode*) (*valMatches)[0] ) -> myDelimType() )  );
		        }
			else
			{
                            (*errStream) << "#transform57# ERROR: no peer tag for: "<<looped_tag_to_free<<endl;
			}
		    }
		    delete valMatches;
		}
	    }
	    else
	    {
	        // it's a looped tag match:

		ASTnode *curRow = (DataItemNode*) (*loopMatch)[matchIdx];
		while( curRow != NULL && ! curRow->isOfType(ASTnode::LOOPROWNODE) )
		   curRow = curRow->myParent();
		if( curRow )
		{
		    // need to find the saveframe and loop too:
		    ASTnode *curLoop = curRow;
		    while( curLoop != NULL && ! curLoop->isOfType(ASTnode::DATALOOPNODE) )
		       curLoop = curLoop->myParent();
		    ASTnode *curSF = curLoop;
		    while( curSF != NULL && ! curSF->isOfType(ASTnode::SAVEFRAMENODE) )
		       curSF = curSF->myParent();
		    if( curSF )
		    {
			// Now find the column for the loop that goes with the look-up name:

		        int nest = -1;
		        int col = -1;
		        ((DataLoopNode*)curLoop)->tagPositionDeep( looped_tag_to_free, &nest, &col );
		        if( col >= 0 )
		        {
			    SaveFrameNode *sfMatchPar = (SaveFrameNode*)( ((SaveFrameNode*)curSF)->myParallelCopy() );
			    sfMatchPar->GiveMyDataList()->insert( 
		                    sfMatchPar->GiveMyDataList()->end(), 
			    	    new DataItemNode( new_free_tag_name,  
					    (* ((LoopRowNode*) curRow) )[col]->myValue(), 
					    (* ((LoopRowNode*) curRow) )[col]->myDelimType() ) );
		        }
		    }
		}
	    }
	}
    }
    if( loopMatch != NULL )
        delete loopMatch;
    if( dictLoops != NULL )
        delete dictLoops;
}
