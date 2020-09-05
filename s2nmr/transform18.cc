///////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.8  2003/07/24 18:49:13  madings
Changes to handle 2.1-3.0 mappings.

Revision 1.7  2000/11/18 03:02:37  madings
*** empty log message ***

// Revision 1.6  2000/11/18  02:26:28  madings
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
// Revision 1.5  2000/03/20  09:45:49  madings
// Weekend fixes to s2nmr, to respond to some of Jurgen's comments:
// Note, the old s2nmr program is under the name "/bmrb/bin/s2nmr-old", just
// in case you discover a major bug and want to get the old version back.
//
// 1 - The max number allowed for a saveframe's order sequence used to be
// 999.  This was too small.  It has been increased to 9999999, and documented
// acccordingly in specs.html
//
// 2 - Documented that only one middle initial is recognized when
// parsing names from strings.
//
// 3 - Numerous places in the code that created new values did so without
// the proper delimiter type  (For example, it might put a multi-line
// value into a quoted string ("...") instead of a semicolon string.)
// Jurgen noticed this on the citation title, but when I looked for it
// in the code, I found that particular mistake was common throughout the
// newer rules that came after transform # 17.  This has been fixed.
//
// 4 - rule 19, change data_tag_name, did not work as expected:
// I looked at the code and this was simply a case of bad documentation.
// What this rule is meant to do is look for the _BMRB_accession_number
// tag somewhere in the file and use its value as the name of the data_
// tag.  That's all it does.  If a _BMRB_accession_number tag is not there,
// it fails to do anything.  The documentation has been changed to properly
// describe this.
//
// 5 - "rule 22 is unclear to me" - I have re-written explanation of this
// rule, and fixed a bug in the code that refused to operate on multiple
// target saveframes.  (If the key/value pair designated more than one
// output saveframe, it would fail.)
//
// 6 - "rule 25 change_tag_value has backward documenation".  Looked at
// the code to see what it does, and the code is really buggy.  A redo
// from scratch would be quicker than trying to fix it.  The documentation
// isn't backward - the code is.  It uses the wrong variables for things,
// but not in a consistant manner, so I can't just quickly swap things
// around.  Also, we need to define what should be the correct behaviour
// when multiple saveframes are found by either of the two key_tag/value
// searches.  If two input saveframes are found, and two output saveframes
// are found, then is the correct behaviour to copy input 1 to output 1 and
// input 2 to output 2?  Or is the correct behaviour to copy in a 'cross
// product' fashion and copy input 1 to both outputs 1 and 2, and copy
// input 2 to both outputs 1 and 2?  Issues of possible name clashes need
// to be considered too with this.  In light of this, I'm going to put off
// working on this rule untill I am sure it is worth the time, and this
// ambigious behaviour is defined better.  I have to be careful not to spend
// too much time on this program.
//
// 7 - Fixed the broken condition where output_sf_name_tag is null (.) in
// values_to_saveframes_many_to_many.
//
// 8 - Repaired a few typos here and there.
//
// CVS----------------------------------------------------------------------
//
// Revision 1.4  1999/07/30  18:22:06  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.3  1999/06/18 21:14:34  zixia
*** empty log message ***

*/
#endif
//////////////////////////////////////////


#include "transforms.h"

const int TITLE_SIZE = 8;
static const char* TITLE[TITLE_SIZE]={"Mr","Ms","Miss", "Mrs","Dr", "Sr", "Jr","Phd"};
const int MAX_NAME = 10; //Maximum number of names including title allowed for a name.

void parse_split_name( StarFileNode *inStar, 
                       StarFileNode *outStar, 
                       BlockNode    *,
		       SaveFrameNode *currDicRuleSF);

void heuristic_parseName(char parse_vlaue[], 
	                 char title[], 
			 char first[], 
			 char middle[], 
			 char last[]);

void loopcase(DataLoopNode *& loopPeer, 
	      LoopTableNode *& mapLoopTbl, 
	      int mL_idx, 
	      int parse_tag_pos, 
	      int output_title_tag_pos,
	      int output_first_tag_pos,
	      int output_middle_tag_pos,
	      int output_last_tag_pos,
	      int keep_old_tag_pos);

