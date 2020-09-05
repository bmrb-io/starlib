///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
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

void save_remove_null_and_question_tags(StarFileNode *AST, 
                           StarFileNode *NMR, 
                           BlockNode    *,
			   SaveFrameNode *currDicRuleSF)
{
   string currDicSaveFrameName = string("save_remove_null_tags");
   DataNode* currDicSaveFrame = currDicRuleSF;


   if(!currDicSaveFrame)//Return if the target saveframe is not found
       return; 
   int questionsToo;
   // Check to see if the question mark tag is on:
   List<ASTnode*>*matches = currDicSaveFrame->searchByTag("_question_marks_too");
   if( matches->size() > 0 )
       questionsToo = 1;
   else
       questionsToo = 0;
   if( matches != NULL ) delete matches;
   //Get the loop in the data dictionary
   string currDicLoopName = string("_key_tag");
   DataNode* currDicLoop = 
       ((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(currDicLoopName);

   List<DataNameNode*>* currDicNameList;
   List<DataValueNode*>* currDicValList;
   ((DataLoopNode*)currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);
   int length =  currDicNameList->Length();  
   int keyTag = 0;
   int keyValue = 0;
   int oldTagName = 0;
   
   for(int position = 0 ; position < currDicNameList->size() ; position++ )
   {
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_key_tag") == 0 )
          keyTag = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_key_value") == 0 )
          keyValue = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_old_tagname") == 0 )
          oldTagName = position;
   }
   currDicValList->Reset();
   string  *iterVal = new string[length];

   while(!currDicValList->AtEnd())
   {
       //Get the values for one loop iteration
       for(int i=0; i<length; i++)
       {
	   iterVal[i] = currDicValList->Current()->myName();
	   currDicValList->Next();
       }

       //Apply this change to the relevant saveframes within this datablock
       //Retrieve the appropriate saveframe containing tag to be changed
       //
       // Several cases for the different types of asterisked fields:
       List<ASTnode *> *saveFrameHits = NULL;
       if( strcmp( iterVal[keyTag].c_str(), "*" ) == 0 )
	   saveFrameHits = AST->searchForType(ASTnode::SAVEFRAMENODE);
       else
	   saveFrameHits = AST->searchForTypeByTagValue(ASTnode::SAVEFRAMENODE, iterVal[keyTag], iterVal[keyValue]);

       ASTnode *curSaveFrame;
       if(saveFrameHits->Length() == 0){
           /* JFD elaboration and correction of error message WAS:
	   (*errStream)<<"#transform-2# ERROR: Save frame named "<<iterVal[keyTag]
	               <<" with value of "<<iterVal[keyValue]<<" is not found."<<endl;
            */
	   (*errStream) <<"#transform-13# ERROR: No saveframe(s) containing tag name: "
                        <<iterVal[keyTag]
	                <<" with value of: "<<iterVal[keyValue]
                        <<" found."<<endl;
       }
       else{
           //For each saveframe, apply the transformation.
	   int count_sf_with_hits = 0;
           for(saveFrameHits->Reset(); !saveFrameHits->AtEnd(); saveFrameHits->Next())
	   {
             curSaveFrame = saveFrameHits->Current();
	     //Find tags with name iterVal[2].
	     List<ASTnode*> *tagHits;
	     if( iterVal[oldTagName] == string("*") )
	     {
		 tagHits = ((SaveFrameNode *)curSaveFrame)->searchForType(ASTnode::DATANODE); 
	     }
	     else
		 tagHits = ((SaveFrameNode *)curSaveFrame)->searchByTag(iterVal[oldTagName]); 
	     if( tagHits->Length() == 0 ){
		 // do nothing.
	     }
	     else{
		 count_sf_with_hits++;
	      
               for(tagHits->Reset(); !tagHits->AtEnd(); tagHits->Next())
	       {
		   DataItemNode *curTag = (DataItemNode *)tagHits->Current();
		   ASTnode * astTagCur = tagHits->Current();
		   ASTnode *nmrTagPeer;
		     
		   if( astTagCur->isOfType(ASTnode::DATAITEMNODE) && 
		          ( strcmp( (curTag->myValue()).c_str(), ".") == 0 ||
			    strcmp( (curTag->myValue()).c_str(), "?") == 0  )  )
		   {
		     nmrTagPeer = astTagCur->myParallelCopy();
		     if (nmrTagPeer !=  NULL)
		       delete ((DataItemNode *)nmrTagPeer);
		   }
		   else
		   {
		 //Walk up the tree.
		       for(;((astTagCur !=NULL) && (astTagCur->myType()!=ASTnode::DATALOOPNODE));astTagCur=astTagCur->myParent())
			   ;
		       
		       bool is_delete = true;
		       int cnt = 0;
                       if(astTagCur != NULL){ 
			   List<DataNameNode*>* nameList;
                           List<DataValueNode*>* valueList;
                           ((DataLoopNode*) currDicLoop)->FlattenNestedLoop(nameList, valueList); 
			   cnt = nameList->Length();
                          
			   List<DataValueNode *>* datalist = ((DataLoopNode *)astTagCur)->returnLoopValues(iterVal[2]);

			  for(datalist->Reset(); !datalist->AtEnd(); datalist->Next())
			  {
			      if( strcmp( (datalist->Current()->myValue()).c_str(), ".") != 0 &&
			          strcmp( (datalist->Current()->myValue()).c_str(), "?") != 0  ){
                                    is_delete = false;
			      }
			  }
		       }
		       if(astTagCur != NULL && is_delete == true )//Delete the colome only.
		       {
			   nmrTagPeer = astTagCur->myParallelCopy();
			   if(nmrTagPeer != NULL)
			   {   ((DataLoopNode *)nmrTagPeer)->RemoveColumn(iterVal[oldTagName]);
			   }
			   bool is_empty = false;
			   if( ((DataLoopNode*)nmrTagPeer)->getNamesPtr()->size() == 0 )
			   {   is_empty = true;
			   }
			   else if( ((DataLoopNode*)nmrTagPeer)->getNames()[0]->size() == 0 )
			   {   is_empty = true;
			   }
			   if( is_empty )
			   {   delete ((DataLoopNode *)nmrTagPeer);
			   }
		       }
		   }
	       }
	     }
	     delete tagHits;
	   }
	   if( count_sf_with_hits <= 0 )
	   { /* JFD correction of error message */
	       (*errStream) <<"#transform-13# ERROR: tag with name of "
                            <<iterVal[oldTagName]<<" is not found."<<endl;
	   }
       }
       delete saveFrameHits;
   }
}

