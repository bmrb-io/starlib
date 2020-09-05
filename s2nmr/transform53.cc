///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"
#include <time.h>

void save_make_submission_date_3(  StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *,
				   SaveFrameNode *dictEntry )
{

    time_t    nowTicks;
    struct tm *nowFields;
    char      nowString[30];
    string    searchTag = string("_Entry.Submission_date");

    nowTicks = time(NULL);

    nowFields = localtime(&nowTicks);
    sprintf(nowString, "%04d-%02d-%02d",
	       1900 + nowFields->tm_year,
	       nowFields->tm_mon + 1,
	       nowFields->tm_mday );

    // We're going to have to find the entry_information
    // saveframe:
    List <ASTnode*>  *hits = outStar->searchForTypeByTag(
			  ASTnode::DATAITEMNODE, searchTag );
    
    if( hits->size() == 0 )
    {
        (*errStream) << "#transform53: error - no "<<searchTag<<
               " found to populate in the file."<<endl;
        return;
    }

    DataItemNode *theHit =
        (DataItemNode*) ((*hits)[0]);

    DataItemNode *newHit = new DataItemNode(
                     string(searchTag), string(nowString) );

    ASTnode *par;
    par = theHit;
    while( par!=NULL && (! par->isOfType(ASTnode::SAVEFRAMENODE )  ) )
    {
        par = par->myParent();
    }
    if( par != NULL )
    {
        delete theHit;

        ((SaveFrameNode*)par)->GiveMyDataList()->insert(
            ((SaveFrameNode*)par)->GiveMyDataList()->end(),
	    newHit );
    }
    delete hits;
    return;
}
