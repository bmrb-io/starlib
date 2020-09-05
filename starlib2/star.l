%{
/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//***************************************************************************
// Scanner flex specification
//***************************************************************************
/* Lex Definitions for a STAR File */

#ifdef YYLMAX
#undef YYLMAX
#endif
// 4 Meg in one token:
#define YYLMAX 4194304
%}

a		     [aA]
b		     [bB]
d		     [dD]
e		     [eE]
g		     [gG]
l		     [lL]
o		     [oO]
p		     [pP]
s		     [sS]
t		     [tT]
v		     [vV]
eoln                 (\r\n|\r|\n)
Dollar	     	     \$
Data		     {d}{a}{t}{a}
Loop		     {l}{o}{o}{p}
Global	  	     {g}{l}{o}{b}{a}{l}
Stop		     {s}{t}{o}{p}
Save		     {s}{a}{v}{e}
Char		     [^\n\r]
Blank		     [\n\r \t]
FakeEOR	             _:;
FakeSUBEXIST         _::
NonBlank	     [!-\377]
NonDQuote	     [^\n\r"]
NonSQuote	     [^\n\r']
NotUnderscore	     [^\n\r \t_]
Space	             [\t ]
Spaces               [\t ]+

%{
#include "portability.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fstream>
#include <algorithm>
#include "scanner.h"
#include "ast.h"
#include "gram.tab.h" 	//token definitions etc.
#include "message.h"


int idx_1;
int line_num=1;
int col_num=1;
bool err_is_printing = false;
int star_err_cnt = 0;
int last_loop_line = 0;
int last_loop_col = 0;
int in_loop_header = 0;
bool star_warning_check = true;
bool star_syntax_print = true;

int cur_nest = 0; /* nest level in loop VALUE just parsed */
int yacc_cur_nest = 0; /* nest level in loop NAME just parsed */
int cur_val = 0;      /* 'column' of loop value just scanned */

int loop_sizes[50];
int rowsThisLoopSoFar = 0;
string loop_first_tagnames[50];
vector<string> g_tags_to_disable_row_counting ;
bool one_row_count_error_per_loop_only = true;

// A new change to the C++ standard refuses to allow assignment code
// to be run during global initialization blocks like this one, but it
// does allow code to be run as a side-effect of initialization of a
// variable, so the following assignments are being run that way:
// Yes, this is ugly to use a side effect as the main intended effect.
// If you have a complaint about that, take it up with the C++ standards
// committies that decided there was nothing wrong with breaking perfectly 
// good code by instituting a standard that doesn't even do what it claims
// to do.  (You can still run whatever arbitrary code you feel like in
// the global section but now you have to make it a side-effect of an
// initialization statement.  Code is still running in this part.  That
// has not changed and thus the rule is an arbitrary headache for no good
// reason.)
vector<string>::iterator _dummy_throwaway_LGKJASLKJIH =
  // A little-used C syntax:  A parenthesized list with
  // comma-separated expressions is itself an expression
  // that returns the last term as its return value.
  // Ta-Da!  Now any arbitrary code can become an
  // expression.
  (
    g_tags_to_disable_row_counting.insert(
      g_tags_to_disable_row_counting.end(),
      string("_Residue_seq_code") )
    ,
    g_tags_to_disable_row_counting.insert(
      g_tags_to_disable_row_counting.end(),
      string("_Residue_author_seq_code") )   
    ,
    g_tags_to_disable_row_counting.insert(
      g_tags_to_disable_row_counting.end(),
      string("_Residue_label")  )
  );

int  prevRowLineNum = 0;
bool subrow_pending = false;

vector<string> g_skipTexts;
vector<int> g_skipTextLineNums;

void fillLineAndColNum();
void handle_whitespace();
void handle_newline();
void count_newlines();
void debug_msg( const char* );

int loop_init_idx;

%}

%s INSEMIDELIM

%s SKIPPING

%array

%%
<INITIAL>{Loop}_              {
		       if( ! in_loop_header )
		       {
			   for(   loop_init_idx = 0 ;
				  loop_init_idx<50 ;
				  loop_init_idx++   )
			       loop_sizes[loop_init_idx] = 99999;
			   cur_nest = 0;
			   cur_val = 0;
		       }
		       in_loop_header = 1;
		       fillLineAndColNum();
		       cur_val = 0;
		       subrow_pending = false;
		       debug_msg( "lex: return LOOP\n" );
                       return (LOOP);
		     }

<INITIAL>{Stop}_              {
		       fillLineAndColNum();
		       debug_msg( "lex: NOT return, stop found\n" );

		       if( cur_nest == 0 )
		       {   in_loop_header = 0;
		       }
                       if( cur_nest > 0 ) 
		       {   cur_nest--;
			   /* end the subtable: */
			   unput(';'); unput(':'); unput('_'); unput(' ');
		       }
		       if( subrow_pending )
		       {   subrow_pending = false;
		           unput(';');unput(':');unput('_');unput(' ');
		       }
		     }
                       
<INITIAL>{Data}_{NonBlank}+   {
		       in_loop_header = 0;
		       fillLineAndColNum();
		       yylval.TOKENval.strVal = new string(yytext);
		       debug_msg( "lex: dataheading found: " );
		       debug_msg( yytext );
		       debug_msg( "\n" );
		       for(   loop_init_idx = 0 ;
		              loop_init_idx<50 ;
			      loop_init_idx++   )
			   loop_sizes[loop_init_idx] = 99999;
		       debug_msg( "lex: return DATA_HEADING: " );
		       debug_msg( yytext );
		       debug_msg( "\n" );
                       return (DATA_HEADING);
		     }

<INITIAL>{Global}_            {
		       in_loop_header = 0;
		       fillLineAndColNum();
		       yylval.TOKENval.strVal = new string(yytext);
		       for(   loop_init_idx = 0 ;
		              loop_init_idx<50 ;
			      loop_init_idx++   )
			   loop_sizes[loop_init_idx] = 99999;
		       debug_msg( "lex: return GLOBAL\n" );
                       return (GLOBAL_HEADING);
		     }

<INITIAL>{Save}_{NonBlank}+   {
		       in_loop_header = 0;
		       fillLineAndColNum();
		       yylval.TOKENval.strVal = new string(yytext);
		       debug_msg( "lex: saveheading found: " );
		       debug_msg( yytext );
		       debug_msg( "\n" );
		       for(   loop_init_idx = 0 ;
		              loop_init_idx<50 ;
			      loop_init_idx++   )
			   loop_sizes[loop_init_idx] = 99999;
		       debug_msg( "lex: return SAVE_HEADING: " );
		       debug_msg( yytext );
		       debug_msg( "\n" );
                       return (SAVE_HEADING);
		     }

<INITIAL>{Save}_              {
		       in_loop_header = 0;
		       fillLineAndColNum();
		       for(   loop_init_idx = 0 ;
		              loop_init_idx<50 ;
			      loop_init_idx++   )
			   loop_sizes[loop_init_idx] = 99999;
		       debug_msg( "lex: return SAVE_END\n" );
                       return (SAVE_END);
		     }
                       
<INITIAL>{Dollar}{NonBlank}+  {
		       in_loop_header = 0;
		       /* If a subrow is still pending, then we didn't get a stop_,
		        * and so this is the next nesting level.  Push this
			* token back onto the stream, preceeded by a
			* subrow-exists fake token for the parser to see.
			*/
		       if( subrow_pending )
		         { /* Token must be pushed back in inverse order */
			   for( idx_1 = strlen(yytext)-1 ; idx_1 >= 0 ; idx_1-- )
			   {   unput(yytext[idx_1]);
			   }
		           unput(' '); unput(':'); unput(':'); unput('_');
		       }
		       else
		       {
			   fillLineAndColNum();
			   yylval.TOKENval.strVal = new string(yytext);
			   if( yylval.TOKENval.strVal == NULL )
			   {
			       fprintf( stderr, "-CANNOT ALLOC MEMORY: %s\n",
				   strerror(errno) );
			       fflush(stderr);
			   }

			   cur_val++;
			   if( cur_val >= loop_sizes[ cur_nest ] )
			   {
			       cur_val = 0;
			       /* If the loop goes deeper, increment the nest
				* level.  If this is the innermost level, don't:
				*/
			       if( loop_sizes[ cur_nest+1 ] != 99999 )
			       {   subrow_pending = true;
			       }
			       else
			       {
				   /* Generate the fake end-of-row token.  Must
				    * be done in inverse order (this is really the
				    * string " _:;" being padded into the input): */
				   unput( ';' ); unput( ':'); unput('_'); unput(' ');
			       }
			   }

			   debug_msg( "lex: return FRAME_CODE:" );
			   debug_msg( yytext );
			   debug_msg( "\n" );
			   return FRAME_CODE;
		       }
		     }

<INITIAL>{FakeEOR}   {
		       /* Return the 'row is finished' token.  This
		        * Token is 'fake' in that it is actually
			* created by 'unput' within this parser
			* rather than being something that was really
			* in the file.
			*/
		       debug_msg( "lex: return FAKE_TOKEN_ROW_END\n" );
                       return FAKE_TOKEN_ROW_END;
                     }
		     
<INITIAL>{FakeSUBEXIST} {
		       /* Return the 'subtable exists' token.  This
		        * Token is 'fake' in that it is actually
			* created by 'unput' within this parser
			* rather than being something that was really
			* in the file.
			*/
		       subrow_pending = false;
		       cur_nest++;
		       debug_msg( "lex: return FAKE_TOKEN_SUBROW_EXISTS \n" );
                       return FAKE_TOKEN_SUBROW_EXISTS;
                     }


<INITIAL>_{NonBlank}+         {
		       // tags allowed in loop header, so no
		       //   'in_loop_header = 0' line here:
		       fillLineAndColNum();
		       yylval.TOKENval.strVal = new string(yytext);
		       cur_val = 0;
		       debug_msg( "lex: return DATA_NAME: " );
		       debug_msg( yytext );
		       debug_msg( "\n" );
                       return (DATA_NAME);
		     }

<INITIAL>\"[^\n\r]*\"{Blank}    {
		       in_loop_header = 0;
		       /* Got the longest "..." string in the line, but
		        * this string might contain smaller strings
			* in it, since regular expressions match the
			* longest thing they can.
			* Example:   _tag1 "val1" _tag2 "val2"
			* matched string is "val1" _tag2 "val2"
			* We want to break it apart if this is the
			* case, and push back the rest of the token
			* for further lexing.  (This ugliness is
			* required because STAR's spec says you can have
			* quotes in quotes provided they have no
			* whitespace after - that's a construct that
			* regular exp's cannot do, so we resort to this
			* icky read-too-far-and-pushback technique.)
			*/
		       char *ch;
		       int  prev_was_quote = 0;
		       for( ch = yytext+1 ; *ch != '\0' ; ch++ )
		       {
		           if( *ch == '\"' )
			   {   prev_was_quote = 1;
			   }
			   else
			   {   if( prev_was_quote )
			       {   prev_was_quote = 0;
				   if(    *ch == ' ' ||
				          *ch == '\t' ||
				          *ch == '\n' ||
					  *ch == '\r'  )
				   {   yyless( (ch - yytext) );
				       yytext[ ch - yytext ] = '\0';
				       ch--; /* pre-undo of for-loop's incr */
				   }
			       }
			   }
		       }
		       /* If a subrow is still pending, then we didn't get a stop_,
		        * and so this is the next nesting level.  Push this
			* token back onto the stream, preceeded by a
			* subrow-exists fake token for the parser to see.
			*/
		       if( subrow_pending )
		       {   /* Token must be pushed back in inverse order */
			   for( idx_1 = strlen(yytext)-1 ; idx_1 >= 0 ; idx_1-- )
			   {   unput(yytext[idx_1]);
			   }
			   unput(' '); unput(':'); unput(':'); unput('_');
		       }
		       else
		       {

			   fillLineAndColNum();
			   yylval.TOKENval.strVal = new string(yytext);

			   cur_val++;
			   if( cur_val >= loop_sizes[ cur_nest ] )
			   {
			       cur_val = 0;
			       /* If the loop goes deeper, increment the nest
				* level.  If this is the innermost level, don't:
				*/
			       if( loop_sizes[ cur_nest+1 ] != 99999 )
			       {   subrow_pending = true;
			       }
			       else
			       {   /* Generate the fake end-of-row token.  Must
				    * be done in inverse order (this is really the
				    * string " _:;" being padded into the input): */
				   unput( ';' ); unput( ':'); unput('_'); unput(' ');
			       }
			   }
			   debug_msg( "lex: return DOUBLE_QUOTED_TEXT_STRING: " );
			   debug_msg( yytext );
			   debug_msg( "\n" );
			   return (DOUBLE_QUOTED_TEXT_STRING);
		       }
		     }

<INITIAL>\'[^\n\r]*\'{Blank}    {
		       in_loop_header = 0;
		       /* Got the longest '...' string in the line, but
		        * this string might contain smaller strings
			* in it, since regular expressions match the
			* longest thing they can.
			* Example:   _tag1 'val1' _tag2 'val2'
			* matched string is 'val1' _tag2 'val2'
			* We want to break it apart if this is the
			* case, and push back the rest of the token
			* for further lexing.  (This ugliness is
			* required because STAR's spec says you can have
			* quotes in quotes provided they have no
			* whitespace after - that's a construct that
			* regular exp's cannot do, so we resort to this
			* icky read-too-far-and-pushback technique.)
			*/
		       char *ch;
		       int  prev_was_quote = 0;
		       for( ch = yytext+1 ; *ch != '\0' ; ch++ )
		       {
		           if( *ch == '\'' )
			   {   prev_was_quote = 1;
			   }
			   else
			   {   if( prev_was_quote )
			       {   prev_was_quote = 0;
				   if(    *ch == ' ' ||
				          *ch == '\t' ||
				          *ch == '\n' ||
					  *ch == '\r'    )
				   {   yyless( (ch - yytext) );
				       yytext[ ch - yytext ] = '\0';
				       ch--; /* pre-undo of for-loop's incr */
				   }
			       }
			   }
		       }

		       /* If a subrow is still pending, then we didn't get a stop_,
		        * and so this is the next nesting level.  Push this
			* token back onto the stream, preceeded by a
			* subrow-exists fake token for the parser to see.
			*/
		       if( subrow_pending )
		       {   /* Token must be pushed back in inverse order */
			   for( idx_1 = strlen(yytext)-1 ; idx_1 >= 0 ; idx_1-- )
			   {   unput(yytext[idx_1]);
			   }
		           unput(' '); unput(':'); unput(':'); unput('_');
		       }
		       else
		       {
			   fillLineAndColNum();
			   yylval.TOKENval.strVal = new string(yytext);

			   cur_val++;
			   if( cur_val >= loop_sizes[ cur_nest ] )
			   {
			       cur_val = 0;
			       /* If the loop goes deeper, increment the nest
				* level.  If this is the innermost level, don't:
				*/
			       if( loop_sizes[ cur_nest+1 ] != 99999 )
			       {   subrow_pending = true;
			       }
			       else
			       {
				   /* Generate the fake end-of-row token.  Must
				    * be done in inverse order (this is really the
				    * string " _;;" being padded into the input): */
				   unput( ';' ); unput( ':'); unput('_'); unput(' ');
			       }
			   }
			   debug_msg( "lex: return SINGLE_QUOTED_TEXT_STRING: " );
			   debug_msg( yytext );
			   debug_msg( "\n" );
			   return (SINGLE_QUOTED_TEXT_STRING);
		       }
		     }

