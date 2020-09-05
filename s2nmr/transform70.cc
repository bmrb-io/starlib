///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2006/11/14 23:12:33  madings
*** empty log message ***

Revision 1.1  2005/03/18 21:07:20  madings
Massive update after too long without a checkin

*/
#endif

/* ----- instruction -----------


   The layout of this rule in the mapping file is like this:

   save_foo
       _Saveframe_category  merge_saveframes_2_to_3

       loop_
           _key_tag
	   _key_value
	   loop_
	       _tag_list
	       _loop_sequence_tag

	   "_Saveframe_category"
	   "assigned_chemical_shifts"
	       MUST_MATCH:                        n/a
	       "_Sample_conditions_label"         n/a
	       "_Chem_shift_reference_set_label"  n/a
	       "_Details"                         n/a
	       "_Sample_label"                    n/a
	       MUST_DIFFER:                       n/a
               "_Mol_system_component_name"      "_Atom_shift_assign_ID"
	   stop_

	   # Continue filling out this mapping loop for all the NMR data types,
	   # such as T1, T2, RDC, etc...
	stop_
   save_

   This example means  "Apply the next rule to the saveframes
   where "_Saveframe_category" equals "assigned_chemical_shifts",
   then within those saveframes, the criteria for merging are
   that the tags in the "MUST_MATCH:" section must be the
   same between two candidate saveframes, and that the tags in
   the "MUST_DIFFER:" section must NOT be the same between two
   candidate saveframes.  "MUST_MATCH:" and "MUST_DIFFER:" are
   hardcoded keywords this rule understands and they must be
   spelled and capitalized exactly as shown (including the 
   colon).  I would have preferred that there was one tag
   for the match list and another for the differ list, but
   with star syntax that's hard to do when they don't have the
   same cardinality (there are 4 items in the match list and 
   only 1 item in the differ list).

   The "_loop_index_tag" field has two purposes.  One thing it does
   is that it tells the rule which loop to move the differ
   tags into when doing the merge.  When tags differ, the differing value
   typically must end up in a loop so it can differentiate the two merged
   saveframe's data.  It is ignored for the MUST_MATCH tags.
   The second thing the _loop_sequence_tag does is tell which of the tags
   in the loop is the one that is supposed to be renumbered to count
   ordinally from 1 to N, where N is the new size of the loop.  For
   example, after merging two chemical shifts saveframes, one with
   200 rows and one with 100 rows, the _Atom_shift_assign_ID must
   be renumbered to go from 1 to 300, instead of going from 
   1 to 200 followed by 1 to 100 again.


------------------------------- 
*/
///////////////////////////////////////////////////////////////
#include "transforms.h"

struct stringcmp
{
    bool   operator()( string s1, string s2) const
    {
        return s1 < s2;
    }
};

bool shouldMerge( SaveFrameNode *thisSf, SaveFrameNode *thatSf, vector<string> &sameTags, vector<string> &differTags );

void doMerge( SaveFrameNode *thisSf, SaveFrameNode *thatSf, vector<string> &sameTags, vector<string> &differTags, vector<string> &differTargetLoopTags, bool reallyMerge );

