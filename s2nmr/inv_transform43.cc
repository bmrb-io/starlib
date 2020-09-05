///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
   $Log: not supported by cvs2svn $
   Revision 1.1  2005/03/23 22:38:21  madings
   *** empty log message ***

 */
#endif
///////////////////////////////////////////////////////////////

#include "transforms.h"

static void do_one_iteration(
	    string    input_key_tag,
	    string    input_sf_name,
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_sf_key_tag_pos,
	    DataValueNode::ValType    output_sf_key_tag_type,
	    string    output_sf_key_value_pos,
	    string    output_sf_base_name,
	    string    output_sf_name_tag,
	    string    copy_or_move_flag,
	    string    output_sf_label_back_tag,
	    string    output_sf_label_fore_tag,
	    LoopTableNode   *mapInnerLoop,
	    LoopTableNode   *outInnerLoop );

static void do_all_iterations(
	    int    input_key_tag_pos,
	    int    input_sf_name_pos,
	    int    input_sf_key_tag_pos,
	    int    input_sf_key_value_pos,
	    int    output_sf_key_tag_pos,
	    int    output_sf_key_value_pos,
	    int    output_sf_base_name_pos,
	    int    output_sf_name_tag_pos,
	    int    copy_or_move_flag_pos,
	    int    output_sf_label_back_tag,
	    int    output_sf_label_fore_tag,
	    LoopTableNode  *mapLoopTbl,
	    LoopTableNode  *outLoopTbl );

