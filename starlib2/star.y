/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
/**********************************************************************
 Parser for STAR files
 **********************************************************************/

%{
    GlobalBlockNode *previousGlobalBlock = NULL;
    extern int line_num, col_num;   /* These filled by the scanner */
    extern int star_err_cnt;  /* starts at zero, ++'s on each error */
    extern void debug_msg( const char * ); /* contained in star.l */
    extern void finish_err_msg( StarErrID, const char * );
    extern int  last_loop_line, last_loop_col;
    extern int  cur_nest, cur_val, yacc_cur_nest, in_loop_header;
    extern int  loop_sizes[50], loop_init_idx;
    extern int   rowsThisLoopSoFar;
    extern string  loop_first_tagnames[50]; 
    bool   already_had_row_count_error_this_loop = false;

#ifndef STAR_TAGS_TO_DISABLE_ROW_COUNTING
#define STAR_TAGS_TO_DISABLE_ROW_COUNTING
    /* Set this to the list of tags for which the text line 
     * loop row warning message should be disabled.
     * (If the first data value of a new row starts in the
     * middle of a text line instead of being the first term
     * on the line, then it is probably the cause of a loop
     * column count error unless it's a loop we expect there
     * to be multiple rows per text line (such as the residue
     * sequence loop in NMR-STAR v2.1).)
     * By default this is set up to ignore the tags in the residue
     * sequence loop and issue the warning for everything else,
     * but the user of this parser can change this
     * by altering the contents of this list.
     */
    extern vector<string> g_tags_to_disable_row_counting;
    extern bool one_row_count_error_per_loop_only;
#endif

    extern int  prevRowLineNum;
    extern ofstream *os;
    extern vector<string> g_skipTexts;
    extern vector<int> g_skipTextLineNums;

#ifndef STAR_SYNTAX_PRINT_DEF
#define STAR_SYNTAX_PRINT_DEF
    extern bool star_syntax_print; /* true = starlib prints syntax errors */
#endif

#ifndef STAR_WARNING_CHECK_DEF
    extern bool star_warning_check; /* false = turn off warning generation */
#define STAR_WARNING_CHECK_DEF
#endif

    extern bool err_is_printing;
#define FINISH_ERR_MSG(id, str)                               \
          {                                                   \
	    char tmp_str[2048] ;                              \
            if(err_is_printing) {                             \
		sprintf( tmp_str,                             \
		         "%s (star.y:%d)", (str), __LINE__ ); \
	    } else {                                          \
	        strcpy(tmp_str,str);                          \
	    }                                                 \
	    finish_err_msg( (id), (tmp_str) );                \
          }                                                   


#define YYERROR_VERBOSE

%}

// The types of the tokens and nonterminals
%union {
    TokenType         	  TOKENval;   // type of all tokens
    StarFileNode*         StarFileNodeval;
    ASTlist<BlockNode*>*     StarListNodeval;
    BlockNode *           BlockNodeval;
    GlobalBlockNode *     GlobalBlockNodeval;
    DataBlockNode*        DataBlockNodeval;
    ASTlist<DataNode*>*      DataBlockListNodeval;
    GlobalHeadingNode*    GlobalHeadingNodeval;
    DataHeadingNode*      DataHeadingNodeval;
    DataNode*             DataNodeval;
    DataItemNode*         DataItemNodeval;
    DVNWithPos*           DataValueNodeval;
    DataNameNode*         DataNameNodeval;
    SaveFrameNode*        SaveFrameNodeval;
    SaveHeadingNode*      SaveHeadingNodeval;
    ASTlist<DataNode *>*     SaveFrameListNodeval;
    DataLoopNode*            DataLoopNodeval;
    DataLoopNameListNode*    DataLoopNameListNodeval;
    LoopTableNode*           LoopTableNodeval;
    LoopRowNode*             DataLoopRowval;
    LoopRowNode*             SimpleDataLoopRowval;
    LoopNameListNode*        LoopNameListNodeval;
    int                      dummy;
};

