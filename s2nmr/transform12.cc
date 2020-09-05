///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.9  2003/07/24 18:49:13  madings
Changes to handle 2.1-3.0 mappings.

Revision 1.8  2001/04/13 20:52:29  jurgen
error messages corrected

Revision 1.7  2000/11/18 03:02:37  madings
*** empty log message ***

// Revision 1.6  2000/11/18  02:26:28  madings
// Fixed a problem that was common to several of the files (apparently
// all the ones edited by 'june', although it's hard to tell if this is
// her doing or not.)  The problem is that there were several places in
// the code where something like this had been done:
//     char *strPtr1, *strPtr2;
//     ...
//     /* strPtr points at some string right now */.
//     strPtr2 = new char [ strlen(strPtr1) ];  /* error: needs a +1 */
//     strcpy( strPtr2, strPtr1 );
// The problem with this, of course. is that strlen() gives the length
// of the string NOT including the null terminator, so the above code
// doesn't allocate enough space for strPtr2 to store the terminator.
// The result is that when the strcpy() is called, a zero-byte (null
// terminator) overwrites whatever was in the next byte just after strPtr2.
// Depending on what that next byte actually is, this might corrupt the
// value, or might not if it was supposed to be a zero anyway.  I suspect
// that this is what was causing some erratic segfaults we were seeing
// in this program - where seemingly irrelevant changes to the input data
// would 'fix' crashes.  (Even though the data changed had nothing to do with
// the part of the code that crashed.)  I suspect that the overwritten byte
// was part of the data the C-library heap uses internally to keep track of
// how much memory a variable on the heap is taking up (so it knows how much
// to free later when free() is called).  When 'free()' was being called, the
// wrong amount of memory was being freed, and thus the heap was a jumbled
// mess after that.)
//
// Revision 1.5  1999/07/30  18:22:06  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.4  1999/06/18 21:14:34  zixia
*** empty log message ***

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"


