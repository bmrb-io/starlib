///////////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//$Log: not supported by cvs2svn $
//Revision 1.5  2001/06/18 16:57:09  madings
//Several fixes for s2nmr segfault bugs.
//
//Revision 1.4  2000/11/18 03:02:39  madings
//*** empty log message ***
//
// Revision 1.3  2000/03/20  09:45:49  madings
// Weekend fixes to s2nmr, to respond to some of Jurgen's comments:
// Note, the old s2nmr program is under the name "/bmrb/bin/s2nmr-old", just
// in case you discover a major bug and want to get the old version back.
//
// 1 - The max number allowed for a saveframe's order sequence used to be
// 999.  This was too small.  It has been increased to 9999999, and documented
// acccordingly in specs.html
//
// 2 - Documented that only one middle initial is recognized when
// parsing names from strings.
//
// 3 - Numerous places in the code that created new values did so without
// the proper delimiter type  (For example, it might put a multi-line
// value into a quoted string ("...") instead of a semicolon string.)
// Jurgen noticed this on the citation title, but when I looked for it
// in the code, I found that particular mistake was common throughout the
// newer rules that came after transform # 17.  This has been fixed.
//
// 4 - rule 19, change data_tag_name, did not work as expected:
// I looked at the code and this was simply a case of bad documentation.
// What this rule is meant to do is look for the _BMRB_accession_number
// tag somewhere in the file and use its value as the name of the data_
// tag.  That's all it does.  If a _BMRB_accession_number tag is not there,
// it fails to do anything.  The documentation has been changed to properly
// describe this.
//
// 5 - "rule 22 is unclear to me" - I have re-written explanation of this
// rule, and fixed a bug in the code that refused to operate on multiple
// target saveframes.  (If the key/value pair designated more than one
// output saveframe, it would fail.)
//
// 6 - "rule 25 change_tag_value has backward documenation".  Looked at
// the code to see what it does, and the code is really buggy.  A redo
// from scratch would be quicker than trying to fix it.  The documentation
// isn't backward - the code is.  It uses the wrong variables for things,
// but not in a consistant manner, so I can't just quickly swap things
// around.  Also, we need to define what should be the correct behaviour
// when multiple saveframes are found by either of the two key_tag/value
// searches.  If two input saveframes are found, and two output saveframes
// are found, then is the correct behaviour to copy input 1 to output 1 and
// input 2 to output 2?  Or is the correct behaviour to copy in a 'cross
// product' fashion and copy input 1 to both outputs 1 and 2, and copy
// input 2 to both outputs 1 and 2?  Issues of possible name clashes need
// to be considered too with this.  In light of this, I'm going to put off
// working on this rule untill I am sure it is worth the time, and this
// ambigious behaviour is defined better.  I have to be careful not to spend
// too much time on this program.
//
// 7 - Fixed the broken condition where output_sf_name_tag is null (.) in
// values_to_saveframes_many_to_many.
//
// 8 - Repaired a few typos here and there.
//
// CVS----------------------------------------------------------------------
//
// Revision 1.2  1999/07/30  18:22:08  june
// june, 7-30-99
// add transform1d.cc
//
//Revision 1.1  1999/06/18 21:15:20  zixia
//*** empty log message ***
//
#endif
////////////////////////////////////////////////////////////////////   

#include "transforms.h"