void nonloopcase(ASTlist<DataNode*>* datalist, 
	         ASTnode * current, 
		 LoopTableNode * mapLoopTbl, 
		 int mL_idx, 
		 int output_title_tag_pos,
		 int output_first_tag_pos,
		 int output_middle_tag_pos,
		 int output_last_tag_pos,
		 int keep_old_tag_pos);

char* nextToken(char value[], char bitmap[]);
int find_title(char** name_list);

void save_parse_split_name( StarFileNode *input, 
                       StarFileNode *output, 
                       BlockNode    *,
		       SaveFrameNode *currDicRuleSF)
{
   List<ASTnode*>        *mapLoopMatch;
   List<ASTnode*>        *matchSF;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;
   int                   mL_idx;//map loop index 
   int                   sf_name_pos = -1;
   int                   key_tag_pos = -1;
   int                   key_value_pos = -1;
   int                   parse_tag_pos = -1;
   int                   output_title_tag_pos = -1;
   int                   output_first_tag_pos = -1;
   int                   output_middle_tag_pos = -1;
   int                   output_last_tag_pos = -1;
   int                   keep_old_tag_pos = -1;

   //Find loop from the mapping file:
   mapLoopMatch = currDicRuleSF->searchForTypeByTag(ASTnode::DATALOOPNODE, string("_sf_name"));
   if( mapLoopMatch->size() < 0)
   { 
       (*errStream) << "#transform-18# ERROR: no loop tag called '_sf_name' in mapping file." << endl;
        delete mapLoopMatch;
        return; /* do nothing */
   }
   
   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames   =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

   delete mapLoopMatch;
   
   sf_name_pos = -1;
   key_tag_pos = -1;
   key_value_pos = -1;
   parse_tag_pos = -1;
   output_title_tag_pos = -1;
   output_first_tag_pos = -1;
   output_middle_tag_pos = -1;
   output_last_tag_pos = -1;
   keep_old_tag_pos = -1;
   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
        if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_sf_name" ) )
            sf_name_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
            key_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
            key_value_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_parse_tag" ) )
            parse_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_title_tag" ) )
            output_title_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_first_tag" ) )
            output_first_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_middle_tag" ) )
            output_middle_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_last_tag" ) )
            output_last_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_keep_old_tag" ) )
            keep_old_tag_pos = i;
    }

    if( sf_name_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_sf_name'." << endl;
        return; /* do nothing */
    }
    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_key_tag'." << endl;
        return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_key_value'." << endl;
        return; /* do nothing */
    }
    if( parse_tag_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_parse_tag'." << endl;
        return; /* do nothing */
    }
    if( output_title_tag_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_output_title_tag'." << endl;
        return; /* do nothing */
    }
    if( output_first_tag_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_output_first_tag'." << endl;
        return; /* do nothing */
    }
    if( output_middle_tag_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_output_middle_tag'." << endl;
        return; /* do nothing */
    }
    if( output_last_tag_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_output_last_tag'." << endl;
        return; /* do nothing */
    }
    if( keep_old_tag_pos < 0  )
    {   (*errStream) << "#transform-18# ERROR: input mapping file is missing tag '_keep_old_tag'." << endl;
        return; /* do nothing */
    }
    
    
    for(mL_idx = 0; mL_idx < mapLoopTbl->size(); mL_idx++ ){
	  //case 1. All three value do not care. The rule operates everywhere it finds a matching tag
          //for _parse_tag in the file (in or out of a saveframe).
          if(   strcmp( (*(*mapLoopTbl)[mL_idx])[sf_name_pos]->myValue().c_str(), "*") == 0
	     && strcmp( (*(*mapLoopTbl)[mL_idx])[key_tag_pos]->myValue().c_str(), "*") == 0
	     && strcmp( (*(*mapLoopTbl)[mL_idx])[key_value_pos]->myValue().c_str(), "*") == 0){
               matchSF = NULL; 
	  }
	  //case 2.Find the saveFrame by name.
	  else if(   strcmp( (*(*mapLoopTbl)[mL_idx])[sf_name_pos]->myValue().c_str(), "*") == 0
	           && strcmp( (*(*mapLoopTbl)[mL_idx])[key_tag_pos]->myValue().c_str(), "*") != 0
	           && strcmp( (*(*mapLoopTbl)[mL_idx])[key_value_pos]->myValue().c_str(), "*") != 0 ){
                   matchSF = AST->searchForTypeByTagValue(
		       ASTnode::SAVEFRAMENODE,
		       (*(*mapLoopTbl)[mL_idx])[key_tag_pos]->myValue(),
		       (*(*mapLoopTbl)[mL_idx])[key_value_pos]->myValue() );
	  }
	  //case 3. Find the saveFrame by tag and tag_value
	  else if( strcmp( (*(*mapLoopTbl)[mL_idx])[sf_name_pos]->myValue().c_str(), "*") != 0
	         && strcmp( (*(*mapLoopTbl)[mL_idx])[key_tag_pos]->myValue().c_str(), "*") == 0
	         && strcmp( (*(*mapLoopTbl)[mL_idx])[key_value_pos]->myValue().c_str(), "*") == 0 ){
                 matchSF = AST->searchForTypeByTag(
		       ASTnode::SAVEFRAMENODE,
		       (*(*mapLoopTbl)[mL_idx])[sf_name_pos]->myValue() );
	  }

	  //For case 2 and 3
	  if( matchSF != NULL){
              for(int sf_i = 0; sf_i < matchSF->size(); sf_i++){
		  
                  //Find the _parse_tag
                  List<ASTnode*> *matchParseTag = (*matchSF)[sf_i]->searchByTag(
			     (*(*mapLoopTbl)[mL_idx])[parse_tag_pos]->myValue() );
		  for(int i=0; i<matchParseTag->size(); i++){
                      ASTnode *current = (*matchParseTag)[i];
		      ASTnode *cur_par = current->myParent(); 
			  
		      while(cur_par!=NULL && ! cur_par->isOfType(ASTnode::DATALOOPNODE))
                                 cur_par = cur_par->myParent();

		      //In a loop
		      if(cur_par != NULL && cur_par->isOfType( ASTnode::DATALOOPNODE )){
			  //Find the Peer.
                          DataLoopNode * loopPeer = (DataLoopNode*)(cur_par->myParallelCopy());
			  loopcase(loopPeer, mapLoopTbl, mL_idx, parse_tag_pos, output_title_tag_pos, output_first_tag_pos,
				   output_middle_tag_pos, output_last_tag_pos, keep_old_tag_pos);
		      }else{//It is a free tag
			  SaveFrameNode *sfPeer = (SaveFrameNode*)(*matchSF)[sf_i]->myParallelCopy();
			  ASTlist<DataNode*>* datalist = sfPeer->GiveMyDataList();
                          nonloopcase(datalist, current, mapLoopTbl, mL_idx, output_title_tag_pos, output_first_tag_pos,output_middle_tag_pos,
				  output_last_tag_pos,keep_old_tag_pos);
			  //delete datalist;
		      }
		   }
		  delete matchParseTag;
	      }
	  }else{//case 1.
             List<ASTnode*> *matchParseTag =  AST->searchByTag(
		      (*(*mapLoopTbl)[mL_idx])[parse_tag_pos]->myValue() );
	  
	     //For each parse_tag found in the file, do the transformation.
	     for(int i=0; i<matchParseTag->size(); i++){
                   ASTnode *current = (*matchParseTag)[i];
		   ASTnode *cur_par = current->myParent(); 
		   bool in_saveframe = false;
		   bool in_loop = false;
		   bool in_blocknode = false;
		   SaveFrameNode * sf_Peer;
		   DataLoopNode *  loopPeer;
		   BlockNode *     blocknodePeer;
		   
		   //Check if in a saveframe
		   while(cur_par!=NULL && !cur_par->isOfType(ASTnode::SAVEFRAMENODE))
		       cur_par = cur_par->myParent();
		   
		   if(cur_par != NULL && cur_par->isOfType(ASTnode::SAVEFRAMENODE)){
                         in_saveframe = true;
			 sf_Peer = (SaveFrameNode*)(cur_par->myParallelCopy());
		   }
		   
		   //Check if in a loop
		   cur_par = current->myParent();
		   while(cur_par!=NULL && ! cur_par->isOfType(ASTnode::DATALOOPNODE))
                                 cur_par = cur_par->myParent();

                   
		   if(cur_par != NULL && cur_par->isOfType( ASTnode::DATALOOPNODE )){
		        in_loop = true; 
			//Find the Peer.
                        loopPeer = (DataLoopNode*)(cur_par->myParallelCopy());
		   }

		   //Check if in a blockNode
		   cur_par = current->myParent();
		   while(cur_par!=NULL && ! cur_par->isOfType(ASTnode::BLOCKNODE))
                                 cur_par = cur_par->myParent();

                   
		   if(cur_par != NULL && cur_par->isOfType( ASTnode::BLOCKNODE )){
		        in_blocknode = true; 
			//Find the Peer.
                        blocknodePeer = (BlockNode*)(cur_par->myParallelCopy());
		   }
			  
		   if(in_loop){//Situation 1: in a loop.
                     loopcase(loopPeer, mapLoopTbl, mL_idx,  parse_tag_pos,
			      output_title_tag_pos, output_first_tag_pos, output_middle_tag_pos, output_last_tag_pos, keep_old_tag_pos);
                  }else{//Situation 2: it is a free tag in a saveframe or not in saveframe at all.
		    //created DataItemNode and insert them into the saveframe.
		    ASTlist<DataNode*>* datalist; 
		    if(in_saveframe){
		      datalist = sf_Peer->GiveMyDataList();
		    }else{
		      datalist = blocknodePeer->GiveMyDataList(); 
		    }
		    nonloopcase(datalist, current, mapLoopTbl, mL_idx, output_title_tag_pos,
			   output_first_tag_pos, output_middle_tag_pos, output_last_tag_pos, keep_old_tag_pos);
		   //delete datalist;
		  }
	     }
	  delete matchParseTag;   
	  }
    }
   delete matchSF;
}

