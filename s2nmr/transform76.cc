
///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2008/08/14 18:44:30  madings
*** empty log message ***

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

struct strCompare
{
    bool operator()(string a, string b) const
    {
	return (a < b);
    }
};

void save_insert_default_isotope_numbers(
                     StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF )
{

    map<string, string, strCompare> defaultIsotopes;

    // For reading the defaults list into a mapping:
    List<ASTnode*> *matches = currDicRuleSF->searchForTypeByTag(  ASTnode::DATALOOPNODE, "_atom_type"  );
    DataLoopNode *ruleLoop = (DataLoopNode*) ((*matches)[0]);
    int atomTypePos        = (ruleLoop->getNames())[0]->tagPosition( "_atom_type" );
    int defaultIsotopePos  = (ruleLoop->getNames())[0]->tagPosition( "_default_isotope_number" );
    delete matches;

    int dictIdx;
    for( dictIdx = 0 ; dictIdx < ruleLoop->getVals().size() ; ++dictIdx ) {
        string atomType = (*((ruleLoop->getVals())[dictIdx]))[atomTypePos]->myValue();
        string defaultIsotope = (*((ruleLoop->getVals())[dictIdx]))[defaultIsotopePos]->myValue();

        defaultIsotopes[atomType] = defaultIsotope;
    }


    // For iterating over the mapping rule:
    matches = currDicRuleSF->searchForTypeByTag(  ASTnode::DATALOOPNODE, "_input_key_tag"  );
    if( matches == 0 ) {
        cerr << "##transform76## Error: input_key_tag not found in insert_default_isotope_numbers rule." << endl;
        return;
    }
    ruleLoop = (DataLoopNode*) ((*matches)[0]);
    int inputKeyTagPos          = (ruleLoop->getNames())[0]->tagPosition( "_input_key_tag" );
    int inputKeyValuePos        = (ruleLoop->getNames())[0]->tagPosition( "_input_key_value" );
    int inputAtomTypeTagPos     = (ruleLoop->getNames())[0]->tagPosition( "_input_atom_type_tag" );
    int outputAtomIsotopeTagPos = (ruleLoop->getNames())[0]->tagPosition( "_output_atom_isotope_tag" );
    delete matches;

    for( dictIdx = 0 ; dictIdx < ruleLoop->getVals().size() ; ++dictIdx ) {
        string inputKeyTag          = (*((ruleLoop->getVals())[dictIdx]))[inputKeyTagPos]->myValue();
        string inputKeyValue        = (*((ruleLoop->getVals())[dictIdx]))[inputKeyValuePos]->myValue();
        string inputAtomTypeTag     = (*((ruleLoop->getVals())[dictIdx]))[inputAtomTypeTagPos]->myValue();
        string outputAtomIsotopeTag = (*((ruleLoop->getVals())[dictIdx]))[outputAtomIsotopeTagPos]->myValue();


	if( inputKeyValue == "*" ) {
            matches = NMR->searchForTypeByTag( ASTnode::SAVEFRAMENODE, inputKeyTag );
	} else {
            matches = NMR->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, inputKeyTag, inputKeyValue );
	}
	int matches_i;
	for( matches_i = 0 ; matches_i < matches->size(); ++matches_i ) {
	    SaveFrameNode *thisSf  = (SaveFrameNode*)  ( (*matches)[matches_i] );
	    List<ASTnode*> *loopMatches = thisSf->searchForTypeByTag( ASTnode::DATALOOPNODE, inputAtomTypeTag );
	    // Should be no more than 1 hit unless there's a star name clash:
	    if( loopMatches->size() > 0 ) {
	        DataLoopNode *thisLoop = (DataLoopNode*) ( (*loopMatches)[0] );

		int atomTagPos = (thisLoop->getNames())[0]->tagPosition( inputAtomTypeTag );
		int isotopeTagPos = (thisLoop->getNames())[0]->tagPosition( outputAtomIsotopeTag );
		string aType, isotope;

		if( isotopeTagPos < 0 ) {
		    // if not there, then insert the column to the right of the atom type tag:
		    thisLoop->getNames()[0]->insert(
		        thisLoop->getNames()[0]->begin() + atomTagPos + 1, outputAtomIsotopeTag );
		    isotopeTagPos = atomTagPos+1;
		}

		LoopTableNode *tbl = thisLoop->getValsPtr();
		// For each row of the loop:
		for( int rNum = 0 ; rNum < tbl->size() ; ++rNum ) {
		    LoopRowNode *row = (*tbl)[rNum];
		    aType   = (*row)[atomTagPos]->myValue();
		    isotope = (*row)[isotopeTagPos]->myValue();

		    // if there is no isotope filled in, then plug in the default:
		    if( isotope == "" || isotope == "?" || isotope == "." ) {
			string def = defaultIsotopes[aType];
			if( def != "" ) {
		            (*row)[isotopeTagPos]->setValue( def );
			}
		    }
		}
	    }

	}
	delete matches;
    }

    return;
}
