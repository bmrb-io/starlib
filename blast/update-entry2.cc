/**
 * Update NMR-STAR 2.1 file w/ BLAST search results.
 * 
 * Update query date if BLAST query ran for the entity.
 * Update revision date if loop is updated.
 * Update loop: assume the hits are sorted in the same order
 * (nobody else edited the loop between BLAST runs). Then,
 * if the number of hits changed, or if DB code or accession numbers
 * don't match in any row of the 2 loops,
 * replace old loop with the new one.
 *
 * Query date, revision date, and the loop are the last elements in
 * entity saveframe (in that order). If they don't exist, query date
 * is always inserted, the other 2 are inserted if there are hits.
 */
#include <iosfwd>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <vector>
#include <time.h>

#include "BlastParser.h"
#include "HitList.h"

#include "portability.h"
#include "ast.h"
#include "parser.h"

StarFileNode * AST;
ostream * os;

void usage( const char * progname ) {
//    "ab:i:c:o:l:s:"
    std::cerr << "Usage: " << progname << " -i starfile -b blastfile -c commentfile -o outfile";
    std::cerr << " [-l length cutoff] [-s score cutoff] [-a]" << std::endl;
    std::cerr << " -a: keep all hits from all databases" << std::endl;
}
/**************************************************************
 * entity saveframe names, actually, since we had no IDs in 2.1
 */
bool get_entity_ids( std::vector<std::string> & eids, StarFileNode * entry ) {
    List<ASTnode *> * result = entry->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE,
            std::string( "_Saveframe_category" ), std::string( "monomeric_polymer" ) );
    if( result->size() < 1 ) return false;
    SaveFrameNode * frame;
    for( List<ASTnode*>::iterator i = result->begin(); i != result->end(); i++ ) {
        frame = (SaveFrameNode *)(*i);
        eids.push_back( frame->myName().substr( 5 ) );
    }
    return true;
}

/**************************************************************
 * Don't update the date as of 2011-02-01 -- set value to "."
 * This way the entry only changes if the list of hits changes.
 */
bool update_query_date( StarFileNode * entry, const std::string & name ) {
    std::string str = "save_" + name;
    List<ASTnode *> * frames = entry->searchForTypeByTag( ASTnode::SAVEFRAMENODE, str );
    if( frames->size() < 1 ) {
        delete frames;
        return false;
    }
    bool rc = true;
    SaveFrameNode * sfn = (SaveFrameNode *)((*frames)[0]);
    List<ASTnode *> * tags = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Sequence_homology_query_date" ) );
//    time_t tp = time( NULL );
//    struct tm * ts = localtime( &tp );
//    char now[11];
//    now[10] = 0;
//    strftime( now, 11, "%Y-%m-%d", ts );
    if( tags->size() == 1 )
        ((DataItemNode *)(*tags)[0])->setValue( "." );
    else {
        std::cerr << tags->size() << "_Sequence_homology_query_date tags in " << sfn->myName() <<  std::endl;
        rc = false;
    }
    delete frames;
    delete tags;
    return rc;
}

/**************************************************************
 *
 * Returns list of hits to deposit in 1st parameter
 * Source hit list (2nd parameter) contains all hits for the entry, possibly > 1 entities
 * In addition, we keep only 5 hits for databases other than BMRB and PDB, unless keep_all
 * is true
 */
void trim_blast_hits( std::vector<HitDef> & result, HitList & hits, const std::string & eid, bool keep_all = false ) {
    std::string db;
    int count = 0;
    for( std::set<HitDef>::iterator i = hits.getHits().begin(); i != hits.getHits().end(); i++ ) {
        if( i->queryDbSubid() != eid ) continue;
// rules: keep all BMRB & PDB hits, top 5 of every other DB (hits are already sorted)
        if( (i->hitDbName() == "BMRB") || (i->hitDbName() == "PDB") )
            result.push_back( *i );
        else {
            if( (db == "") || (db != i->hitDbName()) ) {
                db = i->hitDbName();
                result.push_back( *i );
                count = 1;
            }
            else {
                if( keep_all || (count < 5) ) {
                    result.push_back( *i );
                    count++;
                }
            }
        } //endif bmrb or pdb
    } //endfor
}