void heuristic_parseName(char parse_value[], char title[], char first[], char middle[], char last[]){
    bool first_name_first = false; //Flag for the format: first_name_first
    bool last_name_first  = false; //Flag for the format: last_name_first
    int  i;
    
    //Check what format the value is in.
    if( strstr(parse_value, ",") == NULL ){ // first_name_first format
         first_name_first = true;
	 last_name_first  = false;
    }else{//last_name_first format
         last_name_first = true;
	 first_name_first = false;
    }
    
    char* name_list[MAX_NAME];
    
    for(i=0; i<MAX_NAME; i++){
        name_list[i] = NULL;
    }
    
    char bitmap[MAX_NAME] = "";
    
    char *value = (char*) calloc( strlen(parse_value)+1, sizeof(char) );
    strcpy(value, parse_value);
      
    char *token =  nextToken(value, bitmap);
    char *tmptoken = (char*)calloc( strlen(token)+1, sizeof(char) );
    strcpy(tmptoken, token);
    int title_field = -1;//The index in namelist where the title is
      
    //Put each word into list
    int j=0;
    while( token != NULL && j<MAX_NAME){
        if( j==0){ 
          name_list[j] = (char*)calloc( strlen(tmptoken)+1, sizeof(char) );
          strcpy(name_list[j], tmptoken);
	}else{
	  //cout<<"token="<<token<<endl; 
	  name_list[j] = (char*)calloc( strlen(token)+1, sizeof(char) );
	  strcpy(name_list[j], token);
	}
	j++;
	if( token != NULL )
	    free( token );
	token = nextToken(value, bitmap);
    }
      
    title_field = find_title(name_list);
    int word_cnt=0;
    if(title_field != -1){
        word_cnt = j-1;
        strcpy(title, name_list[title_field]);//Assign title
    }else{
        word_cnt = j;//Counting from 1
        strcpy(title, ".");//No title
    }
    
    //In first_name_first format
    if(first_name_first && !last_name_first){  
      if( word_cnt== 1){//There is one word:last name;
         for(int i=0; i<j; i++){
             if( i != title_field ){
                 strcpy(last, name_list[i]);
	     }
	 }
      }else if( word_cnt == 2){//There are two words:firstname last name
        int cnt = 1;
	for(int i=0; i<j; i++){
          if(i != title_field ){
             if(cnt ==  1){
                strcpy(first, name_list[i]);
	     }else if(cnt == 2 ){
                strcpy(last, name_list[i]);
	     }
	     cnt++;
	  }
	}
      }else if( word_cnt== 3){//There are three words: first, middle, last
        int cnt = 1;
	for(int i=0; i<j; i++){
          if(i != title_field ){
             if(cnt ==  1){
                strcpy(first, name_list[i]);
	     }else if(cnt == 2 ){
                strcpy(middle, name_list[i]);
	     }else if(cnt == 3){
                strcpy(last, name_list[i]);
	     }
	     cnt++;
	  }
	}
      }else if(word_cnt >= 4){//The case not handled by this heuristic algorithm
         (*errStream)<<"#transform-18# ERROR: more than first_name, middle, last_name in the name ="
	     <<parse_value<<endl;
	 return;
      }
    }else if(last_name_first && !first_name_first){
        int comma_1_pos = -1;//The position of first comma in bitmap, counting from zero.
	int comma_2_pos = -1;//The position of second comma in bitmap, counting from zero.
	int cnt = 0;

	//Find the postions of comma
	for(int i=0; i<strlen(bitmap);i++){
            if(bitmap[i] == ',' ){
              cnt++;
	      if(cnt == 1){ comma_1_pos = i;}
	      if(cnt == 2){ comma_2_pos = i;}
	    }
	}
	//Before first comma, anything should be last_name expect title.
	strcpy(last, "");
        for(int y=0; y<comma_1_pos+1; y++){
             if( y!=title_field ){
		 //strcpy(last, name_list[y]);
                 strcat(last, name_list[y]);
             }
	     if( y < comma_1_pos ){
                strcat(last, " ");
	     }
        }
	
        //After first comma, there are two situations:
	//1. there is no second comma
	//2. there is a second comma
	if(comma_2_pos == -1 ){//Situatio 1: there is no second comma
           int word_left;
	   word_left = word_cnt - (comma_1_pos+1);
	   if(title_field >= (comma_1_pos+1) ){
               word_left = word_left;
	   }else if( title_field != -1 && title_field < comma_1_pos+1){
               word_left = word_left+1;
	   }

	   if(word_left == 1){//one word case: assume it is first name.
              strcpy(first, name_list[comma_1_pos+1]);
	   }else if(word_left == 2){//two words case: assume there are first_name, middle_initial
              int cnt = 0; 
	      for(int x=comma_1_pos+1; x<j; x++){
                 if( x != title_field){
                    cnt++;
		    if(cnt == 1){ strcpy(first, name_list[x]); }
		    if(cnt == 2){ strcpy(middle, name_list[x]); }
		 }
	      }
	   }else{
	      int cnt = 0; 
	      for(int x=comma_1_pos+1; x<j; x++){
                 if( x != title_field){
                    cnt++;
		    if(cnt == 1){ strcpy(first, name_list[x]); }
		    if(cnt == 2){ strcpy(middle, name_list[x]); }
		    if(cnt >2 ) { 
			strcat(middle, ".");
			strcat(middle, name_list[x]);
		    }
		 }
	      }
	       /*
	       //Error
	      (*errStream)<<"#transform-18# ERROR: more than lastname, first_name "
		          <<"middle_initial in the name ="
	                  <<parse_value<<endl;
	      return;
	      */
	   }
	}else{//Situation 2: there is a second comma
           //After the second comma, it is either a title or family name
	    if(comma_2_pos+1 != title_field ){
                strcpy(last, name_list[comma_2_pos+1]);
	    }
	    //Befor the second comma
	    //If it is one word, assume it is first_name.
	    //If it is two words, assume they are first_name, middle_initial.
	    //If it is more than two words, error.
	
	    int word_left;
	    if( title_field != -1){
                word_left = word_cnt+1 - (comma_1_pos+1);
		if(title_field >= comma_2_pos+1){
                      word_left--;
		}else if(title_field<comma_1_pos+1){
                      //Do nothing
		}else{
                     word_left--;
		}
	    }else{//No title case
                word_left = word_cnt -( comma_1_pos+1 );
		word_left = word_left-1; //minus the word after second comma
	    }
	   
	    if(word_left == 1){
               strcpy(first, name_list[comma_1_pos+1]);
	    }else if(word_left == 2 ){
               int cnt = 0;
	       for(int x=comma_1_pos; x<comma_2_pos+1; x++){
                  if(x!=title_field){
                       cnt++;
		       if(cnt==1){ strcpy(first, name_list[x]); }
		       if(cnt==2){ strcpy(middle, name_list[x]); }
		  }
	       }
	    }else{//Error
                (*errStream)<<"#transform-18# ERROR: more than lastname, last_name "
		    <<"middle_initial in the name ="
	            <<parse_value<<endl;
	      return;
	    }
	    
	}
	
    }
    if(last[strlen(last)-1] != '.' && strlen(last) == 1 ){
      strcat(last, ".");
    }
    
    if(first[strlen(first)-1] != '.' &&  strlen(first)==1  ){
       strcat(first, ".");
    }
    
    if( middle[strlen(middle)-1] != '.' && ( strlen(middle)==1 || strstr(middle, ".") !=NULL ) ){
       strcat(middle,".");
    }

    if( tmptoken != NULL ) free( tmptoken );
    if( token != NULL ) free( token );
    if( value != NULL ) free( value );
    for( i=0 ; i<MAX_NAME ; i++ )
	if( name_list[i] != NULL ) free( name_list[i] );
    
}

