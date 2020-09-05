///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_strip_chem_comp_from_ligand_sf_names( StarFileNode *inStar, 
                                                StarFileNode *outStar, 
                                                BlockNode    *,
			                        SaveFrameNode *currDicRuleSF)
{
    int matches_i;
    List<ASTnode*> *matches = outStar->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, "_Saveframe_category", "ligand" );

    // For each such saveframe, do the renaming:
    for( matches_i = 0 ; matches_i < matches->size() ; matches_i++ )
    {
        SaveFrameNode *sf = (SaveFrameNode*) (  (*matches)[matches_i]  ); 
	string sfName = sf->myName();

	if( strncmp( sfName.c_str(), "save_chem_comp_", 15) == 0 )
	{
	   string newSfName = string("save_") + string( sfName.c_str() + 15 );
	   sf->changeName(newSfName);
	}
    }
    delete matches;
}
