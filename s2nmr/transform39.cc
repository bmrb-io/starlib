//transform39.cc
//created 7/29/99, june
//this function parse the tag and take one of the token as the value of 
//another tag.

#ifdef BOGUS_DEF_COMMENT_OUT
/*
 * $Log: not supported by cvs2svn $
 */
#endif

#include"transforms.h"
void save_parse_tag_to_new_tag( StarFileNode *inStar, 
				StarFileNode *outStar, 
				BlockNode    *,
				SaveFrameNode *currDicRuleSF)
{
  List<ASTnode*>        *mapLoopMatch;
  List<ASTnode*>        *matchSF;
  DataLoopNameListNode  *mapNames;
  LoopTableNode         *mapLoopTbl;
  int mL_idx;
  int parse_tag_pos = -1;
  int parse_by_pos = -1;
  int which_token_pos = -1;
  int output_key_tag_pos = -1;
  int output_key_value_pos = -1;
  int output_tag_pos = -1;

  //Find loop from the mapping file:
   mapLoopMatch = currDicRuleSF->searchForTypeByTag(ASTnode::DATALOOPNODE, string("_parse_tag"));
   
   if( mapLoopMatch->size() <= 0)
     { 
       (*errStream) << "#transform-39# ERROR: no loop tag called '_parse_tag' in mapping file." << endl;
       delete mapLoopMatch;
       return; /* do nothing */
     }

   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames   =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();
   delete mapLoopMatch;
   
   
   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
        if(   (* ((*mapNames)[0]) )[i]->myName()==string("_parse_tag") )
	  parse_tag_pos = i;
	if(   (* ((*mapNames)[0]) )[i]->myName()==string("_parse_by") )
	  parse_by_pos = i;
	if(   (* ((*mapNames)[0]) )[i]->myName()==string("_which_token") )
	  which_token_pos = i;
	if(   (* ((*mapNames)[0]) )[i]->myName()==string("_output_key_tag") )
	  output_key_tag_pos = i;
	if(   (* ((*mapNames)[0]) )[i]->myName()==string("_output_key-value") )
	  output_key_value_pos = i;
	if(   (* ((*mapNames)[0]) )[i]->myName()==string("_output_tag") )
	  output_tag_pos = i;
    }

   if ( parse_tag_pos < 0)
     {   (*errStream) << "#transform-39# ERROR: input mapping file is missing tag '_parse_tag'." << endl;
        return; /* do nothing */
    }
   if ( parse_by_pos < 0)
     {   (*errStream) << "#transform-39# ERROR: input mapping file is missing tag '_parse_by'." << endl;
        return; /* do nothing */
    }
   if ( which_token_pos < 0)
     {   (*errStream) << "#transform-39# ERROR: input mapping file is missing tag '_which_token'." << endl;
        return; /* do nothing */
    }
   if ( output_key_tag_pos < 0)
     {   (*errStream) << "#transform-39# ERROR: input mapping file is missing tag '_output_key_tag'." << endl;
        return; /* do nothing */
    }
   if ( output_key_value_pos < 0)
     {   (*errStream) << "#transform-39# ERROR: input mapping file is missing tag 'output_key_value'." << endl;
        return; /* do nothing */
    }
   if ( output_tag_pos < 0)
     {   (*errStream) << "#transform-39# ERROR: input mapping file is missing tag '_output_tag'." << endl;
        return; /* do nothing */
    }

   for(mL_idx = 0; mL_idx < mapLoopTbl->size(); mL_idx++ )
     {
       
       //find parse_tag
       List<ASTnode*>* matchTagParse = inStar->searchByTag(      
			 (*(*mapLoopTbl)[mL_idx])[parse_tag_pos]->myValue()
			 );
       if( matchTagParse->size() != 1)
	 {
	   (*errStream) << "#transform39 error: only one parse tag should be found." << endl;
	   delete matchTagParse;
	   continue;
	 }

       //parse the value
       string parse_value = ((DataItemNode*)((*matchTagParse)[0]))->myValue();
       string parse_by =(*(*mapLoopTbl)[mL_idx])[parse_by_pos]->myValue();
       string which_token =(*(*mapLoopTbl)[mL_idx])[which_token_pos]->myValue();
       string new_data_value ; 
       //parse the value
       int whichToken = (atoi)( which_token.c_str() );
       string token = " ";
       if( whichToken >0)
	 {
	   token = strtok( (char*)parse_value.c_str(), parse_by.c_str());
	     for( int i = 1; i<whichToken; i++)
	       token = strtok( NULL, parse_by.c_str());
	 }

       char* temp = new char[strlen(token.c_str()) + 1];
       if( token != string(" ") )
	 {
	   strcpy( temp,  token.c_str());
	   new_data_value = temp;
	 }
       else
	 {
	   (*errStream) << "#transform39 error: no value found."<<endl;
	   continue;
	 }
       string ouput_tag = (*(*mapLoopTbl)[mL_idx])[output_tag_pos]->myValue();

       //find the output saveframe
        List<ASTnode*> *sfSearch = 
		outStar->searchForTypeByTagValue( 
                      ASTnode::SAVEFRAMENODE, 
		      (*(*mapLoopTbl)[mL_idx])[output_key_tag_pos]->myValue(),
		      (*(*mapLoopTbl)[mL_idx])[output_key_value_pos]->myValue()
		      );	
	if(sfSearch->size() <= 0)
	  {
	    (*errStream) << "#transform39 error: can't find output saveframe."<<endl;
	    delete sfSearch;
	    continue;
	  }

	//for each output saveframe
	for (int sf_idx = 0; sf_idx < sfSearch->size(); sf_idx++)
	  {
	    //search for the output tag
	    SaveFrameNode* output_sf = (SaveFrameNode*) (*sfSearch)[sf_idx];
	    List<ASTnode*>* output = output_sf->searchByTag(ouput_tag);
	    
	    if( output-> size() > 1)
	      {
		(*errStream) << "#transform39 error: only one output_tag should be found."<<endl;
		delete output;
		continue;
	      }
 
	    if( output->size()>0)
	      ((DataItemNode*)((*output)[0]))->setValue(new_data_value);
	    else
	      {
		DataItemNode* newDataItem = new DataItemNode(
                        ouput_tag, new_data_value);
		output_sf->GiveMyDataList()->insert(
                        output_sf->GiveMyDataList()->end(), newDataItem);
	      }
	    delete output;
	  }
	delete sfSearch;
     }
   
}
