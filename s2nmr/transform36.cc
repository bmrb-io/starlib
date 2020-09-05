///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.3  1999/07/30  18:22:08  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.2  1999/06/23 20:54:21  zixia
A bug (garbage characters at the end of _Enzyme_commission_number) is fixed.

// Revision 1.1  1999/06/18  21:15:33  zixia
// *** empty log message ***
//
*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_create_enzyme_commission_number( StarFileNode *inStar, 
                     StarFileNode *outStar, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF )
{
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame){//Return if the target saveframe is not found
      return;
   }

   List<ASTnode*>        *matchSF;
   List<ASTnode*>        *matchLoop;
   LoopTableNode         *valTable;
   DataLoopNameListNode  *nameListList;
   DataItemNode          *dataItemPeer;
   char                  *value = new char[1000]; 
   char                  *finalValue = new char[1000];
       
   int rowCount, colCount;
   int Classification_system_name_pos = -1;
   int Classification_system_code_pos = -1;
 
   DataLoopNode          *loopPeer;
   SaveFrameNode         *SFPeer;
   List<DataNode*>       *sf_insides;
   
   //Find the saveframe which contains (_Saveframe_category, monomeric_polymer) 
   matchSF = inStar->searchForTypeByTagValue(
	             ASTnode::SAVEFRAMENODE,
		     string("_Saveframe_category"),
		     string("monomeric_polymer") );

   if(matchSF->size()!= 1){
      (*errStream)<<"#transform-36# ERROR: only one saveframe contains tag and value pair(_Saveframe_category, monomeric_polymer"
	  <<" should be found."<<endl;
      delete matchSF;
      return; /* do nothing */
   }

   //Find the loop with tag _Classification_system_name
   matchLoop = (*matchSF)[0]->searchForTypeByTag( ASTnode::DATALOOPNODE, string("_Classification_system_name") );  

   if(matchLoop->size()!= 1){
       (*errStream) <<"#transform-36# ERROR: only one loop with tag \"_Classification_system_name\" should be found."<<endl;
       delete matchSF;
       delete matchLoop;
       return; /* do nothing */
   }

   loopPeer = (DataLoopNode*)(*matchLoop)[0]->myParallelCopy();
   SFPeer   = (SaveFrameNode*)(*matchSF)[0]->myParallelCopy();
   sf_insides = SFPeer->GiveMyDataList();
  
   nameListList = loopPeer->getNamesPtr();
   valTable     = loopPeer->getValsPtr();
   rowCount = valTable->size();
   colCount = (*nameListList)[0]->size();
   
   //Find the value for _Classification_system_name.
   for(int i=0; i<colCount; i++){
          if( (*((*nameListList)[0]) )[i]->myName() == string("_Classification_system_name") ){
              Classification_system_name_pos = i;
	  }
	  else if( (*((*nameListList)[0]) )[i]->myName() == string("_Classification_system_code") ){
	      Classification_system_code_pos = i;
	  }
   }

   if( Classification_system_name_pos < 0 ){
          (*errStream) <<"#transform-36# ERROR: input data file is missing tag '_Classification_system_name_pos'."
                       <<endl;
          return; /* do nothing */
   }
   if( Classification_system_code_pos < 0 ){
          (*errStream) <<"#transform-36# ERROR: input data file is missing tag '_Classification_system_code'."
                       <<endl;
          return; /* do nothing */
   }
   
   //Clean the value.
   strcpy(value, "");
  
   //Go through each row, find if there is a value 'Enzyme Commission'  for _Classification_system_name.
   //If such a value exists, then create a new tag _Enzyme_commission_number.
   //The value of the tag is the value of tag '_Classification_system_code' which is in the same row
   //as tag '_Classification_system_name'.
   for(int curRow=0; curRow<rowCount; curRow++){
        if( strcasecmp((*(*valTable)[curRow])[Classification_system_name_pos]->myValue().c_str(), "Enzyme Commission") == 0 ){
	    strcpy(value, (*(*valTable)[curRow])[Classification_system_code_pos]->myValue().c_str() );
	    strcpy(finalValue, "");
	    
	    
	    int j=0;//A index for finalValue.
	    for(int i=0; i<strlen(value); i++){
               if(isdigit(value[i]) || value[i]=='.'){
                  finalValue[j] = value[i];
		  j++;
	       }
	    }
	    
	    finalValue[j] = '\0';
	     
	    //Create a new DataItemNode.
            DataItemNode * newDataItem = new DataItemNode(string("_Enzyme_commission_number"), finalValue);
            //Insert the new DataItemNode into the saveframe		
	    sf_insides->insert(sf_insides->end(), newDataItem);
	    break;
	}
   }
   
   //Delete the loop. This is done by the function loops
      
   delete matchSF;
   delete matchLoop;
}