void save_insert_loop_defaults_II( StarFileNode *input, 
                     StarFileNode  *output, 
                     BlockNode     *currDicDataBlock,
		     SaveFrameNode *currDicRuleSF )
{
   string currDicSaveFrameName = string("save_insert_loop_defaults_II");
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
   int positionTag = 0; //The column number where _position_tag is in the loop row.
   
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
      if(  strcmp( (*(*mapNames)[0])[i]->myName().c_str(), "_position_tag") == 0)
      {	  positionTag = i;
	  continue;
      }
      (*errStream)<<"#transform-35# ERROR: Never should be here."<<endl;
   }

   string key_tag;
   string key_value;
   string trigger_tag;
   string trigger_value;
   string insert_code;
   string which_loop;
   string position_tag;
   
   //Go through the map loop row by row and apply the transformation.
   for( int row = 0; row <rowCount; row++){
       //Fill the value for key_tag, key_value, trigger_tag, trigger_value, insert_code.
       key_tag       =  (*(*maptable)[row])[keyTag]->myValue();
       key_value     =  (*(*maptable)[row])[keyValue]->myValue();
       trigger_tag   =  (*(*maptable)[row])[triggerTag]->myValue();
       trigger_value =  (*(*maptable)[row])[triggerValue]->myValue();
       insert_code   =  (*(*maptable)[row])[insert]->myValue();
       which_loop    =  (*(*maptable)[row])[whichLoop]->myValue();
       position_tag  =  (*(*maptable)[row])[positionTag]->myValue();
       
       //Find the save frame.
       List<ASTnode *> *saveFrameHits = input->searchForTypeByTagValue(ASTnode::SAVEFRAMENODE, key_tag, key_value);
       if( saveFrameHits->size() == 0){
            (*errStream)<<"#transform-35# ERROR: Cannot find a saveframe where tag "<<key_tag
		        <<" = "<<key_value<<" in input file."<<endl; 
       }
       else{
          //For each matched save frame, apply the transformation.
          SaveFrameNode * currSaveFrame;
          for(int frameno = 0; frameno < saveFrameHits->size(); frameno++){
	      currSaveFrame = (SaveFrameNode *)((*saveFrameHits)[frameno]);
	       //Find the default tag and value.
              List<ASTnode *> *tagHits = currSaveFrame->searchByTag(trigger_tag);
	      if (tagHits->size() != 1 && strcmp(trigger_tag.c_str(),"*") != 0 ){
                 (*errStream)<<"#transform-35# ERROR: No or more than one tags = "
		     <<trigger_tag<<" is found."<<endl;
	      }
	      else{
                  //Check if the value is equal to the trigger value.
		  DataItemNode *curTag = NULL;
		  if( tagHits->size() > 1 )
		      curTag = (DataItemNode *)((*tagHits)[0]);
		  if(  strcmp(trigger_tag.c_str(),"*") == 0 ||
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
                       (*errStream)<<"#transform-35# ERROR: Cannot find a saveframe"
			   <<" with name = "<<frameName<<" in map file."<<endl;
			break;
		      }
		      insert_inside =  ((SaveFrameNode *)insertSaveFrame)->GiveMyDataList();
		      if ( insert_inside->size() != 1 ){
                          (*errStream)<<"#transform-35# ERROR: No or more than one data loop "
			      <<"in save frame "<<frameName<<endl;
			  break;
		      }
		      insertLoop = (DataLoopNode*)((*insert_inside)[0]);
		      insertTable = insertLoop->getValsPtr();
		      insertTagNames = insertLoop->getNamesPtr();
		      
		      //Find the loop in input where the insertion will happen. 
                      List<ASTnode*> *astLoop = input->searchForTypeByTag(ASTnode::DATALOOPNODE, which_loop);
		      if( astLoop->size() == 0 ){
		         (*errStream)<<"#transform-35# ERROR: No loop with tag name "
			     <<which_loop<<" found in input file. "<<endl;
			 break;
		      }

		      for(int numLoop = 0; numLoop< astLoop->size(); numLoop++){
			  char quote_style;
                          DataLoopNode * cur = (DataLoopNode *)((*astLoop)[numLoop]); 
			  ASTnode * nmrPeer = cur->myParallelCopy();
                          //Insert row by row into the parallele copy.
			  LoopTableNode *peerTable;
			  LoopRowNode   *newRow;
			  peerTable = ((DataLoopNode *)nmrPeer)->getValsPtr();
			  for(int i=0; i<insertTable->size(); i++){
                              newRow = new LoopRowNode(true);
			      for(int j =0; j<(*insertTagNames)[0]->size(); j++){

				   newRow->insert(newRow->end(), new DataValueNode(
					             (*(*insertTable)[i])[j]->myValue(),
						     (*(*insertTable)[i])[j]->myDelimType() ) );
			      }
			      // Added this error check to give a useful message if there is a mismatch:
			      DataLoopNameListNode *peerNames = ((DataLoopNode*)nmrPeer)->getNamesPtr();
			      if( (*peerNames)[0]->size() != newRow->size() )
			      {
				  (*errStream)<<"#transform-35# ERROR: Trying to insert a row with " <<
				      newRow->size() << " values into a loop with " <<
				      (*peerNames)[0]->size() << " values." << endl;
				  ostream *os_remember = os;
				  os->flush(); os = errStream;  // To redirect Unparse() to the error log
				  (*os) << "\tWas trying to insert this row: " << endl;
				  newRow->Unparse(4); (*os)<<endl;
				  (*os) << "\tInto a loop with the following definition: " << endl;
				  peerNames->Unparse(4); (*os)<<endl;
				  os->flush(); os = os_remember;  // To direct Unparse() back where it belongs.
			      } 
			    
			      //If the _position_tag is "F", then insert the new row as the first row.
			      //If the _position_tag is "L", then insert the new row as the last row.
			      if( strcasecmp(position_tag.c_str(), "F") == 0){
			           peerTable->insert( peerTable->begin(), newRow);
			      }else if (strcasecmp(position_tag.c_str(), "L")==0){
                                   peerTable->insert( peerTable->end(), newRow);
			      }
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
