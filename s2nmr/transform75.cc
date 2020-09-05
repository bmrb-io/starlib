///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2008/08/14 18:44:29  madings
*** empty log message ***

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

struct StrCompare
{
    bool operator()(string a, string b) const
    {
	return (a < b);
    }
};

struct IntCompare
{
    bool operator()(int a, int b) const
    {
	return (a < b);
    }
};

struct IntPair
{
    int ordinal;
    int value;
};

struct StringPair
{
    string name;
    string value;
};

void prepopulate_output_counts( 
    map<string,IntPair,StrCompare> &outputCounts );

void save_calculate_data_row_counts(
                     StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF )
{
    map<string,IntPair,StrCompare> outputCounts;
    map<string,IntPair,StrCompare>::iterator countIter;

    map<int,StringPair,IntCompare> outputInMappingOrder;
    map<int,StringPair,IntCompare>::iterator sortedIter;

    int numCounts;


    prepopulate_output_counts( outputCounts );

    // For iterating over the mapping rule:
    List<ASTnode*> *matches = currDicRuleSF->searchForTypeByTag(  ASTnode::DATALOOPNODE, "_input_key_tag"  );
    if( matches == 0 ) {
        cerr << "##transform75## Error: input_key_tag not found in calculate_data_row_counts rule." << endl;
        return;
    }
    DataLoopNode *ruleLoop = (DataLoopNode*) ((*matches)[0]);
    delete matches;
    int outerIdx;
    for( outerIdx = 0 ; outerIdx < ruleLoop->getVals().size() ; ++outerIdx ) {
        numCounts = outputCounts.size()+1; // Start at the end of the list of pre-built ones

        int dummy;
        int outputKeyTagPos   = (ruleLoop->getNames())[0]->tagPosition( "_output_key_tag" );
        int outputKeyValuePos = (ruleLoop->getNames())[0]->tagPosition( "_output_key_value" );
        int outputTypeTagPos  = (ruleLoop->getNames())[0]->tagPosition( "_output_type_tag" );
        int outputCountTagPos = (ruleLoop->getNames())[0]->tagPosition( "_output_count_tag" );
        if( outputKeyTagPos < 0 || outputKeyValuePos < 0 || outputTypeTagPos < 0 ||
	       outputCountTagPos < 0 ) {
	    cerr << "##transform75##: Error: output_key_tag or output_key_value or output_type_tag or output_count_tag not found."<<endl;
	    return;
        }
        string outputKeyTag = (*((ruleLoop->getVals())[outerIdx]))[outputKeyTagPos]->myValue();
        string outputKeyValue = (*((ruleLoop->getVals())[outerIdx]))[outputKeyValuePos]->myValue();
        string outputTypeTag = (*((ruleLoop->getVals())[outerIdx]))[outputTypeTagPos]->myValue();
        string outputCountTag = (*((ruleLoop->getVals())[outerIdx]))[outputCountTagPos]->myValue();


        LoopTableNode *middleLoop= ((ruleLoop->getVals()))[outerIdx]->myLoop();
        if( middleLoop != NULL ) {
            int middleIdx;
	    for( middleIdx = 0 ; middleIdx < middleLoop->size() ; middleIdx++ ) {
	        int inputKeyTagPos          = (ruleLoop->getNames())[1]->tagPosition( "_input_key_tag" );
	        int inputKeyValuePos        = (ruleLoop->getNames())[1]->tagPosition( "_input_key_value" );
	        int labelWhichLoopStringPos = (ruleLoop->getNames())[1]->tagPosition( "_label_whichloop_string" );
	        int inputWhichLoopTagPos    = (ruleLoop->getNames())[1]->tagPosition( "_input_whichloop_tag" );
	        if(     inputKeyTagPos < 0 ||
		        inputKeyValuePos < 0 ||
		        labelWhichLoopStringPos < 0 ||
		        inputWhichLoopTagPos < 0 ) {
		    cerr << "##transform75## Error: input_key_tag, input_key_value, label_whichloop_string, or input_whichloop_tag not found." << endl;
		    return;
	        }
	        string inputKeyTag          = (*(*(middleLoop))[middleIdx])[inputKeyTagPos]->myValue();
	        string inputKeyValue        = (*(*(middleLoop))[middleIdx])[inputKeyValuePos]->myValue();
	        string labelWhichLoopString = (*(*(middleLoop))[middleIdx])[labelWhichLoopStringPos]->myValue();
	        string inputWhichLoopTag    = (*(*(middleLoop))[middleIdx])[inputWhichLoopTagPos]->myValue();


	        vector<string> groupByColumns;

                LoopTableNode *innerLoop= (*(middleLoop))[middleIdx]->myLoop();
		bool innerLoopPresent = false;
		if( innerLoop != NULL &&
		    (*( (*innerLoop)[0] ))[0]->myValue() != "."  &&
		    (*( (*innerLoop)[0] ))[0]->myValue() != "?"  &&
		    (*( (*innerLoop)[0] ))[0]->myValue() != "*"  ) {

		    innerLoopPresent = true;
	        }
	        if( innerLoopPresent ) {
		    int innerIdx;
		    for( innerIdx = 0 ; innerIdx < innerLoop->size() ; innerIdx++ ) {
		        int inputGroupTagsPos = (ruleLoop->getNames())[2]->tagPosition( "_input_group_tags" );
	                if(     inputGroupTagsPos < 0 ) {
			    cerr << "##transform75## Error: input_group_tags not found in mapping rule inner loop."<<endl;
			    return;
		        }
		        string inputGroupsTag = (*(*(innerLoop))[innerIdx])[inputGroupTagsPos]->myValue();
		        groupByColumns.insert( groupByColumns.end(), inputGroupsTag );
		    }
	        }

		List<ASTnode*>* matchSFs;

	        // Now, iterate over the input loops that match this row:
		//
		if( inputKeyTag == "*" && inputKeyValue == "*" ) {
		    matchSFs = AST->searchForType( ASTnode::SAVEFRAMENODE );
		} else if( inputKeyValue == "*" ) {
		    matchSFs = AST->searchForTypeByTag( ASTnode::SAVEFRAMENODE, inputKeyTag );
		} else {
		    matchSFs = AST->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, inputKeyTag, inputKeyValue );
		}
		for( int match_i = 0 ; match_i < matchSFs->size() ; ++match_i ) {
		    SaveFrameNode *matchFrame = (SaveFrameNode*) ( (*(matchSFs))[match_i] );

		    List<ASTnode*> *matchLoops = matchFrame->searchForTypeByTag( ASTnode::DATALOOPNODE, inputWhichLoopTag );
		    if( matchLoops->size() > 0 ) {
			// Should only be one hit - else there's a name clash in the saveframe.
			DataLoopNode *theLoop = (DataLoopNode*) ( (*matchLoops)[0] );

			// If this rule doesn't have a list of input_group_tags, then just count the number
			// of rows and add them to the total - no need to iterate over each one:
			if( groupByColumns.size() == 0 ) {
			    // See if the row in the loop we want is already there.  If so, use that:
			    if( outputCounts.find( labelWhichLoopString ) == outputCounts.end() ){
				// not there yet - initialize:
				struct IntPair insertMe;
				++numCounts;
				insertMe.ordinal = numCounts;
				insertMe.value = theLoop->getValsPtr()->size();

				outputCounts[ labelWhichLoopString ] = insertMe;
			    } else {
				// already there - adding to what's there:
				struct IntPair replaceWithMe;
				replaceWithMe.ordinal = outputCounts[ labelWhichLoopString ].ordinal;
				replaceWithMe.value = outputCounts[ labelWhichLoopString ].value + theLoop->getValsPtr()->size();

				outputCounts[ labelWhichLoopString ] = replaceWithMe;
			    }
			} else  {
			    // otherwise iterate row by row looking at the columns that matter.
			    // first, get the positions of the relevant columns:
			    vector<int> groupByColPos;
			    for( int i=0 ; i < groupByColumns.size() ; ++i ) {
				groupByColPos.insert( groupByColPos.end(), 
						       (theLoop->getNames())[0]->tagPosition( groupByColumns[i] ) );
				if( groupByColPos[i] < 0 ) {
				    cerr<<"##transform-75## Error - no such tag named \""<<groupByColumns[i]<<"\" in the loop."<<endl;
				    return;
				}
			    }
			    LoopTableNode *tbl = theLoop->getValsPtr();
			    for( int row=0 ; row < tbl->size() ; ++row ) {
				string prependName = "";
				// build concatenated name for this row:
				for( int gCol = 0 ; gCol < groupByColPos.size() ; ++gCol ) {
				    prependName += (*(*tbl)[row])[ groupByColPos[gCol] ]->myValue();
				}
				// build full name for this row:
				string fullName = prependName + " " + labelWhichLoopString;
				//
				// See if the row in the loop we want is already there.  If so, use that:
				if( outputCounts.find( fullName ) == outputCounts.end() ){
				    // not there yet - initialize:
				    struct IntPair insertMe;
				    ++numCounts;
				    insertMe.ordinal = numCounts;
				    insertMe.value = 1;
				    outputCounts[ fullName ] = insertMe;
				} else {
				    // already there - adding to what's there:
				    struct IntPair replaceWithMe;
				    replaceWithMe.ordinal = outputCounts[ fullName ].ordinal;
				    replaceWithMe.value = outputCounts[ fullName ].value + 1;

				    outputCounts[ fullName ] = replaceWithMe;
				}

			    }
			}
		    }

		    delete matchLoops;
		}
		delete matchSFs;
		

	    }
        }


	//
	// Results done, now to re-insert them into the file:
	// 
       // Get output saveframe:
       List<ASTnode*> *frames = NMR->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, outputKeyTag, outputKeyValue );

       // Only expect one hit at most:
       if( frames->size() > -0 ) {
	   SaveFrameNode *hitFrame  = (SaveFrameNode*)( (*frames)[0] );
	   // If the loop already exists, get rid of it so we can make it over from scratch with the newly calculated numbers:
	   List<ASTnode*> *loops = hitFrame->searchForTypeByTag( ASTnode::DATALOOPNODE,  outputTypeTag );
	   for( int loop_i = 0 ; loop_i < loops->size() ; ++loop_i ) {
	       delete ( (*loops)[0] );
	   }
	   delete loops;

	   // Make the new loop to insert:
	   LoopNameListNode *outNames = new LoopNameListNode;
	   outNames->insert( outNames->end(), new DataNameNode( outputTypeTag ) );
	   outNames->insert( outNames->end(), new DataNameNode( outputCountTag ) );

	   DataLoopNode *outLoop = new DataLoopNode( "tabulate" );
	   outLoop->getNames().insert( outLoop->getNames().begin(), outNames );

           // Have to iterate in the sorting order encountered (not the one used during the map:)
	   for( countIter = outputCounts.begin() ; countIter != outputCounts.end() ; ++countIter ) {
	       struct StringPair val;
	       val.name = (*countIter).first;
	       char sInt[30];
	       sprintf( sInt, "%d", (*countIter).second.value  );
	       val.value = string(sInt);
	       outputInMappingOrder[ (*countIter).second.ordinal ] = val;
	   }
	   
	   for( sortedIter = outputInMappingOrder.begin() ; sortedIter != outputInMappingOrder.end() ; ++sortedIter ) {
	       LoopRowNode *loopRow = new LoopRowNode(true);
	       string label = (*sortedIter).second.name;
	       string count = (*sortedIter).second.value;
	       if( atoi(count.c_str()) > 0 ) {
		   loopRow->insert( loopRow->end(), new DataValueNode( label, DataValueNode::DOUBLE ) ); // key of the map (data type)
		   loopRow->insert( loopRow->end(), new DataValueNode( count, DataValueNode::NON ) ); // value of the map (data row count)
		   outLoop->getVals().insert( outLoop->getVals().end(), loopRow );
	       }
	   }
	   hitFrame->GiveMyDataList()->insert( hitFrame->GiveMyDataList()->end(), outLoop );
       }

       delete frames;
    }

    return;
}

