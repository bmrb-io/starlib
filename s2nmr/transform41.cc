///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.3  2005/03/04 23:32:05  madings
checking in doc changes and a lot of incremental bugfixes
too many to mention (cvs repository was broken and confused
for a while so this represents about 5 months of stuff)

Revision 1.2  2000/11/18 03:02:39  madings
*** empty log message ***

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_rename_saveframes(StarFileNode *AST, 
                            StarFileNode *NMR, 
                            BlockNode    *,
			    SaveFrameNode *currDicRuleSF)
{
   string currDicSaveFrameName = string("save_remove_saveframes");
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame)//Return if the target saveframe is not found
       return; 
   //Get the loop in the data dictionary
   string currDicLoopName = string("_frame_name");
   DataNode* currDicLoop = 
       ((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(currDicLoopName);

   List<DataNameNode*>* currDicNameList;
   List<DataValueNode*>* currDicValList;
   ((DataLoopNode*)currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);
   int length =  currDicNameList->Length();  
   int frameTag = 0;
   int keyTag = 0;
   int keyValue = 0;
   int newName = 0;
   int newNameSuffix = 0;
   
   for(int position = 0 ; position < currDicNameList->size() ; position++ )
   {
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_frame_name") == 0 )
          frameTag = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_key_tag") == 0 )
          keyTag = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_key_value") == 0 )
          keyValue = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_new_frame_name") == 0 )
          newName = position;
   }
   currDicValList->Reset();
   string  *iterVal = new string[length];

   string oldName;
   while(!currDicValList->AtEnd())
   {
       oldName = string("");
       //Get the values for one loop iteration
       for(int i=0; i<length; i++)
       {
	   iterVal[i] = currDicValList->Current()->myName();
	   currDicValList->Next();
       }
       //Apply this to the relevant saveframes within this datablock
       //
       // Several cases for the different types of asterisked fields:
       //
       // First, list all the saveframes in the whole file as a starting point:
       //
       List<ASTnode *> *saveFrameHits =AST->searchForType(ASTnode::SAVEFRAMENODE);
       List<ASTnode *> *tmpHits = NULL ;

       // Then, replace with only those that match the _frame_name:
       int idx;
       if( strcmp( iterVal[frameTag].c_str(), "*" ) == 0 )
       { } // Do nothing
       else
       {   // Tag as null those saveframes that are not to be removed:
	   for( idx = 0 ; idx < saveFrameHits->size() ; idx++ )
	   {
	       if( (*saveFrameHits)[idx] != NULL )
	       {
		   tmpHits = (*saveFrameHits)[idx]->searchForTypeByTag(
			       ASTnode::SAVEFRAMENODE, iterVal[frameTag] );
		   if( tmpHits->size() <= 0 )
		   {
		       (*saveFrameHits)[idx] = NULL;
		   }
		   delete tmpHits;
	       }
	   }
       }

       // Then, replace with only those that match the tag/value pair:
       if( strcmp( iterVal[keyTag].c_str(), "*" ) == 0 )
       { } // Do nothing.
       else
       {   // Tag as null those saveframes that are not to be removed:
	   for( idx = 0 ; idx < saveFrameHits->size() ; idx++ )
	   {
	       if( (*saveFrameHits)[idx] != NULL )
	       {
		   tmpHits = (*saveFrameHits)[idx]->searchForTypeByTagValue(
			      ASTnode::SAVEFRAMENODE, iterVal[keyTag], iterVal[keyValue]);
		   if( tmpHits->size() <= 0 )
		   {
		       (*saveFrameHits)[idx] = NULL;
		   }
		   delete tmpHits;
	       }
	   }
       }
       // Now, saveFrameHits consists of nothing but the hit saveframes.

       if(saveFrameHits->Length() == 0){
	   (*errStream)<< "#transform-41# ERROR: No Saveframes found for criteria: "
	               << "name,tag,val = " << iterVal[frameTag] << ", "
		       << iterVal[keyTag] << ", " << iterVal[keyValue] << endl;
       }
       else
       {
           // For each saveframe, remove its peer.
           for(saveFrameHits->Reset(); !saveFrameHits->AtEnd(); saveFrameHits->Next())
	   {
               ASTnode *curSaveFrame;
	       ASTnode *nmrPeer;

               curSaveFrame = saveFrameHits->Current();

	       if( curSaveFrame != NULL )
	       {
		   nmrPeer = curSaveFrame->myParallelCopy();
		   if (nmrPeer !=  NULL)
		   {
		       string oldName = ((SaveFrameNode *)nmrPeer)->myName();
		       string newsfName = iterVal[newName];
		       ((SaveFrameNode *)nmrPeer)->changeName( iterVal[newName] );

		       List<ASTnode*> *framecodes = 
			  NMR->searchForType( ASTnode::DATAVALUENODE, DataValueNode::FRAMECODE );
		       if( strncmp( oldName.c_str(), "save_", 5 ) == 0 )
			  oldName = string( oldName.c_str()+5 );
		       if( strncmp( newsfName.c_str(), "save_", 5 ) == 0 )
			  newsfName = string( newsfName.c_str()+5 );
		       int j;
		       int strl;
		       int framecode_idx;
		       for( framecode_idx = 0 ; 
			    framecode_idx < framecodes->size() ;
			    framecode_idx++ )
		       {
			   DataValueNode *dvn = (DataValueNode*)(*framecodes)[framecode_idx];
			   strl = strlen( dvn->myValue().c_str() );
			    
			   // string equal with ignore case:
			   for( j = 0 ; j < strl ; j++ )
			   {   if( toupper( ( dvn->myValue().c_str() )[j]) != toupper(oldName[j]) )
				  break;
			   }
			   if( j == strl ) // they were equal;
			       dvn->setValue( newsfName );
		       }
		       delete framecodes;
		   }
	       }
	   }
       }
       delete saveFrameHits;
   }
}

