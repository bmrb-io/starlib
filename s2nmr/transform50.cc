///////////////////////////////////// RCS LOG //////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////////////////   

#include "transforms.h"

bool is_valid_sym(char  symbol);
bool is_valid_aut(char value[]);
void clean_str(char value[]);

void save_explode_Mol_residue_seq_dict3_0( StarFileNode *AST, 
                     StarFileNode *NMR, 
                     BlockNode    *,
		     SaveFrameNode *currDicRuleSF )
{
   string currDicSaveFrameName = string("save_explode_Mol_residue_seq_dict3_0");
   DataNode* currDicSaveFrame = currDicRuleSF;

   if(!currDicSaveFrame){//Return if the target saveframe is not found
      return;
   }

   List<ASTnode *> *saveFrameHits = AST->searchForTypeByTag(
	   ASTnode::SAVEFRAMENODE, string("_Entity.Polymer_seq_one_letter_code_can") );
   if( saveFrameHits->size() == 0){
	(*errStream)<<"#transform-50# ERROR: Save frame with tag \"_Entity.Polymer_seq_one_letter_code_can\" is not found in AST file."<<endl;
	return;
   }
  
   SaveFrameNode * curFrame;
   for (int i=0; i<saveFrameHits->size(); i++){
       curFrame = (SaveFrameNode*)(*saveFrameHits)[i];
    
       //Get sequence
       List<ASTnode *> *molHits = curFrame->searchForTypeByTag(
        	   ASTnode::DATAITEMNODE, string("_Entity.Polymer_seq_one_letter_code_can"));
       if( molHits->size() == 0){
         (*errStream)<<"#transform-50# ERROR: tag _Entity.Polymer_seq_one_letter_code_can is not found in AST file."<<endl;
	 return;
       }
       bool found_mol = false;
       char mol_value[5000];
       DataItemNode* cur = ((DataItemNode*)(*molHits)[0]);    
       if( molHits->size()!=0){
            found_mol = true;
	    strncpy(mol_value, cur->myValue().c_str(), 5000);  
	    clean_str(mol_value);
      }

      //Get _Residue_author_seq_code
      List<ASTnode *> *authorHits = curFrame->searchForTypeByTag(
        	   ASTnode::DATAITEMNODE, string("__bogus_dont_look_for--_Entity_comp_index.Author_code"));
      bool found_author = false;
      char author_value[8000];
      if( authorHits->size()!=0){
            cur = ((DataItemNode*)(*authorHits)[0]);
            found_author = true;
	    strcpy(author_value, cur->myValue().c_str());  
      } else {
            found_author = false;
	    author_value[0] = '\0';
      }
      
      //Check if the mol_value are valid.
      bool valid_mol = true;
      for(int j=0; j<strlen(mol_value); j++){
             if( !is_valid_sym(mol_value[j]) ){
	            valid_mol = false;
		    break;
             }
      }
  
      //Check if the author_value are valid.
      bool valid_author = is_valid_aut(author_value);

      //Case 1. _Mol_residue_sequence exists, but has a useless value. Then do not generate a loop and issue a waring error.
      if( valid_mol == false ){
          (*errStream)<<"#transform-50# ERROR:  _Entity.Polymer_seq_one_letter_code_can exists but has a useless value."<<endl;
	  continue;
      }
       
      //Create a loop in the nmr
      SaveFrameNode *nmrSaveFrame =(SaveFrameNode *)(curFrame->myParallelCopy());
      if ( nmrSaveFrame == NULL )
	  continue;
     
      //This is the list of tag names that make up the 'heading' of the loop:
      List<string>* NMRtagList = new List<string>;
      //This is the list of values for the data in the loop:
      List<string>* NMRvalList = new List<string>;
      //This is the list of values for _Residue_author_seq_code.
      List<string>* author_seq_code = new List<string>;
      int colCount  = 0;
      
      //Case 2. _Residue_author_seq_code is missing from the iput file. 
      //Or _Residue_author_seq_code exists but has a useless value.
      //Create the loop but without the _Residue_author_seq_code column.
      if( found_author == false || 
	      (found_author == true && valid_author == false) ){
            NMRtagList->AddToEnd(string("_Entity_poly_seq.Num"));
	    NMRtagList->AddToEnd(string("_Entity_poly_seq.Mon_ID"));
	    colCount = 2;
            char seq_code[50]; 	    
	    for(int k = 0;  k< strlen(mol_value); k++){
		sprintf(seq_code, "%d", k+1);
                NMRvalList->AddToEnd(seq_code);
		char tiny_str[2];
		sprintf(tiny_str, "%c", mol_value[k]);
		NMRvalList->AddToEnd(tiny_str);
	    }
      }

      // In NMR-STAR 3.0, this next case should no longer be possible,
      // because there is no author seq tag.  But I'm leaving it in here
      // (commented out) in case I missed something and want to bring
      // it back.

#ifdef BOGUS_COMMENT_OUT

      //Case 3. _Mol_residue_sequence exists and its values are valid.
      //_Residue_author_seq_code exists and its values are valid.
      if( found_author == true && valid_author == true 
	      && found_mol == true && valid_author == true ){
            NMRtagList->AddToEnd(string("_Residue_seq_code"));
	    NMRtagList->AddToEnd(string("_Residue_author_seq_code"));
	    NMRtagList->AddToEnd(string("_Residue_label"));
	    colCount = 3;
	    
           //Fill the author_seq_code list. 
	    for(int k=0; k<strlen(author_value); k++){
	        char tmp = author_value[k];
		if(tmp == ','){
		    k++;
                    tmp = author_value[k]; 
		    char  value[50];
		    int p = 0;
		    while( k<strlen(author_value) && author_value[k] != '\n' && author_value[k] != '\0'){
                        tmp = author_value[k];
			value[p] = author_value[k];
			p++;
			k++;
		    }
		    value[p] ='\0';
		    author_seq_code->AddToEnd(value);
		}
	    }
           //If the number of author_seq_code is not equal to the number of value under _Mol_residue_sequece. Issue a warning.
	   if( author_seq_code->size() != strlen(mol_value) ){
	       (*errStream)<<"#transform-50# ERROR: Unequal number of values for _Entity_poly_seq.Num and _Entity_poly.Mon_ID"
		           <<endl;
	       if( author_seq_code->size() < strlen(mol_value) ){
                    for(int start = author_seq_code->size(); start<strlen(mol_value); start++){
                            author_seq_code->AddToEnd(".");
		    }
	       }
	       else{
                   for(int start = strlen(mol_value); start<author_seq_code->size(); start++){
                             mol_value[start] = '.';
		   }
	       }
	    }
	    char  seq_code[50];
	    char  tiny_str[2];
	    
	    for(int m=0; m<strlen(mol_value); m++){
		  sprintf(seq_code, "%d", m+1); 
		  sprintf(tiny_str, "%c", mol_value[m]);
	          NMRvalList->AddToEnd(seq_code);
		  NMRvalList->AddToEnd((*author_seq_code)[m]);
		  NMRvalList->AddToEnd( tiny_str );
	    }
      }
#endif


      //Remove the existing loop in Peer, if its there.
      List<ASTnode*> *existMatch  = nmrSaveFrame->searchForTypeByTag( ASTnode::DATALOOPNODE, "_Entity_poly_seq.Num" );
      for( int matchNum = 0 ; matchNum < existMatch->size() ; matchNum++ ) {
        delete (*existMatch)[matchNum];
      }
      delete existMatch;

      // Make the new loop we will be attaching:
      int n = 0;
      DataLoopNode * newLoop = new DataLoopNode(string("tabulate"));
      DataLoopNameListNode *nameListList = newLoop->getNamesPtr();
      LoopTableNode *valTable = newLoop->getValsPtr();
      LoopRowNode *valRow;
      
      //Populate the list of tag names for the loop.
      nameListList->insert(nameListList->end(), new LoopNameListNode() );
      LoopNameListNode *nameList = (*nameListList)[0];
      for(n=0; n< NMRtagList->size(); n++){
            nameList->insert(nameList->end(), new DataNameNode( (*NMRtagList)[n] ));
      }

      //Populate the list of values for the loop, by making new LoopRowNodes
      //and attach them to the loop's LoopTableNode:
      for(n=0; n<NMRvalList->size(); n++){
          valRow = new LoopRowNode(true);
	  valRow->insert(valRow->end(), new DataValueNode((*NMRvalList)[n]));
	  valRow->insert(valRow->end(), new DataValueNode((*NMRvalList)[++n]));
          if( colCount == 3 ){
             valRow->insert(valRow->end(), new DataValueNode((*NMRvalList)[++n]));
	  }
	  valTable->insert(valTable->end(), valRow);
      }
      
      //Attach the loop created to the saveframe.
      List<DataNode*>   *sf_insides = nmrSaveFrame->GiveMyDataList();
      sf_insides->insert(sf_insides->end(), newLoop);
     
      delete NMRtagList;
      delete NMRvalList;
      delete author_seq_code;
   }
   
}