void prepopulate_output_counts( 
    map<string,IntPair,StrCompare> &outputCounts )
{
    char canonicalList[][80] = {
	"1H chemical shifts",
	"13C chemical shifts",
	"15N chemical shifts",
	"17O chemical shifts",
	"31P chemical shifts",
	"33S chemical shifts",
	"19F chemical shifts",
	"112Cd chemical shifts",
	"43Ca chemical shifts",
	"coupling constants",
	"chemical shift isotope effects",
	"molecule interaction chemical shift values",
	"T1 relaxation values",
	"T1rho relaxation values",
	"T2 relaxation values",
	"dipole-dipole relaxation values",
	"cross correlation relaxation values",
	"chemical shift anisotropy values",
	"chemical shift anisotropy tensor values",
	"quadrupolar couplings",
	"theoretical chemical shifts",
	"chemical shift tensors",
	"residual dipolar couplings",
	"dipolar coupling values",
	"dipolar coupling tensor values",
	"heteronuclear NOE values",
	"homonuclear NOE values",
	"order parameters",
	"spectral density values",
	"H exchange rates",
	"H exchange protection factors",
	"pKa values",
	"pH NMR parameter values",
	"D/H fractionation factors",
	"bond orientation values",
	"deduced secondary structure values",
	"deduced hydrogen bonds",
	"Distance constraints",
	"ambiguous distance constraints",
	"hydrogen bond distance constraints",
	"torsion angle constraints",
	"chemical shift constraints",
	"residual dipolar coupling constraints",
	"symmetry constraints",
        "__END__"
    };
    int i;
    struct IntPair startVal;
    startVal.value = 0;
    startVal.ordinal = 0;
    for( i = 0 ; strcmp(canonicalList[i], "__END__") != 0 ; ++i ) {
	startVal.ordinal = i + 1;
	outputCounts[ canonicalList[i] ] = startVal;
    }
}
