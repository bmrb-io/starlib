///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.2  2006/04/28 17:18:54  dtolmie
*** empty log message ***

Revision 1.1  2005/11/07 20:21:20  madings
*** empty log message ***

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

static char map_table[][3][15] = 
{
    //        Residue       v3         v2.1
    //         label       name        name
    // --------------------------------------
    {            "",       "",        ""   },  // Put these empty string
                                               // rows on the boundries
                                               // between the sections of
                                               // the table, before and after.
    {            "ALA",    "HB1",     "HB" },
    {            "ALA",    "HB2",     "HB" },
    {            "ALA",    "HB3",     "HB" },
    {            "",       "",        ""   },
    {            "ILE",    "HG21",    "HG2" },
    {            "ILE",    "HG22",    "HG2" },
    {            "ILE",    "HG23",    "HG2" },
    {            "",       "",        ""   },
    {            "ILE",    "HD11",    "HD1" },
    {            "ILE",    "HD12",    "HD1" },
    {            "ILE",    "HD13",    "HD1" },
    {            "",       "",        ""   },
    {            "LEU",    "HD11",    "HD1" },
    {            "LEU",    "HD12",    "HD1" },
    {            "LEU",    "HD13",    "HD1" },
    {            "",       "",        ""   },
    {            "LEU",    "HD21",    "HD2" },
    {            "LEU",    "HD22",    "HD2" },
    {            "LEU",    "HD23",    "HD2" },
    {            "",       "",        ""   },
    {            "LYS",    "HZ1",     "HZ" },
    {            "LYS",    "HZ2",     "HZ" },
    {            "LYS",    "HZ3",     "HZ" },
    {            "",       "",        ""   },
    {            "MET",    "HE1",     "HE" },
    {            "MET",    "HE2",     "HE" },
    {            "MET",    "HE3",     "HE" },
    {            "",       "",        ""   },
    {            "T",    "H71",    "H7" },
    {            "T",    "H72",    "H7" },
    {            "T",    "H73",    "H7" },
    {            "",       "",        ""   },
    {            "THR",    "HG21",    "HG2" },
    {            "THR",    "HG22",    "HG2" },
    {            "THR",    "HG23",    "HG2" },
    {            "",       "",        ""   },
    {            "VAL",    "HG11",    "HG1" },
    {            "VAL",    "HG12",    "HG1" },
    {            "VAL",    "HG13",    "HG1" },
    {            "",       "",        ""   },
    {            "VAL",    "HG21",    "HG2" },
    {            "VAL",    "HG22",    "HG2" },
    {            "VAL",    "HG23",    "HG2" },
    {            "",       "",        ""   },
    {         "end-mark", "end-mark",  "end-mark"   } // end the list with this sentinel
};

static void process_one_collection_2_to_3(
		         DataLoopNode *matchedLoop,
			 string       row_ID_tagname,
			 string       entity_assembly_ID_tagname,
			 string       comp_index_tagname,
			 string       seq_ID_tagname,
			 string       comp_ID_tagname,
			 string       atom_ID_tagname,
			 string       which_list_ID_tagname,
			 LoopTableNode *innerLoop );

