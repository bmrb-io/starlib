/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
/* --------------------------------------------
 * $Id: formatNMRSTAR.cc,v 1.6 2007/02/13 22:26:59 madings Exp $
 * --------------------------------------------
 * (This is based on zhaohua's AddComment.cc code
 * from validate_server.)
 *
 * This function will format a StarFileNode so it
 * adheres to the preferred decoration format of
 * NMRSTAR.  (The right comments, the right number
 * of iterations per line of text on simple loops,
 * etc.)
 *
 * $Log: formatNMRSTAR.cc,v $
 * Revision 1.6  2007/02/13 22:26:59  madings
 * *** empty log message ***
 *
 * Revision 1.5  2003/03/03 21:47:25  madings
 * removed debug messages from formatNMRSTAR
 *
 * Revision 1.4  2003/02/28 03:16:28  madings
 * Fixed formatNMRSTAR that was failing after the
 * ability to store comments at any level of the
 * AST tree had been taken away.
 *
 * Revision 1.3  2001/12/04 20:58:19  madings
 * Major changes  - see doc/changes.html for details:
 *    - setstring / getstring interface at the  loop level
 *    - implement #<START-SKIP> ... #<END-SKIP> directives
 *    - fix compiler errors related to use of iterators and const
 *         on newer versions of gcc.
 *    - ValCache usage, with LoopRows containing one string for the
 *         whole row instead of seperate data value nodes.
 *    - change the type of the *os file stream because the ability to
 *         construct from a unix fileno() handle has been depreciated in
 * 	newer C++ stream libraries.
 *
 * Revision 1.2  2001/11/13 20:30:35  madings
 * This update includes several changes:
 * (See doc/changes.html for the full explanation)
 *    1- The previous compilers we had been using had allowed us to get
 *        away with some mis-use of the const specifier without flagging
 *        it as a warning or error.  That is no longer the case, so we had
 *        to alter some of the code's use of "const".
 *    2- The STL no longer allows vector iterators to be interchangable
 *        with pointers, so we lost the ability to use that feature for
 *        users of the library.
 *    3- The streams library no longer allows you to make an ofstream out of
 *        a pipe or standard output.  Callers of this method must now make
 *        'os' be a pointer to an ostream instead of ofstream.
 *    4- The newer compilers no longer allow a reference to a temporary store
 *        that was created just for the duration of the function call.
 *
// Revision 1.1.1.1  2001/04/17  10:00:41  madings
//
// Revision 2.1  1999/10/18  23:00:20  madings
// Added formatNMRSTAR.cc for the first time.
//
 */

/*
 * $Log: formatNMRSTAR.cc,v $
 * Revision 1.6  2007/02/13 22:26:59  madings
 * *** empty log message ***
 *
 * Revision 1.5  2003/03/03 21:47:25  madings
 * removed debug messages from formatNMRSTAR
 *
 * Revision 1.4  2003/02/28 03:16:28  madings
 * Fixed formatNMRSTAR that was failing after the
 * ability to store comments at any level of the
 * AST tree had been taken away.
 *
 * Revision 1.3  2001/12/04 20:58:19  madings
 * Major changes  - see doc/changes.html for details:
 *    - setstring / getstring interface at the  loop level
 *    - implement #<START-SKIP> ... #<END-SKIP> directives
 *    - fix compiler errors related to use of iterators and const
 *         on newer versions of gcc.
 *    - ValCache usage, with LoopRows containing one string for the
 *         whole row instead of seperate data value nodes.
 *    - change the type of the *os file stream because the ability to
 *         construct from a unix fileno() handle has been depreciated in
 * 	newer C++ stream libraries.
 *
 * Revision 1.2  2001/11/13 20:30:35  madings
 * This update includes several changes:
 * (See doc/changes.html for the full explanation)
 *    1- The previous compilers we had been using had allowed us to get
 *        away with some mis-use of the const specifier without flagging
 *        it as a warning or error.  That is no longer the case, so we had
 *        to alter some of the code's use of "const".
 *    2- The STL no longer allows vector iterators to be interchangable
 *        with pointers, so we lost the ability to use that feature for
 *        users of the library.
 *    3- The streams library no longer allows you to make an ofstream out of
 *        a pipe or standard output.  Callers of this method must now make
 *        'os' be a pointer to an ostream instead of ofstream.
 *    4- The newer compilers no longer allow a reference to a temporary store
 *        that was created just for the duration of the function call.
 *
// Revision 1.1.1.1  2001/04/17  10:00:41  madings
//
// Revision 2.1  1999/10/18  23:00:20  madings
// Added formatNMRSTAR.cc for the first time.
//
 * Revision 1.2  1999/09/14 15:38:58  cai
 * set the server part to the public directory, /bmrb/bin
 *
// Revision 1.1.1.1  1999/02/26  20:54:16  zhaohua
// server for validation software
//
// Revision 1.3  1998/11/30  23:07:30  zhaohua
// adding the doc++ document
//
// Revision 1.2  1998/10/16  19:34:41  zhaohua
// Adding a few more validation functions
//
// Revision 1.1  1998/08/24  20:44:58  zhaohua
// *** empty log message ***
//
 * $Id: formatNMRSTAR.cc,v 1.6 2007/02/13 22:26:59 madings Exp $
 */

#include "ast.h"

/** This function formats the file according to the format
  * defined in the formatTree passed in (which is a text
  * file defining the commenting and loop spacing format
  * for parts of the star file).
  * <P>
  * This function formats the file using the user-level
  * calls like 'setPreComment()' and 'setRowsPerLine()'
  * to configure the star tree passed in, such that when
  * it is Unparse()'ed later, it will print out in the 
  * right format.
  * <P>
  * TODO - Describe the formatTree syntax here when
  *        it is finally fully fleshed out.
  * <P>
  * @param inTree the StarFileNode to modify.
  * @param formatTree the StarFileNode (already parsed)
  *                   that holds the configuration information
  *                   for this function.  Its format is explained
  *                   above.
  */