//token declarations
%token <TOKENval> LOOP
%token <TOKENval> STOP
%token <TOKENval> DATA_NAME
%token <TOKENval> DATA_HEADING
%token <TOKENval> GLOBAL_HEADING
%token <TOKENval> SAVE_HEADING
%token <TOKENval> SAVE_END
%token <TOKENval> NON_QUOTED_TEXT_STRING
%token <TOKENval> DOUBLE_QUOTED_TEXT_STRING
%token <TOKENval> SINGLE_QUOTED_TEXT_STRING
%token <TOKENval> SEMICOLON_BOUNDED_TEXT_STRING 
%token <TOKENval> FRAME_CODE
%token <TOKENval> FAKE_TOKEN_ROW_END
%token <TOKENval> FAKE_TOKEN_SUBROW_EXISTS
%token <TOKENval> SKIPPED_TEXT_BLOCK

//type declarations of all non-terminals
%type <StarFileNodeval>		starFile 
%type <StarListNodeval> 	starList
%type <BlockNodeval> 		block
%type <GlobalBlockNodeval>	globalBlock
%type <DataBlockNodeval> 	dataBlock
%type <DataBlockListNodeval> 	dataBlockList
%type <GlobalHeadingNodeval> 	globalHeading
%type <DataHeadingNodeval> 	dataHeading
%type <DataNodeval>		data
%type <DataItemNodeval> 	dataItem
%type <DataValueNodeval> 	dataValue
%type <SaveFrameNodeval> 	saveFrame
%type <SaveHeadingNodeval>      saveHeading
%type <SaveFrameListNodeval> 	saveFrameList
%type <DataNodeval> 		save
%type <DataLoopNodeval>   	dataLoop
%type <DataLoopNameListNodeval> dataLoopDefList
%type <LoopTableNodeval>        dataLoopTable
%type <LoopNameListNodeval>     loopDefList
%type <DataLoopRowval> 		dataLoopRow
%type <SimpleDataLoopRowval>	simpleDataLoopRow
 

//start non-terminal declaration
%start starFile

// These shift/reduce conflicts are from the fact that the stop_ at
// the end of a loop is optional, and I am trying to do error
// recovery.  The STAR syntax is not condusive to YACC error recovery,
// due to the lack of syncronization tokens (like the semicolon in C).
// In all the cases, yacc's default behaviour of preferring the shift
// over the reduce is exactly what I want.  (It took me a while to
// verify that that is true.)
// %expect 14  # for an older version of lex/yacc
%expect 2

%%
starFile	: starList
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a starFile completed\n" );
#               endif
		    AST = new StarFileNode(new StarListNode($1));
		    AST->setLineNum( line_num );
		    AST->setColNum( col_num );
		    AST->setSkipTexts( g_skipTexts, g_skipTextLineNums );
		    previousGlobalBlock = NULL ; // for the next parse.
		    last_loop_line = 0; // for the next parse.
		    last_loop_col = 0; // for the next parse.
		    in_loop_header = 0; // for the next parse.
		    line_num = 1; // for the next parse.
		    col_num = 1; // for the next parse.
		    cur_nest = 0; // for the next parse.
		    yacc_cur_nest = 0; // for the next parse.
		    cur_val = 0; // for the next parse.
		    rowsThisLoopSoFar = 0; // for the next parse.

		    for(   loop_init_idx = 0 ;
		           loop_init_idx<50 ;
			   loop_init_idx++ ) {
			loop_sizes[ loop_init_idx ] = 99999;
			loop_first_tagnames[ loop_init_idx ] = string("");
		    }
		    
		    err_is_printing = false;
		    if( star_err_cnt > 0 )
		    {
			if( (starErrReport.end()-1) -> errID == STAR_UNKNOWN_ERR )
			    if( star_syntax_print )
				printf( "Parse error.\n" ); /* handle if last err unknown */
			// If yyparse() was able to recover from all its errors,
			// using the 'error' rules provided in this file, then
			// it returns a success code back to the caller.  That's
			// not what we want here - we want to have the yyparse()
			// return a failure code even if it recovered.  The only
			// kind of recovery we were implementing for it was just
			// enough to get it to limp along and find more than one
			// error at a time, not enough to actually produce a real
			// star tree:

			bool  has_real_non_warning_errs = false;
			for( int eIdx = 0 ; eIdx < starErrReport.size() ; eIdx++ )
			{
			    if( starErrReport[eIdx].errID < 500 )
			    {
				has_real_non_warning_errs = true;
			    }
			}
			star_err_cnt = 0; // for the next run, if there is one.
			if( has_real_non_warning_errs )
			{
			    YYABORT;
			}
		    }
		    else
		    {
			// No errors, delete the contents of the error list that
			// may have been hanging around from a previous run of
			// yyparse().
			starErrReport.erase( starErrReport.begin(), starErrReport.end() );
		    }
		    star_err_cnt = 0;
		  }
		;

