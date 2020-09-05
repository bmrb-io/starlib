///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

  ///////////////////////////////
 //    save_remove_3.0_tags   //
///////////////////////////////
void save_remove_version_3_0_tags( StarFileNode *input,
	                           StarFileNode *output,
		                   BlockNode    *blk,
		                   SaveFrameNode *currDicRuleSF)
{
  
    DataNode* currDicSaveFrame = currDicRuleSF;

    SaveFrameNode *removeRuleDict = new SaveFrameNode(string("save__DUMMY") );
    removeRuleDict->GiveMyDataList()->insert(
            removeRuleDict->GiveMyDataList()->end(),
	    new DataItemNode( string("_Saveframe_category"),
	                      string("remove_tag"),
			      DataValueNode::NON )  );

    DataLoopNode *curDicDLN = new DataLoopNode(string("tabulate"));

    DataLoopNameListNode *dataNames = curDicDLN->getNamesPtr();
    LoopNameListNode *names = new LoopNameListNode();
    dataNames->insert( dataNames->end(), names );
    names->insert( names->end(),
                   new DataNameNode( string("_old_tag_name") ) );
    names->insert( names->end(),
                   new DataNameNode( string("_old_saveframe") ) );
    names->insert( names->end(),
                   new DataNameNode( string("_remove_all_loop") ) );


    if(!currDicSaveFrame)
	return;     //Simply return if the desire saveframe is not found

    LoopTableNode *curDicTbl = curDicDLN->getValsPtr();

    removeRuleDict->GiveMyDataList()->insert(
            removeRuleDict->GiveMyDataList()->end(), curDicDLN );

    //Apply this change to all saveframes in the file:
    List<ASTnode *> *saveFrameHits = NULL;
    saveFrameHits = AST->searchForType(ASTnode::SAVEFRAMENODE);

    SaveFrameNode *saveFrameCur;

    for ( saveFrameHits->Reset(); !saveFrameHits->AtEnd(); saveFrameHits->Next() )
    {
	int i,j;
	saveFrameCur = (SaveFrameNode*)( saveFrameHits->Current() );
	string sfNameCur = saveFrameCur->myName();
	if ( saveFrameCur->isOfType(ASTnode::SAVEFRAMENODE) )
	{
	    for( i = 0 ; i < saveFrameCur->GiveMyDataList()->size() ; i++ )
	    {
		ASTnode *curThingy = (*(saveFrameCur->GiveMyDataList()))[i];
		if( curThingy && curThingy->isOfType( ASTnode::DATAITEMNODE ) )
		{
		    string str = ((DataItemNode*)curThingy)->myName();
		    if( strchr( str.c_str(), '.' ) || str == string("FRAMECODE") )
		    {   LoopRowNode *lrn = new LoopRowNode(true);
		        lrn->insert( lrn->end(),
			             new DataValueNode( str,
				              DataValueNode::DOUBLE ) );
		        lrn->insert( lrn->end(),
			             new DataValueNode( string("*"),
			                     DataValueNode::NON ) );
		        lrn->insert( lrn->end(),
			             new DataValueNode( string("."),
			                     DataValueNode::NON ) );
		        curDicTbl->insert( curDicTbl->begin(), lrn );


		    }
		}
		else if( curThingy && curThingy->isOfType( ASTnode::DATALOOPNODE ) )
		{
		    DataLoopNode *dln = (DataLoopNode*)curThingy;
		    LoopNameListNode *names = (dln->getNames())[0];
		    if( names )
			for( j = 0; j < names->size() ; j++ )
			{
			    if( (*names)[j] )
			    {
				string str = (*names)[j]->myName();
				if( strchr( str.c_str(), '.' ) || str == string("FRAMECODE") )
				{   LoopRowNode *lrn = new LoopRowNode(true);
				    lrn->insert( lrn->end(),
						 new DataValueNode( str,
							  DataValueNode::DOUBLE ) );
				    lrn->insert( lrn->end(),
						 new DataValueNode( string("*"),
							 DataValueNode::NON ) );
				    lrn->insert( lrn->end(),
						 new DataValueNode( string("."),
							 DataValueNode::NON ) );
				    curDicTbl->insert( curDicTbl->begin(), lrn );


				}
			    }
			}
		}
	    }
	}

    }

    delete saveFrameHits;

    /* comment-out debug stmt:
     *   (*os)<<"### save_remove_Tag_generated_by transfrom 46: ####"<<endl
     *      <<"#### ["<<endl; removeRuleDict->Unparse(0);(*os)<<"### ]"<<endl;
     */
    save_remove_tag( input, output, blk, removeRuleDict);

}