//If the parse_tag is in loop.
void loopcase(DataLoopNode *& loopPeer, 
	      LoopTableNode *& mapLoopTbl, 
	      int mL_idx, 
	      int parse_tag_pos, 
	      int output_title_tag_pos,
	      int output_first_tag_pos,
	      int output_middle_tag_pos,
	      int output_last_tag_pos,
	      int keep_old_tag_pos){
    DataLoopNameListNode *nameListList = loopPeer->getNamesPtr();
    LoopTableNode *table = loopPeer->getValsPtr();
    LoopNameListNode *nameList = (*nameListList)[0];
    int rowNum = table->size();

    //Find the postion of tags in the loop
    int parse_tag_inloop_pos; 
    int title_pos;
    int first_pos;
    int middle_pos;
    int last_pos;
  
    for(int i=0; i<nameList->size(); i++){
      if(strcmp( (*(*mapLoopTbl)[mL_idx])[parse_tag_pos]->myValue().c_str(),
	      (*nameList)[i]->myName().c_str())==0){
	 parse_tag_inloop_pos = i;	
      }
    }
     
    //Insert new tag names into the loop
    nameList->insert(nameList->end(), new DataNameNode(
	  (*(*mapLoopTbl)[mL_idx])[output_title_tag_pos]->myValue()) );
    title_pos = nameList->size()-1;
    nameList->insert(nameList->end(), new DataNameNode(
         (*(*mapLoopTbl)[mL_idx])[output_first_tag_pos]->myValue()) );
    first_pos = nameList->size()-1;
    nameList->insert(nameList->end(), new DataNameNode(
         (*(*mapLoopTbl)[mL_idx])[output_middle_tag_pos]->myValue()) );
    middle_pos = nameList->size()-1;
    nameList->insert(nameList->end(), new DataNameNode(
         (*(*mapLoopTbl)[mL_idx])[output_last_tag_pos]->myValue()) );
    last_pos = nameList->size() -1;
                             
    //for each value of _parse_tag, do the parse
    for(int j=0; j<rowNum;j++){
       char parse_value[1000];//The value need to be parsed.
       char title[100]="."; //The title
       char first_name[100]=".";//First name
       char middle_name[100]=".";//Middle name
       char last_name[100]=".";//Last name.
       char quote_type;
       strcpy(parse_value, (*(*table)[j])[parse_tag_inloop_pos]->myValue().c_str());
       heuristic_parseName(parse_value, title, first_name, middle_name, last_name);
       //Insert the values into the loop row.
       quote_type = FindNeededQuoteStyle(string(title));
       (*(*table)[j])[title_pos] =
	      ( quote_type == '\'' ) ? new DataValueNode(title, DataValueNode::SINGLE) :
	      ( quote_type == '\"' ) ? new DataValueNode(title, DataValueNode::DOUBLE) :
	      ( quote_type == ';'  ) ? new DataValueNode(title, DataValueNode::SEMICOLON) :
              /*default*/              new DataValueNode(title, DataValueNode::NON);

       // Make the proper quoting style for the value:
       quote_type = FindNeededQuoteStyle(string(first_name));
       (*(*table)[j])[first_pos] =
	      ( quote_type == '\'' ) ? new DataValueNode(first_name, DataValueNode::SINGLE) :
	      ( quote_type == '\"' ) ? new DataValueNode(first_name, DataValueNode::DOUBLE) :
	      ( quote_type == ';'  ) ? new DataValueNode(first_name, DataValueNode::SEMICOLON) :
              /*default*/              new DataValueNode(first_name, DataValueNode::NON);
       
       quote_type = FindNeededQuoteStyle(string(middle_name));
       (*(*table)[j])[middle_pos] = 
	      ( quote_type == '\'' ) ? new DataValueNode(middle_name, DataValueNode::SINGLE) :
	      ( quote_type == '\"' ) ? new DataValueNode(middle_name, DataValueNode::DOUBLE) :
	      ( quote_type == ';'  ) ? new DataValueNode(middle_name, DataValueNode::SEMICOLON) :
              /*default*/              new DataValueNode(middle_name, DataValueNode::NON);

       quote_type = FindNeededQuoteStyle(string(last_name));
       (*(*table)[j])[last_pos] = 
	      ( quote_type == '\'' ) ? new DataValueNode(last_name, DataValueNode::SINGLE) :
	      ( quote_type == '\"' ) ? new DataValueNode(last_name, DataValueNode::DOUBLE) :
	      ( quote_type == ';'  ) ? new DataValueNode(last_name, DataValueNode::SEMICOLON) :
              /*default*/              new DataValueNode(last_name, DataValueNode::NON);
   }

   //Delete the parse_tag or not
   if(strcasecmp((*(*mapLoopTbl)[mL_idx])[keep_old_tag_pos]->myValue().c_str(), "N")==0){
     //Delete the tag_name and the column of values for that tag.
     loopPeer->getNames()[0]->erase( loopPeer->getNames()[0]->begin()+parse_tag_inloop_pos);
  }
   
}
	