void save_expand_2_to_3( StarFileNode *input, 
                           StarFileNode *output, 
                           BlockNode    *,
			   SaveFrameNode *currDicRuleSF)
{
    string currDicSaveFrameName = string("save_expand_2_to_3");

    if(!currDicRuleSF)//Return if the target saveframe is not found
       return; 

    int i,j,k;
    List<ASTnode*>  *matches = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, "_sf_key_tag" );
    if( (*matches).size() <= 0 )
    {  (*errStream)<<"#transform-71# ERROR: loop containing '_sf_key_tag' not found in mapping file"<<endl;
      delete matches;
      return;
    }
    DataLoopNode  *dictLoop = (DataLoopNode*) (*matches)[0];
    LoopNameListNode *dictLoopNames = dictLoop->getNames()[0];
    delete matches;

    LoopTableNode *dictTbl = dictLoop->getValsPtr();

    int dictRowNum;


    for( dictRowNum = 0 ; dictRowNum < dictTbl->size() ; dictRowNum++ )
    {
        LoopRowNode *dictRow = (*dictTbl)[dictRowNum];

	// Read one dictionary row's values into string variables:

        string sf_key_tag = string("");
	string sf_key_value = string("");
	string row_ID_tagname = string("");
	string entity_assembly_ID_tagname = string("");
	string comp_index_tagname = string("");
	string seq_ID_tagname = string("");
	string comp_ID_tagname = string("");
	string atom_ID_tagname = string("");
	string which_list_ID_tagname = string("");
        for( i = 0 ; i < dictLoopNames->size() ; i++ )
        {
	    string columnTitle = (*dictLoopNames)[i]->myName();

	    if(      strcmp(columnTitle.c_str(), "_sf_key_tag") == 0 )
	    {  sf_key_tag = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_sf_key_value") == 0 )
	    {  sf_key_value = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_row_ID_tagname") == 0 )
	    {  row_ID_tagname = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_entity_assembly_ID_tagname") == 0 )
	    {  entity_assembly_ID_tagname = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_comp_index_tagname") == 0 )
	    {  comp_index_tagname = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_seq_ID_tagname") == 0 )
	    {  seq_ID_tagname = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_comp_ID_tagname") == 0 )
	    {  comp_ID_tagname = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_atom_ID_tagname") == 0 )
	    {  atom_ID_tagname = (*dictRow)[i]->myValue();
	    }
	    else if( strcmp(columnTitle.c_str(), "_which_list_ID_tagname") == 0 )
	    {  which_list_ID_tagname = (*dictRow)[i]->myValue();
	    }
        }

	// Done reading this dictionary row now process it.

	// First, have to find the right saveframes to operate on, using the sf key tag and value.
	List<ASTnode*> *matchingSFs =
	    output->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, sf_key_tag, sf_key_value );
	int matchingSFNum;
	for( matchingSFNum = 0 ; matchingSFNum < matchingSFs->size() ; matchingSFNum++ )
	{
	    SaveFrameNode *matchedSF = (SaveFrameNode*)( (*matchingSFs)[matchingSFNum] );

	    // Now, operating on this saveframe, find the right loop inside the saveframe:
	    List<ASTnode*> *matchingLoops = 
	        matchedSF->searchForTypeByTag( ASTnode::DATALOOPNODE, row_ID_tagname );
	    int matchingLoopNum;
	    for( matchingLoopNum = 0 ; matchingLoopNum < matchingLoops->size(); matchingLoopNum++ )
	    {
	        DataLoopNode *matchedLoop = (DataLoopNode*)( (*matchingLoops)[matchingLoopNum] );

	        process_one_collection_2_to_3(
		         matchedLoop,
			 row_ID_tagname,
			 entity_assembly_ID_tagname,
			 comp_index_tagname,
			 seq_ID_tagname,
			 comp_ID_tagname,
			 atom_ID_tagname,
			 which_list_ID_tagname,
			 dictRow->myLoop() );
	    }
	    delete matchingLoops;
	}
	delete matchingSFs;
    }
}

struct stringLessThan
{
  bool operator()(string a, string b) const
  {
    return  a<b;
  }
};

static void process_one_collection_2_to_3(
		         DataLoopNode *matchedLoop,
			 string       row_ID_tagname,
			 string       entity_assembly_ID_tagname,
			 string       comp_index_tagname,
			 string       seq_ID_tagname,
			 string       comp_ID_tagname,
			 string       atom_ID_tagname,
			 string       which_list_ID_tagname,
			 LoopTableNode *innerDictLoop )
{
    LoopNameListNode *loopNames = matchedLoop->getNames()[0];
    LoopTableNode *loopTbl = matchedLoop->getValsPtr();


    // First, get the column indeces of the relevant tags:
    int row_ID_col = -1;
    int entity_assembly_ID_col = -1;
    int comp_index_col = -1;
    int seq_ID_col = -1;
    int comp_ID_col = -1;
    int atom_ID_col = -1;
    int which_list_ID_col = -1;

    int i;
    for( i = 0 ; i < loopNames->size() ; i++ )
    {
	if(      (*loopNames)[i]->myName() == row_ID_tagname )
	{   row_ID_col = i;
	}
	else if( (*loopNames)[i]->myName() == entity_assembly_ID_tagname )
	{   entity_assembly_ID_col = i;
	}
	else if( (*loopNames)[i]->myName() == comp_index_tagname )
	{   comp_index_col = i;
	}
	else if( (*loopNames)[i]->myName() == seq_ID_tagname )
	{   seq_ID_col = i;
	}
	else if( (*loopNames)[i]->myName() == comp_ID_tagname )
	{   comp_ID_col = i;
	}
	else if( (*loopNames)[i]->myName() == atom_ID_tagname )
	{   atom_ID_col = i;
	}
	else if( (*loopNames)[i]->myName() == which_list_ID_tagname )
	{   which_list_ID_col = i;
	}
    }
    
    if( row_ID_col < 0 )
    {  (*errStream)<<"#transform71# Error: no Row ID column found in the loop."<<endl;
       return;
    }
    else if( entity_assembly_ID_col < 0 )
    {  (*errStream)<<"#transform71# Error: no entity assembly ID column found in the loop."<<endl;
       return;
    }
    else if( comp_index_col < 0 )
    {  (*errStream)<<"#transform71# Error: no comp index column found in the loop."<<endl;
       return;
    }
    else if( seq_ID_col < 0 )
    {  (*errStream)<<"#transform71# Error: no seq ID column found in the loop."<<endl;
       return;
    }
    else if( comp_ID_col < 0 )
    {  (*errStream)<<"#transform71# Error: no comp ID column found in the loop."<<endl;
       return;
    }
    else if( atom_ID_col < 0 )
    {  (*errStream)<<"#transform71# Error: no atom ID column found in the loop."<<endl;
       return;
    }
    else if( which_list_ID_col < 0 )
    {  (*errStream)<<"#transform71# Error: no "<<which_list_ID_tagname<<" column found in the loop."<<endl;
       return;
    }

    // Second, do the actual work.

    // Algorithm:  For each row of the loop, see if the tuple (comp_ID, Atom_ID) matches
    // any row of the map_table.
    //    If not, skip the row and do nothing.
    //    If there is a match, then insert rows at this location in
    //    the loop to put out the expanded version of the tuple that
    //    nmrstr version >=3 files are supposed to have.
    // When all done, go back and renumber the row id column because
    // we've inserted values and messed up the count.

 
    int row;

    // idRemapper keeps track of remapped IDs that changed due to rows
    // being added:
    map<string,string,stringLessThan> idRemapper;
    map<string,string,stringLessThan>::iterator idRemapperIter;

    string which_list_ID;

    // It's important that the check against the loop's size
    // be dynamicly recalculated every time.  Don't try to speed it
    // up by pulling the loopTbl->size() out of the for-loop and
    // precalculating it - the loop size will change during the 
    // processing of this loop so doing that would make it not work right:
    for( row = 0 ; row < loopTbl->size() ; row++ )
    {
	LoopRowNode *rowNode = (*loopTbl)[row];
	string entity_assembly_ID = (*rowNode)[entity_assembly_ID_col]->myValue();
	string comp_index = (*rowNode)[comp_index_col]->myValue();
	string comp_ID = (*rowNode)[comp_ID_col]->myValue();
	string seq_ID = (*rowNode)[seq_ID_col]->myValue();
	string atom_ID = (*rowNode)[atom_ID_col]->myValue();
	string row_ID = (*rowNode)[row_ID_col]->myValue();
	which_list_ID = (*rowNode)[which_list_ID_col]->myValue();

	char newRowNum[20];
	sprintf( newRowNum, "%d", row+1 );

	string atomIDTo;
	vector<string> atomIDsFrom;

	atomIDsFrom.clear();

	int mapRow;
	bool mapMatched = false;


	// This is a dumb brute force search, but the table is very small
	// such that it probably doesn't help to use a more complex search:
	for( mapRow = 0 ; strcmp( map_table[mapRow][0], "end-mark" ) != 0 ; mapRow++ )
	{
	    // If this isn't a match, keep going:
	    // ----------------------------------
	    if( (strcmp( map_table[mapRow][2], atom_ID.c_str() ) != 0 ) ||
	        (strcmp( map_table[mapRow][0], comp_ID.c_str() ) != 0 ) )
	    {
		continue;
	    }
	    else // it's a match
	    {
		mapMatched = true;

		break;
	    }
	}
	if( mapMatched )
	{
	    int rowToReplace = row;  // this row will get deleted when we're done
	                             // inserting.
	    bool done=false;
	    bool didSomething = false;
	    do
	    {
	        if( (strcmp( map_table[mapRow][2], atom_ID.c_str() ) != 0 ) ||
	            (strcmp( map_table[mapRow][0], comp_ID.c_str() ) != 0 ) ) 
		{
		    done = true;
		}
		else
		{   // Clone the existing row, and change the relevant values in it,
		    // then insert it:
		    LoopRowNode *newRow = new LoopRowNode( *rowNode );
		    (*newRow)[comp_ID_col]->setValue( map_table[mapRow][0] );
		    (*newRow)[atom_ID_col]->setValue( map_table[mapRow][1] );
		    newRowNum[20];
		    sprintf(newRowNum, "%d", row+1 );
		    string oldVal =  (*newRow)[row_ID_col]->myValue();
		    if( idRemapper[oldVal].length() > 0 ) {
	                idRemapper[ oldVal ] = idRemapper[oldVal] + ",";
		    }
	            idRemapper[ oldVal ] = idRemapper[oldVal] + string(newRowNum);
	            (*newRow)[row_ID_col]->setValue( string( newRowNum ) );
		    loopTbl->insert( loopTbl->begin() + row + 1, newRow );
		    didSomething = true;
		    ++row;
		}
		if( strcmp( map_table[mapRow][0], "end-mark" )==0 || done ) 
		{
		    done = true;
		}
		++mapRow;
	    } while( ! done );

	    // If rows were added, then get rid of the original row, and adjust the
	    // count accordingly;
	    if( didSomething )
	    {
	        loopTbl->erase( loopTbl->begin() + rowToReplace );
	        --row;
	    }

	} else 
	{
	    newRowNum[20];
	    sprintf(newRowNum, "%d", row+1 );
	    string oldVal =  (*rowNode)[row_ID_col]->myValue();
	    if( idRemapper[oldVal].length() > 0 ) {
		idRemapper[ oldVal ] = idRemapper[oldVal] + ",";
	    }
	    idRemapper[ oldVal ] = idRemapper[oldVal] + string(newRowNum);
	    (*rowNode)[row_ID_col]->setValue( string( newRowNum ) );
	}

        // to help keep memory efficient:
	ValCache::flushValCache();
    }

    // General algorithm:
    // For each tag in the innerLoop (_child_link_tags):
    //    search for the tag
    //    if it's a free dataitemnode tag.
    //       its_new_value = idRemapper[its_old_value];
    //    else it's in a loop.
    //       walk the parent list to the the loopnode,
    //       Get the column position of the tag.
    //       for each row of the loop.
    //          link_column's_new_value = idRemapper[its_old_value];
    //       end-for
    //    end-if-else
    //  end-For

    // General algorithm:
    // For each tag in the innerLoop (_child_link_tags):
    int inner_i;
    for( inner_i = 0 ; inner_i < innerDictLoop->size() ; inner_i++ ) {
	// Get the file node that the output loop is inside of.
	ASTnode *par;
	for( par = matchedLoop ; par != NULL && ! par->isOfType(ASTnode::STARFILENODE) ; par = par->myParent() ) {
	    /*void body*/
	}
	if( par == NULL )
	    continue;

        string childWhichLoopTag = (*( (*innerDictLoop)[inner_i] ))[0]->myValue();
        string childTagToChange = (*( (*innerDictLoop)[inner_i] ))[1]->myValue();


	//    search for the relevant loop
	List<ASTnode*> *remapLoops = par->searchForTypeByTagValue( ASTnode::DATALOOPNODE, childWhichLoopTag, which_list_ID  );

	int search_i;
	DataLoopNode *prevRemapLoop = NULL;
	for( search_i = 0 ; search_i < remapLoops->size() ; search_i++ )
	{
	    DataLoopNode *remapLoop = (DataLoopNode*) ( (*remapLoops)[search_i] );
	    // searchForTypeByTag sometimes gives the same loop over and over.  This is
	    // a bug in the library, but I don't have time to fix it there right now,
	    // so I'm making this workaround:
	    if( remapLoop == prevRemapLoop )
	       continue;
	    prevRemapLoop = remapLoop;
	    //       Get the column position of the tag.
	    int nest,col;
	    remapLoop->tagPositionDeep( childTagToChange, &nest, &col );
	    //       for each row of the loop.
	    LoopTableNode *remapLoopVals = remapLoop->getValsPtr();
	    int row_i;
	    for( row_i = 0 ; row_i < remapLoopVals->size() ; row_i++ )
	    {
	        //   link_column's_new_value = idRemapper[its_old_value];
		// This is a comma separated list, so I have to walk through it and build it up:
		string oldVal = (*((*remapLoopVals)[row_i]))[col]->myValue();
		string newVal = string("");
		string tmpVal = string("");
		while( oldVal.length() > 0 )
		{
		   string::size_type commaPos = oldVal.find( ',' );
		   if( commaPos == string::npos )
		   {
		       commaPos = oldVal.length();
		   }
		   tmpVal = oldVal.substr( 0, commaPos );
		   oldVal.erase( 0, commaPos+1 );
		   if( newVal.length() > 0 )
		   {   newVal += ",";
		   }
		   newVal += idRemapper[ tmpVal ];
		}
		(*((*remapLoopVals)[row_i]))[col]->setValue( newVal );
	    }
	}
    }


    idRemapper.clear();

}