void save_remove_null_loop_rows( StarFileNode *AST, 
                                 StarFileNode *NMR, 
                                 BlockNode    *,
				 SaveFrameNode *currDicRuleSF)
{
   string currDicSaveFrameName = string("save_remove_null_loop_rows");
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
       (*errStream)<<"#transform-12# ERROR: The loop with _key_tag is not found in save_remove_null_loop_rows."<<endl;
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
       char defNull1[5] = ".";
       char defNull2[5] = "?";
       
       DataValueNode *thisVal;
       //For each row of the top level of loop.
       for (int curRow = 0; curRow < rowCount; curRow++){
         List<char*> *tagNulls = new List<char*>;
	   //Get the keyTag and keyValue.
         for (int curCol =0; curCol < colCount; curCol++){
	     thisVal =  (*(*table)[curRow])[curCol];
	     if ( strcmp ( (*(*orig_names)[0])[curCol]->myName().c_str(), "_key_tag") == 0 ){
		 keyTag = new char[ strlen((thisVal->myValue()).c_str()) + 1 ];
		 strcpy( keyTag, thisVal->myValue().c_str());
	     }
	     else if ( strcmp ( (*(*orig_names)[0])[curCol]->myName().c_str(), "_key_value") == 0 ){
		 keyValue = new char[ strlen((thisVal->myValue()).c_str()) + 1 ];
		 strcpy( keyValue, thisVal->myValue().c_str());
	     }
	     else {
                 char * tagNull = new char[strlen(thisVal->myValue().c_str()) + 1];
		 strcpy(tagNull, thisVal->myValue().c_str());
	         tagNulls->AddToEnd(tagNull);
	     }
	 }
	 if( tagNulls->size() == 0 ) {
	     tagNulls->AddToEnd(defNull1);
	     tagNulls->AddToEnd(defNull2);
	 }
	 //Find the saveframe in AST with keyTag and keyValue.
	 List<ASTnode *>*saveFrameHits;
	 if( keyTag == string("*") )
	     saveFrameHits = AST->searchForType(ASTnode::SAVEFRAMENODE);
	 else
	     saveFrameHits = AST->searchForTypeByTagValue(ASTnode::SAVEFRAMENODE, keyTag, keyValue);
	 ASTnode *curSaveFrame;
	 if(saveFrameHits->size() == 0){
	     (*errStream)<<"#transform-12# ERROR: Save frame named "<<keyTag
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
		     (*errStream) <<"#transform-12# ERROR: DataLoopNode with "
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
			  //Go through row by row.
			  for(int row=0; row<rowNum; row++){
			      int column =0;
			      bool is_delete = true;
			  
                              for(int position=0; position<tagNames->size(); position++){
                                  bool is_found = false;
				  for(int col = 0; col<colNum; col++){
                                       if( strcmp( (*tagNames)[position], (*(*dataNames)[0])[col]->myName().c_str() ) == 0){
			                  column = col;
				          is_found = true;
				          break;
					}
				        if (is_found = false){
                                           (*errStream)<<"#transformation-12# ERROR: "
					               << (*tagNames)[position] <<" is not found in input file"<<endl;
			                }
			          }
				
                                 if (  is_found ) {
				     is_delete = false;
				     //   (*errStream)<<"#transformation-12# eraseme: checking found value " << (*(*matrix)[row])[column]->myValue().c_str() << " against null types:"<<endl;
				     for( int null_i = 0 ; null_i < tagNulls->size() ; null_i++ ) {
				         //   (*errStream)<<"#transformation-12# eraseme: checking against " <<(*tagNulls)[null_i] <<endl;
				         if( (*tagNulls)[null_i][0] =='!' ) {
					     if( strcmp( (*(*matrix)[row])[column]->myValue().c_str(), (*tagNulls)[null_i]+1 ) != 0 ) {
				                 //   (*errStream)<<"#transformation-12# eraseme: Found it - null because it does not match "<<(*tagNulls)[null_i]+1 <<endl;
						 is_delete = true;
						 break;
					     }
					 }
				         else {
					     if( strcmp( (*(*matrix)[row])[column]->myValue().c_str(), (*tagNulls)[null_i] ) == 0 ) {
				                 //   (*errStream)<<"#transformation-12# eraseme: Found it - null because it does match "<<(*tagNulls)[null_i]<<endl;
						 is_delete = true;
						 break;
					     }
					 }
				     }
				     if( is_delete ) {
				         break;
				     }
				 }
		              }
			      if(is_delete == true ){
				 todelete->AddToEnd(row);						
			      }
			  }
			  
			  if( todelete->size() != 0){
			             // Be sure to sort the list in inverse order from largest row number
				     // to smallest so the deletions will occur without the index-shifting
				     // bug:
				     List<int> *index= new List<int>;
				     int tmp = 0;
				    
							// was this:
							// for(int g=0; g<todelete->size(); g++){
						    // 	 tmp = (*todelete)[g];
						    // 	 if( g == 0 ){ index->AddToEnd( tmp); }
						    // 	 else{
						    // 	    index->AddToEnd( tmp-1 );
						    // 	 }
				    // Put the items into biggest-to-smallest order and remove duplicates:
				    bool skipThisOne = false;
				    for( int g = 0 ; g<todelete->size() ; ++g ) {
				        skipThisOne = false;
				        tmp = (*todelete)[g];
					List<int>::iterator i;
					for( i = index->end() ; i != index->begin(); --i ) {
					    if( tmp == *(i-1) ) {
					        skipThisOne = true; // avoid duplicate row deletions.
				 	        break;
					    }
					    else if( tmp < *(i-1) ) {
					        break;
					    }
					}
					if( ! skipThisOne ) {
					    index->insert( i, tmp );
					}
				    }
                                    ASTnode *nmrTagPeer = (*loopHits)[j]->myParallelCopy();
				    LoopTableNode *peerTable = ((DataLoopNode *)nmrTagPeer)->getValsPtr();
				    if ( index->size() == rowNum ){//Delete the whole loop
                                        delete  ((DataLoopNode *)nmrTagPeer);
				    }
				    else{
				     for(int h=0; h<index->size(); h++){
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