void save_invert_generic_value_mapper_A(
	                 StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *bNode,
			 SaveFrameNode *currDicRuleSF  )
{
    List<ASTnode*>        *mapLoopMatch;
    SaveFrameNode         *reverseMeSF;
    DataLoopNameListNode  *mapNames;
    LoopTableNode         *mapLoopTbl;
    LoopTableNode         *outLoopTbl;
    int                   i;
    int                   input_key_tag_pos = -1;
    int                   input_sf_name_pos = -1;
    int                   input_sf_key_tag_pos = -1;
    int                   input_sf_key_value_pos = -1;
    int                   output_sf_key_tag_pos = -1;
    int                   output_sf_key_value_pos = -1;
    int                   output_sf_base_name_pos = -1;
    int                   output_sf_name_tag_pos = -1;
    int                   copy_or_move_flag_pos = -1;
    int                   output_sf_label_back_tag = -1;
    int                   output_sf_label_fore_tag = -1;

    List<ASTnode*>        *matches;
    matches = currDicRuleSF->searchByTag( "_invert_this_sf" );
    string                otherSfName = string("");
    if( matches->size() > 0 )
    {   otherSfName = (  (DataItemNode*)( (*matches)[0] )   )->myValue();
    }
    delete matches;
    otherSfName = string("save_") + otherSfName;
    matches = bNode->searchByTag( otherSfName );
    if( matches->size() > 0 )
    {   reverseMeSF =  (SaveFrameNode*)( (*matches)[0] );
    }
    delete matches;
    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = reverseMeSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_input_key_tag" ) );
    if( mapLoopMatch->size() <= 0 )
    {
	(*errStream) << "#inv_transform-43# ERROR: no loop tag called '_input_key_tag' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_input_key_tag" ) )
	    input_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_name" ) )
	    input_sf_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_key_tag" ) )
	    input_sf_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_key_value" ) )
	    input_sf_key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_key_tag" ) )
	    output_sf_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_key_value" ) )
	    output_sf_key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_base_name" ) )
	    output_sf_base_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_name_tag" ) )
	    output_sf_name_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_copy_or_move_flag" ) )
	    copy_or_move_flag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_label_back_tag" ) )
	    output_sf_label_back_tag = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_label_fore_tag" ) )
	    output_sf_label_fore_tag = i;
}
    if( input_key_tag_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag '_input_key_tag'." << endl;
	return; /* do nothing */
    }
    if( input_sf_name_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag '_input_sf_name'." << endl;
	return; /* do nothing */
    }
    if( input_sf_key_tag_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag 'input_sf_key_tag'." << endl;
	return; /* do nothing */
    }
    if( input_sf_key_value_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag '_input_sf_key_value'." << endl;
	return; /* do nothing */
    }
    if( output_sf_key_tag_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag '_output_sf_catehgory'." << endl;
	return; /* do nothing */
    }
    if( output_sf_base_name_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag '_output_sf_base_name'." << endl;
	return; /* do nothing */
    }
    if( output_sf_name_tag_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag '_output_sf_name_tag'." << endl;
	return; /* do nothing */
    }
    if( copy_or_move_flag_pos < 0  )
    {   (*errStream) << "#inv_transform-43# ERROR: input mapping file is missing tag 'copy_or_move_flag'." << endl;
	return; /* do nothing */
    }


    DataLoopNode *outLoopNode = new DataLoopNode( "tabulate" );
    LoopNameListNode *outOuterNames = new LoopNameListNode();
    LoopNameListNode *outInnerNames = new LoopNameListNode();

    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_input_key_tag" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_input_sf_name" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_input_sf_key_tag" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_input_sf_key_value" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_output_sf_key_tag" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_output_sf_key_value" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_output_sf_base_name" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_output_sf_name_tag" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_copy_or_move_flag" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_output_sf_label_back_tag" ) );
    outOuterNames->insert( outOuterNames->end(),
                           new DataNameNode( "_output_sf_label_fore_tag" ) );

    outInnerNames->insert( outInnerNames->end(),
                           new DataNameNode( "_input_tag" ) );
    outInnerNames->insert( outInnerNames->end(),
                           new DataNameNode( "_delimiter_filter" ) );
    outInnerNames->insert( outInnerNames->end(),
                           new DataNameNode( "_free_or_loop_flag" ) );
    outInnerNames->insert( outInnerNames->end(),
                           new DataNameNode( "_new_tag_name" ) );

    ( outLoopNode->getNames() ).insert( ( outLoopNode->getNames() ).end(),
                                       outOuterNames );
    ( outLoopNode->getNames() ).insert( ( outLoopNode->getNames() ).end(),
                                       outInnerNames );

    outLoopTbl = outLoopNode->getValsPtr();

    DataLoopNode *outSfNameNode = new DataLoopNode( "tabulate" );
    LoopNameListNode *names = new LoopNameListNode();

    names->insert( names->end(),
                           new DataNameNode( "_key_tag" ) );
    names->insert( names->end(),
                           new DataNameNode( "_key_value" ) );
    names->insert( names->end(),
                           new DataNameNode( "_renaming_tag" ) );

    ( outSfNameNode->getNames() ).insert( ( outSfNameNode->getNames() ).end(),
                                       names );



    SaveFrameNode *newSaveFrameMapRule = new SaveFrameNode( currDicRuleSF->myName() + string("(INVERTED)") );
    newSaveFrameMapRule->GiveMyDataList()->insert(
            newSaveFrameMapRule->GiveMyDataList()->end(),
	    new DataItemNode( string("_Saveframe_category"),
	                      string("generic_value_mapper_A"),
			      DataValueNode::NON  ) );
    newSaveFrameMapRule->GiveMyDataList()->insert(
            newSaveFrameMapRule->GiveMyDataList()->end(), outLoopNode );

    do_all_iterations(
	    input_key_tag_pos,
	    input_sf_name_pos,
	    input_sf_key_tag_pos,
	    input_sf_key_value_pos,
	    output_sf_key_tag_pos,
	    output_sf_key_value_pos,
	    output_sf_base_name_pos,
	    output_sf_name_tag_pos,
	    copy_or_move_flag_pos,
	    output_sf_label_back_tag,
	    output_sf_label_fore_tag,
	    mapLoopTbl,
	    outLoopTbl
	    );

    // (*os)<<"#### [ BEGIN TEST - OUTPUT OF THE INVERTED RULE #####"<<endl;
    // newSaveFrameMapRule->Unparse(0);
    // (*os)<<"#### ] END TEST - OUTPUT OF THE INVERTED RULE #####"<<endl;

    save_generic_value_mapper_A( inStar, outStar, bNode, newSaveFrameMapRule  );


    delete newSaveFrameMapRule;
    return;
}