/**************************************************************
 *
 * updates entity saveframe:
 *  revised date and hit list are updated only if hit list changed (in which case
 *  old list is discarded and the new one inserted),
 * 2011-02-01: query date is set to "."
 */
bool update_savefame( StarFileNode * entry, const std::string & name, const std::vector<HitDef> & hits ) {
    std::string str = "save_" + name;
    List<ASTnode *> * result = entry->searchForTypeByTag( ASTnode::SAVEFRAMENODE, str );
    if( result->size() != 1 ) {
        delete result;
        return false;
    }
    SaveFrameNode * sfn = (SaveFrameNode *)((*result)[0]);
    time_t tp = time( NULL );
    struct tm *ts = localtime( &tp );
    char now[11];
    now[10] = 0;
    strftime( now, 11, "%Y-%m-%d", ts );
#ifdef DEBUG
std::cerr << "Today is " << now << std::endl;
#endif
//
// last elements in entity sf are query date, revised date, and db link loop, in that order
//
    std::ostringstream strbuf;
// 2011-02-01 don't update query date, set to null to overwrite old value.
    DataItemNode * qdate = new DataItemNode( "_Sequence_homology_query_date", "." );
    DataItemNode * rdate = new DataItemNode( "_Sequence_homology_query_revised_last_date", now );
    DataLoopNode * loop = new DataLoopNode( "tabulate" );
    DataLoopNameListNode * namelist = loop->getNamesPtr();
    LoopNameListNode * names = new LoopNameListNode();
    namelist->insert( namelist->end(), names );
    names->insert( names->end(), new DataNameNode( "_Database_name" ) );
    names->insert( names->end(), new DataNameNode( "_Database_accession_code" ) );
    names->insert( names->end(), new DataNameNode( "_Database_entry_mol_name" ) );
    names->insert( names->end(), new DataNameNode( "_Sequence_query_to_submitted_percentage" ) );
    names->insert( names->end(), new DataNameNode( "_Sequence_subject_length" ) );
    names->insert( names->end(), new DataNameNode( "_Sequence_identity" ) );
    names->insert( names->end(), new DataNameNode( "_Sequence_positive" ) );
    names->insert( names->end(), new DataNameNode( "_Sequence_homology_expectation_value" ) );
    LoopTableNode * vals = loop->getValsPtr();
    for( std::vector<HitDef>::const_iterator i = hits.begin(); i != hits.end(); i++ ) {
        LoopRowNode * row = new LoopRowNode( true );
        row->insert( row->end(), new DataValueNode( i->hitDbName(), DataValueNode::findAppropriateDelimiterType( i->hitDbName() ) ) );
        row->insert( row->end(), new DataValueNode( i->hitDbId(), DataValueNode::findAppropriateDelimiterType( i->hitDbId() ) ) );
        if( i->hitDescription() == "" ) row->insert( row->end(), new DataValueNode( "." ) );
        else row->insert( row->end(), new DataValueNode( i->hitDescription(), DataValueNode::findAppropriateDelimiterType( i->hitDescription() ) ) );
        strbuf.str( "" );
        strbuf << std::fixed << std::setprecision( 2 ) << i->queryToSubPercent();
        row->insert( row->end(), new DataValueNode( strbuf.str() ) );
        strbuf.str( "" );
        strbuf << std::dec << i->hitLength();
        row->insert( row->end(), new DataValueNode( strbuf.str() ) );
        strbuf.str( "" );
        strbuf << std::fixed << std::setprecision( 2 ) << i->pctIdentities();
        row->insert( row->end(), new DataValueNode( strbuf.str() ) );
        strbuf.str( "" );
        strbuf << std::fixed << std::setprecision( 2 ) << i->pctPositives();
        row->insert( row->end(), new DataValueNode( strbuf.str() ) );
        strbuf.str( "" );
        strbuf << std::scientific << std::setprecision( 2 ) << i->expVal();
        row->insert( row->end(), new DataValueNode( strbuf.str() ) );
        vals->insert( vals->end(), row );
    }
#ifdef DEBUG
    std::cerr << "New loop:" << std::endl;
    os = &std::cout;
    loop->Unparse( 0 );
#endif
// figure out if we need to update it
    bool update = false;
    result = sfn->searchForTypeByTag( ASTnode::DATALOOPNODE, std::string( "_Database_name" ) );
    if( result->size() < 1 )
        update = true;
    else {
        DataLoopNode * oldloop = (DataLoopNode *) ((* result)[0]);
        LoopTableNode * oldvals = oldloop->getValsPtr();
        if( oldvals->size() != hits.size() )
            update = true;
        else {
// compare hits. both lists are sorted the same way
            DataValueNode * val;
            int j = 0;
            for( std::vector<HitDef>::const_iterator i = hits.begin(); i != hits.end(); i++, j++ ) {
                val = (*(*oldvals)[j])[0];
// cmp dbname
                if( val->myValue() != i->hitDbName() ) {
#ifdef DEBUG
std::cerr << "diff. DB name: " << val->myValue() << " vs " << i->hitDbName() << std::endl;
#endif
                    update = true;
                    break;
                }
                val = (*(*oldvals)[j])[1];
// cmp dbid
                if( val->myValue() != i->hitDbId() ) {
#ifdef DEBUG
std::cerr << "diff. DB ID: " << val->myValue() << " vs " << i->hitDbId() << std::endl;
#endif
                    update = true;
                    break;
                }
                val = (*(*oldvals)[j])[2];
// cmp description
                if( val->myValue() != i->hitDescription() ) {
#ifdef DEBUG
std::cerr << "diff. Descr: " << val->myValue() << " vs " << i->hitDescription() << std::endl;
#endif
                    update = true;
                    break;
                }
            } //endfor
        } // endif old.size == new.size
        if( update ) {
#ifdef DEBUG
std::cerr << "*** Updating loop" << std::endl;
#endif
// delete old loop
            for( result->Reset(); ! result->AtEnd(); result->Next() )
                delete result->Current();
        }
    }
    delete result;
// and free tags
    if( update ) {
        result = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Sequence_homology_query_date" ) );
        if( result->size() > 0 ) {
            for( result->Reset(); ! result->AtEnd(); result->Next() )
                delete result->Current();
        }
        delete result;
        result = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Sequence_homology_query_revised_last_date" ) );
        if( result->size() > 0 ) {
            for( result->Reset(); ! result->AtEnd(); result->Next() )
                delete result->Current();
        }
        delete result;
// update
        sfn->GiveMyDataList()->insert( sfn->GiveMyDataList()->end(), qdate );
        sfn->GiveMyDataList()->insert( sfn->GiveMyDataList()->end(), rdate );
        sfn->GiveMyDataList()->insert( sfn->GiveMyDataList()->end(), loop );
        return true;
    }
#ifdef DEBUG
    else
    std::cerr << "*** Not updating entry" << std::endl;
#endif
    return false;
}