<INITIAL>{eoln};                   { 
		       in_loop_header = 0;
		       line_num++;
                       BEGIN INSEMIDELIM;  /* prevents "#" from being
		                            * interpeted as comments when
					    * in a semicolon string
					    */
		     }

<INSEMIDELIM>([^\n\r]*{eoln})({eoln}|([^;\n\r][^\n\r]*{eoln}))*;  {
		       in_loop_header = 0;
		       /* If a subrow is still pending, then we didn't get a stop_,
		        * and so this is the next nesting level.  Push this
			* token back onto the stream, preceeded by a
			* subrow-exists fake token for the parser to see.
			*/
		       if( subrow_pending )
		       {   /* Token must be pushed back in inverse order */
			   for( idx_1 = strlen(yytext)-1 ; idx_1 >= 0 ; idx_1-- )
			   {
			       unput(yytext[idx_1]);
			   }
		           unput(';'); unput('\n'); /* put the start of
						       INSEMIDELIM state
						       marker back on. */
			   unput(' ');unput(':'); unput(':'); unput('_');
			   BEGIN INITIAL; /* get out of INSEMIDELIM state, so
					     it can read the
					     FAKE_SUBROW_EXISTS token*/
		       }
		       else
		       {
			   count_newlines();
			   fillLineAndColNum();
			   yylval.TOKENval.strVal = new string(
					      string(";") + string(yytext)  );

			   BEGIN INITIAL;  /* get out of INSEMIDELIM state. */

			   debug_msg( "lex: semiquoted found...(not printed)" );
			   debug_msg( yytext );

			   cur_val++;
			   if( cur_val >= loop_sizes[ cur_nest ] )
			   {
			       cur_val = 0;
			       /* If the loop goes deeper, increment the nest
				* level.  If this is the innermost level, don't:
				*/
			       if( loop_sizes[ cur_nest+1 ] != 99999 )
			       {
				   subrow_pending = true;
			       }
			       else
			       {
				   /* Generate the fake end-of-row token.  Must
				    * be done in inverse order (this is really the
				    * string " _:;" being padded into the input): */
				   unput( ';' ); unput( ':'); unput('_'); unput(' ');
			       }
			   }
			   debug_msg( "lex: return SEMICOLON_BOUNDED_TEXT_STRING: " );
			   debug_msg( yytext );
			   return (SEMICOLON_BOUNDED_TEXT_STRING);
		       }
		     }

