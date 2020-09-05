///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2005/03/18 21:07:20  madings
Massive update after too long without a checkin

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_flat_file_name_if_not_exist( StarFileNode *input, 
                                StarFileNode *output, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF)
{
   string currDicSaveFrameName = string("save_remove_null_tags");
   DataNode* currDicSaveFrame = currDicRuleSF;


   if(!currDicSaveFrame)//Return if the target saveframe is not found
       return; 


   string accNum = string("????");
   List<ASTnode *> *hits = NULL;

   hits = output->searchForTypeByTag( ASTnode::DATAITEMNODE, "_BMRB_accession_number" );
   if( hits->size() > 0 )
   {
       accNum = ( (DataItemNode*)( (*hits)[0] ) )->myValue();
   }
   if( hits != NULL ) { delete hits; hits = NULL; }
   hits = output->searchByTag( "_BMRB_flat_file_name" );

   if( hits->size() == 0 )
   {
       if( hits != NULL ) { delete hits; hits = NULL; }
       hits = output->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, "_Saveframe_category", "entry_information" );
       if( hits->size() > 0 )
       {
           SaveFrameNode *sfn = (SaveFrameNode*) ( (*hits)[0] );
	   string newValue = string("");
	   newValue = "bmr" + accNum + ".str";
	   sfn->GiveMyDataList()->insert(
	       sfn->GiveMyDataList()->end(),
	       new DataItemNode( "_BMRB_flat_file_name", newValue ) );
       }
       if( hits != NULL ) { delete hits; hits = NULL; }
   }
   if( hits != NULL ) { delete hits; hits = NULL; }
}
