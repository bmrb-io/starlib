//########################################################################################
//#######################################################################################

void save_renamed_tags(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode* currDicDataBlock)

{

//The function applies transformation 1a  for a specified dictionary datablock
//First get the "save_renamed_tags" saveframe in the current dictionary datablock
  
  string currDicSaveFrameName = string("save_renamed_tags");
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
  int length = currDicNameList->Length();

  //This transformation is applied on a datablock basis

  List<BlockNode*>* listOfDataBlocks =  AST->GiveMyDataBlockList();
    
  listOfDataBlocks->Reset();
  while(!listOfDataBlocks->AtEnd())
    {
      BlockNode* currDatablock    = listOfDataBlocks->Current();
      string currDatablockName    = currDatablock->myName();

      //For each datablock we will apply this transformation
      currDicValList->Reset();
      string*  iterVal = new string[length];
      
      //This while loop processes successive iterations in the dictionary loop
      while(!currDicValList->AtEnd())
	{
	  //Get the values for one loop iteration
	  for(int i =0; i<length ; i++)
	    {
	      iterVal[i] = tagToVal(currDicValList->Current()->myName());
	      currDicValList->Next();
	    }
	  //Apply this change to the relevant saveframes within this datablock

	  //Retrieve the appropriate saveframe containing tag to be changed
	  DataNode* NMRsaveframe = NMR->ReturnDataBlockDataNode(currDatablockName, iterVal[1]); 
	  
	  
	  if(NMRsaveframe !=  NULL) //Which means such a saveframe does exist
	    {
	      //Get the new saveframe into which the tag must go
	      DataNode* newNMRsaveframe = NMR->ReturnDataBlockDataNode(currDatablockName,iterVal[3]);
	      
	      if((newNMRsaveframe == NULL)||(newNMRsaveframe->myType() != DataNode::SAVETYPE))
		{
		  cout<<"Error....destination saveframe does not exist!!!"<<endl;
		  return;
		}
	      else
		{
		  //First check if the new_tag_name already exists in the new_saveframe
                  //if it does, flag an error
		  List<DataNode*>* newNMRsaveframeList = ((SaveFrameNode*)newNMRsaveframe)->GiveMyDataList();
		  
		  newNMRsaveframeList->Reset();
		  while(!newNMRsaveframeList->AtEnd())
		    {
		      if(newNMRsaveframeList->Current()->myName() == iterVal[2])
			 {
			   cout <<"This is an error. The new tag already exists in saveframe"<<endl;
			   return;
			 }

		      newNMRsaveframeList->Next();
		    }

		  //At this point we know that the new tag does not exist in new_saveframe
		  
		  //Get the item with _old_tag_name from the NMRsaveframeList
		  List<DataNode*>* NMRsaveframeList =((SaveFrameNode*) NMRsaveframe)->GiveMyDataList();
		  NMRsaveframeList->Reset();
		  bool found = false;
		  DataNode* currNMRsaveframeListItem;
		  while(!found && !NMRsaveframeList->AtEnd())
		    {
		      currNMRsaveframeListItem = NMRsaveframeList->Current();
		      found =   (currNMRsaveframeListItem->myName() == iterVal[0]);
		      NMRsaveframeList->Next();
		    }

		  if(found)
		    {
		       //When item is found "construct" the new dataitem with new tagname
		      if(currNMRsaveframeListItem->myType()== DataNode::ITEMTYPE)
			{
			  
			  //Construct a new data item
			  string tagValue = ((DataItemNode*)currNMRsaveframeListItem)->myValue();
			  DataItemNode* newItem = new DataItemNode(iterVal[2],tagValue);
			  
			  //And finally....remove dataitem from old saveframe
			  NMRsaveframeList->Prev();
			  NMRsaveframeList->RemoveCurrent();

			  //Insert this item into the newNMRsaveframeList
			  newNMRsaveframeList->AddToEnd(newItem);
			}
		      else
			{
			  cout<<"Cannot operate on looped items"<<endl;
			  return;
			}
		    }
		  else
		    {
		      cout<<"The tagname does not exist in the source saveframe"<<endl;
		      return;
		    }
	
		}
	     	      
	    }

	} //End while loop -- for all dictionary entries for this transformation
     
      
      listOfDataBlocks->Next();
    }
}

