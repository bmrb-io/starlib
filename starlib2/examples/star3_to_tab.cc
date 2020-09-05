/*
 * Extract chemical shifts from NMR-STAR 3.1 entry
 *  to TALOS file(s).
 *
 * Input: "-i filename" or stdin
 * Output files are named bmr<Entry_ID>_<Entity_ID>_<Shift_list_ID>.tab
 * in "-o directory" or cwd.
 * 
 * Output files are created for each polypeptide(L) or (D) entity that has
 * assigned chemical shifts (tested). If there are multiple sets of shifts
 * (list IDs), they go into separate files (not tested).
 * 
 * Program will exit with 1: parameter error
 *                        2: input file won't open
 *                        3: parse error on input file
 *                        4: output file won't open
 *
 * Other errors in input file (missing tags) will generate messages on stderr,
 * but exit code will be 0.
 *
 * Note: if this segfaults at argv[0] on large star files, you'll need to 
 * either run "ulimit -s unlimited" first, or ask me to rewrite it using
 * heap memory instead of stack.
 */

#include <iosfwd>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>

#include "portability.h"
#include "ast.h"
#include "parser.h"

//#define DEBUG

StarFileNode * AST;
ostream * os;

const char        AACODES [] =  { 'A',   'R',   'N',   'D',   'C', 
                                  'Q',   'E',   'G',   'H',   'I', 
                                  'L',   'K',   'M',   'F',   'P', 
                                  'S',   'T',   'W',   'Y',   'V' };
const std::string AALABELS [] = { "ALA", "ARG", "ASN", "ASP", "CYS", 
                                  "GLN", "GLU", "GLY", "HIS", "ILE", 
                                  "LEU", "LYS", "MET", "PHE", "PRO", 
                                  "SER", "THR", "TRP", "TYR", "VAL" };

char code( const std::string& label ) {
    if( label == "" ) return '\0';
    for( int i = 0; i < 20; i++ )
        if( label == AALABELS[i] )
            return AACODES[i];
    return 'X';
}

void usage( const char * prog ) {
    std::cout << "Usage: " << prog << "[-i FILE] [-o DIR]" << std::endl;
    std::cout << "    FILE: NMR-STAR 3.1 file, default: stdin" << std::endl;
    std::cout << "    DIR:  output directory,  default: cwd" << std::endl;
}