bool is_valid_sym(char  symbol){
    bool is_valid = false;
    if( symbol<='Z' && symbol >='A'){
         is_valid = true;
    }
    if ( symbol <='z' && symbol >= 'a'){
          is_valid = true;
    }
   return is_valid;
}

bool is_valid_aut(char value[]){
  bool is_valid = false;
  char tmp;
  int newlineCnt = 0;
  int commaCnt = 0;
  for(int i=0; i<strlen(value); i++){
     tmp = value[i];
     if( tmp == ',' ){
	 commaCnt++;
     }
     if( tmp == '\n' ){
	 newlineCnt++;
     }
  }

  if( newlineCnt != 0 && newlineCnt == commaCnt-1 )
  {  is_valid = true;
  }

  return is_valid;
}


void clean_str(char value[]){
 char *tmp_value = new char[strlen(value)];
 strcpy(tmp_value, value);
 strcpy(value, "");
 char tmp;
 int cnt = 0;
 for(int i=0; i<strlen(tmp_value); i++){
    tmp = tmp_value[i];
    if( tmp != '\n' && tmp != ' ' && tmp != '\r' && tmp != '\t' ){
       value[cnt] = tmp;
       cnt++;
    }
 }
 value[cnt] = '\0';

 delete tmp_value;
 
}
