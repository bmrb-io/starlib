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

int main( int argc, char **argv ) {

    std::string infile;
    std::string outfile;
    int opt;
    while( (opt = getopt( argc, argv, "i:o:" )) != -1 ) {
        switch( opt ) {
            case 'i' : 
                infile = optarg;
                break;
            case 'o' : // output
                outfile = optarg;
                break;
        }
    }
    if( infile == "" ) {
	std::cerr << "Missing parameter!" << std::endl;
	exit( 1 );
    }
// parse star file
    StarFileNode * star;
    if( (yyin = fopen( infile.c_str(), "r" )) == NULL )
        exit( 2 );
    if( yyparse() != 0 )
        exit( 3 );
    fclose( yyin );
    star = AST;

// delete loop columns    
    List<ASTnode *> * loops = star->searchForTypeByTag( ASTnode::DATALOOPNODE,
        std::string( "_Chem_comp_bond.PDB_atom_ID_1" ) );
    if( loops->size() < 1 ) {
        std::cerr << "found 0 loops" << std::endl;
//        exit( 4 );
    }
    for( List<ASTnode *>::iterator i = loops->begin(); i != loops->end(); i++ ) {
        DataLoopNode *loop = (DataLoopNode *)(*i);
        loop->RemoveColumn( std::string( "_Chem_comp_bond.PDB_atom_ID_1" ) );
        loop->RemoveColumn( std::string( "_Chem_comp_bond.PDB_atom_ID_2" ) );
    }

    loops = star->searchForTypeByTag( ASTnode::DATALOOPNODE,
        std::string( "_Chem_comp_descriptor.Ordinal" ) );
    if( loops->size() < 1 ) {
        std::cerr << "found 0 cc_descr loops" << std::endl;
//        exit( 4 );
    }
    for( List<ASTnode *>::iterator i = loops->begin(); i != loops->end(); i++ ) {
        DataLoopNode *loop = (DataLoopNode *)(*i);
        loop->RemoveColumn( std::string( "_Chem_comp_descriptor.Ordinal" ) );
    }

    loops = star->searchForTypeByTag( ASTnode::DATALOOPNODE,
        std::string( "_Chem_comp_identifier.Ordinal" ) );
    if( loops->size() < 1 ) {
        std::cerr << "found 0 cc_ident loops" << std::endl;
//        exit( 4 );
    }
    for( List<ASTnode *>::iterator i = loops->begin(); i != loops->end(); i++ ) {
        DataLoopNode *loop = (DataLoopNode *)(*i);
        loop->RemoveColumn( std::string( "_Chem_comp_identifier.Ordinal" ) );
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