void save_merge_saveframes_2_to_3(
                         StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF)
{
    List<ASTnode*>        *outerMapLoops;
    DataLoopNameListNode  *outerMapNames;
    LoopTableNode         *outerMapLoopTbl;
    LoopTableNode         *innerMapLoopTbl;
    int                   mL_Idx; /* map loop index */
    string                tagname;
    int                   key_tag_pos;
    int                   key_value_pos;

    // Find loop from the mapping file:
    // --------------------------------
    outerMapLoops = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_key_tag" ) );
    if( outerMapLoops->size() < 0 )
    {
	(*errStream) << "#transform-70# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
	delete outerMapLoops;
	return; /* do nothing */
    }
    outerMapLoopTbl = ((DataLoopNode*)(*outerMapLoops)[0])->getValsPtr();
    outerMapNames =  ((DataLoopNode*)(*outerMapLoops)[0])->getNamesPtr();

    delete outerMapLoops;

    key_tag_pos = -1;
    key_value_pos = -1;
    for( int i = 0 ; i < (*outerMapNames)[0]->size() ; i++ )
    {
	if(      (* ((*outerMapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	if(      (* ((*outerMapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
    }

    for( int outerMapI = 0 ; outerMapI < outerMapLoopTbl->size() ; outerMapI++ )
    {
        string keyTag   = (*((*outerMapLoopTbl)[outerMapI]))[key_tag_pos]->myValue();
        string keyValue = (*((*outerMapLoopTbl)[outerMapI]))[key_value_pos]->myValue();
	vector<string>  sameTags;
	vector<string>  differTags;
	vector<string>  differTargetLoopTags;

	LoopTableNode *innerMapLoop = (*outerMapLoopTbl)[outerMapI]->myLoop();
	if( innerMapLoop )
	{
	    bool readingDifferTags = false;
	    for( int innerMapI = 0 ; innerMapI < innerMapLoop->size() ; innerMapI++ )
	    {
	        string curVal = (*(*innerMapLoop)[innerMapI])[0]->myValue();
	        string curTargetLoopVal = (*(*innerMapLoop)[innerMapI])[1]->myValue();
		if( curVal == string("MUST_MATCH:") )
		{   readingDifferTags = false;
		}
		else if( curVal == string("MUST_DIFFER:") )
		{   readingDifferTags = true;
		}
		else
		{   if( readingDifferTags )
		    {   differTags.insert(differTags.end(), curVal );
		        differTargetLoopTags.insert(differTargetLoopTags.end(), curTargetLoopVal );
		    } else
		    {   sameTags.insert(sameTags.end(), curVal );
		    }
		}
	    }
	}
	List<ASTnode*>  *hitSaveFrames = inStar->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, keyTag, keyValue );

	// About the next loop:  This is a bit odd.  The loop does not
	// count all the way to the end - stops one short of the end.
	// the purpose of the loop is to compare every saveframe hit to
	// every other saveframe hit.  This means you don't need to do
	// the last iteration - all the comparasins of other saveframes to
	// it will have already occured by then.
	// Also, in this loop is is important that the end-check is dynamic.
	// It calculates the size of the list every time because it might
	// change as saveframes are consumed out of the list.  Sometimes it's
	// just inefficient programming to call size() with each iteration,
	// but in this case it's actually necessary.
	for( int thisSfIdx = 0 ; thisSfIdx +1 < hitSaveFrames->size() ; thisSfIdx++ )
	{
	    SaveFrameNode *thisSf = (SaveFrameNode*) (*hitSaveFrames)[thisSfIdx];

	    for( int thatSfIdx = thisSfIdx+1 ; thatSfIdx < hitSaveFrames->size() ; thatSfIdx++ )
	    {
	        SaveFrameNode *thatSf = (SaveFrameNode*) (*hitSaveFrames)[thatSfIdx];
		doMerge( thisSf, thatSf, sameTags, differTags, differTargetLoopTags, 
	                   shouldMerge( thisSf, thatSf, sameTags, differTags ) );
	    }

	}
	delete hitSaveFrames;
    }

}

bool shouldMerge( SaveFrameNode *thisSf,
                  SaveFrameNode *thatSf,
		  vector<string> &sameTags,
		  vector<string> &differTags )
{
   (*errStream)<<"#Transform70# - info: Comparing saveframe "<<thisSf->myName()<<" to saveframe "<<thatSf->myName()<<endl;
   // All of the values of the sameTags list shoud match:
   int i;
   bool retVal = true; // default assumption until proven otherwise
   vector<string> theseVals;
   vector<string> thoseVals;
   string thisVal, thatVal;

   // Iterate over both lists:
   for( i = 0 ; retVal && i < sameTags.size()+differTags.size() ; i++ )
   {
       theseVals.clear();
       thoseVals.clear();
       bool differCheck=true; // true=differCheck, false=sameCheck
       List<ASTnode*> *theseHits;
       List<ASTnode*> *thoseHits;
       string curTagName;
       if( i < sameTags.size() )
       {
           curTagName = sameTags[i];
	   theseHits = thisSf->searchByTag(curTagName);
	   thoseHits = thatSf->searchByTag(curTagName);
	   differCheck = false;
       } else 
       {
           curTagName = differTags[i-sameTags.size()];
	   theseHits = thisSf->searchByTag(curTagName);
	   thoseHits = thatSf->searchByTag(curTagName);
	   differCheck = true;
       }
       (*errStream)<<"#transform70# - info:   Checking for tag "<<curTagName<<", which is a "<<(differCheck?"MUST_DIFFER":"MUST_MATCH")<<" tag."<<endl;

       // Should not be more than one hit, just take the zeroth hit:
       ASTnode *thisHit = ( theseHits->size() > 0 ) ? (*theseHits)[0] : NULL ;
       ASTnode *thatHit = ( thoseHits->size() > 0 ) ? (*thoseHits)[0] : NULL ;

       int thatCol;
       int thisCol;
       int dummy;

       if( thisHit == NULL ) // Treat a nonexistant tag like a null value tag - they
                             // count as being the same thing for the sake of this check:
       {
	   theseVals.insert( theseVals.end(), string(".") );
       } else if( thisHit->isOfType(ASTnode::DATAITEMNODE) ) // free tag
       {
           (*errStream)<<"#transform70# - info:  this Hit is a free tag."<<endl;
	   theseVals.insert( theseVals.end(), ((DataItemNode*)thisHit)->myValue() );
       } else if( thisHit->isOfType(ASTnode::DATANAMENODE) ) // looped tag.
       {
           (*errStream)<<"#transform70# - info:  this Hit is a looped tag."<<endl;
           ASTnode *par = thisHit;
	   while( par != NULL && ! par->isOfType(ASTnode::DATALOOPNODE) )
	   {  par = par->myParent();
	   }
	   ((DataLoopNode*)par)->tagPositionDeep( curTagName, &dummy, &thisCol );
	   if( thisCol >= 0 )
	   {   LoopTableNode *tbl = ((DataLoopNode*)par)->getValsPtr();
	       for( int i = 0 ; i < tbl->size(); i++ )
	       {   theseVals.insert(theseVals.end(),(*(*tbl)[i])[thisCol]->myValue() );
	       }
	   }
       } else // Should not be possible case:
       {
	   theseVals.insert( theseVals.end(), string(".") );
       }

       if( thatHit == NULL ) // Treat a nonexistant tag like a null value tag - they
                             // count as being the same thing for the sake of this check:
       {
	   thoseVals.insert( thoseVals.end(), string(".") );
       } else if( thatHit->isOfType(ASTnode::DATAITEMNODE) ) // free tag
       {
           (*errStream)<<"#transform70# - info:  that Hit is a free tag."<<endl;
	   thoseVals.insert( thoseVals.end(), ((DataItemNode*)thatHit)->myValue() );
       } else if( thatHit->isOfType(ASTnode::DATANAMENODE) ) // looped tag.
       {
           (*errStream)<<"#transform70# - info:  that Hit is a looped tag."<<endl;
           ASTnode *par = thatHit;
	   while( par != NULL && ! par->isOfType(ASTnode::DATALOOPNODE) )
	   {  par = par->myParent();
	   }
	   ((DataLoopNode*)par)->tagPositionDeep( curTagName, &dummy, &thatCol );
	   if( thatCol >= 0 )
	   {   LoopTableNode *tbl = ((DataLoopNode*)par)->getValsPtr();
	       for( int i = 0 ; i < tbl->size(); i++ )
	       {   thoseVals.insert(thoseVals.end(),(*(*tbl)[i])[thatCol]->myValue() );
	       }
	   }
       } else // Should not be possible case:
       {
	   thoseVals.insert( thoseVals.end(), string(".") );
       }

       int checkEnd;
       if( theseVals.size() > thoseVals.size() )
       {   checkEnd = theseVals.size();
       } else
       {   checkEnd = thoseVals.size();
       }
       for( int i = 0 ; retVal && i < checkEnd ; i++ )
       {
           if( i >= theseVals.size() )
	   {   thisVal = string(".");
	   } else
	   {   thisVal = theseVals[i];
	       if( thisVal == string("?") || thisVal == string("") ) 
	       {   thisVal = string(".");
	       }
	   }
           if( i >= thoseVals.size() )
	   {   thatVal = string(".");
	   } else
	   {   thatVal = thoseVals[i];
	       if( thatVal == string("?") || thatVal == string("") ) 
	       {   thatVal = string(".");
	       }
	   }

	   if( differCheck )
	   {  if( thisVal == thatVal )
	      {  
		  (*errStream)<<"#transform70# - info:       Values for tag are a match. ["<<thisVal<<"] == ["<<thatVal<<"]."<<endl;
		  retVal = false;
	      } else {
		  (*errStream)<<"#transform70# - info:       Values for tag are different: ["<<thisVal<<"] != ["<<thatVal<<"]."<<endl;
	      }
	   } else
	   {  if( thisVal != thatVal )
	      {
		  (*errStream)<<"#transform70# - info:       Values for tag are different: ["<<thisVal<<"] != ["<<thatVal<<"]."<<endl;
		  retVal = false;
	      } else {
		  (*errStream)<<"#transform70# - info:       Values for tag are a match. ["<<thisVal<<"] == ["<<thatVal<<"]."<<endl;
	      }
	   }
       }
       delete theseHits;
       delete thoseHits;
   }
   (*errStream) <<"*transform70# - info: Final verdict: These saveframes "<<(retVal?"ARE":"ARE NOT")<<" supposed to be merged."<<endl;

   return retVal;
}

void doMerge( SaveFrameNode *thisSf,   // The saveframe to merge into
              SaveFrameNode *thatSf,   // The saveframe to get copied and then removed.
	      vector<string> &sameTags,
	      vector<string> &differTags,
	      vector<string> &differTargetLoopTags,
	      bool           reallyMerge  // If false, then just move the tag to the target loop, don't merge)
	      )
{
    // sameTags is not really used.

    SaveFrameNode *thisPeerSf = (SaveFrameNode*) thisSf->myParallelCopy();
    SaveFrameNode *thatPeerSf = (SaveFrameNode*) thatSf->myParallelCopy();
    string diffTag;
    string targetTag;

    List<ASTnode*> *matches = NULL;

    if( thisPeerSf == NULL || thatPeerSf == NULL )
    {
        return;
    }

    int i,j,k;

    // First, move the differ tags into their respective loops
    // in this and that SF
    for( i = 0 ; i < differTags.size() ; i++ )
    {
        string diffTag = differTags[i];
	string targetTag = differTargetLoopTags[i];

        // Get the loops from this and that:
	for( int loopNum = 0 ; loopNum < 2 ; loopNum++ )
	{
	    DataLoopNode *dln = NULL;
	    DataValueNode diffTagValue("");
	    switch(loopNum)
	    {   case 0: matches = thisPeerSf->searchForTypeByTag( ASTnode::DATALOOPNODE, targetTag );
	                if( matches->size() > 0 )
			{   dln = (DataLoopNode*) ((*matches)[0]);
			}
			delete matches;

			// Get the value of the differ tag, as free or as row 0 of a loop:
			matches = thisPeerSf->searchForTypeByTag( ASTnode::DATAITEMNODE, diffTag );
			if( matches->size() > 0 )
			{   diffTagValue.setValue(     ( (DataItemNode*) ((*matches)[0]) )->myValue() );
			    diffTagValue.setDelimType( ( (DataItemNode*) ((*matches)[0]) )->myDelimType() );
			    delete ((*matches)[0]); // Get rid of the flat tag
			    delete matches;
			} else
			{   matches = thisPeerSf->searchForTypeByTag( ASTnode::DATALOOPNODE, diffTag );
			    if( matches->size() > 0 )
			    {
			       int nest,col;
			       ((DataLoopNode*) ((*matches)[0]))->tagPositionDeep( diffTag, &nest, &col );
			       diffTagValue.setValue(      ((*(  (DataLoopNode*) ((*matches)[0]))->getVals()[0]  ))[col]->myValue() );
			       diffTagValue.setDelimType(  ((*(  (DataLoopNode*) ((*matches)[0]))->getVals()[0]  ))[col]->myDelimType() );
			    }
			}
		        break;
		case 1: matches = thatPeerSf->searchForTypeByTag( ASTnode::DATALOOPNODE, targetTag );
	                if( matches->size() > 0 )
			{   dln = (DataLoopNode*) ((*matches)[0]);
			}
			delete matches;

			// Get the value of the differ tag, as free or as row 0 of a loop:
			matches = thatPeerSf->searchForTypeByTag( ASTnode::DATAITEMNODE, diffTag );
			if( matches->size() > 0 )
			{   diffTagValue.setValue(     ( (DataItemNode*) ((*matches)[0]) )->myValue() );
			    diffTagValue.setDelimType( ( (DataItemNode*) ((*matches)[0]) )->myDelimType() );
			    delete ((*matches)[0]); // Get rid of the flat tag
			    delete matches;
			} else
			{   matches = thatPeerSf->searchForTypeByTag( ASTnode::DATALOOPNODE, diffTag );
			    if( matches->size() > 0 )
			    {
			       int nest,col;
			       ((DataLoopNode*) ((*matches)[0]))->tagPositionDeep( diffTag, &nest, &col );
			       diffTagValue.setValue(      ((*(  (DataLoopNode*) ((*matches)[0]))->getVals()[0]  ))[col]->myValue() );
			       diffTagValue.setDelimType(  ((*(  (DataLoopNode*) ((*matches)[0]))->getVals()[0]  ))[col]->myDelimType() );
			    }
			}
		        break;
	    }
	    if( dln == NULL )
	    {   continue;
 	    }
	    // Skip if it's already in the loop:
	    matches = dln->searchByTag( diffTag );
	    if( matches->size() == 0 )
	    {
	       dln->getNames()[0]->insert( dln->getNames()[0]->end(), diffTag, diffTagValue );
	    }
	    delete matches;
	}
    }


    // Second, actually do the merging:
    // (skip if the reallyMerge flag is not true)

    if( reallyMerge )
    {

	vector<int> ind;  // Indeces mapping to loop1 from loop2.
			  // For example if ind had three elements like so: ind[0]=0, ind[1]=2, ind[2]=1
			  // then that would mean "map column 0 from 0 , column 1 from column 2, and column 2 from column 1.

	// First, move the differ tags into their respective loops
	// in this and that SF
	for( i = 0 ; i < differTags.size() ; i++ )
	{
	    int nest,col;
	    char numb[30];
	    diffTag = differTags[i];
	    targetTag = differTargetLoopTags[i];

	    ind.clear();
	    List<ASTnode*> *matchesThis = thisPeerSf->searchForTypeByTag( ASTnode::DATALOOPNODE, diffTag );
	    List<ASTnode*> *matchesThat = thatPeerSf->searchForTypeByTag( ASTnode::DATALOOPNODE, diffTag );

	    if( matchesThis->size() == 0 || matchesThat->size() == 0  )
	    {
		continue;
	    }

	    LoopNameListNode *thoseNames =  ( (* ( (DataLoopNode*)((*matchesThat)[0]) )->getNamesPtr() ))[0];
	    LoopNameListNode *theseNames =  ( (* ( (DataLoopNode*)((*matchesThis)[0]) )->getNamesPtr() ))[0];
	    for( j = 0 ; j < theseNames->size() ; j++ )
	    {
		((DataLoopNode*)((*matchesThat)[0]))->tagPositionDeep( ((*theseNames)[j])->myName(), &nest, &col );
		ind.insert( ind.end(), col );
	    }

	    // Now we have our indicator list:

	    LoopRowNode *thisNewRow;
	    LoopRowNode *thatRow;
	    LoopTableNode *thisTbl = ((DataLoopNode*)((*matchesThis)[0]))->getValsPtr();
	    LoopTableNode *thatTbl = ((DataLoopNode*)((*matchesThat)[0]))->getValsPtr();

	    ((DataLoopNode*)((*matchesThis)[0]))->tagPositionDeep( targetTag, &nest, &col );
	    int targetTagCol = col;

	    string lastTargetTagVal = (*((*thisTbl)[thisTbl->size()-1]))[targetTagCol]->myValue();
	    int lastTargetVal = atoi( lastTargetTagVal.c_str() );

	    DataValueNode numbVal("dummy");
	    for( j = 0 ; j < thatTbl->size() ; j++ )
	    {
		thatRow = (*thatTbl)[j];
		thisNewRow = new LoopRowNode(true);
		for( k = 0 ; k < theseNames->size(); k++ )
		{
		   if( k == targetTagCol )
		   {
		       sprintf( numb, "%d", lastTargetVal + 1 + j );
		       numbVal.setValue( string(numb) );
		       numbVal.setDelimType( DataValueNode::NON );
		       thisNewRow->insert( thisNewRow->end(), numbVal  );
		   }
		   else
		   {   thisNewRow->insert( thisNewRow->end(), *(  (*thatRow)[ ind[k] ] ) );
		   }
		}
		thisTbl->insert( thisTbl->end(), *(thisNewRow) );
		delete thisNewRow;
	    }

	    delete matchesThis;
	    delete matchesThat;
	}
        delete thatPeerSf;

    }

}

