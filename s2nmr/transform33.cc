///////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.3  2000/11/18  02:26:30  madings
// Fixed a problem that was common to several of the files (apparently
// all the ones edited by 'june', although it's hard to tell if this is
// her doing or not.)  The problem is that there were several places in
// the code where something like this had been done:
//     char *strPtr1, *strPtr2;
//     ...
//     /* strPtr points at some string right now */.
//     strPtr2 = new char [ strlen(strPtr1) ];  /* error: needs a +1 */
//     strcpy( strPtr2, strPtr1 );
// The problem with this, of course. is that strlen() gives the length
// of the string NOT including the null terminator, so the above code
// doesn't allocate enough space for strPtr2 to store the terminator.
// The result is that when the strcpy() is called, a zero-byte (null
// terminator) overwrites whatever was in the next byte just after strPtr2.
// Depending on what that next byte actually is, this might corrupt the
// value, or might not if it was supposed to be a zero anyway.  I suspect
// that this is what was causing some erratic segfaults we were seeing
// in this program - where seemingly irrelevant changes to the input data
// would 'fix' crashes.  (Even though the data changed had nothing to do with
// the part of the code that crashed.)  I suspect that the overwritten byte
// was part of the data the C-library heap uses internally to keep track of
// how much memory a variable on the heap is taking up (so it knows how much
// to free later when free() is called).  When 'free()' was being called, the
// wrong amount of memory was being freed, and thus the heap was a jumbled
// mess after that.)
//
// Revision 1.2  1999/07/30  18:22:08  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.1  1999/06/18 21:14:53  zixia
*** empty log message ***

// Revision 1.2  1999/06/11  19:01:32  zixia
// *** empty log message ***
//
// Revision 1.1  1999/06/10  17:39:47  zixia
// Add new function transform19.
//
*/
#endif
//////////////////////////////////////////

#include "transforms.h"
void save_assign_citation_type(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF)
{
   List<ASTnode*> *matchSF;
   List<ASTnode*> *journal_abbre_tag;
   List<ASTnode*> *citation_type_tag;
   DataItemNode   *dataItemPeer;
   
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame){//Return if the target saveframe is not found
      return;
   }

   //Find the saveframe with a tag_value pair (_Saveframe_category, entry_citation).
   matchSF = inStar->searchForTypeByTagValue(
	                                 ASTnode::SAVEFRAMENODE,
					 string("_Saveframe_category"),
					 string("entry_citation") );
   
   if( matchSF->size() != 1){
       (*errStream)<<"#transform-33# ERROR: only one saveframe with (_Saveframe_category, entry_citation) "
	           <<"should be found."
	           <<endl;
       delete matchSF;
       return;
   }
 
   //Find the tag '_Journal_abbreviation'
   journal_abbre_tag = ((SaveFrameNode*)(*matchSF)[0])->searchForTypeByTag(
	                        ASTnode::DATAITEMNODE,
				string("_Journal_abbreviation") );
   
   if( journal_abbre_tag->size() != 1){
      (*errStream)<<"#transform-33# ERROR: only one tag _Journal_abbreviation should be found."
	  <<endl;
      delete journal_abbre_tag;
      delete matchSF;
      return;
   }
   
   //Find the tag '_Citation_type'.
   citation_type_tag =((SaveFrameNode*)(*matchSF)[0])->searchForTypeByTag(
                          ASTnode::DATAITEMNODE,
			  string("_Citation_type") );

   if( citation_type_tag->size() != 1){
        (*errStream)<<"#transform-33# ERROR: only one tag _Citation_type should be found."
	            <<endl;
	delete citation_type_tag;
	delete journal_abbre_tag;
	delete matchSF;
	return;
   }
  
   //Find the peer of '_citation_type'.
   dataItemPeer = (DataItemNode*)(*citation_type_tag)[0]->myParallelCopy();
   
   if(dataItemPeer == NULL){
     (*errStream)<<"#transform-33# ERROR: on parallel copy found for _Citation_type"
	         <<endl;
     delete citation_type_tag;
     delete journal_abbre_tag;
     delete matchSF;
     return;
   }

   
   //If the value of '_Journal_abbreviation' is not "." or "?", 
   //assign 'journal' as the value of '_Citation_type'.
   if( strcasecmp(((DataItemNode*)(*journal_abbre_tag)[0])->myValue().c_str(), "?") != 0
	   && strcasecmp(((DataItemNode*)(*journal_abbre_tag)[0])->myValue().c_str(), ".") != 0 ){
            dataItemPeer->setValue(string("journal"));
   }
	   
   delete citation_type_tag;
   delete journal_abbre_tag;
   delete matchSF;
   
}
