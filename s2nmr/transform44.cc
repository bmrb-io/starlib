///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2003/07/24 18:49:13  madings
Changes to handle 2.1-3.0 mappings.

*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

string getEntryId( StarFileNode* sfn );

// Holds a saveframe name and tag category:
struct SfSpec
{ 
  string sfName;
  string tagCat;
};

// compares an sf spec.  If the tag category
// is an empty string, then we don't care
// about that and the first hit on the sf name
// is returned as a match.
// STL comparators must return true if s1 < s2, or
// false if they are equal or greater.
struct compareSpec
{
  bool operator()(SfSpec s1, SfSpec s2) const
  {
    bool isLess = false;
    int  compareSfNames;

    compareSfNames = strcasecmp(s1.sfName.c_str(), s2.sfName.c_str()) ;

    isLess = (compareSfNames < 0);

    // If tag categories were given, they are relevant
    // and need to be searched too if the saveframe names
    // matched:
    if( ( s1.tagCat.length() != 0 &&
          s2.tagCat.length() != 0 )  && compareSfNames == 0 )
    {
	isLess = strcasecmp(s1.tagCat.c_str(), s2.tagCat.c_str()) < 0;
    }
    return isLess;
  }
};

void debugDumpIdMap( map<SfSpec,int,compareSpec> &m )
{
    map<SfSpec,int,compareSpec>::iterator iter;

    for( iter = m.begin() ; iter != m.end() ; iter ++ )
    {
        pair<SfSpec,int> thisPair = *iter;
	cerr<<"debug:   map [ "<<thisPair.first.sfName<<", "<<
	    thisPair.first.tagCat<<" ] = "<<thisPair.second<<endl;
    }
}


