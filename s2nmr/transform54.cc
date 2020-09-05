///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"
#include "string.h"

void save_delete_invalid_tag_syntax( StarFileNode *AST, 
				     StarFileNode *NMR, 
				     BlockNode    *currDicDataBlock,
				     SaveFrameNode *currDicRuleSF )
{
    int hitNum;
    char validContinueChars[] =
       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
       "abcdefghijklmnopqrstuvwxyz"
       "0123456890"
       "_-~!@#$%^&*()+=[]{}<>,.;:\'\"/|\\`~";
    int valid;
    int matchLen;


    // We're going to have to find every data name node.
    List <ASTnode*>  *hits =
        NMR->searchForType( ASTnode::DATANAMENODE );
    
    if( hits->size() == 0 )
    {
        (*errStream) << "#transform54: error - no data names" <<
               " found to populate in the file."<<endl;
        return;
    }

    for( hitNum = 0 ; hitNum < hits->size() ; hitNum++ )
    {
        if( (*hits)[hitNum]->isOfType(ASTnode::DATANAMENODE ) )
	{
	    DataNameNode *thisHit = (DataNameNode*)( (*hits)[hitNum] );
	    string hitName = thisHit->myName();

	    valid = 0;
	    if( hitName[0] == '_' )
	    {
	       matchLen = strspn( hitName.c_str(), validContinueChars );
	       if( matchLen >= strlen(hitName.c_str()) )
	       {
		 valid = 1;
	       }
	    }

	    if( !valid )
	    {
	       thisHit->setName( "_This_name_was_invalid-See_the_above_comment." );
	       if( thisHit->myParent() != NULL )
		   thisHit->myParent()->setPreComment(
				       string("# This name was previously: ") + hitName + 
				       string("\n") +
				       string("# It had to be renamed to make "
						"the STAR syntax valid so "
						"processing could continue.\n") ); 
	    }
	    else
	    {
	    }
	}
    }
    delete hits;
    return;
}
