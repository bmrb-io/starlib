///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
// Revision 1.3  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
//Revision 1.2  1999/06/11 19:01:32  zixia
//*** empty log message ***
//
// Revision 1.1  1999/06/10  17:43:39  zixia
// *** empty log message ***
//
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_check_chemical_shift_nomenclature(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF)
{  
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int residue_label_pos = -1;
   int old_atom_name_pos = -1;
   int new_atom_name_pos = -1;
   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_residue_label" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-29# ERROR: no loop tag called '_residue_label' in mapping file." << endl;
       delete mapLoopMatch;
       return; /* do nothing */
   }
   
   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

   delete mapLoopMatch;

   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
   {
        if( (* ((*mapNames)[0]) )[i]->myName()==string( "_residue_label" ) )
            residue_label_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_old_atom_name" ) )
            old_atom_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_atom_name" ) )
            new_atom_name_pos = i;
   }
   if( residue_label_pos < 0  )
   {   (*errStream) << "#transform-29# ERROR: input mapping file is missing tag '_residue_label'." 
                    << endl;
       return; /* do nothing */
   }
   if( old_atom_name_pos < 0  )
   {   (*errStream) << "#transform-29# ERROR: input mapping file is missing tag '_old_atom_name'." 
                    << endl;
       return; /* do nothing */
   }
   if( new_atom_name_pos < 0  )
   {   (*errStream) << "#transform-29# ERROR: input mapping file is missing tag '_new_atom_name'." 
                    << endl;
       return; /* do nothing */
   }

   
   // For each row of the loop, rename the name of saveframe. 
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {   
       List<ASTnode*>        *matchSF;
       List<ASTnode*>        *matchLoop;
       DataLoopNode          *peerLoop;
       DataLoopNameListNode  *nameListList;
       LoopTableNode         *valTable;
       
       
       int rowCount, colCount;
       int curCol, curRow;
       int Residue_label_pos = -1;
       int Atom_name_pos = -1;
       
       
       //Find the saveframe which contains the assigned chemical shifts.
       matchSF = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        string("_Saveframe_category"),
			string("assigned_chemical_shifts") );
       
       if(matchSF->size() != 1){
          (*errStream)<<"#transform-29# ERROR: only one saveframe with assigned_chemical_shifts "
	              <<"should be found"<<endl;
	  delete matchSF;
	  return;
       }
      
       //Within the saveframe, find the chemical shift loop.
       matchLoop = (*matchSF)[0]->searchForTypeByTag(
	                          ASTnode::DATALOOPNODE,
				  string("_Atom_shift_assign_ID") );
       if(matchLoop->size() != 1){
          (*errStream)<<"#transform-29# ERROR: only one dataloop with _Atom_shift_assign_ID "
	              <<"should be found"<<endl;
	  delete matchSF;
	  delete matchLoop;
	  
	  return;
       }
      
       //Find the corresponding loop in the output NMR-STAR file.
       peerLoop = (DataLoopNode*)(*matchLoop)[0]->myParallelCopy();

       nameListList = peerLoop->getNamesPtr();
       valTable     = peerLoop->getValsPtr();
       
       //The number of columns in the loop.
       rowCount = valTable->size();
       //Size of the first row of names(the outermost loop level).
       colCount = (*nameListList)[0]->size();
     
       
       for(int i=0; i<colCount; i++){
          if( (*((*nameListList)[0]) )[i]->myName() ==string("_Residue_label") )
	      Residue_label_pos = i;
	  else if( (*((*nameListList)[0]) )[i]->myName() ==string("_Atom_name") )
	      Atom_name_pos = i;
       }
      
       if( Residue_label_pos < 0 ){
          (*errStream) <<"#transform-29# ERROR: input data file is missing tag '_Residue_label'."
	               <<endl;
	  continue; /* do nothing */
       }
       if( Atom_name_pos < 0 ){
          (*errStream) <<"#transform-29# ERROR: input data file is missing tag '_Atom_name'."
	               <<endl;
	  continue; /* do nothing */
       }
       
       
       //For each row of the loop, apply the changing of nomenclature.
       for(curRow=0; curRow<rowCount; curRow++){
            if( strcmp( (*(*valTable)[curRow])[Residue_label_pos]->myValue().c_str(), 
		         (*(*mapLoopTbl)[mL_Idx])[residue_label_pos]->myValue().c_str() )==0 &&
		strcmp( (*(*valTable)[curRow])[Atom_name_pos]->myValue().c_str(), 
		         (*(*mapLoopTbl)[mL_Idx])[old_atom_name_pos]->myValue().c_str() )==0  ){
		    
                (*(*valTable)[curRow])[Atom_name_pos]->setValue(
		              (*(*mapLoopTbl)[mL_Idx])[new_atom_name_pos]->myValue());
			
	    }
	      
	  
       }
       
       delete matchSF;
       delete matchLoop;
   }
}