int main( int argc, char **argv ) {

    std::string infile;
    std::string xmlfile;
    std::string commentfile;
    std::string outfile;
    float ident_cutoff = HitList::IDENT_CUTOFF;
    float qsub_cutoff = HitList::QSUB_CUTOFF;
    bool keep_all = false;
    
    int opt;
    while( (opt = getopt( argc, argv, "ab:i:c:o:l:s:" )) != -1 ) {
	switch( opt ) {
	    case 'a' :
	        keep_all = true;
	        break;
	    case 'b' : 
	    	xmlfile = optarg;
	    	break;
	    case 'i' : 
	    	infile = optarg;
	    	break;
	    case 'c' : 
	    	commentfile = optarg;
	    	break;
	    case 'o' : 
	    	outfile = optarg;
	    	break;
	    case 's' : {
	    	std::istringstream s( optarg );
	    	if( ! (s >> ident_cutoff))
	    		ident_cutoff = HitList::IDENT_CUTOFF;
	    	}
	    	break;
	    case 'l' : { 
	    	std::istringstream t( optarg );
	    	if( ! (t >> qsub_cutoff))
	    		qsub_cutoff = HitList::QSUB_CUTOFF;
	    	}
	    	break;
	    default :
	        usage( argv[0] );
	    	exit( 1 );
	}
    }
    if( (infile == "") || (xmlfile == "") ) {
    	std::cerr << "Missing parameter" << std::endl;
	usage( argv[0] );
    	exit( 2 );
    }
#ifdef DEBUG
std::cerr << "* Keep: " << (keep_all ? "all" : "5" ) << " hits w/ ident score > " << ident_cutoff << " and ql > " << qsub_cutoff << std::endl;
#endif
// parse blast result
    HitList hits;
    if( ident_cutoff > 0.0F ) hits.setIdentCutoff( ident_cutoff ); 
    if( qsub_cutoff > 0.0F ) hits.setQsubCutoff( qsub_cutoff ); 
    BlastParser p( &hits );
    if( ! p.parse( xmlfile ) )
    	exit( 3 );

#ifdef DEBUG
std::cerr << "** Hits:" << std::endl << hits << std::endl;
#endif
// parse star file
    StarFileNode * star;
    if( (yyin = fopen( infile.c_str(), "r" )) == NULL ) 
    	exit( 4 );
    if( yyparse() != 0 )
    	exit( 5 );
    fclose( yyin );
    star = AST;
// get list of hits from the entry
    std::vector<std::string> file_entities;
    if( ! get_entity_ids( file_entities, star ) ) {
    	std::cerr << "No entities in " << infile << std::endl;
    	delete star;
    	exit( 8 );
    }
#ifdef DEBUG
std::cerr << "Found " << file_entities.size() << " entity ids in the entry" << std::endl;
#endif
// get list of hits from blast result
    std::vector<std::string> blast_entities;
    hits.getEntityIds( blast_entities );
    int eid = 0;
// file_entities is a list of saveframe names. Just count them starting from 1 to get entity IDs
    for( std::vector<std::string>::iterator fe = file_entities.begin(); fe != file_entities.end(); fe++ ) {
    	eid++;
    	ostringstream s;
    	s << eid;
        if( blast_entities.size() < 1 ) {
#ifdef DEBUG
std::cerr << "No blast hits, checkin ID" << std::endl;
#endif
//            if( hits.eidWasQueried( s.str() ) ) 
//                BmrbEntry::update_query_date2( star, *fe );
        }
        else {
#ifdef DEBUG
std::cerr << "Got entity ids from blast, looking for " << s.str() << std::endl;
#endif
            for( std::vector<std::string>::iterator be = blast_entities.begin(); be != blast_entities.end(); be++ ) {
#ifdef DEBUG
std::cerr << "* checking " << (*be) << std::endl;
#endif
    		if( s.str() == (*be) ) {
#ifdef DEBUG
std::cerr << "* matches " << (*be) << std::endl;
#endif
    		    std::vector<HitDef> entity_hits;
    		    trim_blast_hits( entity_hits, hits, s.str(), keep_all );
#ifdef DEBUG
std::cerr << entity_hits.size() << " hits for entity " << *fe << std::endl;
#endif
    		    if( entity_hits.size() > 0 ) {
#ifdef DEBUG
std::cerr << "Updating saveframe (main)" << std::endl;
#endif
// returns false if nothing got updated. Overwrite query date with null.
    			if( ! update_savefame( star, *fe, entity_hits ) )
                            update_query_date( star, *fe );
#ifdef DEBUG
std::cerr << "Done updating (main)" << std::endl;
#endif
    		    }
    		} // endif str == be
    	    } // endfor blast entities
    	} // endif got hits
    } // endfor file entities

// parse comment file
    StarFileNode * comment = NULL;
    if( commentfile != "" ) {
    	if( (yyin = fopen( commentfile.c_str(), "r" )) == NULL ) 
    		exit( 6 );
       star_syntax_print = false; // An extern global declared by the parser.
                                  // Suppresses the parser's own error messages if false.
    	if( yyparse() != 0 )
    		exit( 7 );
    	fclose( yyin );
    	comment = AST;
    }
    if( comment != NULL ) {
    	formatNMRSTAR( star, comment );
    	delete comment;
    }

// unparse entry
    std::ofstream out;
    if( outfile == "" ) os = &std::cout;
    else {
        out.open( outfile.c_str() );
        if( ! out.is_open() ) {
        	std::cerr << "Cannot open " << outfile << " for writing" << std::endl;
        	exit( 9 );
        }
        os = &out;
    }
    star->Unparse( 0 );
    if( out.is_open() ) out.close();
    delete star;
    return 0;
}