<INITIAL>^#<START-SKIP>.*{eoln} {
		           handle_newline();
			   fillLineAndColNum();
			   yylval.TOKENval.strVal = new string(yytext);
			   BEGIN SKIPPING;
			   debug_msg( "lex: First line of SKIPPING found:" );
			}

<SKIPPING>^[^\n\r]*{eoln} {
		       handle_newline();
		       yylval.TOKENval.strVal->append( string(yytext) );
		       if( strncmp( yytext, "#<END-SKIP>",11) == 0 )
		       {
			   g_skipTexts.insert( g_skipTexts.end(),
					     *(yylval.TOKENval.strVal) );
			   g_skipTextLineNums.insert( g_skipTextLineNums.end(),
			                          yylval.TOKENval.lineNum );
                           delete yylval.TOKENval.strVal;
		           BEGIN INITIAL;
			   debug_msg( "lex: Last line of SKIPPING found:" );
		       }
		       debug_msg( "lex: Another line of SKIPPING found:" );
		       debug_msg( yytext );
		       }

<INITIAL>#[^\n\r]*         {  // was: handle_newline();
                              count_newlines();
			      fillLineAndColNum();
                              /**** THROW AWAY COMMENT READER ****/
			   }


<INITIAL>{NotUnderscore}{NonBlank}*   {
		       in_loop_header = 0;
		       /* If a subrow is still pending, then we didn't get a stop_,
		        * and so this is the next nesting level.  Push this
			* token back onto the stream, preceeded by a
			* subrow-exists fake token for the parser to see.
			*/
		       if( subrow_pending )
		       {   /* Token must be pushed back in inverse order */
			   for( idx_1 = strlen(yytext)-1 ; idx_1 >= 0 ; idx_1-- )
			   {   unput(yytext[idx_1]);
			   }
		           unput(' ');unput(':'); unput(':'); unput('_');
		       }
		       else
		       {
			   fillLineAndColNum();
			   yylval.TOKENval.strVal = new string(yytext);

			   cur_val++;
			   if( cur_val >= loop_sizes[ cur_nest ] )
			   {
			       cur_val = 0;
			       /* If the loop goes deeper, increment the nest
				* level.  If this is the innermost level, don't:
				*/
			       if( loop_sizes[ cur_nest+1 ] != 99999 )
			       {   subrow_pending = true;
			       }
			       else
			       {   /* Generate the fake end-of-row token.  Must
				    * be done in inverse order (this is really the
				    * string " _:;" being padded into the input): */
				   unput( ';' ); unput( ':'); unput('_'); unput(' ');
			       }
			   }
			   debug_msg( "lex: return NON_QUOTED_TEXT_STRING: " );
			   debug_msg( yytext );
			   debug_msg( "\n" );
			   return (NON_QUOTED_TEXT_STRING);
		       }
		     }