starList 	: starList block                        
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a starList completed\n" );
#               endif
		    $$ = $1;
		    if( $2 != NULL )
		      $$->AddToEnd($2); 
		  }
		| /* epsilon */                         
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a starList completed (epsilon)\n" );
#               endif
		    $$ = new ASTlist<BlockNode *>;
		  }
		;

block		: globalBlock
                {
#               ifdef DEBUG
		    debug_msg( "yacc: a Block completed (version A:global)\n" );
#               endif
		    $$ = $1;
		    if( $$ != NULL )
		    {
			$$->setLineNum( $1->getLineNum() );
			$$->setColNum( $1->getColNum() ); 
		    }
                }
		| dataBlock
                {
#               ifdef DEBUG
		    debug_msg( "yacc: a Block completed (version B:datablock)\n" );
#               endif
		    $$ = $1;
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() ); 
		}
		| error
		{
		    $$ = NULL;
		    FINISH_ERR_MSG( EXP_GLOBAL_OR_DATA,
		                   "Expected a global_ or data_ block.");
		    yyclearin; yyerrok;
		}

		;

globalBlock	: globalHeading dataBlockList
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a globalBlock completed\n" );
#               endif
		      if( previousGlobalBlock == NULL )
		      {
			  $$ = new GlobalBlockNode($1, new DataListNode($2));
			  $$->setLineNum( $1->getLineNum() );
			  $$->setColNum( $1->getColNum() );
			  previousGlobalBlock = $$;
		      }
		      else
		      {
			  ASTlist<DataNode*> *contents = 
			      previousGlobalBlock->GiveMyDataList();
			  contents->AddToEnd( *($2) );
			  $$ = NULL;
		      }
		  }
		;

dataBlock	: dataHeading dataBlockList         
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataBlock completed\n" );
#               endif
		    $$ = new DataBlockNode($1, new DataListNode($2));
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() );
		  }
		;

dataBlockList 	: dataBlockList data             
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a DataBlockList completed\n" );
#               endif
		    $$ = $1;
		    if( $2 != NULL )
			$$->AddToEnd($2);
		  }
		| data
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a starList completed (epsilon)\n" );
#               endif
		    $$ = new ASTlist<DataNode *>;
		    if( $1 != NULL )
			$$->AddToEnd($1);
		  }
		| dataBlockList error data
		  {
#               ifdef DEBUG
		    debug_msg( "yacc: a DataBlockList completed\n" );
#               endif
		    $$ = $1;
		    if( $3 != NULL )
			$$->AddToEnd($3);
		    FINISH_ERR_MSG( EXP_DATANODE,
		                    "In a data block, encountered something "
		                    "that was not a tag/value, loop, or saveframe." );
		    yyclearin; yyerrok;
		  }
		| error data
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a starList completed (epsilon)\n" );
#               endif
		    $$ = new ASTlist<DataNode *>;
		    if( $2 != NULL )
			$$->AddToEnd($2);
		    FINISH_ERR_MSG( EXP_DATANODE,
		                    "In a data block, encountered something "
		                    "that was not a tag/value, loop, or saveframe." );
		    yyclearin; yyerrok;
		  }
		;

