///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.7  2000/11/18 03:02:39  madings
*** empty log message ***

// Revision 1.6  1999/07/30  18:22:09  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.5  1999/01/28 06:15:08  madings
Some bugfixes from Eldon's attempts to use this, as well as a major
design change:

   Bug fix 1: save_loops was re-creating tags that had been removed
      earlier in the run.  This has been fixed.

   Bug fix x: after bug fix 1 above, save_loops was leaving behind
      loops that had no contents.  This has been fixed.

   Massive design change:
      There is no longer any default order of operations at all.
      Instead, it uses the new 'save_order_of_operations' saveframe
      in the mapping dictionary file to decide what rules to run
      when.  Also, to allow it to have the same rule run more than
      once at different times from different mapping dictionary
      saveframes, the name of the saveframe is no longer the rule
      to run.  Instead the name of the saveframe is irrelevant, and
      it's the new _Saveframe_category tag that tells the program
      what rule to run for that saveframe.  This required a small,
      but repetative change in all the transform rules so that the
      main.cc program can tell the transforms which saveframe in the
      mapping file is the one they are supposed to use instead of the
      transforms trying to look for it themselves.

// Revision 1.4  1999/01/25  23:16:53  zixia
// Add some new transforms on the way to fixing the s2nmr converter.
//
// Revision 1.3  1999/01/25  22:40:08  zixia
// Testing CVS.
//
// Revision 1.2  1998/12/23  17:33:55  zixia
// *** empty log message ***
//
// Revision 1.1  1998/11/20  06:33:25  madings
// Added some new transforms on the way to fixing the s2nmr converter.
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_make_citation_full_tag(  StarFileNode *AST, 
                                   StarFileNode *NMR, 
                                   BlockNode    *,
			           SaveFrameNode *currDicRuleSF)
{
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame)//Return if the target saveframe is not found
       return; 

   string key_tag,key_value, new_tagname;

   // was like this - error prone: char * new_tagvalue = new char[200]; 
   char new_tagvalue[20000]; // Wasteful, but it is on the stack so it will
                             // go away after this function is done, so it's
			     // not too bad.  Ideally, we should use a
			     // C++ string, or have code to dynamically
			     // allocate a big enough C-string.  But this
			     // will have to do for now - I don't have time.

   //Initialize the values for key_tag, key_value, new_tagname;
   key_tag = string("_Saveframe_category");
   key_value = string("entry_citation");
   new_tagname = string("_Citation_full");

   string currDicLoopName = string("_key_tag");
   DataNode* currDicLoop = 
       ((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(currDicLoopName);

   List<DataNameNode*>* currDicNameList;
   List<DataValueNode*>* currDicValList;
   ((DataLoopNode*)currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);

   currDicValList->Reset();
   string* interval = new string[9];
   //Initialize interval value.
   for(int j=0;j<9;j++)
   {
       interval[j] = tagToVal(currDicValList->Current()->myName());
       currDicValList->Next();
   }
   
   List<ASTnode *> *return_frame_list =
       AST->searchForTypeByTagValue(ASTnode::SAVEFRAMENODE,key_tag,key_value);

   if(return_frame_list->size() != 1)
       (*errStream)<<"#transform-6# The save frame containing _Saveframe_category is not found."<<endl;
   else
   {
       //Debug cout<<"length of return_frame_list = "<<return_frame_list->Length()<<endl;
       
       for(return_frame_list->Reset(); !return_frame_list->AtEnd();return_frame_list->Next())
       {
	   SaveFrameNode *astSaveFrame;
	   
	   //Get the values from author loop
	   List<ASTnode*> *matchAuthorLoop =
	       ((SaveFrameNode*)return_frame_list->Current()
	       )->searchForTypeByTag( ASTnode::DATALOOPNODE, interval[0] );
	   DataNode *authorLoop;
	   if( matchAuthorLoop->size() > 0 )
	       authorLoop = (DataNode*)(*matchAuthorLoop)[0];
	   else
	       authorLoop = NULL;
	   delete matchAuthorLoop;
	   if( authorLoop == NULL )
	   {
	       (*errStream)<<"transform-6: The loop containing a tag called"
		   << interval[0] << " was not found in "
		   << ((SaveFrameNode*)(return_frame_list->
		        Current()))->myName() <<"."<<endl;
	       break;
	   }
           astSaveFrame = (SaveFrameNode*)return_frame_list->Current();
	   
	   List<DataNameNode*>* authorTagList;
	   List<DataValueNode*>* authorValueList;
	   ((DataLoopNode*)authorLoop)->FlattenNestedLoop(authorTagList,authorValueList);
    
           int length = authorTagList->Length();
           authorValueList->Reset();
	   
           int author_ordinal;
	   int author_family;
	   int author_given;
	   int author_middle;
	   int author_title;

	   for(int position = 0; position < length; position++){
              if(strcmp( (*authorTagList)[position]->myName().c_str(), "_Author_ordinal") == 0 )
		  author_ordinal = position;
	      if(strcmp( (*authorTagList)[position]->myName().c_str(), "_Author_family_name") == 0 )
		  author_family = position;
	      if(strcmp( (*authorTagList)[position]->myName().c_str(), "_Author_given_name") == 0 )
		  author_given = position;
	      if(strcmp( (*authorTagList)[position]->myName().c_str(), "_Author_middle_initials") == 0 )
		  author_middle = position;
	      if(strcmp( (*authorTagList)[position]->myName().c_str(), "_Author_family_title") == 0 )
		  author_title = position;
	   }
	   
           string* authornames = new string[length];
	   int valuelength = (authorValueList->Length())/length;
	   
           int counter = 1;
           while(!authorValueList->AtEnd())
           {
	       string tmpstring; //Holding individual char.
	       char * tmp_name = new char[10];
	       
	       //Get all names for a author.
	      for(int i=0; i<length; i++)
	      {
	         authornames[i] = tagToVal(authorValueList->Current()->myName());
	         authorValueList->Next();
	      }
	     
	      //Between the last author and next last author, there should be "and"
	      if(counter == valuelength)
	      {
		  strcat(new_tagvalue, "and ");
	      }
	      
              //Put the family name in the new_tagvalue.
              strcat(new_tagvalue,  authornames[author_family].c_str());
	      
              //Put "," after family name. 
	      strcat(new_tagvalue, ", ");
	     
	      //Put the initial of given name in new_tagvalue.
	      strcpy(tmp_name, authornames[author_given].c_str());
	      int given_name_length = strlen(tmp_name);
	      
	      tmpstring = tmp_name[0];
	      
	      strcat(new_tagvalue, tmpstring.c_str());
	      //If the given name is Wen-Pine, it will be W-P
	      for(int p=1;p<given_name_length;p++)
	      {
		  tmpstring = tmp_name[p];
		  if(strcmp(tmpstring.c_str(), "-") == 0 )
		  {
		      strcat(new_tagvalue, tmpstring.c_str());
		      tmpstring = tmp_name[p+1];
		      strcat(new_tagvalue,  tmpstring.c_str());
		      break;
		  }
	      }
              //Put a . and , at the end of given name initial.
	      strcat(new_tagvalue, ".");
	      
              //_Author_middle_initials
	      strcpy(tmp_name, authornames[author_middle].c_str());
              tmpstring = tmp_name[0];
	      if(strcmp(tmpstring.c_str(),".") != 0)
	      {   
		  int cnt = 0;
		  strcat(new_tagvalue, " ");
		  for(int m = 0; m<strlen(tmp_name); m++)
		  {   
		      tmpstring = tmp_name[m];
		      if(strcmp(tmpstring.c_str(), ".") == 0)
		      { cnt++;
		      }
		  }
		  if(cnt == 1 || cnt == 0){
		      tmpstring = tmp_name[0];
                      strcat(new_tagvalue, tmpstring.c_str() );
		  }
		  else{
                      strcat(new_tagvalue, tmp_name);   
		  }
		  strcat(new_tagvalue, ".");
	      }
	      
	      //_Author_family_title
              strcpy(tmp_name, authornames[author_title].c_str());
	      tmpstring = tmp_name[0];
	      if(strcmp(tmpstring.c_str(), ".") != 0 )	  
	      {   strcat(new_tagvalue, " ");
		  strcat(new_tagvalue, authornames[author_title].c_str());
	      }

	      strcat(new_tagvalue, ", ");
              counter++;
	      delete [] tmp_name;
            }

           //So far done with the authors.
	   //_Citation_title.

	   List <ASTnode *> *target_tags =
	                astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[1]);
           if(target_tags->Length() != 1 || target_tags == NULL )
	       (*errStream)<<"#transform-6# Error: no or more than one _Citation_title in the save frame."<<endl;
	   else
	   {   strcat(new_tagvalue, "\"");
	       strcat(new_tagvalue, (((DataItemNode *)( (*target_tags)[0] ))->myValue()).c_str());
	       strcat(new_tagvalue, ",\" ");
	   }
          
           //_Journal_abbrevitaion
           string s1;
	   int read_full_name = 0;
	   
	   target_tags =
	                astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[2]);
	   if(target_tags->Length() != 1 || target_tags == NULL)
	       (*errStream)<<"#transform-6# Error: no or more than one _Journal_abbrevitation in the save frame."<<endl;
	   else
	   {   s1 = ((DataItemNode *)( (*target_tags)[0] ))->myValue();
	       if (strcmp (s1.c_str(), ".") == 0 || strcmp(s1.c_str(),"na") == 0)
	       {
		   read_full_name = 1;
	       }
	       else
	       { 
	          strcat(new_tagvalue, s1.c_str());
		  //strcat(new_tagvalue, " ");
	       }
	   }
	   //_Journal_name_full
	   if( read_full_name != 0)
	   {
	       target_tags =
		   astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[3]);
	       if(target_tags->Length() != 1 || target_tags == NULL)
		   (*errStream)<<"#transform-6# Error: Should be exactly one _Journal_name_full in the save frame. no more or less."<<endl;
	       else
	       {
		   s1 = ((DataItemNode *)( (*target_tags)[0] )) ->myValue();
		   if(strcmp(s1.c_str(), ".") != 0 && strcmp(s1.c_str(), "na") != 0)
		   {
		       strcat(new_tagvalue, s1.c_str());
		       //strcat(new_tagvalue, " ");
		   }
	       }
	   }
	   //_Journal_volume
	   //If no information on journal volume, do nothing.
	   target_tags =
	                astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[4]);
	   if(target_tags->Length() != 1 || target_tags == NULL)
	       (*errStream)<<"#transform-6# Error: no or more than one _Journal_volume in the save frame."<<endl;
	   else
	   {   s1 = ((DataItemNode *)( (*target_tags)[0] ))->myValue();
	       if (strcmp (s1.c_str(), ".") != 0 && strcmp(s1.c_str(),"na")!=0 )
	       {  strcat(new_tagvalue, " ");
	          strcat(new_tagvalue, s1.c_str());
		  strcat(new_tagvalue, ", ");
	       }
	   }
	   //_page_first
	   //If no information on _page_first, do nothing.
	   target_tags =
	                astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[5]);
	   if(target_tags->Length() != 1 || target_tags == NULL)
	       (*errStream)<<"#transform-6# Error: no or more than one _page_first in the save frame."<<endl;
	   else
	   {   s1 = ((DataItemNode *)( (*target_tags)[0] ))->myValue();
	       if (strcmp (s1.c_str(), ".") != 0 && strcmp(s1.c_str(),"na")!=0 )
	       {
	          strcat(new_tagvalue, s1.c_str());
		  strcat(new_tagvalue, "-");
	       }
	   }
	   //_page_last
	   //If no information on _page_last, do nothing.
	   target_tags =
	                astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[6]);
	   if(target_tags->Length() != 1 || target_tags == NULL)
	       (*errStream)<<"#transform-6# Error: no or more than one _page_last in the saveframe."<<endl;
	   else
	   {   s1 = ((DataItemNode *)( (*target_tags)[0] ))->myValue();
	       if (strcmp (s1.c_str(), ".") != 0 && strcmp(s1.c_str(),"na")!=0 )
	       {
	          strcat(new_tagvalue, s1.c_str());
		  //strcat(new_tagvalue, " ");
	       }
	   }
	   //_Citation_status
           target_tags = 
	                astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[7]);
	   if(target_tags->Length() != 1 || target_tags == NULL)
	       (*errStream)<<"#transform-6# Error: no or more than one _Citation_status in the saveframe."<<endl;
	   else
	   { s1 = ((DataItemNode *)( (*target_tags)[0] ))->myValue();
	       if (strcmp (s1.c_str(), ".") != 0 && strcmp(s1.c_str(), "na") !=0 )
	       { 
		   if( strcmp(s1.c_str(), "published") !=0 ){
                       strcat(new_tagvalue, ", ");
		       strcat(new_tagvalue, s1.c_str());
		       //strcat(new_tagvalue, " ");
		   }
	       }
	   }
	   
	   //_Year
	   //If no information on year, do nothing.
	   target_tags =
	                astSaveFrame->searchForTypeByTag(ASTnode::DATAITEMNODE,interval[8]);
	   if(target_tags->Length() != 1 || target_tags == NULL)
	       (*errStream)<<"#transform-6# Error: no or more than one _year in the save frame."<<endl;
	   else
	   {   s1 = ((DataItemNode *)( (*target_tags)[0] ))->myValue();
	       if (strcmp (s1.c_str(), ".") != 0 && strcmp(s1.c_str(),"na")!=0 )
	       {  strcat (new_tagvalue, " (");
	          strcat(new_tagvalue, s1.c_str());
		  strcat(new_tagvalue, ")");
	       }
	   }

	   strcat(new_tagvalue, ".");
	   //Create the new tag and insert the new value.

	   SaveFrameNode *nmr_current_frame;
	   nmr_current_frame;
	   nmr_current_frame = (SaveFrameNode *)(astSaveFrame->myParallelCopy());
	   //Now create a new tag and value in NMR tree
	   if ( (((SaveFrameNode*)nmr_current_frame)->AddDataItem(new_tagname, new_tagvalue,
		   DataValueNode::SEMICOLON)) == StarFileNode::OK)
	   {
	   }
	   else
	   {
	       (*errStream)<<"#transform-6# Error: adding of new tag is failed."<<endl;
	   }
       }
   }
   delete return_frame_list;
}
