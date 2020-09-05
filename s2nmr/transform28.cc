///////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
// $Log: not supported by cvs2svn $
// Revision 1.5  2005/03/04 23:32:05  madings
// checking in doc changes and a lot of incremental bugfixes
// too many to mention (cvs repository was broken and confused
// for a while so this represents about 5 months of stuff)
//
// Revision 1.4  2003/07/24 18:49:13  madings
// Changes to handle 2.1-3.0 mappings.
//
// Revision 1.3  2000/11/18 03:02:38  madings
// *** empty log message ***
//
// Revision 1.2  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
// Revision 1.1  1999/06/10  17:43:26  zixia
// *** empty log message ***
//
#endif
///////////////////////////////////////////////////////////////


#include "transforms.h"

void save_rename_saveframe(StarFileNode* inStar,
		        StarFileNode* outStar,
		        BlockNode*,
		        SaveFrameNode *currDicRuleSF)
{  
    
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int key_tag_pos = -1;
   int key_value_pos = -1;
   int renaming_tag_pos = -1;
   int renaming_prefix_pos = -1;
   int new_framecode_pos = -1;
   
   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, 
	   string( "_key_tag" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-28# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
       delete mapLoopMatch;
       return; /* do nothing */
   }
   
   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

   delete mapLoopMatch;

   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
   {
        if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
            key_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
            key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_renaming_tag" ) )
            renaming_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_renaming_prefix" ) )
            renaming_prefix_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_framecode_tag" ) )
            new_framecode_pos = i;
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-28# ERROR: input mapping file is missing tag '_key_tag'." 
                    << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-28# ERROR: input mapping file is missing tag '_key_value'." 
                    << endl;
       return; /* do nothing */
   }
   if( renaming_tag_pos < 0  )
   {   (*errStream) << "#transform-28# ERROR: input mapping file is missing tag '_renaming_tag'." 
                    << endl;
       return; /* do nothing */
   }

   string oldName;
   
   // For each row of the loop, rename the name of saveframe. 
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {   
       List<ASTnode*>        *matchSF;
       
       oldName = string("");
       
       if( (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue() == string("*") )
           matchSF = inStar->searchForType(
			    ASTnode::SAVEFRAMENODE );
       else if( (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() == string("*") ) 
	   matchSF = inStar->searchForTypeByTag(
			    ASTnode::SAVEFRAMENODE, 
			    (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue() );
       else
	   matchSF = inStar->searchForTypeByTagValue(
			    ASTnode::SAVEFRAMENODE, 
			    (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
			    (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
       
       
       for(int i=0; i<matchSF->size(); i++){
           
	   List<ASTnode*>        *matchTag;
	   SaveFrameNode         *peerSF;

	   char value[256]="save_";
	   if( renaming_prefix_pos >= 0 )
	   {
		strcat( value,
	                (*(*mapLoopTbl)[mL_Idx])[renaming_prefix_pos]->myValue().c_str() );
	   }

	   matchTag = (*matchSF)[i]->searchByTag(
	                  (*(*mapLoopTbl)[mL_Idx])[renaming_tag_pos]->myValue() );
	   peerSF = (SaveFrameNode *)(*matchSF)[i]->myParallelCopy();
	   if( peerSF == NULL ){
           
                (*errStream) << "#transform-28# Error: internal error, no peer at line "
                        << __LINE__ << ".  Fatal error, quitting rule." << endl;
                    delete matchTag;
                    delete matchSF;
                    return;
           }

	   
           if( matchTag->size() < 1){
	       continue;
           }
           
	   if( (*matchTag)[0]->isOfType(ASTnode::DATAITEMNODE) )
	   {
	       strcat(value,  ((DataItemNode*)(*matchTag)[0])->myValue().c_str() );
	       strcat(value, "\0");

	       if( renaming_prefix_pos >= 0 )
	       {
		   DataItemNode *dinpeer = (DataItemNode*)(*matchTag)[0];
		   if( dinpeer != NULL )
		       dinpeer->setValue(
			    (*(*mapLoopTbl)[mL_Idx])[renaming_prefix_pos]->myValue().c_str() +
			    dinpeer->myValue() ) ;
	       }
	   }
	   else if( (*matchTag)[0]->isOfType(ASTnode::DATANAMENODE) )
	   {
	       // Find the first loop value to the tag and use that.
	       ASTnode *cur;
	       int     nest;
	       int     col;
	       for(   cur = (*matchTag)[0] ;
	              cur != NULL && ! cur->isOfType(ASTnode::DATALOOPNODE) ;
		      cur = cur->myParent()   )
	       { /*null body*/ }
	       DataLoopNode *dln = (DataLoopNode*)cur;
	       dln->tagPositionDeep( ( (DataNameNode*)( (*matchTag)[0] ) )->myName(), &nest, &col );
	       if( nest > 1 )
	       {   cerr<<"#transform-28# ERROR: tag with name"
                      <<(*(*mapLoopTbl)[mL_Idx])[renaming_tag_pos]->myValue()<<" is nested."<<endl;
		   delete matchTag;
		   break;
	       }
	       LoopTableNode *tbl = dln->getValsPtr();
	       string firstLoopVal = string("S2NMR_ERROR:VALUE_NOT_FOUND");
	       if( tbl->size() > 0 )
		   firstLoopVal = (*(*tbl)[0])[col]->myValue();

	       strcat(value,  firstLoopVal.c_str() ); // Changed 2003-10-16

	       if( renaming_prefix_pos >= 0 )
	       {
		   DataValueNode *dvnPeer = (DataValueNode*) (*(*tbl)[0])[col]->myParallelCopy();
		   if( dvnPeer != NULL )
		   {
		       dvnPeer->setValue( 
			    (*(*mapLoopTbl)[mL_Idx])[renaming_prefix_pos]->myValue().c_str() +
			    dvnPeer->myValue() );
		   }
	       }

	       strcat(value, "\0");

	   }

	   
	   for(int j=0; j<strlen(value); j++){
	       
               if(value[j]==' '){
                  value[j]='_';
	       }
	   }


	   string newName = (string)value;

	   // Rename, but only if the value is not null:
	   if( newName != string("save_.") && newName != string("save_?") )
	   {
	       oldName = peerSF->myName();
	       peerSF->changeName( newName );
	       List<ASTnode*> *framecodes = 
	          outStar->searchForType( ASTnode::DATAVALUENODE, DataValueNode::FRAMECODE );
	       if( strncmp( oldName.c_str(), "save_", 5 ) == 0 )
	          oldName = string( oldName.c_str()+5 );
	       if( strncmp( newName.c_str(), "save_", 5 ) == 0 )
		  newName = string( newName.c_str()+5 );
	       int framecode_idx;
	       int j;
	       int strl;
	       for( framecode_idx = 0 ; 
	            framecode_idx < framecodes->size() ;
		    framecode_idx++ )
	       {
	           DataValueNode *dvn = (DataValueNode*)(*framecodes)[framecode_idx];
		   strl = strlen( dvn->myValue().c_str() );
		    
		   // string equal with ignore case:
		   // If the strings are different lengths, don't bother
		   // checking at all:
		   if( strl == strlen( oldName.c_str() ) )
		   {
		       for( j = 0 ; j < strl ; j++ )
		       {   if( toupper( (dvn->myValue().c_str() ) [j]) != toupper(oldName[j]) )
			      break;
		       }
		       if( j == strl ) // they were equal;
			   dvn->setValue( newName );
		   }
	       }
	       delete framecodes;
	   }

	   // Also make a new value for that tag itself to match the new name:
	   if( renaming_prefix_pos >= 0 && new_framecode_pos >= 0 )
	   {
	       List<ASTnode*> *matchSF_framecode_tag;
	       matchSF_framecode_tag =
	            peerSF->searchForTypeByTag(
				ASTnode::DATAITEMNODE,
				(*(*mapLoopTbl)[mL_Idx])[new_framecode_pos]->myValue() );
	       if( matchSF_framecode_tag->size() > 0 ) 
	       {
	           DataItemNode *sf_framecode_tag = (DataItemNode*) (*matchSF_framecode_tag)[0];
		   if( sf_framecode_tag != NULL )
		   {   sf_framecode_tag->setValue( newName );
		   }
	       }
	   }
	   
	   delete matchTag;
       }
       delete matchSF;
   }
}