void formatNMRSTAR( StarFileNode *inTree, StarFileNode *formatTree )
{
    
    List<ASTnode *>  *frameMatch = NULL;
    List<ASTnode *>  *loopMatch = NULL;

    SaveFrameNode    *saveframeNode;
    DataLoopNode     *dataLoopNode;
    DataValueNode    *dataValNode;
    
    List<DataValueNode *>  *valList = new List<DataValueNode *>;
                                    //may be changed to read from the list instead of defination
    int               tagNum1 = 3;  //tag number of first level
    int               tagNum2 = 1;  //tag number of second level
    
    string            comment;
    string            everyFlag;
    string            category;
    string            tag;
    
    List<ASTnode *>  *match;
    SaveFrameNode    *matchSaveFrame;
    DataNameNode     *matchDataName;
    

    //get frameMatch
    frameMatch = formatTree->searchForTypeByTagValue(
	    ASTnode::SAVEFRAMENODE,
	    "_Saveframe_category", 
	    "comment" );

    if(frameMatch->Length() != 1){
	cerr<<"# of matched saveframenode is "<<frameMatch->Length()<<endl;
	delete frameMatch;
	return;
    }
    
    //get loopMatch
    frameMatch->Reset();
    saveframeNode = (SaveFrameNode *) (frameMatch->Current());
    loopMatch = saveframeNode->searchForType(ASTnode::DATALOOPNODE);
    if(loopMatch->Length() != 1){
	cerr<<"# of matched dataloopnode is "<<loopMatch->Length()<<endl;
	delete frameMatch;
	delete loopMatch;
	return;
    }
    
    //get dataValNode
    loopMatch->Reset();
    dataLoopNode = (DataLoopNode *) (loopMatch->Current());

    DataLoopValListNode::Status retStatus; 
    dataLoopNode->reset();
    retStatus = dataLoopNode->returnNextLoopElement( dataValNode );

    while( retStatus != DataLoopValListNode::END ){
	//get valList
	while( retStatus == DataLoopValListNode::CONTINUE ) {
	    valList->AddToEnd(dataValNode);
	    retStatus = dataLoopNode->returnNextLoopElement(dataValNode);
	}
        /* test
	valList->Reset();
	while(!valList->AtEnd()){
	    cout<<"tag = "<<valList->Current()->myName();
	    cout<<endl;
	    valList->Next();
	}
        end test */
	
	//get comment
	valList->Reset();
	if(valList->Length() < (tagNum1 + tagNum2)){
	    cerr<<"wrong # of values: "<<valList->Length()<<endl;
	    return;
	}
	comment = valList->Current()->myName();
	valList->Next();
	everyFlag = valList->Current()->myName();
	valList->Next();
	category = valList->Current()->myName();
	valList->Next();
	
	//search STAR file to add the comment
	string cat_tag;
	const char *startPos = category.c_str();
	const char *colonPos = strchr( startPos, ':' );
	if( colonPos == NULL )
	{
	    // 2.1 verison:
	    cat_tag = string("_Saveframe_category");
	}
	else
	{
	    // 3.0 and above verison:
	    cat_tag = category.substr( 0, (colonPos-startPos) ) ;
	    category = string( colonPos+1 ) ;
	}
	match = inTree->searchForTypeByTagValue(
		ASTnode::SAVEFRAMENODE,
		cat_tag,
		category);
	if(match->Length() != 0){
	    bool done = false;
	    for(match->Reset(); !done && !match->AtEnd(); match->Next()) {
		while( ! valList->AtEnd() ) {
		    ASTnode *matchNode = match->Current();
		    tag = valList->Current()->myName();
		    valList->Next();
		    if( tag != string(".") ) {
			List<ASTnode*>*match2 = matchNode->searchByTag( tag );
			if( match2->Length() == 0 ) {
			    matchNode = NULL;
			}
			if( match2->Length() > 0 ) {
			    matchNode = (*match2)[0];
			}
			delete match2;
		    }
		    if( matchNode ) {
		        matchNode->setPreComment(comment);
		    }
		}
		if( everyFlag == string("N") ) {
		    done = true;
		}
	    }
	}

	match->FreeList();
	valList->FreeList();
	retStatus = dataLoopNode->returnNextLoopElement( dataValNode );
        if(retStatus == DataLoopValListNode::STOP )
	    retStatus = dataLoopNode->returnNextLoopElement( dataValNode );
    }
    
    //thisAST->Unparse(0); in validation.cc destructor


    delete frameMatch;
    delete loopMatch;
    delete valList;
    delete match;
    
    //
    // Additionally, The following loop needs to be writeen 5-per
    // line: The loop starting with _Residue_seq_code in the
    // save frame with category = monomeric_polymer
    match = inTree->searchForTypeByTagValue(
			ASTnode::SAVEFRAMENODE,
			string("_Saveframe_category"),
			string("monomeric_polymer") );
    int matchIdx;
    int loopMatchIdx;
    for( matchIdx = 0 ; matchIdx < match->size(); matchIdx++ )
    {
	loopMatch = (*match)[matchIdx]->searchForTypeByTag(
			ASTnode::DATALOOPNODE,
			string("_Residue_seq_code") );
	for(    loopMatchIdx = 0 ;
		loopMatchIdx < loopMatch->size() ;
		loopMatchIdx++    )
	{
	    ( (DataLoopNode*) ((*loopMatch)[loopMatchIdx]) )->setRowsPerLine(5);
	}
    }
}
