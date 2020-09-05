/**
 * Use with caution.
 */
#include <iosfwd>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include "portability.h"
#include "ast.h"
#include "parser.h"

StarFileNode * AST;
ostream * os;

static void usage( const char * progname ) {
    std::cout << "Usage: " << progname << " -i <input file> -o <output file> -t <tag to delete>" << std::endl;
}

int main( int argc, char **argv ) {

    std::string infile;
    std::string outfile;
    std::string tag;
    int opt;
    while( (opt = getopt( argc, argv, "i:o:t:" )) != -1 ) {
        switch( opt ) {
            case 'i' : 
                infile = optarg;
                break;
            case 'o' : // output
                outfile = optarg;
                break;
            case 't' : 
                tag = optarg;
                break;
            default :
                usage( argv[0] );
                exit( 1 );
        }
    }
    if( tag == "" ) {
        std::cerr << "Missing parameter!" << std::endl;
        usage( argv[0] );
        exit( 1 );
    }
// parse star file
    StarFileNode * star;
    if( infile == "" ) yyin = stdin;
    else { 
        if( (yyin = fopen( infile.c_str(), "r" )) == NULL )
            exit( 2 );
    }
    if( yyparse() != 0 )
        exit( 3 );
    fclose( yyin );
    star = AST;

// delete loop column
    List<ASTnode *> * loops = star->searchForTypeByTag( ASTnode::DATALOOPNODE, tag );
    if( loops->size() < 1 ) {
        std::cerr << "found 0 loops with tag " << tag << std::endl;
        exit( 4 );
    }
    for( List<ASTnode *>::iterator i = loops->begin(); i != loops->end(); i++ ) {
        DataLoopNode *loop = (DataLoopNode *)(*i);
        loop->RemoveColumn( tag );
    }

// unparse entry
    std::ofstream out;
    if( outfile == "" ) os = &std::cout;
    else {
        out.open( outfile.c_str() );
        if( ! out.is_open() ) {
            std::cerr << "Cannot open " << outfile << " for writing" << std::endl;
            exit( 8 );
        }
        os = &out;
    }
    star->Unparse( 0 );
    if( out.is_open() ) out.close();
    return 0;
}
