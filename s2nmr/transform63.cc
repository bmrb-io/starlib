///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

  ///////////////////////////////
 //    save_rows_concatenated_to_free
///////////////////////////////
void save_rows_concatenated_to_free(   StarFileNode *input,
				       StarFileNode *output,
				       BlockNode    *blk,
				       SaveFrameNode *currDicRuleSF)
{

   List<ASTnode*>        *mapLoopMatch;
   List<ASTnode*>        *matchSF;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;
   int                   mL_idx;//map loop index 
   int                   i,j,k;
   int                   key_tag_pos = -1;
   int                   key_value_pos = -1;
   int                   split_tag_pos = -1;
   int                   new_tag_pos = -1;
   int                   keep_old_tag_pos = -1;


   //Find loop from the mapping file:
   mapLoopMatch = currDicRuleSF->searchForTypeByTag(ASTnode::DATALOOPNODE, string("_key_tag"));
   if( mapLoopMatch->size() < 0)
   { 
       (*errStream) << "#transform-49# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
        delete mapLoopMatch;
        return; /* do nothing */
   }
   
   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames   =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

   delete mapLoopMatch;
   
   key_tag_pos = -1;
   key_value_pos = -1;
   split_tag_pos = -1;
   new_tag_pos = -1;
   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
        if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
            key_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
            key_value_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_split_tag" ) )
            split_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag" ) )
            new_tag_pos = i;
    }

    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-49# ERROR: input mapping file is missing tag '_key_tag'." << endl;
        return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-49# ERROR: input mapping file is missing tag '_key_value'." << endl;
        return; /* do nothing */
    }
    if( split_tag_pos < 0  )
    {   (*errStream) << "#transform-49# ERROR: input mapping file is missing tag '_split_tag'." << endl;
        return; /* do nothing */
    }
    if( new_tag_pos < 0  )
    {   (*errStream) << "#transform-49# ERROR: input mapping file is missing tag '_new_tag'." << endl;
        return; /* do nothing */
    }
    
    
    for(mL_idx = 0; mL_idx < mapLoopTbl->size(); mL_idx++ )
    {
      string     thisKeyTag =   (*( (*mapLoopTbl)[mL_idx] ))[key_tag_pos]->myValue();
      string     thisKeyValue = (*( (*mapLoopTbl)[mL_idx] ))[key_value_pos]->myValue();
      string     thisSplitTag = (*( (*mapLoopTbl)[mL_idx] ))[split_tag_pos]->myValue();
      string     thisNewTag =   (*( (*mapLoopTbl)[mL_idx] ))[new_tag_pos]->myValue();

      List<ASTnode*> *sfMatches = input->searchForTypeByTagValue(
                                     ASTnode::SAVEFRAMENODE,
				     thisKeyTag, thisKeyValue );


      // For each saveframe that matches:
      for( i = 0 ; i < sfMatches->size() ; i++ )
      {
          SaveFrameNode *thisSf = (SaveFrameNode*) (*sfMatches)[0];

	  // Find the tag that matches in it:
	  List<ASTnode*> *matches = thisSf->searchByTag( thisSplitTag );
	  if( matches->size() == 0 )
	     break;
	  string splitVal; 
	  if( (*matches)[0]->isOfType( ASTnode::DATAITEMNODE ) )
	      splitVal = ( (DataItemNode*) (*matches)[0] )->myValue();
	  else
	  {
	      ASTnode *cur;
	      for( cur = (*matches)[0] ; cur != NULL && ! cur->isOfType(ASTnode::DATALOOPNODE) ; cur = cur->myParent() )
	      { /*void body*/ }
	      if( cur != NULL )
	      {  
	          DataLoopNode *dln = (DataLoopNode*) ( cur );
		  int dummy, col;
		  dln->tagPositionDeep( thisSplitTag , &dummy, &col );
	          int rNum;
		  splitVal = string("");
		  for( rNum = 0 ; rNum < dln->getVals().size() ; rNum++ )
		  {   if( rNum > 0 )
		          splitVal += "\n";
		      splitVal += (*( dln->getVals() [rNum] ))[ col ] -> myValue();
		  }
	      }
	  }
	  DataItemNode *newItem;
	  newItem = new DataItemNode( thisNewTag, splitVal );
	  newItem->setDelimType( DataValueNode::SEMICOLON );
	  ( (SaveFrameNode*) ( thisSf->myParallelCopy() ) )->GiveMyDataList()->AddToEnd(newItem);
	  delete matches;

      }

      delete sfMatches;
    }

}