globalHeading	: GLOBAL_HEADING
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a globalHeading completed\n" );
#               endif
		    $$ = new GlobalHeadingNode(*($1.strVal));
		    $$->setLineNum( $1.lineNum );
		    $$->setColNum( $1.colNum );
		    delete $1.strVal;
		  }
		;

dataHeading	: DATA_HEADING
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataHeading completed\n" );
#               endif
		    $$ = new DataHeadingNode(*($1.strVal));
		    $$->setLineNum( $1.lineNum );
		    $$->setColNum( $1.colNum );
		    delete $1.strVal;
		  }
	        ;

data		: dataLoop 
                {
#               ifdef DEBUG
		    debug_msg( "yacc: a data completed (version A)\n" );
#               endif
		    $$ = $1;
		    // $$->setLineNum( $1->getLineNum() );
		    // $$->setColNum( $1->getColNum()  );
		}
		| dataItem
		{
#               ifdef DEBUG
		    debug_msg( "yacc: a data completed (version B)\n" );
#               endif
		    $$ = $1;
		    if( $1 != NULL )
		    {
			$$->setLineNum( $1->getLineNum() );
			$$->setColNum( $1->getColNum()  );
		    }
		}
		| saveFrame
		{
#               ifdef DEBUG
		    debug_msg( "yacc: a data completed (version C)\n" );
#               endif
		    $$ = $1;
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum()  );
		}
		;

dataItem	: DATA_NAME dataValue           
		  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataItem completed\n" );
#               endif
		    DataNameNode *name = new DataNameNode(*($1.strVal));
		    name->setLineNum( $1.lineNum );
		    name->setColNum( $1.colNum );

		    $$ = new DataItemNode(name, new DataValueNode(*$2) );
		    $$->setLineNum( name->getLineNum() );
		    $$->setColNum( name->getColNum() );
		    delete $1.strVal;
		    delete $2;
		  }
	        ;

