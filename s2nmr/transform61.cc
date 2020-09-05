///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

ASTnode::ASTtype astTypes[] = 
  {
      ASTnode::BLOCKNODE,
      ASTnode::DATABLOCKNODE,
      ASTnode::SAVEFRAMENODE,
      ASTnode::DATALOOPNODE,
      ASTnode::DATAITEMNODE, 
      ASTnode::ASTNODE  // USED AS A SENTINEL TO END THE LIST - DO NOT DELETE
  };


void save_remove_header_comments(    StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF)
{
    int curType;
    string  emptyString = string("");
    // For each data block, saveframe, and loop, remove the comment:
    for( curType = 0 ; astTypes[curType] != ASTnode::ASTNODE ; curType++ )
    {
	int i;
	List<ASTnode*> *matches = outStar->searchForType( astTypes[curType] );

	// Set the commoent to nullstring:
	for( i = 0 ; i < matches->size() ; i++ )
	{
	    (*matches)[i]->setPreComment( emptyString );
	}
	delete matches;
    }
    return;
}