<INITIAL>{Spaces}             {
                       handle_whitespace();
		     }

<INITIAL>{eoln}                   {
                       handle_newline();
		     }

<INITIAL>.                    {
                       scanner_error(line_num, col_num, "illegal character");
		       fillLineAndColNum();
		     }

%%
void fillLineAndColNum()
{
  yylval.TOKENval.lineNum = line_num;
  yylval.TOKENval.colNum = col_num;
  col_num += yyleng;
  return;
}

void handle_newline()
{
	col_num = yylval.TOKENval.colNum = 1;
	line_num++;
	yylval.TOKENval.lineNum = line_num;
	return;
}

void handle_whitespace()
{
	col_num += yyleng;
	yylval.TOKENval.colNum = col_num;
	yylval.TOKENval.lineNum = line_num;
	return;
}

void count_newlines()
{
  for (int i = 0; yytext[i] != 0; i++)
    if (yytext[i] == '\n')
      line_num++;

  col_num = 1;
}

void debug_msg( const char *print_this )
{
#ifdef DEBUG
    printf( "DEBUG:" );
    printf( "line:%d: %s, ", line_num, print_this );
    fflush( stdout );  /* debug msg MUST get out in case of a crash, */
                       /* else we can't trust it to tell us where it */
                       /* really crashed if it gets too buffered. */
#else
    char shut_up_lint = print_this[0];  // so it does something with the param.
#endif
}