dataValue	: NON_QUOTED_TEXT_STRING
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataValue completed (version A)\n" );
#               endif
		      $$ = new DVNWithPos(  (*$1.strVal),
			                       DataValueNode::NON );
		      $$->setLineNum( $1.lineNum );
		      $$->setColNum( $1.colNum );
		      delete $1.strVal;
		  }
		| DOUBLE_QUOTED_TEXT_STRING
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataValue completed (version B)\n" );
#               endif
		      $$ = new DVNWithPos(  *($1.strVal),
			                       DataValueNode::DOUBLE );
		      $$->setLineNum( $1.lineNum );
		      $$->setColNum( $1.colNum );
		      delete $1.strVal;
		  }
		| SINGLE_QUOTED_TEXT_STRING
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataValue completed (version C)\n" );
#               endif
		      $$ = new DVNWithPos(  *($1.strVal),
			                       DataValueNode::SINGLE );
		      $$->setLineNum( $1.lineNum );
		      $$->setColNum( $1.colNum );
		      delete $1.strVal;
		  }
		| SEMICOLON_BOUNDED_TEXT_STRING
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataValue completed (version D)\n" );
#               endif
		      char *str = strdup( (*($1.strVal)).c_str() );

		      $$ = new DVNWithPos(  str, DataValueNode::SEMICOLON );
		      $$->setLineNum( $1.lineNum );
		      $$->setColNum( $1.colNum );
		      // Check for warning: keywords inside the semicolon
		      // string:
		      char *keyword_pos = NULL;
		      char keyword[20];
		      keyword[0] = '\0';
		      char errMsg[200];
		      if( (keyword_pos = strstr( str, "stop_" )) != NULL )
			  strcpy( keyword, "stop_" );
		      else if( (keyword_pos = strstr( str, "loop_" )) != NULL )
			  strcpy( keyword, "loop_" );
		      else if( (keyword_pos = strstr( str, "save_" )) != NULL )
			  strcpy( keyword, "save_" );
		      else if( (keyword_pos = strstr( str, "data_" )) != NULL )
			  strcpy( keyword, "data_" );
		      if( keyword_pos != NULL )
		      {
			  int   start_line=0, keyword_line=0, cur_line=0;
			  char  *ch=NULL;

			  // Calculate the line numbers to report in the
			  // error message (counting backwards):
			  for(   cur_line = $1.lineNum,
			         ch = &( str[strlen(str)-1] )
			         ;
			         ch >= &(str[0])
				 ;
				 ch-- )
			  {
			      if( *ch == '\n' )
				  cur_line--;
			      if( ch == keyword_pos )
				  keyword_line = cur_line;
			  }
			  start_line = cur_line;

			  if( star_warning_check )
			  {
			      sprintf( errMsg, 
				      "Possible runaway value from line %d:\n"
				      "\t(This suspicion is due to the '%s' "
				      "on line %d.)",
				      start_line, keyword, keyword_line );
			      yyerror( "WARNING" );
			      FINISH_ERR_MSG( WARN_KEYWORD_IN_VALUE, errMsg);
			  }
		      }
		      free(str);
		      delete $1.strVal;
		  }
		| FRAME_CODE
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataValue completed (version E)\n" );
#               endif
		      $$ = new DVNWithPos(  *($1.strVal),
			                       DataValueNode::FRAMECODE );
		      $$->setLineNum( $1.lineNum );
		      $$->setColNum( $1.colNum );
		      delete $1.strVal;
		  }
		;

saveFrame	: saveHeading saveFrameList SAVE_END
		  {
#               ifdef DEBUG
		    debug_msg( "yacc: a saveFrame completed\n" );
#               endif
		    $$ = new SaveFrameNode($1, new SaveFrameListNode($2));
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() );
		  }
		| saveHeading SAVE_END /*empty contents*/
		  { 
#               ifdef DEBUG
		    debug_msg( "yacc: a saveFrame completed\n" );
#               endif
		    $$ = new SaveFrameNode($1->myName() );
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() );
		  }
		;

saveHeading     : SAVE_HEADING
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a saveHeading completed\n" );
#               endif
		    $$ = new SaveHeadingNode( *($1.strVal ));
		    $$->setLineNum( $1.lineNum );
		    $$->setColNum( $1.colNum );
		    delete $1.strVal;
		  }
	        ;

saveFrameList 	: saveFrameList save           
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a saveFrameList completed\n" );
#               endif
		      $$ = $1;
		      if( $2 != NULL )
		      {   $$->AddToEnd($2);
		      }
		  }
		| saveFrameList error save
		  { 
#               ifdef DEBUG
		    debug_msg( "yacc: a saveFrameList completed (err 1)\n" );
#               endif
		      $$ = $1;
		      if( $3 != NULL )
		      {   $$->AddToEnd($3);
	              }
		    FINISH_ERR_MSG( EXP_ITEM_OR_LOOP,
		                    "Encountered something that was not a loop_ or tag/value.");
		    yyclearin; yyerrok;
		  }
		| error save
		  { 
#               ifdef DEBUG
		    debug_msg( "yacc: a saveFrameList completed (err 2)\n" );
#               endif
		    $$ = new ASTlist<DataNode *>;
		    if( $2 != NULL )
		    {   $$->AddToEnd($2);
		    }
		    FINISH_ERR_MSG( EXP_ITEM_OR_LOOP,
		                    "Encountered something that was not a loop_ or tag/value.");
		    yyclearin; yyerrok;
		  }
		| save
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a saveFrameList completed (epsilon)\n" );
#               endif
		     $$ = new ASTlist<DataNode *>;
		     if( $1 != NULL )
		     {   $$->AddToEnd($1);
		     }
		  }
		  ;