struct compareString
{
  bool operator()(string s1, string s2) const
  {
    return strcasecmp(s1.c_str(), s2.c_str()) < 0;
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


    string entryId = getEntryId( input );
    //Apply this change to all saveframes in the file:
    List<ASTnode *> *saveFrameHits = NULL;
    saveFrameHits = AST->searchForType(ASTnode::SAVEFRAMENODE);

    SaveFrameNode *saveFrameCur;
    ASTlist<DataNode*> *saveFrameCurInside;

    map<SfSpec,int, compareSpec> idMap;

    int  currentSfId = 0;
    map<string, int, compareString> localIdMap;
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


	// Start at '1' to cut off the leading underscore:
	string currentSfTagCat = tagName.substr( 1, dotPos - 1 );

	// Only increment numbers on the saveframes that actually
	// have dotted names.  (Skips past this if they don't):
	int currentLocalId = -9999;
	if( localIdMap.find( currentSfTagCat ) == localIdMap.end() )
	{
	    localIdMap[ currentSfTagCat ] = currentLocalId = 1;
	}
	else
	{
	    currentLocalId = ++( localIdMap[ currentSfTagCat ] );
	}
	int localIdToUse = currentLocalId;
        currentSfId--;  // Make these negative to remember they
	                // are not quite right.

	// Insert the entry id field for this saveframe too:
	string entryIdTagName;
	if( currentSfTagCat == string("Entry") )
	{
	    entryIdTagName = string("_Entry.ID");
	    localIdToUse = atoi( entryId.c_str() );
	}
	else
	{
	    entryIdTagName = string("_") + string(currentSfTagCat) +
	                     string(".Entry_ID");
	}

	if( parallel )
	{
	   List<ASTnode*> *entryIdTagMatches =
		   parallel->myParent()->searchByTag( entryIdTagName );
	   if( entryIdTagMatches->size() > 0  &&
	        (*entryIdTagMatches)[0]->isOfType(ASTnode::DATAITEMNODE) )
	   {   
	        ( (DataItemNode*)((*entryIdTagMatches)[0]) )->setValue( entryId );
	   }
	   else
	   {   parallel->insert( parallel->begin(),
				 new DataItemNode( entryIdTagName, entryId,
						   DataValueNode::NON ) );
	   }
	}
	

	// Now build the local ID name:
	string curLocalIdTagName = string("_") + currentSfTagCat + string(".ID");
	if( curLocalIdTagName != entryIdTagName ) // If it's the entry ID then it's been taken care of already.
	{
	    DataItemNode *din = NULL;

	    // Check if the name already exists.  If it exists and has non-bogus
	    // data, then use it and don't add the new one.  If it exists but has
	    // bogus data, then overwrite it in-place, and if it doesn't exist, make a
	    // new one:
	    match = saveFrameCur->searchByTag( curLocalIdTagName );
	    if( match == NULL || match->size() == 0 )
	    {
		din = new DataItemNode( curLocalIdTagName, string("replace_me") );
		if( parallel )
		    parallel->insert( parallel->begin(), din );
	    }
	    else
	    {
		din = ((DataItemNode*)( (*match)[0]->myParallelCopy() ));
	    }
	    if( din->myValue() != string(".") && din->myValue() != string("?") )
	    {
		if( curLocalIdTagName != string("_Entry.ID") )
		{
		  sprintf( tmpStr, "%d", localIdToUse );
		  din->setValue(string(tmpStr));
		  ASTnode *parent = *(parallel->begin());
		  while(   parent != NULL &&
			   ! parent->isOfType(ASTnode::SAVEFRAMENODE)   )
		  {   parent = parent->myParent(); }
		  if( parent != NULL )
		  {
		      SfSpec key;
		      int dotPos;
		      key.sfName = ((SaveFrameNode*)parent)->myName();
		      key.tagCat = currentSfTagCat;
		      idMap[ key ] = localIdToUse;
		  }
		}
	    }
	    delete match;
	}

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
		string capitalisedCurSfTagCat = currentSfTagCat;
		// uppercase the first letter:
		capitalisedCurSfTagCat.replace(0,1,1, toupper(capitalisedCurSfTagCat[0]) );

	        curLocalIdTagName = string("_") + currentLoopTagCat +
					   string(".") +
		                           string(capitalisedCurSfTagCat) +
					   string("_ID");

		// check if the name already exists.
		// If so, delete it.  Then, after that
		// whether it had existed before or not,
		// insert a new one, recalculated:
		match = lnln->searchByTag( curLocalIdTagName );
		if(     ( match != NULL && match->size() > 0 ) &&
		        curLocalIdTagName != string("_Entry.ID")    )
		{
		    ASTnode *peer;
		    int match_i;
		    for( match_i = 0 ; match_i < match->size() ; match_i++ )
		    {  peer = (*match)[match_i] -> myParallelCopy();
		       if( peer != NULL )
			  delete peer;
		    }
		}
	        sprintf( tmpStr, "%d", localIdToUse );
		DataValueNode *dvn = new DataValueNode( string(tmpStr) ,
						 DataValueNode::NON ) ;

		if( lnlnParallel )
		    lnlnParallel->insert( lnlnParallel->begin(),
			      new DataNameNode( curLocalIdTagName ), *dvn );
		ASTnode *parent = *(lnlnParallel->begin());
		while( parent != NULL && ! parent->isOfType(ASTnode::SAVEFRAMENODE) )
		{   parent = parent->myParent(); }
		if( parent != NULL )
		{
		  SfSpec key;
		  int dotPos;
		  key.sfName = ((SaveFrameNode*)parent)->myName();
		  key.tagCat = currentSfTagCat;
		  idMap[ key ] = localIdToUse;
		}

		delete dvn;
	     
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

	        // Add the entry ID to the loop as well:
	        entryIdTagName = string("_") + string(currentLoopTagCat) +
	                         string(".Entry_ID");
		// check if the name already exists.  If so, delete it first.
		// In either case, add a new one:
		match = lnln->searchByTag( entryIdTagName );
		if( match != NULL && match->size() != 0 )
		{
		    ASTnode  *para = (*match)[0]->myParallelCopy();
		    if( para != NULL )
		       delete para;
		}
		dvn = new DataValueNode( entryId ,
						 DataValueNode::NON ) ;
		if( lnlnParallel )
		    lnlnParallel->insert( lnlnParallel->begin(),
			      new DataNameNode( entryIdTagName ), *dvn );
		delete dvn;

		delete match;

	    }
	}

    }

    // Now go back over it again and make the mappings of ID's
    // run parallel to the labels, as per the information in the
    // mapping saveframe:


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

	  int dotPos = labelTagName.find('.');
	  int labelPos = labelTagName.find("_label");
	  string labelPointsAtCat = labelTagName.substr(dotPos+1, labelPos-dotPos-1);
	  
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
		SfSpec key;
		int    id;
		strcpy( tmpStr,((DataItemNode*)thisHit)->myValue().c_str() );
		if( strncmp( tmpStr, "save_", 5 ) != 0 )
		    sprintf( tmpStr, "save_%s", ((DataItemNode*)thisHit)->myValue().c_str() );
		key.sfName = string(tmpStr);
		key.tagCat = labelPointsAtCat;


		//   If there is no exact match, then try for something
		//   else that matches the same saveframe name, by setting
		//   the key's tagCat to "don't-care"
		if( idMap.find(key) == idMap.end() )
		{
		    key.tagCat = string("");
		}

		sprintf( tmpStr, "%d", idMap[key] );

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
	      int idCol = lnln->tagPosition( idTagName );
	      if( labelCol >= 0 )
	      {
		// If the ID does not already exist, make it and recalc the
		// column number:
		if( idCol < 0 )
		{ lnln->insert( lnln->begin() + labelCol, idTagName );
	          idCol = lnln->tagPosition( idTagName );
		}
	      }
	      // Now iterate over the loop and reset the values of that
	      // New tag:
	      labelCol = lnln->tagPosition( labelTagName );
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
		      if( strncmp( tmpStr, "save_", 5 ) != 0 )
			sprintf( tmpStr, 
				 "save_%s",
				 (*(*tbl)[row])[labelCol]-> myValue().c_str() );

		      SfSpec key;
		      int    id;
		      key.sfName = string(tmpStr);
		      key.tagCat = labelPointsAtCat;

		      //   If there is no exact match, then try for something
		      //   else that matches the same saveframe name, by setting
		      //   the key's tagCat to "don't-care"
		      if( idMap.find(key) == idMap.end() )
		      {
			  key.tagCat = string("");
		      }
		      if( idMap[key] > 0 )
		      {
		          sprintf( tmpStr, "%d", idMap[key] );
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

string getEntryId( StarFileNode* sfn )
{
    string retVal = "";

    List<ASTnode*> *accNumMatch = sfn->searchByTag( "_BMRB_accession_number" );
    if( accNumMatch == NULL || accNumMatch->size() == 0 )
        accNumMatch = sfn->searchByTag( "_Entry.Temp_BMRB_accession_code" );
    if( accNumMatch == NULL || accNumMatch->size() == 0 )
        accNumMatch = sfn->searchByTag( "_Entry.Update_BMRB_accession_code" );
    if( accNumMatch == NULL || accNumMatch->size() == 0 )
        accNumMatch = sfn->searchByTag( "_Entry.Replace_BMRB_accession_code" );
    if( accNumMatch == NULL || accNumMatch->size() == 0 )
    {
        retVal = "NEED_ACC_NUM";
    }
    else
    {
	// only look at the first match:
	ASTnode *firstMatch = (*accNumMatch)[0];
	if( firstMatch->isOfType( ASTnode::DATAITEMNODE ) )
	{
	    retVal = ((DataItemNode*)firstMatch)->myValue();
	}
	else if( firstMatch->isOfType( ASTnode::DATANAMENODE ) )
	{
            retVal = "(accession_code_should_not_be_looped)";
	}
	else
	{
            retVal = "(accession_code_not_in_a_free_or_looped_tag)";
	}
    }
    if( retVal == "?" || retVal == "." ) {
        retVal = "NEED_ACC_NUM";
    }
    return retVal;
}
