///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
// $Log: not supported by cvs2svn $
// Revision 1.3  2008/04/16 21:29:19  madings
// Haven't committed in a while and this is a working version so I should.
//
// Revision 1.2  2006/10/10 19:31:56  madings
// Changed the sprintf so that now when the field is '?' or '.', it renders
// in the output as a null string instead.
//
// Revision 1.1  2006/07/14 21:00:34  madings
// *** empty log message ***
//
*/
#endif
///////////////////////////////////////////////////////////////

#include "transforms.h"
#include <regex.h>

static void do_all_iterations(
	    int input_sf_key_tag_pos,
	    int input_sf_key_value_pos,
	    int output_tag_pos,
	    int format_tag_pos,
	    LoopTableNode *mapLoopTbl, 
	    StarFileNode *inStar,
	    StarFileNode *outStar );

static void do_one_iteration(
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_tag,
	    string    format_tag,
	    LoopTableNode *mapInnerLoop,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar );

void save_join_via_sprintf(
	                 StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF  )
{
    List<ASTnode*>        *mapLoopMatch;
    DataLoopNameListNode  *mapNames;
    LoopTableNode         *mapLoopTbl;
    int                   input_sf_key_tag_pos = -1;
    int                   input_sf_key_value_pos = -1;
    int                   output_tag_pos = -1;
    int                   format_tag_pos = -1;




    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_input_sf_key_tag" ) );
    if( mapLoopMatch->size() <= 0 )
    {
	(*errStream) << "#transform-69# ERROR: no loop tag called '_input_sf_key_tag' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_key_tag" ) )
	    input_sf_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_key_value" ) )
	    input_sf_key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_tag" ) )
	    output_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_format_tag" ) )
	    format_tag_pos = i;
    }
    if( input_sf_key_tag_pos < 0  )
    {   (*errStream) << "#transform-69# ERROR: input mapping file is missing tag 'input_sf_key_tag'." << endl;
	return; /* do nothing */
    }
    if( input_sf_key_value_pos < 0  )
    {   (*errStream) << "#transform-69# ERROR: input mapping file is missing tag '_input_sf_key_value'." << endl;
	return; /* do nothing */
    }
    if( output_tag_pos < 0 )
    {   (*errStream) << "#transform-69# ERROR: input mapping file is missing tag '_output_tag'." << endl;
	return; /* do nothing */
    }
    if( format_tag_pos < 0 )
    {   (*errStream) << "#transform-69# ERROR: input mapping file is missing tag '_format_tag'." << endl;
	return; /* do nothing */
    }

    do_all_iterations(
	    input_sf_key_tag_pos,
	    input_sf_key_value_pos,
	    output_tag_pos,
	    format_tag_pos,
	    mapLoopTbl, inStar, outStar );

    return;
}