save		: dataLoop
                {
#               ifdef DEBUG
		    debug_msg( "yacc: a save completed (version A)\n" );
#               endif
		    $$ = $1;
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() );
		}
		| dataItem
		{
#               ifdef DEBUG
		    debug_msg( "yacc: a save completed (version B)\n" );
#               endif
		    $$ = $1;
		    if( $1 != NULL )
		    {
			$$->setLineNum( $1->getLineNum() );
			$$->setColNum( $1->getColNum() );
		    }
		}
		;

dataLoop	: LOOP dataLoopDefList dataLoopTable
                  { 
#               ifdef DEBUG
		    debug_msg( "yacc: a dataLoop completed\n" );
#               endif
                    $$ = new DataLoopNode( $3->getTabFlag() ? "tabluate" :
						"linear" );

		    $$->setNames( *$2 );
		    $$->setVals( *$3 );
		    $$->setLineNum( $1.lineNum );
		    $$->setColNum( $1.colNum );

		    last_loop_line = line_num;
		    last_loop_col = col_num;

		    /* Reset loop counters for the next loop that might
		     * come along:
		     */
		    yacc_cur_nest = 0;
		    cur_nest = 0;
		    cur_val = 0;
		    rowsThisLoopSoFar = 0;
		    for (  loop_init_idx = 0 ; loop_init_idx<50 ; loop_init_idx++ )  {
			loop_sizes[ loop_init_idx ] = 99999;
			loop_first_tagnames[ loop_init_idx ] = string("");
		    }
		    
		  }
		;

dataLoopDefList : dataLoopDefList LOOP loopDefList
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataLoopDefList completed (version A)\n" );
#               endif
		    $$ = $1;
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() );
		    $$->insert( $$->end(),  $3 );
		    loop_sizes[yacc_cur_nest] = $3->size();
		    loop_first_tagnames[ yacc_cur_nest ] = (* $3)[0]->myName();
		    yacc_cur_nest++;
		    already_had_row_count_error_this_loop = false;
		  }
                | loopDefList   
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataLoopDefList completed (version B)\n" );
#               endif
		    $$ = new DataLoopNameListNode();
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() );
		    $$->insert( $$->end(), $1 );
		    loop_sizes[yacc_cur_nest] = $1->size();
		    loop_first_tagnames[ yacc_cur_nest ] = (* $1)[0]->myName();
		    yacc_cur_nest++;
		    already_had_row_count_error_this_loop = false;
		  }
                ;

dataLoopTable : dataLoopTable dataLoopRow
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataLoopTable completed (version A)\n" );
#               endif
		    $$ = $1;
		    $$->insert( $$->end(), $2 );
		  }
                | dataLoopRow
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a dataLoopTable completed (version B)\n" );
#               endif
		    $$ = new LoopTableNode( $1->getTabFlag() );
		    $$->setLineNum( $1->getLineNum() );
		    $$->setColNum( $1->getColNum() );
		    $$->insert( $$->end(), $1 );
		  }
                ;

