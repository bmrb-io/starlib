///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2005/03/18 21:07:20  madings
Massive update after too long without a checkin

*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

void save_default_if_not_exist( StarFileNode *input, 
                                StarFileNode *output, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF)
{
   string currDicSaveFrameName = string("save_remove_null_tags");
   DataNode* currDicSaveFrame = currDicRuleSF;


   if(!currDicSaveFrame)//Return if the target saveframe is not found
       return; 
   int questionsToo;
   // Check to see if the question mark tag is on:
   List<ASTnode*>*matches = currDicSaveFrame->searchByTag("_question_marks_too");
   if( matches->size() > 0 )
       questionsToo = 1;
   else
       questionsToo = 0;
   if( matches != NULL ) delete matches;
   //Get the loop in the data dictionary
   string currDicLoopName = string("_key_tag");
   DataNode* currDicLoop = 
       ((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(currDicLoopName);

   List<DataNameNode*>* currDicNameList;
   List<DataValueNode*>* currDicValList;
   ((DataLoopNode*)currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);
   int length =  currDicNameList->Length();  
   int keyTag = 0;
   int keyValue = 0;
   int tagToChange = 0;
   int newValue = 0;
   
   for(int position = 0 ; position < currDicNameList->size() ; position++ )
   {
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_key_tag") == 0 )
          keyTag = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_key_value") == 0 )
          keyValue = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_tag_to_change") == 0 )
          tagToChange = position;
       if( strcmp( (*currDicNameList)[position]->myName().c_str(), "_new_value") == 0 )
          newValue = position;
   }
   currDicValList->Reset();
   string  *iterVal = new string[length];
   DataValueNode::ValType delim;

   while(!currDicValList->AtEnd())
   {
       int i;
       delim = DataValueNode::NON;
       //Get the values for one loop iteration
       for( i=0; i<length; i++)
       {
	   iterVal[i] = currDicValList->Current()->myName();
	   if( i == newValue )
	   {   delim = currDicValList->Current()->myDelimType();
	   }
	   if( iterVal[i] == string("*") ) {
	       (*errStream) << "#transform59# ERROR: wildcard asterisks are not supported in this rule (line "<<currDicValList->Current()->getLineNum()<<".";
	   }
	   currDicValList->Next();
       }

       //Apply this change to the relevant hits within this datablock

       List<ASTnode *> *hits = NULL;


       // if it's a dummy-don't-care value for the key tag, then build a list of all
       // values that hit (all rows of a loop if it's looped, or just the free tags
       // if it's free.
       if( iterVal[keyValue] == string("*") || iterVal[keyValue] == string(".") || iterVal[keyValue] == string("?") )
       {
	   List<ASTnode*> *tagHits = output->searchByTag( iterVal[keyTag] );
           hits = new List<ASTnode *>;
	   for( int i = 0 ; i < tagHits->size() ; ++i ) 
	   {
	       if( (*tagHits)[i]->isOfType(ASTnode::DATAITEMNODE) )
	       {
	           hits->insert(hits->end(), (*tagHits)[i]);
	       }
	       else
	       {
	           DataLoopNode *dln = (DataLoopNode*)
		            ((*tagHits)[i]->
			         myParent()->
				    myParent()->
				        myParent()  );
		   int col, nest;
		   dln->tagPositionDeep( iterVal[keyTag], &nest, &col );
		   if( col >= 0 )
		   {
		       for( int j = 0 ; j < dln->getVals().size() ; ++j )
		       {
			   // insert every value of the tag in the loop as a hit:
			   hits->insert(hits->end(), (*(dln->getVals()[j]))[col] );
		       }
		   }
	       }
	   }
	   delete tagHits;
       }
       // if it's got a real value for the key tag, then build a list of only
       // those cases where there was a match (so it doesn't end up affecting all
       // rows of the loop - just the ones that matched.)
       else
       {
	   hits = output->searchByTagValue( iterVal[keyTag], iterVal[keyValue] );
       }


       //Iterate over all the hits
       int hitIdx;
       for( hitIdx = 0 ; hitIdx < hits->size() ; hitIdx++ )
       {
	   List<ASTnode*> *tagToChangeHits = NULL;
	   LoopRowNode    *lrn = NULL;
	   DataLoopNode   *dln = NULL;
	   DataValueNode  *dvn = NULL;
	   DataItemNode   *din = NULL;
	   int            tagToChangeCol = -1;
	   string         tmpNewVal = iterVal[newValue];

	   // Get the value for the tag as it is right now:

           if( (*hits)[hitIdx]->isOfType( ASTnode::DATAITEMNODE ) )
	   {
	       // Get the saveframe that this free tag is inside of:
	       ASTnode *cur = NULL;
	       for( cur = (*hits)[hitIdx] ;
	           cur != NULL && 
		       !(cur->isOfType(ASTnode::SAVEFRAMENODE)) ;
		   cur = cur->myParent() )
	       {
	       }
	       if( cur != NULL )
	       {
	           tagToChangeHits = cur->searchByTag( iterVal[tagToChange] );

		   // If it's not there, then create it with a null value:
		   if( tagToChangeHits->size() == 0 )
		   {
		       din = new DataItemNode( iterVal[tagToChange], string(".") );
		       ((SaveFrameNode*)cur)->GiveMyDataList()->AddToEnd( din );
		       delete tagToChangeHits;
		       tagToChangeHits = new List<ASTnode*>;
		       tagToChangeHits->insert( tagToChangeHits->end(), din );
		   }
	       }
	       if( iterVal[newValue].c_str()[0] == '_' )
	       {
	           List<ASTnode*> *newValList = cur->searchForTypeByTag( ASTnode::DATAITEMNODE, iterVal[newValue] );
		   if( newValList->size() >= 0 )
		   {  tmpNewVal = ( (DataItemNode*)((*newValList)[0]) )->myValue();
		      delim     = ( (DataItemNode*)((*newValList)[0]) )->myDelimType();
		   }
		   delete newValList;
	       }
	       else
	       {   tmpNewVal = iterVal[newValue];
	       }
	   }
           else if( (*hits)[hitIdx]->isOfType( ASTnode::DATAVALUENODE ) )
	   {
	       // Get the loop row that this value is inside of:
	       ASTnode *cur = NULL;
	       for( cur = (*hits)[hitIdx] ;
	           cur != NULL && 
		       !(cur->isOfType(ASTnode::LOOPROWNODE)) ;
		   cur = cur->myParent() )
	       {
	       }
	       if( cur != NULL )
	       {
	           lrn = (LoopRowNode*)cur;
		   for( ;
		       cur != NULL && 
			   !(cur->isOfType(ASTnode::DATALOOPNODE)) ;
		       cur = cur->myParent() )
	           {
		   }
		   if( cur != NULL )
		   {
		       dln = (DataLoopNode*)cur;
		   }
		   int col = -1;
		   int nest = -1;
		   dln->tagPositionDeep( iterVal[tagToChange], &nest, &col );

		   // ignoring nesting level, assuming outermost:
		   if( col>=0 )
		   {
		       // insert the hit into the change list:
		       if( tagToChangeHits == NULL )
		           tagToChangeHits = new List<ASTnode*>;
		       tagToChangeHits->insert( tagToChangeHits->end(), (*lrn)[col] );
		   }
		   // If it's not there, insert it with a null values:
		   if( tagToChangeHits == NULL || tagToChangeHits->size() == 0 )
		   {
		       // Adding the name should cause it to insert the nulls for us:
		       dln->getNames()[0]->insert(
			   dln->getNames()[0]->end(), new DataNameNode( iterVal[tagToChange] ) );

		       // Now the last column in the row should be the newly added one:
		       if( tagToChangeHits == NULL )
		           tagToChangeHits = new List<ASTnode*>;
		       tagToChangeHits->insert( tagToChangeHits->end(), (*lrn)[lrn->size()-1] );
		   }
		   // If the value starts with an underscore:
		   if( iterVal[newValue].c_str()[0] == '_' )
		   {
		       // Get the value from the same row, from that tag:
		       dln->tagPositionDeep( iterVal[newValue], &nest, &col );
		       if( col>= 0 )
		       {   tmpNewVal = (*lrn)[col]->myValue();
		           delim     = (*lrn)[col]->myDelimType();
		       }
		   }
		   else
		   {   tmpNewVal = iterVal[newValue];
		   }
	       }
	   }
           else // Neither a DataItemNode nor a DataValueNode hit
	   {
	       (*errStream)<< "#transform59# ERROR: unknown hit type when searching for "
	                      "tag = "<<iterVal[keyTag]<<", value = "<<iterVal[keyValue]<<"."<<endl;
	       continue;
	   }

	   if( tagToChangeHits == NULL || tagToChangeHits->size() == 0 )
	   {
	       (*errStream)<< "#transform59# ERROR: failed to insert new value, this is an internal code error." <<
	                      "  This occured when operating on this row of the rule: "<<
	                      "_key_tag = "<<iterVal[keyTag]<<", _key_value = "<<iterVal[keyValue]<<"."<<endl;
	       continue;
	   }

	   // In either case, whether it was free or looped, tagToChangeHits now contains a single
	   // entity, and it's the value to change.

	   string oldVal;
	   if( (*tagToChangeHits)[0]->isOfType(ASTnode::DATAITEMNODE) )
	   {   oldVal = ( (DataItemNode*)( (*tagToChangeHits)[0] ) )->myValue();
	   }
	   else if( (*tagToChangeHits)[0]->isOfType(ASTnode::DATAVALUENODE) )
	   {   oldVal = ( (DataValueNode*)( (*tagToChangeHits)[0] ) )->myValue();
	   }

	   // only make a change if the data already there is null:
	   if( oldVal == string("?") || oldVal == string(".") || oldVal == string("") )
	   {

	       if( (*tagToChangeHits)[0]->isOfType(ASTnode::DATAITEMNODE) )
	       {   ( (DataItemNode*)( (*tagToChangeHits)[0] ) )->setValue( tmpNewVal );
	           ( (DataItemNode*)( (*tagToChangeHits)[0] ) )->setDelimType( delim );
	       }
	       else if( (*tagToChangeHits)[0]->isOfType(ASTnode::DATAVALUENODE) )
	       {   ( (DataValueNode*)( (*tagToChangeHits)[0] ) )->setValue( tmpNewVal );
	           ( (DataValueNode*)( (*tagToChangeHits)[0] ) )->setDelimType( delim );
	       }
	       else
	       {   (*errStream)<<"#transform59# ERROR: value to change ("<<iterVal[tagToChange]
			       <<") is not in the same scope as the key tag ("<<iterVal[keyTag]<<")."<<endl;
	       }
	      
	   }
	   delete tagToChangeHits;
	   tagToChangeHits = NULL;
       }
       delete hits;
    }
}
