///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.2  2006/08/28 19:48:26  madings
More fixes for ADIT-NMR handling of NMR_applied_experiemnt

Revision 1.1  2005/03/18 21:07:20  madings
Massive update after too long without a checkin

Revision 1.8  2003/07/24 18:49:13  madings
Changes to handle 2.1-3.0 mappings.

Revision 1.7  2001/02/13 23:00:43  jurgen
Elaborated and corrected error messages

Revision 1.6  2000/11/18 03:02:37  madings
*** empty log message ***

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"


/**
  *   SLOW PERFORMANCE WARNING:  This algorithm is Order(N^2) so it is VERY
  *      slow on large loops.  It should be streamlined to a better algorithm
  *      if it is
  *      to be used on large loops.  For now I don't think this will be
  *      necessary since it is only used on loops with 20 or less rows.
  *      If it does need to work on large loops, the way to
  *      fix it is to do this:
  *         1 - insert an original order column to the loop, which
  *                sequentially increments.
  *         2 - use STL vector's sort methods to sort the loop: Order(N*log(N))
  *         3 - and then the duplicates will be adjacent - remove them easily
  *         4 - then re-order the loop back to its original order with a
  *               second sort, this time on the original order column
  *         5 - then finally remove the original order column.
  */


void save_remove_duplicate_rows(StarFileNode *AST, 
                           StarFileNode *NMR, 
                           BlockNode    *,
			   SaveFrameNode *currDicRuleSF)


