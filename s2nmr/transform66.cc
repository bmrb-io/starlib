//////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_move_PDBX_poly_seq_to_assembly(
                           StarFileNode *input, 
                           StarFileNode *output, 
                           BlockNode    *,
			   SaveFrameNode *currDicRuleSF)
{

  // To make things simpler, I'm forgoeing the notions of input/output for this
  // function - I'll just operate entirely on the output tree, making all my
  // changes in-place.

  // Find the output entity_assembly loop, of which there should be only one:
  List<ASTnode*> *assemList = output->searchForTypeByTag( ASTnode::DATALOOPNODE, "_Entity_assembly.Entity_ID" );

  if( assemList->size() <= 0 ) {
    cerr<<"#transform-66# ERROR: No _Entity_assembly.Entity_ID loop is present in the file."<<endl;
    delete assemList;
    return;
  }
  DataLoopNode *entityAssemLoop = (DataLoopNode*) ((*assemList)[0]);
  delete assemList;

  // Find the saveframe the entity_assembly loop is inside of (which should be the Assembly.
  ASTnode *par;  // walking up the parents in the tree:
  for( par = entityAssemLoop ; par != NULL ; par = par->myParent() )
  { if( par->isOfType(ASTnode::SAVEFRAMENODE) ) {
      break;
    }
  }
  if( par == NULL ) {
    cerr<<"#transform-66# ERROR: No _Entity_assembly.Entity_ID loop is present, but is not inside a saveframe in the file."<<endl;
    return;
  }
  SaveFrameNode *assemSF  = (SaveFrameNode*)par;

  // Remember what the SFID of this saveframe is:
  List<ASTnode*> *sfIdHits = assemSF->searchForTypeByTag( ASTnode::DATAITEMNODE, "_Assembly.Sf_ID");
  if( sfIdHits->size() == 0 ) {
    cerr<<"#transform-66# ERROR: No _Assembly.Sf_ID in the file"<<endl;
    return;
  }
  DataItemNode *sfIdNode = (DataItemNode*) ( (*sfIdHits)[0] );
  string sfId = sfIdNode->myValue();

  // Find all the loops where there's a _PDBX_poly_seq loop within the output,
  // and perform the algorithm on them:
  List<ASTnode*> *pdbxPolySeqLoops = output->searchForTypeByTag( ASTnode::DATALOOPNODE, "_PDBX_poly_seq_scheme.Sf_ID" );
  if( pdbxPolySeqLoops->size() <= 0 ) {
    cerr<<"#transform-66# ERROR: No _PDBX_poly_seq_scheme loops in the file."<<endl;
    delete pdbxPolySeqLoops;
    return;
  }
  int loopsIdx;
  int destAsymIdCol = -1;

  DataLoopNode *destLoop = NULL;
  LoopTableNode *destTbl = NULL;
  for( loopsIdx = 0 ; loopsIdx < pdbxPolySeqLoops->size() ; loopsIdx++ ) {
    DataLoopNode *srcLoop = (DataLoopNode*)( (*pdbxPolySeqLoops)[loopsIdx] );
    LoopTableNode *srcLoopVals = srcLoop->getValsPtr();
    int entityIdCol;
    int dummyNest;
    int destAsymIdCol = -1;
    srcLoop->tagPositionDeep( "_PDBX_poly_seq_scheme.Entity_ID", &dummyNest, &entityIdCol );
    if( entityIdCol < 0 ) {
      cerr<<"#transform-66# ERROR: No _PDBX_poly_seq_scheme.Entity_ID tag in this loop."<<endl;
      continue;
    }
    if( srcLoopVals->size() <= 0 ) {
      cerr<<"#transform-66# ERROR: _PDBX_poly_seq_scheme loop has no rows in it."<<endl;
      continue;
    }
    // Assume the whole loop is the same entity ID, since it is inside an
    // entity saveframe:
    string thisEntity = (*( (*srcLoopVals)[0] ))[entityIdCol]->myValue();

    // Go prepare the destination loop if need be:
    // starting it off with one column.
    if( destLoop == NULL ) {
      destLoop = new DataLoopNode(string("tabulate"));
      LoopNameListNode *tmpList = new LoopNameListNode();
      destLoop->getNames().insert(destLoop->getNames().begin(), tmpList );
      destLoop->getNames()[0]->insert( destLoop->getNames()[0]->begin(),
	  new DataNameNode( string("_PDBX_poly_seq_scheme.Asym_ID" ) ) );
      destAsymIdCol = 0;

      assemSF->GiveMyDataList()->insert(
          assemSF->GiveMyDataList()->end(), destLoop );
      destTbl = destLoop->getValsPtr();
    }

    int entityAssemRow;
    int entityAssemEntityIdCol;
    entityAssemLoop->tagPositionDeep( "_Entity_assembly.Entity_ID", &dummyNest, &entityAssemEntityIdCol );
    if( entityAssemEntityIdCol < 0 ) {
      cerr<<"#transform-66# ERROR: _Entity_assembly.Entity_ID not found in _Entity_assembly loop."<<endl;
      break;
    }
    int entityAssemIdCol;
    entityAssemLoop->tagPositionDeep( "_Entity_assembly.ID", &dummyNest, &entityAssemIdCol );
    if( entityAssemIdCol < 0 ) {
      cerr<<"#transform-66# ERROR: _Entity_assembly.ID not found in _Entity_assembly loop."<<endl;
      break;
    }

    // For each matching row of the Entity_assembly table:
    for( entityAssemRow = 0 ;
         entityAssemRow < entityAssemLoop->getValsPtr()->size() ;
	 entityAssemRow++ ) {
      LoopRowNode *curAssemRow = entityAssemLoop->getVals()[ entityAssemRow ];
      string curEntity = (*curAssemRow)[ entityAssemEntityIdCol ]->myValue();
      string curAsymId = (*curAssemRow)[ entityAssemIdCol ]->myValue();
      if( curEntity != thisEntity ) {
        // (skip if it is not this entity:)
        continue;
      }

      // For each row of the source table, copy it to dest, with the asym id added:
      int colNum;
      int rowNum;
      for( rowNum = 0 ; rowNum < srcLoopVals->size() ; rowNum++ ) {
	LoopRowNode *srcRow = (*srcLoopVals)[rowNum];
        LoopNameListNode *srcNames = srcLoop->getNames()[0];
	// create dummy row of appropriate size:
	LoopRowNode *destRow = new LoopRowNode( true );
	for( int j = 0 ; j < destLoop->getNames()[0]->size() ; j++ ) {
	  destRow->insert( destRow->end(), new DataValueNode( "." ) );
	}
	// insert it into the table:
	destTbl->insert( destTbl->end(), destRow );
	int destLastRowNum = destTbl->size() - 1;
	int destCol;
	for( colNum = 0 ; colNum < srcRow->size() ; colNum++ ) {
	  destLoop->tagPositionDeep( (*srcNames)[colNum]->myName(), &dummyNest, &destCol );
	  // If the dest col is not found, then add it
	  if( destCol < 0 ) {
	    destLoop->getNames()[0]->insert( destLoop->getNames()[0]->end(), 
	              new DataNameNode( (*srcNames)[colNum]->myName() ) );
	    destCol = destLoop->getNames()[0]->size() - 1;
	  }
	  // replace the value at the given col in the destination row:
	  (* destTbl )[ destLastRowNum ]->setElementAt(
	       destCol, *( (*srcRow)[colNum] ) );
	}
	// Then set the asym ID in the row:
	destLoop->tagPositionDeep( "_PDBX_poly_seq_scheme.Asym_ID", &dummyNest, &destCol );
	if( destCol >= 0 ) {
	  DataValueNode *newVal  = new DataValueNode(curAsymId);
	  (* destTbl )[ destLastRowNum ]->setElementAt( destCol, (*newVal) );
	}
	// Then set the SF ID in the row:
	destLoop->tagPositionDeep( "_PDBX_poly_seq_scheme.Sf_ID", &dummyNest, &destCol );
	if( destCol >= 0 ) {
	  DataValueNode *newVal  = new DataValueNode(sfId);
	  (* destTbl )[ destLastRowNum ]->setElementAt( destCol, (*newVal) );
	}
      }
    }
    delete srcLoop;
  }
  delete pdbxPolySeqLoops;
}