//If it is not in loop
void nonloopcase(ASTlist<DataNode*>* datalist, 
	         ASTnode * current, 
		 LoopTableNode * mapLoopTbl, 
		 int mL_idx, 
		 int output_title_tag_pos,
		 int output_first_tag_pos,
		 int output_middle_tag_pos,
		 int output_last_tag_pos,
		 int keep_old_tag_pos){
  char parse_value[1000];//The value need to be parsed.
  char title[100]="."; //The title
  char first_name[100]=".";//First name
  char middle_name[100]=".";//Middle name
  char last_name[100]=".";//Last name.
  char quote_style;

  strcpy(parse_value, ((DataItemNode*)current)->myValue().c_str());
  heuristic_parseName(parse_value, title, first_name, middle_name, last_name);
  //cout<<"last name"<<last_name<<endl;
                        
  DataItemNode* curPeer = (DataItemNode*)current->myParallelCopy();
  int i=0;
  for(i=0; i<datalist->size();i++){
     if((*datalist)[i]->isOfType(ASTnode::DATAITEMNODE)){ 
         if( (DataItemNode*)(*datalist)[i] == curPeer){
            break;	  
         };
     }
   }
 
   //If the title is more than one word, put the value in quotes.
   quote_style = FindNeededQuoteStyle( title );
   datalist->insert( datalist->begin()+i+1,  
	  new DataItemNode( (*(*mapLoopTbl)[mL_idx])[output_title_tag_pos]->myValue(),
		            title,
			    ( quote_style == '\'' ) ? DataValueNode::SINGLE :
			    ( quote_style == '\"' ) ? DataValueNode::DOUBLE :
			    ( quote_style == ';'  ) ? DataValueNode::SEMICOLON :
			    /*default*/               DataValueNode::NON 
			    ) );

   //If the first_name is more than one word, put the value in quotes.
   quote_style = FindNeededQuoteStyle( first_name );
   datalist->insert( datalist->begin()+i+2,  
	  new DataItemNode( (*(*mapLoopTbl)[mL_idx])[output_first_tag_pos]->myValue() ,
		            first_name,
			    ( quote_style == '\'' ) ? DataValueNode::SINGLE :
			    ( quote_style == '\"' ) ? DataValueNode::DOUBLE :
			    ( quote_style == ';'  ) ? DataValueNode::SEMICOLON :
			    /*default*/               DataValueNode::NON 
			    ) );
   
   //If the middle_name is more than one word, put the value in single quotes.
   quote_style = FindNeededQuoteStyle( middle_name );
   datalist->insert( datalist->begin()+i+3,  
	  new DataItemNode( (*(*mapLoopTbl)[mL_idx])[output_middle_tag_pos]->myValue(),
		            middle_name,
			    ( quote_style == '\'' ) ? DataValueNode::SINGLE :
			    ( quote_style == '\"' ) ? DataValueNode::DOUBLE :
			    ( quote_style == ';'  ) ? DataValueNode::SEMICOLON :
			    /*default*/               DataValueNode::NON 
			    ) );
   
   //If the last_name is more than one word, put the value in single quotes.
   quote_style = FindNeededQuoteStyle( last_name );
   datalist->insert( datalist->begin()+i+4,  
	  new DataItemNode( (*(*mapLoopTbl)[mL_idx])[output_last_tag_pos]->myValue(),
		            last_name,
			    ( quote_style == '\'' ) ? DataValueNode::SINGLE :
			    ( quote_style == '\"' ) ? DataValueNode::DOUBLE :
			    ( quote_style == ';'  ) ? DataValueNode::SEMICOLON :
			    /*default*/               DataValueNode::NON 
			    ) );
   
   if(strcasecmp((*(*mapLoopTbl)[mL_idx])[keep_old_tag_pos]->myValue().c_str(), "N")==0){
         datalist->erase( datalist->begin()+i); 
   }
}



