///////////////////////////////////// RCS LOG //////////////////////////////
//
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.9  2003/07/24 18:49:13  madings
Changes to handle 2.1-3.0 mappings.

Revision 1.8  2001/06/18 16:57:08  madings
Several fixes for s2nmr segfault bugs.

Revision 1.7  2001/04/13 21:17:07  madings
Changes to use the new starlib2.
Also discovered an error in transform10 as well.

Revision 1.6  2000/11/18 03:02:36  madings
*** empty log message ***

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"


void save_insert_loop_defaults( StarFileNode *input, 
                     StarFileNode  *output, 
                     BlockNode     *currDicDataBlock,
		     SaveFrameNode *currDicRuleSF )
{
   string currDicSaveFrameName = string("save_insert_loop_defaults");
   DataNode* currDicSaveFrame = currDicRuleSF;


   if(!currDicSaveFrame){//Return if the target saveframe is not found
      return;
   }
   
   //Get the loop in the map file
   DataNode* currDicLoop =
       ((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(string("_key_tag"));

   LoopTableNode *maptable = ((DataLoopNode *)currDicLoop)->getValsPtr();
   DataLoopNameListNode *mapNames = ((DataLoopNode *)currDicLoop)->getNamesPtr();

   int rowCount = maptable->size();
   int colCount = (*mapNames)[0]->size();

   int keyTag = 0;//The column number where _key_tag is in the loop row.
   int keyValue = 0;//The column number where _key_value is in the loop row.
   int triggerTag = 0;//The column number where _trigger_tag is in the loop row.
   int triggerValue = 0;//The column number where _trigger_value is in the loop row.
   int insert = 0;//The column number where _insert_framecode is in the loop row.
   int whichLoop = 0; //The column number where _whichloop_tag is in the loop row.
   //Find the position for above tag.
   for( int i=0; i<colCount; i++){
      if(  strcmp( (*(*mapNames)[0])[i]->myName().c_str(), "_key_tag") == 0)
      {	  keyTag = i;
	  continue;
      }
      if(  strcmp( (*(*mapNames)[0])[i]->myName().c_str(), "_key_value") == 0)
      {	  keyValue = i;
	  continue;
      }
      if(  strcmp( (*(*mapNames)[0])[i]->myName().c_str(), "_trigger_tag") == 0)
      {	  triggerTag = i;
	  continue;
      }
      if(  strcmp( (*(*mapNames)[0])[i]->myName().c_str(), "_trigger_value") == 0)
      {	  triggerValue = i;
	  continue;
      }
      if(  strcmp( (*(*mapNames)[0])[i]->myName().c_str(), "_insert_framecode") == 0)
      {	  insert = i;
	  continue;
      }
      if(  strcmp( (*(*mapNames)[0])[i]->myName().c_str(), "_whichloop_tag") == 0)
      {	  whichLoop = i;
	  continue;
      }
      (*errStream)<<"#transform-10# ERROR: Never should be here."<<endl;
   }

   string key_tag;
   string key_value;
   string trigger_tag;
   string trigger_value;
   string insert_code;
   string which_loop;

#ifdef DEBUG_RULES_PROGRESS
   int numDicRows =  (*(currDicRuleSF->GiveMyDataList()))[1]->getValsPtr()->size();
   int curDicRowNum = 0;
#endif
   
   //Go through the map loop row by row and apply the transformation.
   for( int row = 0; row <rowCount; row++){
#ifdef DEBUG_RULES_PROGRESS
       cerr<<"Rule "<<++curDicRowNum<<" out of "<<numDicRows<<"."<<endl;
#endif
       //Fill the value for key_tag, key_value, trigger_tag, trigger_value, insert_code.
       key_tag       =  (*(*maptable)[row])[keyTag]->myValue();
       key_value     =  (*(*maptable)[row])[keyValue]->myValue();
       trigger_tag   =  (*(*maptable)[row])[triggerTag]->myValue();
       trigger_value =  (*(*maptable)[row])[triggerValue]->myValue();
       insert_code   =  (*(*maptable)[row])[insert]->myValue();
       which_loop    =  (*(*maptable)[row])[whichLoop]->myValue();
       //Find the save frame.
       List<ASTnode *> *saveFrameHits = input->searchForTypeByTagValue(ASTnode::SAVEFRAMENODE, key_tag, key_value);


       if( saveFrameHits->size() == 0){
            (*errStream)<<"#transform-10# ERROR: Cannot find a saveframe where tag "<<key_tag
		        <<" = "<<key_value<<" in input file."<<endl; 
       }
       else{
          //For each matched save frame, apply the transformation.
          SaveFrameNode * currSaveFrame;
          for(int frameno = 0; frameno < saveFrameHits->size(); frameno++){
	      currSaveFrame = (SaveFrameNode *)((*saveFrameHits)[frameno]);
	       //Find the default tag and value.
              List<ASTnode *> *tagHits = currSaveFrame->searchByTag(trigger_tag);
	      if (tagHits->size() != 1 && strcmp( trigger_tag.c_str(), "*") != 0 ){
                 (*errStream)<<"#transform-10# ERROR: No or more than one tags = "
		     <<trigger_tag<<" is found."<<endl;
	      }
	      else{
                  //Check if the value is equal to the trigger value.
		  DataItemNode *curTag = NULL;
		  if( tagHits->size() >= 1 )
		      curTag = (DataItemNode *)((*tagHits)[0]);
		  if(  strcmp(trigger_value.c_str(), "*" ) == 0 ||
		       strcmp(curTag->myValue().c_str(), trigger_value.c_str()) == 0 ){
		      //Find the insert loop in map file.
		      char frameName[500] = "save_";
		      DataNode* insertSaveFrame;
		      List<DataNode*> *insert_inside;
		      DataLoopNode * insertLoop;
		      LoopTableNode * insertTable;
		      DataLoopNameListNode * insertTagNames;
		      
		      strcat(frameName, insert_code.c_str());
		      insertSaveFrame = currDicDataBlock->ReturnDataBlockDataNode(frameName);
		      if (!insertSaveFrame){
                       (*errStream)<<"#transform-10# ERROR: Cannot find a saveframe"
			   <<" with name = "<<frameName<<" in map file."<<endl;
			break;
		      }
		      insert_inside =  ((SaveFrameNode *)insertSaveFrame)->GiveMyDataList();
		      if ( insert_inside->size() != 1 ){
                          (*errStream)<<"#transform-10# ERROR: No or more than one data loop "
			      <<"in save frame "<<frameName<<endl;
			  break;
		      }
		      insertLoop = (DataLoopNode*)((*insert_inside)[0]);
		      insertTable = insertLoop->getValsPtr();
		      insertTagNames = insertLoop->getNamesPtr();
		      
		      //Find the loop in input where the insertion will happen. 
                      List<ASTnode*> *astLoop = input->searchForTypeByTag(ASTnode::DATALOOPNODE, which_loop);
		      if( astLoop->size() == 0 ){
		         (*errStream)<<"#transform-10# ERROR: No loop with tag name "
			     <<which_loop<<" found in input file. "<<endl;
			 break;
		      }

		      for(int numLoop = 0; numLoop< astLoop->size(); numLoop++){
                          DataLoopNode * cur = (DataLoopNode *)((*astLoop)[numLoop]); 
			  ASTnode * nmrPeer = cur->myParallelCopy();
                          //Insert row by row into the parallel copy.
			  LoopTableNode *peerTable;
			  LoopRowNode   *newRow;
			  peerTable = ((DataLoopNode *)nmrPeer)->getValsPtr();
			  vector<int> columnMap;

			  // Build up a mapping of one tag name to another, in case they
			  // are in different columns.  The resulting loop will have the
			  // union of the set of  tags of the original loop and the inserted
			  // loop.
			  columnMap.clear();
			  int i, j, k;
			  DataLoopNameListNode *peerNames = ((DataLoopNode*)nmrPeer)->getNamesPtr();

			  // For each column name in the mapper's loop:
			  for( j = 0 ; j < (*insertTagNames)[0]->size() ; j++ ) {
			      // Find the same name in the output loop's header
			      // and record the index of it in the mapper:
			      for( k = 0 ; k < (*peerNames)[0]->size() ; k++ ) {
			          if( (*(*insertTagNames)[0])[j]->myName() == (*(*peerNames)[0])[k]->myName() ) {
				      columnMap.insert( columnMap.end(), k );
				      break;
				  }
			      }
			      // If the name was not in the output loop, add a column
			      // for it and then add that to the mapper:
			      if( k == (*peerNames)[0]->size() ) {
				  // This should have the side effect of inserting nill
				  // values into the associated rows already there.
				  (*peerNames)[0]->insert( (*peerNames)[0]->end(),
				                     new DataNameNode( (*(*insertTagNames)[0])[j]->myName() ) );
				  columnMap.insert( columnMap.end(), k );
			      }
			  }
			  for(i=0; i<insertTable->size(); i++){
                              newRow = new LoopRowNode(true);
			      //Make the row have the right number of columns:
			      for( j =0; j<(*peerNames)[0]->size(); j++){
			         newRow->insert( newRow->end(), new DataValueNode( string(".") ) );
			      }
			      // Then fill it.
			      for( j =0; j<(*insertTagNames)[0]->size(); j++){
				 (*newRow)[ columnMap[j] ]->setValue( (*(*insertTable)[i])[j]->myValue() );
				 (*newRow)[ columnMap[j] ]->setDelimType( (*(*insertTable)[i])[j]->myDelimType() );
			      }
			      // Added this error check to give a useful message if there is a mismatch:
			      if( (*peerNames)[0]->size() != newRow->size() )
			      {
				  (*errStream)<<"#transform-10# ERROR: Trying to insert a row with " <<
				      newRow->size() << " values into a loop with " <<
				      (*peerNames)[0]->size() << " values." << endl;
				  ostream *os_remember = os;
				  os->flush(); os = errStream;  // To redirect Unparse() to the error log
				  (*os) << "#\tWas trying to insert this row: " << endl;
				  newRow->Unparse(4); (*os)<<endl;
				  (*os) << "#\tInto a loop with the following definition: " << endl;
				  peerNames->Unparse(4); (*os)<<endl;
				  os->flush(); os = os_remember;  // To direct Unparse() back where it belongs.
			      }
			      peerTable->insert( peerTable->end(), newRow);
			  }
		      }
		      delete astLoop;
		  }else{//The trigger value is "no".
                      continue;
		  }
	      }
	      delete tagHits;
         }
       }
       delete saveFrameHits;
   }
   
}
