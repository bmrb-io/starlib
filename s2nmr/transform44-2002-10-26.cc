///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

struct compareString
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

  //////////////////////////////////
 //    save_insert_nmrstr_3_IDs  //
//////////////////////////////////
void save_insert_nmrstr_3_IDs( StarFileNode *input,
	                       StarFileNode *output,
		               BlockNode    *blk,
		               SaveFrameNode *currDicRuleSF)
{
  
    DataNode* currDicSaveFrame = currDicRuleSF;

    if(!currDicSaveFrame)
	return;     //Simply return if the desired saveframe is not found


    //Apply this change to all saveframes in the file:
    List<ASTnode *> *saveFrameHits = NULL;
    saveFrameHits = AST->searchForType(ASTnode::SAVEFRAMENODE);

    SaveFrameNode *saveFrameCur;
    ASTlist<DataNode*> *saveFrameCurInside;

    map<const char*,int, compareString> idMap;

    int  currentSfId = 0;
    int  currentLocalId = 0;
    char tmpStr[80];

    for ( saveFrameHits->Reset(); !saveFrameHits->AtEnd(); saveFrameHits->Next() )
    {

	saveFrameCur = (SaveFrameNode*)( saveFrameHits->Current() );
        saveFrameCurInside = saveFrameCur->GiveMyDataList();

	ASTlist<DataNode*> *parallel = NULL;
	if( saveFrameCur->myParallelCopy() != NULL )
	    parallel = ( (SaveFrameNode*)(saveFrameCur->myParallelCopy())
	               ) -> GiveMyDataList();

	// Find the tag category name of the free tags
	// of this saveframe, by finding the first free
	// tag there is, and looking at the stuff preceeding
	// the "dot".
	string tagName = string("");
	List<ASTnode*> *match;
	int curIdx;
	int dotPos = -1;
	for( curIdx = 0 ; curIdx < saveFrameCurInside->size() ; curIdx++ )
	{   if( (*saveFrameCurInside)[curIdx]->isOfType(
	                ASTnode::DATAITEMNODE) )
	    {   tagName = ( (DataItemNode*)
	                  ( (*saveFrameCurInside)[curIdx] ) )->myName();
		dotPos = tagName.find('.',0);
		if( dotPos >= 0 )
		    break;
	    }
	}
	// No dotted tag names were found - so ignore this Saveframe.
	if( dotPos < 0 )
	   continue;

	// Only increment numbers on the saveframes that actually
	// have dotted names.  (Skips past this if they don't):
	currentLocalId++;
        currentSfId--;  // Make these negative to remember they
	                // are not quite right.

	// Start at '1' to cut off the leading underscore:
	string currentSfTagCat = tagName.substr( 1, dotPos - 1 );

	// Now build the local ID name:
	string curLocalIdTagName = string("_") + currentSfTagCat + string(".ID");
	// check if the name already exists.  If not, add the new one:
	match = saveFrameCur->searchByTag( curLocalIdTagName );
	if( match == NULL || match->size() == 0 )
	{
	    sprintf( tmpStr, "%d", currentLocalId );
	    if( parallel )
		parallel->insert( parallel->begin(),
	           new DataItemNode( curLocalIdTagName, string(tmpStr) ) );
	    ASTnode *parent = *(parallel->begin());
	    while( parent != NULL && ! parent->isOfType(ASTnode::SAVEFRAMENODE) )
	    {   parent = parent->myParent(); }
	    if( parent != NULL )
	       idMap[ ((SaveFrameNode*)parent)->myName().c_str() ] = currentLocalId;
	}
	delete match;

	// Now build the SFID name:
	string curSfIdTagName = string("_") + currentSfTagCat + string(".Sf_ID");
	// check if the name already exists.  If not, add the new one:
	match = saveFrameCur->searchByTag( curSfIdTagName );
	if( match == NULL || match->size() == 0 )
	{
	    sprintf( tmpStr, "%d", currentSfId );
	    if( parallel )
		parallel->insert( parallel->begin(),
	           new DataItemNode( curSfIdTagName, string(tmpStr) ) );
	}
	delete match;


	// Now do the loops:
	for( curIdx = 0 ; curIdx < saveFrameCurInside->size() ; curIdx++ )
	{   if( (*saveFrameCurInside)[curIdx]->isOfType(
	                ASTnode::DATALOOPNODE) )
	    {   DataLoopNode *dln = ( (DataLoopNode*)
	                  ( (*saveFrameCurInside)[curIdx] ) );
		LoopNameListNode *lnln = (*( dln->getNamesPtr() ))[0];
		LoopNameListNode *lnlnParallel = (LoopNameListNode*)
		                     lnln->myParallelCopy();
		int curLoopName = -1;
	        int colDotPos = -1;
		string colName = string("");
		for(    curLoopName = 0 ;
		        curLoopName < lnln->size() ;
			curLoopName++    )
		{
		    colName = ( (*lnln)[curLoopName] )->myName();
		    colDotPos = colName.find('.',0);
		    if( colDotPos >= 0 )
			break;
		}
		// Skip if the loop has no dotted names:
		if( colDotPos < 0 )
		    continue;
	        
		string currentLoopTagCat = colName.substr( 1, colDotPos - 1 );

		// Now build the local ID name:
	        curLocalIdTagName = string("_") + currentLoopTagCat +
					   string(".") +
		                           string(currentSfTagCat) +
					   string("_ID");
		// check if the name already exists.  If not, add the new one:
		match = lnln->searchByTag( curLocalIdTagName );
		if( match == NULL || match->size() == 0 )
		{   sprintf( tmpStr, "%d", currentLocalId );
		    DataValueNode *dvn = new DataValueNode( string(tmpStr) ,
			                             DataValueNode::NON ) ;
		    if( lnlnParallel )
			lnlnParallel->insert( lnlnParallel->begin(),
			          new DataNameNode( curLocalIdTagName ), *dvn );
		    ASTnode *parent = *(lnlnParallel->begin());
		    while( parent != NULL && ! parent->isOfType(ASTnode::SAVEFRAMENODE) )
		    {   parent = parent->myParent(); }
		    if( parent != NULL )
		       idMap[ ((SaveFrameNode*)parent)->myName().c_str() ] = currentLocalId;

		    delete dvn;
		}
		delete match;

	        curSfIdTagName = string("_") + currentLoopTagCat +
					   string(".Sf_ID");
		// check if the name already exists.  If not, add the new one:
		match = lnln->searchByTag( curSfIdTagName );
		if( match == NULL || match->size() == 0 )
		{   sprintf( tmpStr, "%d", currentSfId );
		    DataValueNode *dvn = new DataValueNode( string(tmpStr) ,
			                             DataValueNode::NON ) ;
		    if( lnlnParallel )
			lnlnParallel->insert( lnlnParallel->begin(),
			          new DataNameNode( curSfIdTagName ), *dvn );
		    delete dvn;
		}
		delete match;
	    }
	}

    }

    // Now go back over it again and make the mappings of ID's
    // run parallel to the labels, as per the information in the
    // mapping saveframe:

    /*eraseme*/cerr<<"DUMPING MAP FOR DEBUG"<<endl;
    /*eraseme*/map<const char*, int, compareString>::iterator cur;
    /*eraseme*/for( cur = idMap.begin(); cur != idMap.end() ; cur++ ) { cerr << (*cur).first << " has ID " << (*cur).second <<endl; }

    delete saveFrameHits;

    DataLoopNode *rules = NULL;
    List<ASTnode*> *matches = currDicRuleSF->searchForType( 
                                   ASTnode::DATALOOPNODE );
    if( matches->size() > 0 )
    {
	rules = (DataLoopNode*) ( (*matches)[0] );

	int ruleNum;

	for( ruleNum = 0 ; ruleNum < rules->getValsPtr()->size() ; ruleNum++ )
	{
	  LoopRowNode *lrn = (*( rules->getValsPtr() ))[ruleNum];
	  string idTagName =    ((*lrn)[0])->myValue();
	  string labelTagName = ((*lrn)[1])->myValue();
	  
	  List<ASTnode*>*labelMatches = output->searchByTag( labelTagName );
	  int labelMatchIdx;
	  for(     labelMatchIdx = 0 ;
	           labelMatchIdx < labelMatches->size() ;
		   labelMatchIdx++   )
	  {
	    ASTnode *thisHit = (*labelMatches)[labelMatchIdx];
	    if( thisHit->isOfType( ASTnode::DATAITEMNODE ) )
	    {
	      if( ((DataItemNode*)thisHit)->myDelimType() ==
	                 DataValueNode::FRAMECODE )
	      {
		char tmpStr[255];
		strcpy( tmpStr,((DataItemNode*)thisHit)->myValue().c_str() );
		if( strncmp( tmpStr, "save_", 5 ) != 0 )
		    sprintf( tmpStr, "save_%s", ((DataItemNode*)thisHit)->myValue().c_str() );
		sprintf( tmpStr, "%d", idMap[tmpStr] );
		DataItemNode *din = new DataItemNode( idTagName, 
							string(tmpStr),
							DataValueNode::NON );
		ASTnode *parent;
		for(     parent = thisHit ;
			 parent != NULL && ! parent->isOfType( ASTnode::SAVEFRAMENODE ) ;
			 parent = parent->myParent()  )
		{ /*void body*/ }
		if( parent != NULL )
		{   ((SaveFrameNode*)parent)->GiveMyDataList()->SeekTo(
						    (DataItemNode*)thisHit);
		    ((SaveFrameNode*)parent)->GiveMyDataList()->InsertAfter(din);
		}
	      }
	    }
	    else
	    {
	      // First find the position of the matched column in the loop:
	      // And add the new ID tag next to it.
	      LoopNameListNode *lnln = (LoopNameListNode*)
	                           ( ((DataNameNode*)thisHit)->myParent() );
	      int labelCol = lnln->tagPosition( labelTagName );
	      if( labelCol >= 0 )
	      {
	        lnln->insert( lnln->begin() + labelCol, idTagName );
	      }
	      // Now iterate over the loop and reset the values of that
	      // New tag:
	      labelCol = lnln->tagPosition( labelTagName );
	      int idCol = lnln->tagPosition( idTagName );
	      int row;

	      char tmpStr[255];

	      ASTnode *parent;
	      for(     parent = thisHit ;
	               parent != NULL && ! parent->isOfType( ASTnode::DATALOOPNODE ) ;
		       parent = parent->myParent()  )
	      { /*void body*/ }
	      
	      if( parent != NULL )
	      {
	          DataLoopNode *dln = (DataLoopNode*) parent;
		  LoopTableNode *tbl = dln->getValsPtr();
		  for( row = 0 ; row < tbl->size() ; row++ )
		  {
		    if( (*(*tbl)[row])[labelCol]->myDelimType()
		              == DataValueNode::FRAMECODE )
		    {
		      strcpy( tmpStr,
			      (*(*tbl)[row])[labelCol]->myValue().c_str() );
		      /*eraseme*/cerr<<"line "<<__LINE__<<", tmpStr = "<<tmpStr<<endl;
		      if( strncmp( tmpStr, "save_", 5 ) != 0 )
			sprintf( tmpStr, 
				 "save_%s",
				 (*(*tbl)[row])[labelCol]-> myValue().c_str() );
		      /*eraseme*/cerr<<"line "<<__LINE__<<", tmpStr = "<<tmpStr<<endl;
		      if( idMap[tmpStr] > 0 )
		      {
		          sprintf( tmpStr, "%d", idMap[tmpStr] );
		          /*eraseme*/cerr<<"line "<<__LINE__<<", tmpStr = "<<tmpStr<<endl;
			  /*eraseme*/cerr<<"line "<<__LINE__<<", Going to change the value of "<< (*(*tbl)[row])[idCol]->myValue()<<" to "<<tmpStr<<endl;
			  (*(*tbl)[row])[idCol]->
			       setValue( string(tmpStr) );
			  (*(*tbl)[row])[idCol]->
			       setDelimType( DataValueNode::NON );
		      }
		    }
		  }
	      }
	    }
	  }

	}
    }
    delete matches;



}