// Loop over all the iterations of the mapping file, doing all
// the rules found there:
static void do_all_iterations(
	    int    input_key_tag_pos,
	    int    input_sf_name_pos,
	    int    input_sf_key_tag_pos,
	    int    input_sf_key_value_pos,
	    int    output_sf_key_tag_pos,
	    int    output_sf_key_value_pos,
	    int    output_sf_base_name_pos,
	    int    output_sf_name_tag_pos,
	    int    copy_or_move_flag_pos,
	    int    output_sf_label_back_tag,
	    int    output_sf_label_fore_tag,
	    LoopTableNode *mapLoopTbl,
	    LoopTableNode *outLoopTbl )
{
    int i;


    for( i = 0 ; i < mapLoopTbl->size() ; i++ )
    {
        LoopRowNode *outLoopRow = new LoopRowNode(false);
	DataValueNode *newVal = NULL;
	LoopTableNode *mapInner = (*mapLoopTbl)[i]->myLoop();
	List<ASTnode*> *matches;
	int            j;
	bool           free = true;

	// _input_key_tag  = if saveframe_category, replace with output_sf_key_tag,
	//                 = else if anything else, find it's match in the map loop's
	//                 inner loop and look for the nmrstar 3.0 version of it.
	newVal = (*(*mapLoopTbl)[i])[output_sf_key_tag_pos];
	if( (*(*mapLoopTbl)[i])[input_key_tag_pos]->myValue() != string("_Saveframe_category") )
	{   free = false;
	    if( mapInner != NULL )
	    {   for( j = 0 ; j < mapInner->size() ; j++ )
	        {   if( (*(*mapInner)[j])[0]->myValue() == (*(*mapLoopTbl)[i])[input_key_tag_pos]->myValue() )
		    {   newVal = (*(*mapInner)[j])[3];
		        break;
		    }
		}
	    }
	}
	outLoopRow->insert( outLoopRow->end(), *newVal );

	// _input_sf_name (always *)
	outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode( string("*"), DataValueNode::NON ) );

        // _input_sf_key_tag (swap with output_sf_key_tag)
	outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode(  (*(*mapLoopTbl)[i])[output_sf_key_tag_pos]->myValue(),
						(*(*mapLoopTbl)[i])[output_sf_key_tag_pos]->myDelimType() ) );
        // _input_sf_key_value (swap with output_sf_key_value)
	outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode(  (*(*mapLoopTbl)[i])[output_sf_key_value_pos]->myValue(),
						(*(*mapLoopTbl)[i])[output_sf_key_value_pos]->myDelimType() ) );
        // _output_sf_key_tag (swap with input_sf_key_tag)
	outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode(  (*(*mapLoopTbl)[i])[input_sf_key_tag_pos]->myValue(),
						(*(*mapLoopTbl)[i])[input_sf_key_tag_pos]->myDelimType() ) );
        // _output_sf_key_value (swap with input_sf_key_value)
	outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode(  (*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myValue(),
						(*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myDelimType() ) );

	// _output_sf_base_name (if it was NEW_SAME, then keep it that way, else
	// if it's free tags, copy from input_sf_key_value, else use a dot (.).
	// --------------------------------------------------------------
	if( (*(*mapLoopTbl)[i])[output_sf_base_name_pos]->myValue() == string("NEW_SAME") ) {
	  outLoopRow->insert( outLoopRow->end(), new DataValueNode( string("NEW_SAME"), DataValueNode::NON ) );
	}
	else if( (*(*mapLoopTbl)[i])[output_sf_base_name_pos]->myValue() != string(".") ) {
	    outLoopRow->insert( outLoopRow->end(), 
				new DataValueNode(  (*(*mapLoopTbl)[i])[output_sf_base_name_pos]->myValue(),
						    (*(*mapLoopTbl)[i])[output_sf_base_name_pos]->myDelimType() ) );
	}
	else if( free )
	    outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode(  (*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myValue(),
						(*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myDelimType() ) );
	else
	    outLoopRow->insert( outLoopRow->end(), new DataValueNode( string("."), DataValueNode::NON ) );

	// _output_sf_name_tag (pass through as is)
	outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode(  (*(*mapLoopTbl)[i])[output_sf_name_tag_pos]->myValue(),
						(*(*mapLoopTbl)[i])[output_sf_name_tag_pos]->myDelimType() ) );
	// _copy_or_move_tag (pass through as is)
	outLoopRow->insert( outLoopRow->end(), 
	                    new DataValueNode(  (*(*mapLoopTbl)[i])[copy_or_move_flag_pos]->myValue(),
						(*(*mapLoopTbl)[i])[copy_or_move_flag_pos]->myDelimType() ) );
	// _output_sf_label_back_tag (always dot)
	outLoopRow->insert( outLoopRow->end(), new DataValueNode( string("."), DataValueNode::NON ) );
	// _output_sf_label_fore_tag (always dot)
	outLoopRow->insert( outLoopRow->end(), new DataValueNode( string("."), DataValueNode::NON ) );

	LoopTableNode *outInnerLoop = new LoopTableNode(true);

	do_one_iteration( 
	    (*(*mapLoopTbl)[i])[input_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_name_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_key_value_pos]->myDelimType(),
	    (*(*mapLoopTbl)[i])[output_sf_key_value_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_base_name_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_name_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[copy_or_move_flag_pos]->myValue(),
	    ( output_sf_label_back_tag >= 0 ) ?
		(*(*mapLoopTbl)[i])[output_sf_label_back_tag]->myValue() :
		string(""),
	    ( output_sf_label_fore_tag >= 0 ) ?
		(*(*mapLoopTbl)[i])[output_sf_label_fore_tag]->myValue() :
		string(""),
	    mapInner,
	    outInnerLoop
	    );
	// only if there was a real mapping made, insert the rule:
	if( outInnerLoop->size() > 0 ) {
	    outLoopRow->setMyLoop( outInnerLoop );
	    outLoopTbl->insert( outLoopTbl->end(), outLoopRow );
	}
	else { // nothing
	}
	ValCache::flushValCache();

    }
}