{
   string currDicSaveFrameName = string("save_remove_duplicate_rows");
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame){//Return if the target saveframe is not found
      return;
   }
   int questionsToo;

   // Check to see if the question_marks tag is activated:
   List<ASTnode *>*qmMatches = currDicSaveFrame->searchByTag("_question_marks_too");
   if( qmMatches->size() > 0 )
       questionsToo = 1;
   else
       questionsToo = 0;
   if( qmMatches != NULL )
       delete qmMatches;
   //Get the loop in the map file.
   List<ASTnode *> *curDicLoopHits = currDicSaveFrame->searchForTypeByTag(
	                ASTnode::DATALOOPNODE, string("_key_tag") );
   if ( curDicLoopHits->size() == 0){
       (*errStream)<<"#transform-56# ERROR: The loop with _key_tag is not found in save_remove_duplicate_rows."<<endl;
       return;
   }
   else{
       LoopTableNode *table  = ((DataLoopNode*)((*curDicLoopHits)[0]))->getValsPtr();
       DataLoopNameListNode *orig_names = ((DataLoopNode*)((*curDicLoopHits)[0]))->getNamesPtr();
       int rowCount = table->size();
       int colCount = (*orig_names)[0]->size();
       int innerRow = 0;
       
       LoopTableNode *innerTable;
       char *keyTag;
       char *keyValue;
       
       DataValueNode *thisVal;
       //For each row of the top level of loop.
       for (int curRow = 0; curRow < rowCount; curRow++){
	   //Get the keyTag and keyValue.
         for (int curCol =0; curCol < colCount; curCol++){
	     thisVal =  (*(*table)[curRow])[curCol];
	     if ( strcmp ( (*(*orig_names)[0])[curCol]->myName().c_str(), "_key_tag") == 0 ){
		 keyTag = new char[ strlen((thisVal->myValue()).c_str()) + 1 ];
		 strcpy( keyTag, thisVal->myValue().c_str());
	     }
	     else {
		 keyValue = new char[ strlen((thisVal->myValue()).c_str()) + 1 ];
		 strcpy( keyValue, thisVal->myValue().c_str());
	     }
	 }
	 //Find the saveframe in AST with keyTag and keyValue.
	 List<ASTnode *>*saveFrameHits;
	 if( keyTag == string("*") )
	     saveFrameHits = AST->searchForType(ASTnode::SAVEFRAMENODE);
	 else
	     saveFrameHits = AST->searchForTypeByTagValue(ASTnode::SAVEFRAMENODE, keyTag, keyValue);
	 ASTnode *curSaveFrame;
	 if(saveFrameHits->size() == 0){
	     (*errStream)<<"#transform-56# ERROR: Save frame named "<<keyTag
		         <<" with value of "<<keyValue<<" is not found."<<endl;
	 }
	 else{
              //Get the inner loop value of map file. 
              innerTable = ((*table)[curRow])->myLoop();
              innerRow = innerTable->size();
	      List<char*> *tagNames = new List<char*>;
	      
	      for(int i=0; i<innerRow; i++){
                 thisVal = (*(*innerTable)[i])[0];
                 char * tagName = new char[strlen(thisVal->myValue().c_str()) + 1];
		 strcpy(tagName, thisVal->myValue().c_str());
	         tagNames->AddToEnd(tagName);
	      }
              
	      //For each matched save frame, apply this transformation. 
	      for(int k=0; k<saveFrameHits->size(); k++){
	         curSaveFrame = (*saveFrameHits)[k];                
		 List<ASTnode*> *loopHits;
		 if( (*tagNames)[0] == string("*") )
		     loopHits = curSaveFrame->searchForType( ASTnode::DATALOOPNODE);
		 else
		     loopHits = curSaveFrame->searchForTypeByTag(
			 ASTnode::DATALOOPNODE, (*tagNames)[0]);
		 if(loopHits->size() == 0){
		     (*errStream) <<"#transform-56# ERROR: DataLoopNode with "
			          <<(*tagNames)[0]<<" is not found in input file."<<endl;
		 }
		 else{
		     //For each matched loop, apply this transformation.
                    for(int j=0; j<loopHits->size(); j++){
                          DataLoopNode * loop_ptr = (DataLoopNode*)(*loopHits)[j];
			  LoopTableNode * matrix = loop_ptr->getValsPtr();
			  DataLoopNameListNode *dataNames = loop_ptr->getNamesPtr();

			  int rowNum = matrix->size();
			  int colNum = (*dataNames)[0]->size();
			  List<int> *todelete = new List<int>; 

			  // Go through row by row, starting with the second row (the 1-th row),
			  // since the first row cannot possibly be a duplicate of anything yet:

			  for(int row=1; row<rowNum; row++){
			      int column =0;
			      bool is_delete = true; // Start off assuming the previous row is a duplicate
			 	                     // until proven otherwise.

			      // iterate over all rows previous to this one (this is what
			      // makes this execute (N^2)/2 times: and thus be slow)

			      bool is_found = false;
			      bool any_column_ever_found = false;
			      for(int prevRow = row-1 ; prevRow >= 0 ; prevRow-- ) {
			          is_delete = true;
				  any_column_ever_found = false;

				  for(int position=0; position<tagNames->size(); position++){
				      is_found = false;
				      for(int col = 0; col<colNum; col++){
					   if( strcmp( (*tagNames)[position], (*(*dataNames)[0])[col]->myName().c_str() ) == 0){
					      column = col;
					      is_found = true;
					      break;
					    }
					    if (is_found = false){
					       (*errStream)<<"#transformation-56# ERROR: "
							   << (*tagNames)[position] <<" is not found in input file"<<endl;
					    }
				      }
				    
				     if (  is_found )
				     {
				         any_column_ever_found = true;
					 // If this row's value on this column and the prev row's value on this
					 // column differ, and one of the two is non-null, then this is not a deletable row.
					 string str1 = (*(*matrix)[prevRow])[column]->myValue();
					 string str2 = (*(*matrix)[row])[column]->myValue();
					 if (	
						 str1 != str2  &&
						 str1 != "." &&
						 str1 != "?"  &&
						 str1 != ""  &&
						 str2 != "."  &&
						 str2 != "?"  &&
						 str2 != "" 
					     ) {
					     is_delete = false;
					     break;
					 } else {
					 }
				     }
				  }
				  if( is_delete && any_column_ever_found )
				    break;
			      }
			      if( is_delete && any_column_ever_found ){
				 todelete->AddToEnd(row);						
			      }
			  }
			  
			  if( todelete->size() != 0){
				     List<int> *index= new List<int>;
				     int tmp = 0;
				    
				    for(int g=0; g<todelete->size(); g++){
					 tmp = (*todelete)[g];
					 index->AddToEnd( tmp);
				    }
                                    ASTnode *nmrTagPeer = (*loopHits)[j]->myParallelCopy();
				    LoopTableNode *peerTable = ((DataLoopNode *)nmrTagPeer)->getValsPtr();
				    if ( index->size() == rowNum ){//Delete the whole loop
                                         delete  ((DataLoopNode *)nmrTagPeer);
				    }
				    else{
				     // delete in descending order so you don't mess up the indexes:
				     for(int h= index->size()-1; h >= 0 ; h--){
				       tmp = (*index)[h];	
				       if (nmrTagPeer != NULL ){
                                            delete (*peerTable)[tmp];
				       }
				     }
				    }
				    delete index;
			  }
		         delete todelete;	  
		    }

		 }
	     }
	      delete tagNames;
	   }
	   delete keyTag;
           delete keyValue;
	   
	 }
       }
   
}