//Return next token, the delimiter are: space, period, or coma
char* nextToken(char value[], char bitmap[]){

   int size = strlen(value);
   char* token = (char*)calloc( size+1, sizeof(char) );
   int i=0, k=0;
   int bitmap_index = strlen(bitmap);
   if(size == 0)  return NULL;
   
   for(i=0; i<size; i++){
       if(value[i] == ' ' || value[i] == ',' || value[i] == '.' ){
         if(i!=0)  {
	     bitmap[bitmap_index] = value[i]; 
	     break;
	 }
	 continue;
       }else{
           token[k]=value[i];
	   k++;
       }

   }

   //Token with a length of zero.
   if( k == 0)
   {   free( token );
       return NULL;
   }
   
   token[k] = '\0';
   int j=0;
   for(i=i+1; i<size; i++){
      value[j]=value[i];
      j++;
   }
   value[j] = '\0';
   
   return token;
}

//Check if any work is a title.
//Yes, return the index of the title field
//No, return -1.
int find_title(char** name_list){
   for(int i=0; i<MAX_NAME; i++){
      if( name_list[i] != NULL){
          for(int j=0; j<TITLE_SIZE; j++){
             if( strcmp(name_list[i], TITLE[j]) == 0 ){
                 return i;
	     }
	  }
      }
   }

   return -1;
}