int main( int argc, char **argv ) {

    std::string infile;
    std::string outdir;
    int opt;

    while( (opt = getopt( argc, argv, "i:o:h?" )) != -1 ) {
        switch( opt ) {
            case 'i' :
                infile = optarg;
                break;
            case 'o' :
                outdir = optarg;
                break;
            case 'h' :
            case '?' :
                usage( argv[0] );
                return 0;
            default :
                usage( argv[0] );
                return 1;
        }
    }
// parse star file
    StarFileNode * star;
    if( infile == "" ) yyin = stdin;
    else {
        if( (yyin = fopen( infile.c_str(), "r" )) == NULL )
            exit( 2 );
    }
    if( yyparse() != 0 ) exit( 3 );
    if( infile != "" ) fclose( yyin );
    star = AST;

// entry id: there may not be one, nor a filename (read from stdin)
    std::string entryid;
    List<ASTnode *> * result = star->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entry.ID" ) );
    if( result->size() > 0 )
        entryid = ((DataItemNode *)((*result)[0]))->myValue();
    if( (entryid == "") || (entryid == ".") || (entryid == "?") )
        entryid = "NO_ENTRY_ID";
#ifdef DEBUG
std::cerr << "Entry ID: " << entryid << std::endl;
#endif

// entities
    std::vector<std::string> entities;
    result = star->searchForTypeByTag( ASTnode::SAVEFRAMENODE, std::string( "_Entity.Sf_category" ) );
    if( result->size() < 1 ) return 0;
    SaveFrameNode * sfn;
    DataItemNode * node;
    int eid = 0;
    for( List<ASTnode*>::iterator i = result->begin(); i != result->end(); i++ ) {
        eid++;
        sfn = dynamic_cast<SaveFrameNode *>( *i );
        List<ASTnode *> * tags = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entity.Polymer_type" ) );
        if( tags->size() != 1 ) {
            std::cerr << "Found " << tags->size() << " _Entity.Polymer_type tags in ";
            std::cerr << sfn->myName() << " saveframe" << std::endl;
            continue;
        }
        node = dynamic_cast<DataItemNode *>( (*tags)[0] );
// need proteins
        if( ! ((node->myValue() == "polypeptide(L)") || (node->myValue() == "polypeptide(D)")) )
            continue;
        std::string entityid;
        tags = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entity.ID" ) );
        if( tags->size() < 1 ) {
            std::ostringstream s;
            s << eid;
            entityid = s.str();
        }
        else {
            node = dynamic_cast<DataItemNode *>( (*tags)[0] );
            entityid = node->myValue();
        }
#ifdef DEBUG
std::cerr << "Entity ID: " << entityid << std::endl;
#endif
        entities.push_back( entityid );
    } // endfor entity saveframes
#ifdef DEBUG
std::cerr << "Entities:" << std::endl;
for( std::vector<std::string>::iterator i = entities.begin(); i != entities.end(); i++ )
    std::cerr << " " << (*i) << std::endl;
#endif
    if( entities.size() < 1 ) return 0;

// shifts
    result = star->searchForTypeByTag( ASTnode::SAVEFRAMENODE, std::string( "_Assigned_chem_shift_list.Sf_category" ) );
    if( result->size() < 1 ) return 0;
    DataLoopNode * loop;
    LoopNameListNode * names;
    LoopTableNode * vals;
    LoopRowNode * row;
    int sid = 0;
    for( List<ASTnode*>::iterator i = result->begin(); i != result->end(); i++ ) {
        int sid_idx = -1;
        int eid_idx = -1;
        int compidx_idx = -1;
        int compid_idx = -1;
        int atomid_idx = -1;
        int val_idx = -1;
        std::map<std::string, std::vector<std::string> > shifts;

        sid++;
        sfn = dynamic_cast<SaveFrameNode *>( *i );
#ifdef DEBUG
std::cerr << "Saveframe: " << sfn->myName() << ", ID" << sid << std::endl;
#endif
        List<ASTnode *> * tags = sfn->searchForTypeByTag( ASTnode::DATALOOPNODE, std::string( "_Atom_chem_shift.ID" ) );
        if( tags->size() < 1 ) continue;
        loop = dynamic_cast<DataLoopNode *>( (*tags)[0] );
        names = loop->getNames()[0];
        vals = loop->getValsPtr();
        row = dynamic_cast<LoopRowNode *>( (*vals)[0] );
        std::string listid;
        sid_idx = names->tagPosition( "_Atom_chem_shift.Assigned_chem_shift_list_ID" );
        if( sid_idx >= 0 ) 
            listid = row->elementAt( sid_idx )->myValue();
        else listid = "1";

        eid_idx = names->tagPosition( "_Atom_chem_shift.Entity_ID" );
        compidx_idx = names->tagPosition( "_Atom_chem_shift.Comp_index_ID" );
        if( compidx_idx < 0 ) {
            std::cerr << "No _Atom_chem_shift.Comp_index_ID tag in " << sfn->myName() << " saveframe" << std::endl;
            continue;
        }
        compid_idx = names->tagPosition( "_Atom_chem_shift.Comp_ID" );
        if( compid_idx < 0 ) {
            std::cerr << "No _Atom_chem_shift.Comp_ID tag in " << sfn->myName() << " saveframe" << std::endl;
            continue;
        }
        atomid_idx = names->tagPosition( "_Atom_chem_shift.Atom_ID" );
        if( atomid_idx < 0 ) {
            std::cerr << "No _Atom_chem_shift.Atom_ID tag in " << sfn->myName() << " saveframe" << std::endl;
            continue;
        }
        val_idx = names->tagPosition( "_Atom_chem_shift.Val" );
        if( val_idx < 0 ) {
            std::cerr << "No _Atom_chem_shift.Val tag in " << sfn->myName() << " saveframe" << std::endl;
            continue;
        }

// need to collect all residue labels
        std::string seq;
        for( ASTlist<LoopRowNode *>::iterator j = vals->begin(); j != vals->end(); j++ ) {
            row = dynamic_cast<LoopRowNode *>( *j );
            std::string eid;
            if( eid_idx >= 0) eid = row->elementAt( eid_idx )->myValue();
            if( ( eid == "") || (eid == ".") || (eid == "?") )
                eid = "1";
// check if protein
            bool found = false;
            for( std::vector<std::string>::iterator k = entities.begin(); k != entities.end(); k++ ) {
                if( (*k) == eid ) {
                    found = true;
                    break;
                }
            }
            if( ! found ) continue;

            if( shifts.find( eid ) == shifts.end() ) {
                std::vector<std::string> rows;
                shifts[eid] = rows;
            }
            std::ostringstream s;
            s.width( 4 );
            s << row->elementAt( compidx_idx )->myValue() << " ";
            s.width( 1 );
            s << code( row->elementAt( compid_idx )->myValue() ) << " ";
            if( seq == "" ) seq += code( row->elementAt( compid_idx )->myValue() );
            else { 
                if( seq.at( seq.length() - 1 ) != code( row->elementAt( compid_idx )->myValue() ) )
                    seq += code( row->elementAt( compid_idx )->myValue() );
            }
            s.width( 4 );
            s << row->elementAt( atomid_idx )->myValue() << " ";
//            s.setf( std::ios::fixed );
//            s.presision( 3 );
            s.width( 8 );
            s << row->elementAt( val_idx )->myValue();
            shifts[eid].push_back( s.str() );
        } // endfor rows
#ifdef DEBUG
std::cerr << seq << std::endl;
#endif

// open files
        for( std::map<std::string, std::vector<std::string> >::iterator j = shifts.begin(); j != shifts.end(); j++ ) {
            if( j->second.size() > 0 ) {
                std::string filename = "bmr" + entryid + "_" + j->first + "_" + listid + ".tab";
#ifdef DEBUG
std::cerr << "outfile: " << filename << std::endl;
for( std::vector<std::string>::iterator k = j->second.begin(); k != j->second.end(); k++ )
std::cerr << *k << std::endl;
#endif
                std::ofstream out( filename.c_str() );
                if( ! out.is_open() )
                    exit( 4 );
                out << "REMARK BMRB Entry_ID " << entryid << " Entity_ID " << j->first << " Assigned_chem_shift_list_ID " << listid << std::endl;
                out << std::endl;
                out << "DATA SEQUENCE ";
                for( int k = 0; k < seq.length(); k++ ) {
                    if( (k != 0) && (k % 10 == 0) )
                        out << " ";
                    if( (k != 0) && (k % 80 == 0) )
                        out << std::endl << "DATA SEQUENCE ";
                    out << seq.at( k );
                }
                out << std::endl << std::endl;
                out << "VARS   RESID RESNAME ATOMNAME SHIFT" << std::endl;
                out << "FORMAT %4d %1s %4s %8.3f" << std::endl;
                out << std::endl;
                for( std::vector<std::string>::iterator k = j->second.begin(); k != j->second.end(); k++ )
                    out << *k << std::endl;
                out.close();
            }
        }
    } // endfor shift saveframes

    return 0;
}