
//////////////////////////// RCS COMMENTS: /////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
$Log: not supported by cvs2svn $
Revision 1.1  2005/11/08 16:11:48  madings
*** empty log message ***

#endif
////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "astlist.h"
#include "loopvals.h"
#include "transforms.h"
#include "parser.h"

// GLOBAL variable: the root of the AST built by a (successful) parse)

StarFileNode *AST;

ostream * os;
ofstream * errStream;

void doOneSaveframe( SaveFrameNode *inputSF );

  ///////////////////
 //    main()     //
///////////////////
int main( int argc, char **argv )
{


    if( argc < 2 )
    {   cerr<<"Needs 1 arg: template file name"<<endl;
        exit(2);
    }
    char *templateFname = argv[1];

    yyin = fopen(templateFname, "r" );
    os = &cout;

    if( yyparse() != 1 )
    {
        cout<<"save_order_mapping"<<endl<<
	      "    _Saveframe_category   order_and_loops"<<endl<<
	      "    loop_"<<endl<<
	      "        _saveframe_name"<<endl<<
	      "        _key_tag"<<endl<<
	      "        _key_value"<<endl<<
	      "        loop_"<<endl<<
	      "            _tag_name"<<endl<<
	      "            _which_loop"<<endl<<
	      "            _tabulate_flag"<<endl<<endl;


	List<ASTnode*> *inputSFs = AST->searchForType( ASTnode::SAVEFRAMENODE );

	int i;
	for( i = 0 ; i < inputSFs->size() ; i++ )
	{
	    doOneSaveframe( (SaveFrameNode*)( (*inputSFs)[i] ) );
	}
	delete inputSFs;

	cout<<"    stop_"<<endl<<endl<<"save_"<<endl;
    }
    else
    {
        exit(1);
    }
}
void doOneSaveframe( SaveFrameNode *inputSF )
{
    int sfIdx;

    for( sfIdx = 0 ; sfIdx < inputSF->GiveMyDataList()->size() ; sfIdx++ )
    {

        if( (*(inputSF->GiveMyDataList()))[sfIdx]->isOfType( ASTnode::DATAITEMNODE ) )
	{
	    DataItemNode *curItem = (DataItemNode*)( (*(inputSF->GiveMyDataList()))[sfIdx] );
	    // Assuming the sf category is always the first thing:
	    if( sfIdx == 0 )
	    {
	      cout<<"    *  \""<<curItem->myName()<<"\"	\""<<curItem->myValue()<<"\""<<endl;
	    }
	    cout<<"        \""<<curItem->myName()<<"\"  F  ."<<endl;
	}
	else  // Assume it's a loop:
	{
	    DataLoopNode *curLoop = (DataLoopNode*)( (*(inputSF->GiveMyDataList()))[sfIdx] );
	    // Assume only the outermost nest level is present or relevant:
	    if( curLoop->getNamesPtr()->size() > 0 )
	    {
	        LoopNameListNode  *curNames = (*(curLoop->getNamesPtr()))[0];
	        int j;
		for( j = 0 ; j < curNames->size() ; j++ )
		{   DataNameNode *cur = (*curNames)[j];
		    cout<<"        \""<<cur->myName()<<"\"  loop_"<<sfIdx<<" "<<
		        (curLoop->getTabFlag() ? "tabulate" : "linear")<<endl;
		}
	    }
	}
    }
    cout<<"        stop_"<<endl<<endl;
}