void yyerror(char *msg)
{
    StarErrEntry  defaultError;

    defaultError.errID = STAR_UNKNOWN_ERR;
    defaultError.lineNum = line_num;
    defaultError.colNum = col_num;
    strncpy( defaultError.tokenStr, yytext, sizeof(defaultError.tokenStr)-1 );
    strncpy( defaultError.errStr, msg, sizeof(defaultError.errStr)-1 );
    char    tokenPrint[102];

    if( star_err_cnt == 0 )
    {
	// This is the first error ever encountered in
	// This particular parse.  If there was a previous
	// call to yyparse() in this living process, the
	// error list might still be populated from it,
	// so let's clear it before adding the errors for
	// this new parse.
	starErrReport.erase( starErrReport.begin(), starErrReport.end() );
    }
    else
    {
	if( (starErrReport.end()-1) -> errID == STAR_UNKNOWN_ERR )
	    if( star_syntax_print )
	        fprintf( stderr, "Parse error.\n" );
		                 // previous error was unknown and
		                 // its error message was never
				 // finished with finish_err_msg()
    }
    starErrReport.insert( starErrReport.end(), defaultError );
    star_err_cnt++;

    if(     strchr( yytext, '\n' ) != NULL ||
            strlen(yytext) > 100 )
	strcpy( tokenPrint, "[not printed]");
    else
	strcpy( tokenPrint, yytext );

    if(  strcmp( msg, "parse error" ) == 0 ||
         strcmp( msg, "Parse Error" ) == 0 ||
	 strcmp( msg, "Parse error" ) == 0 ||
	 strcmp( msg, "PARSE ERROR" ) == 0    )
    {
        err_is_printing = true;
	if( star_syntax_print )
	    fprintf( stderr, "ERR: at '%s': line %d, character %d: ",
		    tokenPrint, line_num, col_num );
    }
    else if(  strcmp( msg, "Warning" ) == 0 ||
	      strcmp( msg, "WARNING" ) == 0    )
    {
        err_is_printing = true;
	if( star_syntax_print )
	    fprintf( stderr, "WARN: at '%s': line %d, character %d: ",
		    tokenPrint, line_num, col_num );
    }
    else
    {
	if( star_syntax_print )
	    fprintf( stderr, "%s (line %d, col %d), reading '%s')\n",
		    msg, line_num, col_num, tokenPrint);
    }
}

void finish_err_msg( StarErrID id, const char *msg )
{

    // Update some of the fields on the most recent
    // error message (created by the last call to yyerror().)
    strcpy( (starErrReport.end() - 1)->errStr,  msg );
    (starErrReport.end() - 1)->errID = id;

    // Print the error msg:
    if( star_syntax_print )
    {
	fprintf( stderr, "%s\n", msg );
	fflush(stderr);
    }
    err_is_printing = false;
}

