///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_generate_residue_mol_labels( StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF )
{

   vector<string> labelsToAdd;
   vector<string> seqsToAdd;

   // For each residue:
   List<ASTnode*> *residueSaveFrames = NMR->searchForTypeByTagValue(
              ASTnode::SAVEFRAMENODE, "_Saveframe_category", "polymer_residue" );

   int i;
   for( i = 0 ; i < residueSaveFrames->size() ; i++ )
   {
       SaveFrameNode *resSf = (SaveFrameNode*) (*residueSaveFrames)[i];

       string resName = resSf->myName();
       // Strip off the "save_" from the saveframe name:
       if( resName.substr(0, 5) == string("save_") )
       {
           resName.replace(0, 5, "" );
       }

       labelsToAdd.clear();
       seqsToAdd.clear();

       int nest,col;
       // Now find all the cases where an entity mentions this residue, and get
       // those saveframe names.
       List<ASTnode*> *polymerHits = NMR->searchForTypeByTagValue( 
               ASTnode::SAVEFRAMENODE, "_Saveframe_category", "monomeric_polymer" );
       int j;
       for( j = 0 ; j < polymerHits->size() ; j++ ) 
       {
           SaveFrameNode *polySf = (SaveFrameNode*) (*polymerHits)[j];

	   string polyName = polySf->myName();
           // Strip off the "save_" from the saveframe name:
	   if( polyName.substr(0, 5) == string("save_") )
	   {
	       polyName.replace(0, 5, "" );
	   }

	   // Now we know that this is one of the saveframe names
	   // that points to this residue:

	   List<ASTnode*> *valHits = polySf->searchForTypeByTagValue(
	       ASTnode::LOOPROWNODE, string("_Residue_label"), resName );

           string seqCode = string("");

	   int k;
	   for( k = 0 ; k < valHits->size() ; k ++ )
	   {
	       DataLoopNode *dln = (DataLoopNode*) (*valHits)[k]->myParent()->myParent();
	       dln->tagPositionDeep( "_Residue_seq_code", &nest, &col );
	       if( col >= 0 )
	       {
	           seqCode = (*( (LoopRowNode*)((*valHits)[k]) ))[col]->myValue();
		   labelsToAdd.insert(labelsToAdd.end(), polyName );
		   seqsToAdd.insert(seqsToAdd.end(), seqCode );
	       }
	   }
	   delete valHits;
       }
       delete polymerHits;

       int molLabelPos = -1;
       int seqCodePos = -1;
       // Now make the target loop:
       List<ASTnode*> *loopHits = resSf->searchByTag( "_Mol_label" );
       DataLoopNode *thisLoop;
       if( loopHits->size() == 0 )
       {
           thisLoop = new DataLoopNode("tabulate");
	   DataLoopNameListNode *thisDLNames = thisLoop->getNamesPtr();
	   if( thisDLNames->size() == 0 )
	   {
	       thisDLNames->insert( thisDLNames->begin(), new LoopNameListNode() );
	   }
	   (*thisDLNames)[0]->insert( (*thisDLNames)[0]->end(), new DataNameNode( "_Mol_label" ) );
	   (*thisDLNames)[0]->insert( (*thisDLNames)[0]->end(), new DataNameNode( "_Residue_seq_code" ) );
	   molLabelPos = 0;
	   seqCodePos = 1;
           resSf->GiveMyDataList()->insert(resSf->GiveMyDataList()->end(), thisLoop );
       }
       else
       {
           thisLoop = (DataLoopNode*) (*loopHits)[0];
	   thisLoop->tagPositionDeep( "_Mol_label", &nest, &molLabelPos );
	   if( molLabelPos < 0 )
	   {
	      thisLoop->getNames()[0]->insert( thisLoop->getNames()[0]->end(), new DataNameNode( "_Mol_label" ) );
	      molLabelPos = thisLoop->getNames()[0]->size();
	   }
	   thisLoop->tagPositionDeep( "_Residue_seq_code", &nest, &seqCodePos );
	   if( seqCodePos < 0 )
	   {
	      thisLoop->getNames()[0]->insert( thisLoop->getNames()[0]->end(), new DataNameNode( "_Residue_seq_code" ) );
	      seqCodePos = thisLoop->getNames()[0]->size();
	   }
       }
       delete loopHits;

       // Now the loop exists, fill it with the value found:
       int a = 0;
       int b = 0;
       for( a = 0 ; a < labelsToAdd.size() ; a++ )
       {
	  bool alreadyExist = false;
          for( b = 0 ; b < thisLoop->getValsPtr()->size() ; b++ )
	  {
	      string mol = (*(thisLoop->getVals()[b]) )[molLabelPos]->myValue();
	      string seq = (*(thisLoop->getVals()[b]) )[seqCodePos]->myValue();

	      if( mol == labelsToAdd[a] && seq == seqsToAdd[a] )
	      {
	         alreadyExist = true;
		 break;
	      }
	  }
	  if( ! alreadyExist )
	  {
	      int row_i = 0;
	      LoopRowNode *newRow = new LoopRowNode(true);
	      for( row_i = 0 ; row_i < thisLoop->getNames()[0]->size() ; row_i++ )
	      {
	         if( row_i == molLabelPos )
		 {
		     newRow->insert( newRow->end(), new DataValueNode( labelsToAdd[a], DataValueNode::FRAMECODE ) );
		 }
		 else if( row_i == seqCodePos )
		 {
		     newRow->insert( newRow->end(), new DataValueNode(  seqsToAdd[a] ) );
		 }
		 else
		 {
		     newRow->insert( newRow->end(), new DataValueNode( string(".") ) );
		 }
	      }
	      thisLoop->getVals().insert( thisLoop->getVals().end(), newRow );
	  }
       }
   }


   delete residueSaveFrames;
}