// Loop over all the iterations of the mapping file, doing all
// the rules found there:
static void do_all_iterations(
	    int   input_sf_key_tag_pos,
	    int   input_sf_key_value_pos,
	    int   output_tag_pos,
	    int   format_tag_pos,
	    LoopTableNode *mapLoopTbl,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar )
{
    int i;

    for( i = 0 ; i < mapLoopTbl->size() ; i++ )
    {
	do_one_iteration( 
	    (*(*mapLoopTbl)[i])[input_sf_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[format_tag_pos]->myValue(),
	    (*mapLoopTbl)[i]->myLoop(), inStar, outStar );
    }
}

// Do one iteration of the mapping file
static void do_one_iteration(
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_tag,
	    string    format_tag,
	    LoopTableNode *mapInnerLoop,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar )
{
    // For each matching saveframe with key tag/value:
    List<ASTnode*> *matchedFrames = NULL;
    if( input_sf_key_value == string("*") ) 
    {   matchedFrames = inStar->searchForTypeByTag( ASTnode::SAVEFRAMENODE, input_sf_key_tag );
    }
    else 
    {   matchedFrames = inStar->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, input_sf_key_tag, input_sf_key_value );
    }
    for( int frames_i = 0 ; frames_i < matchedFrames->size() ; frames_i++ )
    {
        SaveFrameNode *inputSF = (SaveFrameNode*)( (*matchedFrames)[frames_i] );

        // Build a list of the values from the inner loop tags:
	List<string> paramTags;
	paramTags.erase(paramTags.begin(),paramTags.end());
	List<string> paramVals;
	ASTnode *parentContext = NULL;
	int loopIter = 0;
	for( int inner_i = 0 ; inner_i < mapInnerLoop->size() ; inner_i++ )
	{
	    string paramTagName = (* ((*mapInnerLoop)[inner_i]))[0]->myValue();  // only one column in inner loop.
	    paramTags.insert( paramTags.end(), paramTagName );
	    string cookedTagName = paramTagName;
	    const char *spacePos = strchr( cookedTagName.c_str(), ' ' );
	    if( spacePos != NULL )
	    {   cookedTagName.resize( spacePos - cookedTagName.c_str() );
	    }

	    // If parentContext not set yet (this is the first parameter), then
	    // find the paramTag in the same sf - get it's context (loop row or saveframe).
	    if( parentContext == NULL )
	    {
	        List<ASTnode*> *paramMatch = inputSF->searchByTag( cookedTagName );
		if( paramMatch->size() == 0 )
		{   continue;
		}
		parentContext = (*paramMatch)[0];
		while( parentContext == NULL ||
		       ( ( ! parentContext->isOfType(ASTnode::SAVEFRAMENODE ) ) && 
		         ( ! parentContext->isOfType(ASTnode::DATALOOPNODE ) ) )         )
		{
		   parentContext = parentContext->myParent();
		}
		if( parentContext == NULL )
		{   continue;
		}
	    }
	}
	if( parentContext == NULL )
	{   continue;
	}

	int numRows;
	// if free tags;
        if( parentContext->isOfType(ASTnode::SAVEFRAMENODE) )
	{   numRows = 1;
	}
	// else it's loop tags;
	else
	{
	    numRows = ( (DataLoopNode*)parentContext)->getValsPtr()->size();
	}

        int col, dummy;
	List<int> paramCols;
	paramCols.erase(paramCols.begin(),paramCols.end());
	for( int row = 0 ; row < numRows ; row++ )
	{
	    paramVals.erase( paramVals.begin(), paramVals.end() );
	    int maxCharLen = format_tag.length();
	    for( int tagNum = 0 ; tagNum < paramTags.size() ; tagNum++ ) 
	    {
	        string val;
		string thisTagName = paramTags[tagNum];
		string thisTagRegexStr = string("");
		const char *spacePos = strchr( thisTagName.c_str(), ' ' );
		if( spacePos != NULL )
		{   thisTagName.resize( spacePos - thisTagName.c_str() );
		    thisTagRegexStr = string(spacePos+1);
		}
		regex_t reg;
		regmatch_t matches[2];
		if( thisTagRegexStr != string("") )
		{
		    regcomp( &reg, thisTagRegexStr.c_str(), REG_EXTENDED );
		}
		if( parentContext->isOfType( ASTnode::SAVEFRAMENODE) )
		{  
		     List<ASTnode*> *matchTag = parentContext->searchForTypeByTag( ASTnode::DATAITEMNODE, thisTagName );
		     if( matchTag->size() == 0 )
		     {   val = string("");
		     }
		     else
		     {   val = ( (DataItemNode*)((*matchTag)[0]) )->myValue();
		     }

		     // If there was a regex, then do the regex trim:
		     if( thisTagRegexStr != string("") )
		     {
			 int err = regexec( &reg, val.c_str(), 2, matches, 0 );
			 if( err != 0 )
			 {
			    char errmsg[1024];
			    regerror( err, &reg, errmsg, 1024 );
			    // optional - put out errmsg somehow as an error message
			 }
			 if( err == 0 && matches[1].rm_so >= 0 )
			 {   val = val.substr( matches[1].rm_so, matches[1].rm_eo  - matches[1].rm_so );
			 }
		     }
		     if( val == string("?") || val == string(".") ) {
		       val = string("");
		     }
		     paramVals.insert( paramVals.end(), val );
		     maxCharLen += val.length();
		}
		else // loop case:
		{
		     col = -1; dummy = -1;
		     // If this is the first time through the loop, figure out the column position of the tag.
		     if( paramCols.size() <= tagNum )
		     {   ((DataLoopNode*)parentContext)->tagPositionDeep( thisTagName, &dummy, &col );
		         paramCols.insert( paramCols.end(), col );
		     }
		     // If this is NOT the first time through the loop, use the previously figured column position of the tag:
		     else
		     {   col = (paramCols)[tagNum];
		     }

		     if( col >= 0 ) 
		     {   val = (* ( ( (DataLoopNode*)parentContext)->getVals() [row]))[col]->myValue();
		     } else 
		     {   val = string("");
		     }
		     // If there was a regex, then do the regex trim:
		     if( thisTagRegexStr != string("") )
		     {
			 int err = regexec( &reg, val.c_str(), 2, matches, 0 );
			 if( err != 0 )
			 {
			    char errmsg[1024];
			    regerror( err, &reg, errmsg, 1024 );
			    // optional - put out errmsg somehow as an error message
			 }
			 if( err == 0 && matches[1].rm_so >= 0 )
			 {   val = val.substr( matches[1].rm_so, matches[1].rm_eo  - matches[1].rm_so );
			 }
		     }

		     if( val == string("?") || val == string(".") ) {
		       val = string("");
		     }
		     paramVals.insert( paramVals.end(), val );
		     maxCharLen += val.length();
		}
		if( thisTagRegexStr != string("") )
		{
		    regfree( &reg );
		}
	    }

	    char *outputValTmpStr = (char*) calloc( maxCharLen+200, sizeof(char) );
	    // Now paramVals is the list of parameters to format.
	    // Unfortunately we need this ugly case statement because there is
	    // no way to use the vsprintf format function when the number of parameters is
	    // unknown at compile time.  While I can use elipsis parameters like '...'
	    // in a library, there still needs to be a program CALLING that library that knows
	    // at COMPILE time how many parameters to put in that elipsis.  In this case,
	    // that information is not known until runtime, so there's no way to do this
	    // without this ugly if/else ladder for each case:
	    if( paramVals.size() == 0 )
	    {   // do nothing
	    } else if( paramVals.size() == 1 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str() );

	    } else if( paramVals.size() == 2 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str() );

	    } else if( paramVals.size() == 3 )
	    {
	      sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str() );

	    } else if( paramVals.size() == 4 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str(),
	      (paramVals)[3].c_str() );

	    } else if( paramVals.size() == 5 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str(),
	      (paramVals)[3].c_str(),
	      (paramVals)[4].c_str() );

	    } else if( paramVals.size() == 6 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str(),
	      (paramVals)[3].c_str(),
	      (paramVals)[4].c_str(),
	      (paramVals)[5].c_str() );

	    } else if( paramVals.size() == 7 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str(),
	      (paramVals)[3].c_str(),
	      (paramVals)[4].c_str(),
	      (paramVals)[5].c_str(),
	      (paramVals)[6].c_str() );

	    } else if( paramVals.size() == 8 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str(),
	      (paramVals)[3].c_str(),
	      (paramVals)[4].c_str(),
	      (paramVals)[5].c_str(),
	      (paramVals)[6].c_str(),
	      (paramVals)[7].c_str() );

	    } else if( paramVals.size() == 9 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str(),
	      (paramVals)[3].c_str(),
	      (paramVals)[4].c_str(),
	      (paramVals)[5].c_str(),
	      (paramVals)[6].c_str(),
	      (paramVals)[7].c_str(),
	      (paramVals)[8].c_str() );

	    } else if( paramVals.size() == 10 )
	    { sprintf( outputValTmpStr, format_tag.c_str(),
	      (paramVals)[0].c_str(),
	      (paramVals)[1].c_str(),
	      (paramVals)[2].c_str(),
	      (paramVals)[3].c_str(),
	      (paramVals)[4].c_str(),
	      (paramVals)[5].c_str(),
	      (paramVals)[6].c_str(),
	      (paramVals)[7].c_str(),
	      (paramVals)[8].c_str(),
	      (paramVals)[9].c_str() );

	    } else
	    { outputValTmpStr = "The rule only supports up to 10 %s parameters, this rule has too many."; // point it here instead.
	    }

	    DataValueNode::ValType  newDelim;
	    char charDelim = FindNeededQuoteStyle( string(outputValTmpStr) );
	    if( charDelim == '\'' )
	        newDelim = DataValueNode::SINGLE;
	    else if( charDelim == '\"' )
	        newDelim = DataValueNode::DOUBLE;
	    else if( charDelim == ';' )
	        newDelim = DataValueNode::SEMICOLON;
	    else 
	        newDelim = DataValueNode::NON;

	    // Now we know the new value.  Need to decide where to put it.
	    if( parentContext->isOfType(ASTnode::SAVEFRAMENODE ) )
	    {
	       // It's a free tag:
	       DataItemNode *newThing = new DataItemNode( output_tag, outputValTmpStr, newDelim );

	       SaveFrameNode *peerSF = (SaveFrameNode*) (((SaveFrameNode*)parentContext)->myParallelCopy() );
	       peerSF->GiveMyDataList()->insert(
	           peerSF->GiveMyDataList()->end(),
		   newThing );
	    }
	    else
	    {
	        // It's a looped tag, so first check to see if the target column was made on a previous
		// pass through.  If not, then make it first.
		col = -1; dummy = -1;
		DataLoopNode *peerLoop = (DataLoopNode*) (  ((DataLoopNode*)parentContext)->myParallelCopy()  );
		peerLoop->tagPositionDeep( output_tag, &dummy,&col );
		if( col < 0 ) {
		    peerLoop->getNames()[0]->insert(
		       peerLoop->getNames()[0]->end(),
		       output_tag );
		    col = peerLoop->getNames()[0]->size() - 1;
		}
		(* (peerLoop->getVals()[row]) )[col]->setValue( outputValTmpStr );
		(* (peerLoop->getVals()[row]) )[col]->setDelimType( newDelim );

	    }
	    free(outputValTmpStr);
	}

    }

    delete matchedFrames;
}
