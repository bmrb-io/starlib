/* This software is copyright (c) 2001-2009 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
/**
 * Pretty-printer for NMR-STAR v. 3.1. 
 *
 * Differences from 2.1 version:
 *  comment file doesn't have nested loops
 *  command-line switches
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and prints to
//  standard output the very same star file unparsed.
//  This program uses the NMRSTAR formatter to add proper comments
//  to the file.
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include "ast.h"
#include "parser.h"

//#define COMMENT_FILE_NAME "/bmrb/lib/comment.str"

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

// output stream:
ostream * os;

bool formatNMRSTAR3( StarFileNode *, StarFileNode * );
void usage( const char * prog ) {
    std::cerr << "Usage: " << prog << " [-c commentfile]";
}
                                                                           
  ///////////////////
 //    main()     //
///////////////////
int main( int argc, char **argv )
{
    std::string commentfile;
    int opt;
    while( (opt = getopt( argc, argv, "c:" )) != -1 ) {
        switch( opt ) {
            case 'c' :
                commentfile = optarg;
                break;
            default :
                usage( argv[0] );
                exit( 1 );
        }
    }
                                                                               
// parse star file
    StarFileNode * star;
    yyin = stdin; // input
    if( yyparse() != 0 ) exit( 2 );
    star = AST;
// parse comment file
    StarFileNode * comment = NULL;
    if( commentfile != "" ) {
        if( (yyin = fopen( commentfile.c_str(), "r" )) == NULL )
            exit( 3 );
        if( yyparse() != 0 )
            exit( 4 );
        fclose( yyin );
        comment = AST;
    }
    if( comment != NULL ) {
        formatNMRSTAR3( star, comment );
        delete comment;
    }
// unparse
    os = &std::cout;
    star->Unparse( 0 );
    os->flush();
    delete star;
    return 0;
}

///
/// 3.1 version of formatNMRSTAR: I got rid of nested loop in 3.1
/// comment file: we don't use them in 3.1 so coment file might as
/// well be consistent with that rule.
///
bool formatNMRSTAR3( StarFileNode * entry, StarFileNode * comments ) {
    List<ASTnode *> * result = comments->searchForTypeByTag( ASTnode::DATALOOPNODE, std::string( "_comment" ) );
    if( result->size() != 1 ) {
        std::cerr << "Found " << result->size() << " _comment loops in comment file!" << std::endl;
        return false;
    }
    DataLoopNode * loop = dynamic_cast<DataLoopNode *>((*result)[0]);
    DataLoopNameListNode * nlist = loop->getNamesPtr();
    LoopNameListNode * names = (*nlist)[0];
    LoopTableNode * vals = loop->getValsPtr();
    int comidx = names->tagPosition( "_comment" );
    int flgidx = names->tagPosition( "_every_flag" );
    int catidx = names->tagPosition( "_category" );
    int tagidx = names->tagPosition( "_tagname" );
    LoopRowNode * row;
    DataValueNode * val;
    SaveFrameNode * sf;
    DataItemNode * item;
    bool every_flag;
    bool comment_set;
    for( LoopTableNode::iterator i = vals->begin(); i != vals->end(); i++ ) {
        every_flag = false;
        comment_set = false;
        row = *i;
// just in case
        val = (*row)[comidx];
        if( (val->myValue() == "") || (val->myValue() == ".") || (val->myValue() == "?") )
            continue;
// every flag
        val = (*row)[flgidx];
        if( (val->myValue().at( 0 ) == 'Y') || (val->myValue().at( 0 ) == 'y') )
            every_flag = true;
// all saveframes
        result = entry->searchForType( ASTnode::SAVEFRAMENODE );
        if( result->size() < 1 ) return false;
        for( List<ASTnode *>::iterator j = result->begin(); j != result->end(); j++ ) {
            sf = dynamic_cast<SaveFrameNode *>( *j );
#ifdef DEBUG
std::cerr << "SF " << sf->myName() << std::endl;
#endif
            ASTlist<DataNode *> * tags = sf->GiveMyDataList();
// comment is either sf category - level or tag - level
//try tag-level first
            val = (*row)[tagidx];
            if( (val->myValue() == "") || (val->myValue() == ".") || (val->myValue() == "?") ) {
                val = (*row)[catidx];
                if( (val->myValue() == "") || (val->myValue() == ".") || (val->myValue() == "?") )
                    continue;
// sf-level coment
                for( ASTlist<DataNode *>::iterator tag = tags->begin(); tag != tags->end(); tag++ ) {
                    if( ! (*tag)->isOfType( ASTnode::DATAITEMNODE ) ) continue;
                    item = dynamic_cast<DataItemNode *>( *tag );
                    if( item->myName().length() < 11 ) continue;
                    if( item->myName().substr( item->myName().length() - 11 ) != "Sf_category" )
                        continue;
                    if( item->myValue() == val->myValue() ) {
                        if( (! comment_set) || every_flag ) {
                            sf->setPreComment( ((*row)[comidx])->myValue() );
                            comment_set = true;
                        }
                        break;
                    }
                } // endfor tags
                if( comment_set && (! every_flag) ) break;
            } // endif sf-level comment
            else {
                for( ASTlist<DataNode *>::iterator tag = tags->begin(); tag != tags->end(); tag++ ) {
                    if( ! (*tag)->isOfType( ASTnode::DATAITEMNODE ) ) continue;
                    item = dynamic_cast<DataItemNode *>( *tag );
                    if( item->myName() == val->myValue() ) {
                        if( (! comment_set) || every_flag ) {
                            item->setPreComment( ((*row)[comidx])->myValue() );
                            comment_set = true;
                        }
                        break;
                    }
                } // endfor tags
                if( comment_set && (! every_flag) ) break;
            } // endif tag-level comment
        } // endfor saveframes
    } // endfor comments
    return true;
}
