///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_make_framecodes_agree( StarFileNode *AST, 
                                 StarFileNode *NMR, 
                                 BlockNode    *,
		                 SaveFrameNode *currDicRuleSF )
{
   string currDicSaveFrameName = string("save_make_framecodes_agree");
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame){//Return if the target saveframe is not found
     return;
   }


   // Do the work right on the output tree, since the changes are
   // changes that occur in-place:
   int sfIdx;
   List<ASTnode*> *allSF = NMR->searchForType( ASTnode::SAVEFRAMENODE );
   if( allSF == NULL )
     return;


   // For each saveframe:
   // -------------------

   for( sfIdx = 0 ; sfIdx < allSF->size() ; sfIdx++ )
   {
     SaveFrameNode  *thisSF = (SaveFrameNode*) (*allSF)[sfIdx];
     string  thisFullName = thisSF->myName();

     string  thisStripName = thisFullName;
     
     if( thisStripName.substr(0,5) == string("save_") )
        thisStripName.erase(0,5);
     
     List<ASTnode*> *freeTagsThisSF  = thisSF->searchForType( ASTnode::DATAITEMNODE );
     if( freeTagsThisSF == NULL )
       continue;
     
     DataItemNode *framecodeTag = NULL;
     string stuffBeforeDot = string("");

     //  Find which free tag is the sf_framecode:
     //  ----------------------------------------
     int freeTagsIdx;
     for(   freeTagsIdx = 0 ;
            freeTagsIdx < freeTagsThisSF->size() ;
	    freeTagsIdx++ )
     {
       DataItemNode *thisItem = (DataItemNode*) (*freeTagsThisSF)[freeTagsIdx];

       // For any free tag, remeber the stuff before the dot.
       // It will be the same for all free tags, so just look at the
       // first one we find:
       if( stuffBeforeDot == string("") )
       {
         stuffBeforeDot = thisItem->myName().substr( 0, thisItem->myName().find( '.' ));
       }

       // If the name ends with 'Sf_framecode', then it's the framecode tag:
       if( thisItem->myName().rfind( ".Sf_framecode" ) < thisItem->myName().length() )
       {
         framecodeTag = thisItem;
	 break;
       }
     }

     // If no such tag found, make one:
     if( framecodeTag == NULL )
     {
       framecodeTag = new DataItemNode( string(stuffBeforeDot) + string(".Sf_framecode"),
                                        string("."), DataValueNode::NON );
       thisSF->GiveMyDataList()->insert( 
           thisSF->GiveMyDataList()->begin(), framecodeTag );
     }


     // If the tag's value does not match the saveframe name,
     // then change it to match:
     if( framecodeTag->myValue() != thisStripName )
       framecodeTag->setValue( thisStripName );

   } // End- for each saveframe

   delete allSF;

   return;
}