// Do one iteration of the mapping file
static void do_one_iteration(
	    string    input_key_tag,
	    string    input_sf_name,
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_sf_key_tag,
	    DataValueNode::ValType output_sf_key_tag_type,
	    string    output_sf_key_value,
	    string    output_sf_base_name,
	    string    output_sf_name_tag,
	    string    copy_or_move_flag,
	    string    output_sf_label_back_tag,
	    string    output_sf_label_fore_tag,
	    LoopTableNode   *mapInnerLoop,
	    LoopTableNode   *outInnerLoop )
{
    int                       i;
    int                       j;
    int                       k;
    int                       m;
    int                       sf_name_tag_pos;
    int                       sf_label_fore_pos;
    bool                      freeFlag; //to tell if input_key_tag not in loop


    for( i = 0 ; i < mapInnerLoop->size() ; i++ )
    {
        LoopRowNode *row = new LoopRowNode(true);
	// position 0 = old position 3
	row->insert( row->end(), new DataValueNode(
	                               (*(*mapInnerLoop)[i])[3]->myValue(),
	                               (*(*mapInnerLoop)[i])[3]->myDelimType()
				       )
		   );
	// position 1 = old position 1 (pass through unchanged)
	row->insert( row->end(), new DataValueNode(
	                               (*(*mapInnerLoop)[i])[1]->myValue(),
	                               (*(*mapInnerLoop)[i])[1]->myDelimType()
				       )
		   );
	// position 2 = old position 2 (pass through unchanged)
	row->insert( row->end(), new DataValueNode(
	                               (*(*mapInnerLoop)[i])[2]->myValue(),
	                               (*(*mapInnerLoop)[i])[2]->myDelimType()
				       )
		   );
	// position 3 = old position 0
	row->insert( row->end(), new DataValueNode(
	                               (*(*mapInnerLoop)[i])[0]->myValue(),
	                               (*(*mapInnerLoop)[i])[0]->myDelimType()
				       )
		   );
         if( (*row)[3]->myValue() != string("?") &&
             (*row)[3]->myValue() != string(".") ) {

	    outInnerLoop->insert( outInnerLoop->end(), row );
	 }
	 else { // nothing
	 }
    }
    
}
