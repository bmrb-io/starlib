///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.6  2008/04/16 21:29:18  madings
Haven't committed in a while and this is a working version so I should.

Revision 1.5  2006/09/12 16:33:47  madings
*** empty log message ***

Revision 1.4  2006/08/28 19:50:57  madings
I forgot to remove the 'eraseme' debug print lines. - fixed now.

Revision 1.3  2006/08/28 19:48:26  madings
More fixes for ADIT-NMR handling of NMR_applied_experiemnt

Revision 1.2  2006/08/24 21:25:06  madings
*** empty log message ***

Revision 1.1  2005/03/18 21:07:20  madings
Massive update after too long without a checkin

*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

  ///////////////////////////////
 //    save_split_lines_to_loop
///////////////////////////////
void save_split_lines_to_loop(     StarFileNode *input,
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
   int                   remove_original_pos = -1;


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
   remove_original_pos = -1;
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
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_remove_original" ) )
            remove_original_pos = i;
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
      string     removeOriginalTag;
      if( remove_original_pos >= 0 ) {
         removeOriginalTag = (*( (*mapLoopTbl)[mL_idx] ))[remove_original_pos]->myValue();
      } else {
         removeOriginalTag = string("no");
      }

      List<ASTnode*> *sfMatches = input->searchForTypeByTagValue(
                                     ASTnode::SAVEFRAMENODE,
				     thisKeyTag, thisKeyValue );


      // For each saveframe that matches:
      for( i = 0 ; i < sfMatches->size() ; i++ )
      {
          SaveFrameNode *thisSf = (SaveFrameNode*) (*sfMatches)[i];

	  // Find the tag that matches in it:
	  List<ASTnode*> *matches = thisSf->searchByTag( thisSplitTag );
	  if( matches->size() == 0 )
	     continue;
	  string splitVal;
	  if( (*matches)[0]->isOfType( ASTnode::DATAITEMNODE ) )
	      splitVal = ( (DataItemNode*) (*matches)[0] )->myValue();
	  else
	      splitVal = string("<<Transform_49_Does_not_work_on_loops>>");

	  DataLoopNode *dln = new DataLoopNode(string("tabulate"));
	  DataLoopNameListNode *dln_nln = dln->getNamesPtr();
	  dln_nln->insert(dln_nln->end(), new LoopNameListNode() );
          LoopNameListNode *nameList = (*dln_nln)[0];
	  nameList->insert(nameList->end(), new DataNameNode( thisNewTag ) );
	  LoopTableNode *tbl = dln->getValsPtr();

          // anlyize the string - only if it has more than one non-whitespace
	  // line do we do this rule:
	  bool foundNonBlankLine = false;
	  int  numNonBlankLines = 0;
	  for( j = 0 ; j < splitVal.length() ; j++ ) {
	    if( splitVal[j] == '\n' || splitVal[j] == '\r' ||
	        j == splitVal.length() - 1  ) {
	        if( foundNonBlankLine ) {
		    numNonBlankLines++;
		    foundNonBlankLine = false;
		}
	    } else if( splitVal[j] != ' ' &&
	             splitVal[j] != '\t' ) {
	        foundNonBlankLine = true;
	    }
	  }
	  if( numNonBlankLines <= 1 ) {
	     continue;
	  }


	  string textLine = string("");

	  bool prevWasEoln = false;
	  int numLines = 0;

	  for( j = 0 ; j < splitVal.length() ; j++ )
	  {
	    if( splitVal[j] == '\n' || splitVal[j] == '\r' )
	    {
	       if( ! prevWasEoln )
	       {
	           numLines++;
		   // Trim the line's values - no leading or trailing whitespaces
		   int firstPos = textLine.find_first_not_of( " \t" );
		   int lastPos = textLine.find_last_not_of( " \t" );
		   if( firstPos >= 0 ) {
		       textLine = textLine.substr( firstPos, (lastPos-firstPos)+1 );
		   } else {
		       textLine = string("");
		   }
		   char quote_style = FindNeededQuoteStyle( textLine );

		   DataValueNode *dvn = new DataValueNode( textLine,
				    ( quote_style == '\'' ) ? DataValueNode::SINGLE :
				    ( quote_style == '\"' ) ? DataValueNode::DOUBLE :
				    ( quote_style == ';'  ) ? DataValueNode::SEMICOLON :
				    /*default*/               DataValueNode::NON 
				    );
	           LoopRowNode *lrn = new LoopRowNode(true);
	           lrn->insert( lrn->end(), *dvn );
		   tbl->insert( tbl->end(), *lrn );
		   delete lrn;
		   ASTnode *par = (SaveFrameNode*) thisSf->myParallelCopy();
		   if( par != NULL )
		   {
			par= par->myParent()->myParent();
		   }
		   if( par != NULL )
		   {
		       char sfName[1024];
		       sprintf( sfName, "%s_%d_%d", "save_NMR_spec_expt_", i, numLines );
		       SaveFrameNode *newSf = new SaveFrameNode( string(sfName) );
		       ASTlist<DataNode*> *newSfData = newSf->GiveMyDataList();
		       ASTlist<DataNode*> *thisSfData = thisSf->GiveMyDataList();
		       newSfData->insert(
		          newSfData->end(),
			  new DataItemNode( thisNewTag, dvn->myValue(), dvn->myDelimType() )
			  );
		       // Now copy everything else from the saveframe over to the new one,
		       // with the exception of this tag itself:
		       for( int i = 0 ; i < thisSfData->size() ; i++ ) {
		         if( (*thisSfData)[i]->isOfType(ASTnode::DATAITEMNODE) ) {
			   string thisName = (  (DataItemNode*)(*thisSfData)[i]  )->myName();
			   if( thisName == thisSplitTag  ) {
			     continue;
			   }
			 }
			 DataNode *insertMe =  (*thisSfData)[i]  ;
			 if( insertMe->isOfType(ASTnode::DATAITEMNODE ) ) {
			   newSfData->insert( newSfData->end(), new DataItemNode( *(DataItemNode*)insertMe ) );
			 } else if( insertMe->isOfType(ASTnode::DATALOOPNODE ) ) {
			   newSfData->insert( newSfData->end(), new DataLoopNode( *(DataLoopNode*)insertMe ) );
		         }
		       }

		       // Now, one test before the insertion - check to see if there exists another SF that has
		       // values matching this one.  If so, don't do this insertion after all:
		       List <ASTnode*> *searchContext = output->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, thisSplitTag, textLine );
		       bool insertThis = true;  // becomes false when a reason to not insert this is found
		       for( int i = 0 ; i < searchContext->size() && insertThis ; i++ ) {
		         bool allDups = true; // Becomes false the first time a non-duplicate is found.
		         // Is this saveframe also matching all free tags that exist in the new one?
			 SaveFrameNode *searchSf = (SaveFrameNode*) (*searchContext)[i];
			 for( int j = 0 ; j < newSfData->size() && allDups ; j++ ) {
			   if( (*newSfData)[j]->isOfType(ASTnode::DATAITEMNODE)  ) {
			     DataItemNode *din = (DataItemNode*) (*newSfData)[j];
			     string tagNameToLookFor = din->myName();
			     if( tagNameToLookFor == thisNewTag ) {
			       tagNameToLookFor = thisSplitTag;
			     }
			     string valToLookFor = din->myValue();
			     if( valToLookFor == "." || valToLookFor == "" || valToLookFor == "?" ) {
				 // skip this check - Comparing a null to a real value should be considered a match always.
			     } else {
			     
				 List <ASTnode*> *findCheck = searchSf->searchByTagValue( tagNameToLookFor, valToLookFor );
				 if( findCheck->size() == 0 ) {
				   allDups = false;
				 }
				 delete findCheck;
			     }
			   }
			 }
			 if( allDups ) {
			   insertThis = false;
			 }
		       }
		       delete searchContext;
		       
		       if( insertThis ) {
			   ((DataBlockNode*)par)->GiveMyDataList()->
			       insert( ((DataBlockNode*)par)->GiveMyDataList()->end(), newSf );
		       }

		   }
	       }
	       textLine = string("");
	       prevWasEoln = true;
	    }
	    else
	    {
	        prevWasEoln = false;
		textLine += splitVal[j];
	    }
	  }
	  if( removeOriginalTag == "yes" ) {
	    ASTnode *parToDel = ( (DataItemNode*) (*matches)[0] )->myParallelCopy();
	    if( parToDel ) {
	       delete parToDel;
	    }
	  }

	  delete matches;

          delete thisSf->myParallelCopy();
      }

      delete sfMatches;
    }

}
