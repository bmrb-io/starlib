//########################################################################################
//########################################################################################


void save_all_tag_to_scope(StarFileNode* AST,
			   StarFileNode* NMR,
			   BlockNode* currDicDataBlock)
{
  
  //The function applies transformation 2 for a specified dictionary datablock
  //First get the "save_tag_to_value" saveframe in the current dictionary datablock
  //NOTE: this function assumes that for all saveframes to which this transformation applies
  //there CAN BE NO FREE TAGS AFTER loop tags.

  const int FREETAGMODE = 1;
  const int LOOPTAGMODE = 2;
  
  string currDicSaveFrameName = string("save_tag_to_scope");
  DataNode* currDicSaveFrame = currDicDataBlock->ReturnDataBlockDataNode(currDicSaveFrameName);

  if(!currDicSaveFrame)
    return;     //Simply return if the desire saveframe is not found
  


  //Get the loop in the data dictionary
  string currDicLoopName = string("_old_tag_name");
  DataNode* currDicLoop = ((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(currDicLoopName);
  
  //Flatten the nested loop
  List<DataNameNode*>* currDicNameList;
  List<DataValueNode*>* currDicValList;
  ((DataLoopNode*) currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);

  
  //Using the ScopeTagList class to collect scoped tags under appropriate saveframes.
  //This class is expressely designed to group all tags (which cause new saveframes) in the 
  //dictionary loop associated with a saveframe name (See class definition)

  string _old_tag_name;
  string _old_saveframe;
  ScopeTagList saveframeTagList;


  currDicValList->Reset();
  while(!currDicValList->AtEnd())
    {
      _old_tag_name =  currDicValList->Current()->myName();
      currDicValList->Next();
      _old_saveframe = currDicValList->Current()->myName();
      currDicValList->Next();
      saveframeTagList.Insert(tagToVal(_old_saveframe), tagToVal(_old_tag_name));
    }

  
//Now apply transformations to each saveframe in generic star as appropriate

List<BlockNode*>* listOfDataBlocks =  AST->GiveMyDataBlockList();

listOfDataBlocks->Reset();
while(!listOfDataBlocks->AtEnd())
  {
    //Process each data block
    DataNode* currDatablockItem;
    List<DataNode*>* datablockItemsList = listOfDataBlocks->Current()->GiveMyDataList();
    string currDatablockName = listOfDataBlocks->Current()->myName();

    datablockItemsList->Reset();
    while(!datablockItemsList->AtEnd())
      {
	  currDatablockItem = datablockItemsList->Current();
	  if( (currDatablockItem->myType() == DataNode::SAVETYPE))
	    {

	      string currSaveframeName = currDatablockItem->myName();
	      List<string>* scopeTagList = saveframeTagList.Lookup(currSaveframeName);
	      if(scopeTagList) //if indeed this is one of the saveframes to be altered
		{
		  //Apply transformation to this saveframe item
		  
		  //First remove the current saveframe from NMR tree
		  NMR->RemoveSaveFrame( currDatablockName,currSaveframeName); 


		  //Then get the list of saveframe items
		  List<DataNode*>* listOfSaveframeItems = ((SaveFrameNode*)currDatablockItem)->GiveMyDataList();
		  
		  //Process all items in a given saveframe
		  List<DataNode*>* origScopeDataItemList = new List<DataNode*>;
		  bool finished = false;
		  int state     = FREETAGMODE;
		  listOfSaveframeItems->Reset();
		  
                  DataNode* currSaveframeItem = listOfSaveframeItems->Current();
		  
		  //This loop processes ALL items in a saveframe to be split
		  while(!finished)
		    {
		   
		      if(currSaveframeItem->myType()== DataNode::LOOPTYPE)
			state = LOOPTAGMODE;
		      
		      if(state == FREETAGMODE)
			{
			  
			  if(!scopeTagList->Lookup(currSaveframeItem->myName()))
			     {
			     
			       DataItemNode* currItem = new DataItemNode(*((DataItemNode*)currSaveframeItem));
			       origScopeDataItemList->AddToEnd(currItem);
			     }
			  else  //if the current tag is a scoping tag
			    {
			      
			      //Create a saveframelist  node with the current list
			      SaveFrameListNode* l = new SaveFrameListNode(origScopeDataItemList);
			      SaveHeadingNode* h = new SaveHeadingNode(string("save_")+ tagToVal(((DataItemNode*)currSaveframeItem)->myValue()));
			      
			      //Create the new saveframe 
			      SaveFrameNode* s = new SaveFrameNode(h,l);
			 			 
			      //Start saving all the remaining items to this saveframe
			      listOfSaveframeItems->Next();
			      currSaveframeItem = listOfSaveframeItems->Current();

			      while(!finished)
				{
				  DataNode* currItem;
				  if(currSaveframeItem->myType() == DataNode::LOOPTYPE)
				    currItem = new DataLoopNode(*((DataLoopNode*)currSaveframeItem));
				  else
				    currItem = new DataItemNode(*((DataItemNode*)currSaveframeItem));

				  s->AddItemToSaveFrame(currItem);
				  
				  listOfSaveframeItems->Next();
				  if(!listOfSaveframeItems->AtEnd())
				    currSaveframeItem = listOfSaveframeItems->Current();
				  else
				    finished = true;
				}
			      //Add this saveframe to the NMR tree
			     
			      NMR->AddSaveFrameToDataBlock( currDatablockName,s);
			    }
			}
		      else // if state is LOOPTAGMODE
			{
			  //Process a loop item here...NOTE MUST BE A LOOP ITEM
			  
			  List<DataNameNode*>* attrNameList;
			  List<DataValueNode*>* attrValueList;
			  ((DataLoopNode*)currSaveframeItem)->FlattenNestedLoop(attrNameList,attrValueList);
			  
			  //Now to find out if this loop needs to be decomposed into saveframes
			  bool foundScopeTag  = false;
			  attrNameList->Reset();
			  string splitTag;
			  while(!foundScopeTag && !attrNameList->AtEnd())
			    {
			      splitTag = attrNameList->Current()->myName();
			      foundScopeTag  = scopeTagList->Lookup(splitTag);
			      attrNameList->Next();
			    }
			  if(foundScopeTag) 
			    {
			      
			      
			      attrValueList->Reset();
			      while(!attrValueList->AtEnd())
				{

				  List<DataNode*>*  newSaveFrameItemsList = new List<DataNode*>;
				  DataNameNode * currDataTag;
				  DataValueNode * currDataVal;
				  DataItemNode* currDataItem;

				  attrNameList->Reset();
				  string currTag = attrNameList->Current()->myName();
				  string currVal = attrValueList->Current()->myName();

				  while(currTag != splitTag)
				    {
				      //Create a new dataitem

				      currDataTag = new DataNameNode(currTag);
				      currDataVal = new DataValueNode(currVal);
				      currDataItem  = new DataItemNode(currDataTag, currDataVal);
				      
				      //Add this item to list
				      newSaveFrameItemsList->AddToEnd(currDataItem);
				      attrNameList->Next();
				      attrValueList->Next();
				      currTag = attrNameList->Current()->myName();
				      currVal = attrValueList->Current()->myName();
				    }
				  
				  //Construct the new saveframe here
				  SaveFrameListNode *l = new SaveFrameListNode(newSaveFrameItemsList);
				  SaveHeadingNode *h   = new SaveHeadingNode(string("save_") + tagToVal(currVal));
				  SaveFrameNode* s = new SaveFrameNode(h,l);

                                  //Add this saveframe to NMR loop
				  NMR->AddSaveFrameToDataBlock(currDatablockName,s);
				  
				  //Move one entry ahead on both the attrValList and attrNameList
				  attrNameList->Next();
				  attrValueList->Next();

				  //Add the remaining dataitems of the loop
				  while(!attrNameList->AtEnd())
				    {
				     currTag = attrNameList->Current()->myName();
				     currVal = attrValueList->Current()->myName();

				     //Create a new data item
				     currDataTag = new DataNameNode(currTag);
				     currDataVal = new DataValueNode(currVal);
				     currDataItem  = new DataItemNode(currDataTag, currDataVal);
				     
				     s->AddItemToSaveFrame(currDataItem);
				     
				      attrNameList->Next();
				      attrValueList->Next();
				    }

				} //Complete processing current LOOP
			    }
			  else //if scope tag is not found
			    {
			      origScopeDataItemList->AddToEnd(currSaveframeItem);
			    }
			}      

		      listOfSaveframeItems->Next();
		      if(!listOfSaveframeItems->AtEnd())
			currSaveframeItem = listOfSaveframeItems->Current();
		      else
			finished = true;
		      
		    }                  //finshed all items in current saveframe

		  //If the origScopeDataItemList is not empty make a saveframe
		  if( (origScopeDataItemList->Length() != 0)&& (state != FREETAGMODE))
		    {
		      //Construct the new saveframe here
		      SaveFrameListNode *l = new SaveFrameListNode(origScopeDataItemList);
		      SaveHeadingNode *h   = new SaveHeadingNode(currSaveframeName);
		      SaveFrameNode* s = new SaveFrameNode(h,l);
		      
		      //Add this saveframe to NMR loop
		      NMR->AddSaveFrameToDataBlock(currDatablockName,s);
		    }
		  
		}
	      
	    }
 
	  datablockItemsList->Next();
      }

    listOfDataBlocks->Next();
  }

}

