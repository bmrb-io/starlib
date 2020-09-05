///////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.3  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.2  1999/06/11 19:01:32  zixia
*** empty log message ***

// Revision 1.1  1999/06/10  17:40:34  zixia
// Add a new function transform 20 by Zixia Gu.
//
*/
#endif
//////////////////////////////////////////

#include "string.h"
#include "transforms.h"
void save_remove_global_block(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF)
{
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame){//Return if the target saveframe is not found
      return;
   }

   //Find tags called "global_"
   List<ASTnode*> *matchParseTag = inStar->searchForTypeByTag(ASTnode::BLOCKNODE, "global_");
   
    

   if( matchParseTag->size() != 1){
       (*errStream)<<"#transform-20# The tag global_ is not found."<<endl;
       delete matchParseTag;
       return;
   }
   
   
   GlobalHeadingNode *tagPeer = (GlobalHeadingNode*)(((*matchParseTag)[0])->myParallelCopy());
  

   //remove the old GlobleBlockNode
   tagPeer->removeMe();
   
   delete matchParseTag;
}