loopDefList	: loopDefList DATA_NAME
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a loopDefList completed (version A)\n" );
#               endif
		      DataNameNode *name = new DataNameNode( *($2.strVal) );
		      name->setLineNum( $2.lineNum );
		      name->setColNum( $2.colNum );

		      $$ = $1;
		      $$->insert( $$->end(), name );
		      delete $2.strVal;
		      loop_sizes[yacc_cur_nest] ++;
		  }
		| DATA_NAME
                  {
#               ifdef DEBUG
		    debug_msg( "yacc: a loopDefList completed (version B)\n" );
#               endif
		    DataNameNode *name = new DataNameNode( *($1.strVal) );
		    name->setLineNum( $1.lineNum );
		    name->setColNum( $1.colNum );

		    $$ = new LoopDefListNode();
		    $$->setLineNum( $1.lineNum );
		    $$->setColNum( $1.colNum );
		    $$->insert( $$->end(), name ); 
		    delete $1.strVal;
		    loop_sizes[yacc_cur_nest] = 1;
		  }
		;

dataLoopRow   : simpleDataLoopRow FAKE_TOKEN_SUBROW_EXISTS dataLoopTable FAKE_TOKEN_ROW_END
		  {
		      // If only one column in the row, turn its tab
		      // flag back on (it defaults to linear until the
		      // second value is checked to see if it is on
		      // the same line, so in the case of just one
		      // value, it never made this check yet.
		      if( $1->size() == 1 )
		          $1->setTabFlag(true);
		      $$ = $1;
		      $$->setMyLoop( $3 );
		      ++rowsThisLoopSoFar;
		  }
	      | simpleDataLoopRow FAKE_TOKEN_ROW_END
	          {
		      // If only one column in the row, turn its tab
		      // flag back on (it defaults to linear until the
		      // second value is checked to see if it is on
		      // the same line, so in the case of just one
		      // value, it never made this check yet.
		      if( $1->size() == 1 )
		          $1->setTabFlag(true);
		      $$ = $1;
		      ++rowsThisLoopSoFar;
		  }
              ;

simpleDataLoopRow :  simpleDataLoopRow dataValue
	          {
		      $$ = $1;
		      if( $$->getLineNum() == $2->getLineNum() )
		          $$->setTabFlag( true );

		      prevRowLineNum = $2->getLineNum() ;
		      $$->insertElementAt( $$->size(), *$2 );
		      delete $2;
		  }
	          | dataValue
	          {
		      if( prevRowLineNum == $1->getLineNum() ) {
		        if( star_warning_check ) {
			  // Don't issue the warning for the tags that we have it 
			  // defined to ignore it for:
			  bool in_ignore_list = false;
			  for( vector<string>::iterator it = g_tags_to_disable_row_counting.begin() ;
			       it != g_tags_to_disable_row_counting.end() ;
			       ++it ) {
			     if( loop_first_tagnames[cur_nest]  == *it ) {
			       in_ignore_list = true;
			     }
			  }
			  if(    ! in_ignore_list  ) {

			    if( already_had_row_count_error_this_loop
				       &&
				 one_row_count_error_per_loop_only  ) {

			      // do nothing;
			    } else {

			      char errMsg[2048];
			      sprintf( errMsg, 
				      "The number of values in a row of text does not match the number of column names in the header.\n"
				      "This can either be because the number of column names in the header is wrong "
				      "or because the number of values in the table is wrong. "
				      "<i><p>%s</p></i><br/>\n"
				      "%s",
				      (rowsThisLoopSoFar <=2 ?
				          "Because this error was detected in the first one or two rows of "
					  "data, it is most likely due to the loop header being wrong "
					  "rather than the loop data being wrong. "
					  :
					  "" ),
				      (one_row_count_error_per_loop_only ?
				         "(Only the first such error is reported for a table, to avoid a flood of messages.)" :
					 "" )
				     ) ;
			      yyerror( "" );
			      FINISH_ERR_MSG( WARN_LOOP_VALUES_MISALIGNED, errMsg);
			      already_had_row_count_error_this_loop = true;
			    }
			  }
			}
		      }
		      $$ = new LoopRowNode( false, loop_sizes[cur_nest] );
		      $$->setLineNum( $1->getLineNum() );
		      $$->setColNum( $1->getColNum() );
		      $$->insertElementAt( $$->size(), *$1 );

		      delete $1;
		  }
		  ;
