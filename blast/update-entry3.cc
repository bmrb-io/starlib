/**
 * Update NMR-STAR 3.1 file w/ BLAST search results.
 *
 * Update query date if BLAST query ran for the entity.
 * Update revision date if loop is updated.
 * Update loop: keep author-suppled rows 
 *      (where "author supplied" is not "no").
 * Assume the hits are sorted in the same order (nobody else edited
 * the loop between BLAST runs). Then, if the number of BlAST hits 
 * changed, or if DB code or accession numbers don't match in any row
 * of the 2 loops, replace old rows with the new ones.
 *
 * If query date doesn't exist, it is always inserted.
 * If revision date and the loop don't exist, they are inserted if
 * there are hits.
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
#include <map>
#include <utility>

#include "BlastParser.h"
#include "HitList.h"

#include "portability.h"
#include "ast.h"
#include "parser.h"

StarFileNode * AST;
ostream * os;

bool formatNMRSTAR3( StarFileNode *, StarFileNode * );

void usage( const char * progname ) {
//    "ab:i:c:o:l:s:"
    std::cerr << "Usage: " << progname << " -i starfile -b blastfile -c commentfile -o outfile";
    std::cerr << " [-l length cutoff] [-s score cutoff] [-a]" << std::endl;
    std::cerr << " -a: keep all hits from all databases" << std::endl;
}

/**************************************************************
 * 
 */
bool get_entity_ids( std::vector<std::string> & eids, StarFileNode * entry ) {
    List<ASTnode *> * result = entry->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entity.ID" ) );
    if( result->size() < 1 ) return false;
    for( List<ASTnode*>::iterator i = result->begin(); i != result->end(); i++ )
        eids.push_back( ((DataItemNode *)(*i))->myValue() );
    return true;
}
/**************************************************************
 * Don't update the date as of 2011-02-01 -- set value to "."
 * This way the entry only changes if the list of hits changes.
 */
bool update_query_date( StarFileNode * entry, const std::string & eid ) {
    List<ASTnode *> * frames = entry->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, 
            std::string( "_Entity.ID" ), eid );
    if( frames->size() < 1 ) {
        delete frames;
        return false;
    }
    bool rc = true;
    SaveFrameNode * sfn = (SaveFrameNode *)((*frames)[0]);
    List<ASTnode *> * tags = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entity.DB_query_date" ) );
    if( tags->size() == 1 )
        ((DataItemNode *)(*tags)[0])->setValue( "." );
    else {
        std::cerr << tags->size() << "_Entity.DB_query_date " << sfn->myName() <<  std::endl;
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
bool update_savefame( StarFileNode * entry, const std::string & eid, const std::vector<HitDef> & hits ) {
    List<ASTnode *> * result = entry->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, 
            std::string( "_Entity.ID" ), eid );
    if( result->size() != 1 ) {
        delete result;
        return false;
    }

// _Entity_db_link is the 1st loop after free tags
// Last free tags are query date and revised date
// (hope Eldon doesn't change that)

    SaveFrameNode * sfn = (SaveFrameNode *)((*result)[0]);
    bool update = false;
    std::vector<std::map<std::string, std::string> > savehits;
    result = sfn->searchForTypeByTag( ASTnode::DATALOOPNODE, std::string( "_Entity_db_link.Author_supplied" ) );
    if( result->size() > 0 ) {
        DataLoopNode * oldloop = (DataLoopNode *)((* result)[0]);

// author-provided
        List<ASTnode *> * author_hits = oldloop->searchForTypeByTagValue( ASTnode::LOOPROWNODE, std::string( "_Entity_db_link.Author_supplied" ), std::string( "yes" ) );
#ifdef DEBUG
std::cerr << "Check for author-supplied rows: " << author_hits->size() << std::endl;
#endif
        if( author_hits->size() > 0 ) {
// put row in a map
            DataLoopNameListNode * oldnamelist = oldloop->getNamesPtr();
            LoopNameListNode * oldnames = (* oldnamelist)[0];
            for( ASTlist<ASTnode*>::iterator i = author_hits->begin(); i != author_hits->end(); i++ ) {
                std::map<std::string, std::string> saverow;
                LoopRowNode * row = (LoopRowNode *)(* i);
                LoopNameListNode::iterator n = oldnames->begin();
                for( LoopRowNode::iterator v = row->begin(); v != row->end(); v++, n++ ) {
                    DataNameNode * tag_node = (DataNameNode *)(* n);
                    DataValueNode * val_node = (DataValueNode *)(* v);                    
                    saverow.insert( std::pair<std::string, std::string>( tag_node->myName(), val_node->myValue() ) );
                }
                savehits.push_back( saverow ); 
            }
        }
#ifdef DEBUG
for( std::vector<std::map<std::string, std::string> >::iterator i = savehits.begin(); i != savehits.end(); i++ )
    for( std::map<std::string, std::string>::iterator j = i->begin(); j != i->end(); j++ )
        std::cerr << "author-supplied tag " << j->first << " value " << j->second << std::endl;
#endif
//
// for now, just update entries with author-supplied rows always. Some day we'll move this to a DB, then I'll fix it.
//
// see if it needs updating
        LoopTableNode * oldvals = oldloop->getValsPtr();
        if( oldvals->size() != hits.size() )
            update = true;
        else {
// compare hits. both lists are sorted the same way
            DataValueNode * val;
            int j = 0;
            for( std::vector<HitDef>::const_iterator i = hits.begin(); i != hits.end(); i++, j++ ) {
// cmp dbname
                val = (*(*oldvals)[j])[1];
                if( val->myValue() != i->hitDbName() ) {
#ifdef DEBUG
std::cerr << "diff. DB name: " << val->myValue() << " vs " << i->hitDbName() << std::endl;
#endif
                    update = true;
                    break;
                }
// cmp dbid
                val = (*(*oldvals)[j])[2];
                if( val->myValue() != i->hitDbId() ) {
#ifdef DEBUG
std::cerr << "diff. DB ID: " << val->myValue() << " vs " << i->hitDbId() << std::endl;
#endif
                    update = true;
                    break;
                }
// cmp description (want this to replace descriptions > 127 chars - 2011-02-01)
                val = (*(*oldvals)[j])[4];
                if( val->myValue() != i->hitDescription() ) {
#ifdef DEBUG
std::cerr << "diff. Descr: |" << val->myValue() << "| vs |" << i->hitDescription() << "|" << std::endl;
#endif
                    update = true;
                    break;
                }
            } //endfor
        }
        if( update ) {
#ifdef DEBUG
std::cerr << "*** Will update list" << std::endl;
#endif
// delete old loop -- all 1 of them
            for( result->Reset(); ! result->AtEnd(); result->Next() )
                delete result->Current();
        }
    } //endif result.size() > 0 (has db-link loop)
    else update = true; // no db-link loop (new entry)
//
    if( update ) {
        time_t tp = time( NULL );
        struct tm *ts = localtime( &tp );
        char now[11];
        now[10] = 0;
        strftime( now, 11, "%Y-%m-%d", ts );
#ifdef DEBUG
std::cerr << "Today is " << now << std::endl;
#endif
// 2011-02-01 don't update query date, set to null to overwrite old value.
        DataItemNode * qdate = new DataItemNode( "_Entity.DB_query_date", "." );
        DataItemNode * rdate = new DataItemNode( "_Entity.DB_query_revised_last_date", now );
        result = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entity.DB_query_revised_last_date" ) );
        if( result->size() > 0 ) {
            for( result->Reset(); ! result->AtEnd(); result->Next() )
                delete result->Current();
        }
        delete result;
        result = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entity.DB_query_date" ) );
        if( result->size() > 0 ) {
            for( result->Reset(); ! result->AtEnd(); result->Next() )
                delete result->Current();
        }
        delete result;

// construct new loop
        result = entry->searchForTypeByTag( ASTnode::DATAITEMNODE, std::string( "_Entry.ID" ) );
        if( result->size() != 1 ) {
            std::cerr << "Can't find entry ID!" << std::endl;
            exit( 10 );
        }
        DataItemNode * item = (DataItemNode *)((*result)[0]);
        std::string entryid = item->myValue();

        int NUMTAGS = 22;
        std::string TAGS[] = {
            "_Entity_db_link.Ordinal",                        //  0
            "_Entity_db_link.Author_supplied",                //  1
            "_Entity_db_link.Database_code",                  //  2
            "_Entity_db_link.Accession_code",                 //  3
            "_Entity_db_link.Entry_mol_code",                 //  4
            "_Entity_db_link.Entry_mol_name",                 //  5
            "_Entity_db_link.Entry_experimental_method",      //  6
            "_Entity_db_link.Entry_structure_resolution",     //  7
            "_Entity_db_link.Entry_relation_type",            //  8
            "_Entity_db_link.Entry_details",                  //  9
            "_Entity_db_link.Chimera_segment_ID",             // 10
            "_Entity_db_link.Seq_query_to_submitted_percent", // 11
            "_Entity_db_link.Seq_subject_length",             // 12
            "_Entity_db_link.Seq_identity",                   // 13
            "_Entity_db_link.Seq_positive",                   // 14
            "_Entity_db_link.Seq_homology_expectation_val",   // 15
            "_Entity_db_link.Seq_align_begin",                // 16
            "_Entity_db_link.Seq_align_end",                  // 17
            "_Entity_db_link.Seq_difference_details",         // 18
            "_Entity_db_link.Seq_alignment_details",          // 19
            "_Entity_db_link.Entry_ID",                       // 20
            "_Entity_db_link.Entity_ID",                      // 21
        };

        std::ostringstream strbuf;
        DataLoopNode * loop = new DataLoopNode( "tabulate" );
        DataLoopNameListNode * namelist = loop->getNamesPtr();
        LoopNameListNode * names = new LoopNameListNode();
        namelist->insert( namelist->end(), names );

        for( int j = 0; j < NUMTAGS; j++ )
            names->insert( names->end(), new DataNameNode( TAGS[j] ) );

        LoopTableNode * vals = loop->getValsPtr();

        int ordinal = 0;
// author-provided
        if( savehits.size() > 0 ) {
            for( std::vector<std::map<std::string, std::string> >::iterator i = savehits.begin(); i != savehits.end(); i++ ) {
                LoopRowNode * row = new LoopRowNode( true );

                ordinal++;
                strbuf.str( "" );
                strbuf << ordinal;
                row->insert( row->end(), new DataValueNode( strbuf.str() ) );

                for( int j = 1; j < NUMTAGS; j++ ) {
                    std::map<std::string, std::string>::iterator val = i->find( TAGS[j] );
                    if( val == i->end() ) row->insert( row->end(), new DataValueNode( "." ) );
                    else row->insert( row->end(), new DataValueNode( val->second, DataValueNode::findAppropriateDelimiterType( val->second ) ) );
                }
                vals->insert( vals->end(), row );
            }
        }

// blast
        for( std::vector<HitDef>::const_iterator i = hits.begin(); i != hits.end(); i++ ) {
            LoopRowNode * row = new LoopRowNode( true );

            ordinal++;
            strbuf.str( "" );
            strbuf << ordinal;
            row->insert( row->end(), new DataValueNode( strbuf.str() ) );

            row->insert( row->end(), new DataValueNode( "no" ) );
            row->insert( row->end(), new DataValueNode( i->hitDbName(), DataValueNode::findAppropriateDelimiterType( i->hitDbName() ) ) );
            row->insert( row->end(), new DataValueNode( i->hitDbId(), DataValueNode::findAppropriateDelimiterType( i->hitDbId() ) ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            if( i->hitDescription() == "" ) row->insert( row->end(), new DataValueNode( "." ) );
            else row->insert( row->end(), new DataValueNode( i->hitDescription(), DataValueNode::findAppropriateDelimiterType( i->hitDescription() ) ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( "." ) );
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
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( "." ) );
            row->insert( row->end(), new DataValueNode( entryid ) );
            row->insert( row->end(), new DataValueNode( eid ) );            
            vals->insert( vals->end(), row );
        }
#ifdef DEBUG
std::cerr << "New loop:" << std::endl;
os = &std::cout;
loop->Unparse( 0 );
#endif
// insert new nodes (backwards) before the 1st loop
        DataNode * node;
        ASTlist<DataNode *>  * nodes = sfn->GiveMyDataList();
        for( ASTlist<DataNode*>::iterator i = nodes->begin(); i != nodes->end(); i++ ) {
            node = (DataNode *)(* i);
            if( node->isOfType( ASTnode::DATALOOPNODE ) ) {
                nodes->insert( i, loop );
                nodes->insert( i, rdate );
                nodes->insert( i, qdate );
                break;
            }
        }
    }

    return true;
}

/************************************************************************************************/
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
                	if( ! (s >> ident_cutoff)) ident_cutoff = HitList::IDENT_CUTOFF;
                }
                break;
            case 'l' : {
                	std::istringstream s( optarg );
                	if( ! (s >> qsub_cutoff)) qsub_cutoff = HitList::QSUB_CUTOFF;
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
// parse comment file
    StarFileNode * comment = NULL;
    if( commentfile != "" ) {
        if( (yyin = fopen( commentfile.c_str(), "r" )) == NULL )
                exit( 6 );
        if( yyparse() != 0 )
                exit( 7 );
        fclose( yyin );
        comment = AST;
    }
    if( comment != NULL ) {
        formatNMRSTAR3( star, comment );
        delete comment;
    }
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
// file_entities is a list of entity IDs.    
    for( std::vector<std::string>::iterator fe = file_entities.begin(); fe != file_entities.end(); fe++ ) {
// this is just so we don't update query date twice
        if( blast_entities.size() <  1 ) {
#ifdef DEBUG
std::cerr << "No blast hits, checkin ID" << std::endl;
#endif
            if( hits.eidWasQueried( *fe ) )
                update_query_date( star, *fe );
        }
        else {
#ifdef DEBUG
std::cerr << "Got entity ids from blast, looking for " << (*fe) << std::endl;
#endif
            for( std::vector<std::string>::iterator be = blast_entities.begin(); be != blast_entities.end(); be++ ) {
                if( (*fe) == (*be) ) {
#ifdef DEBUG
std::cerr << "* matches " << (*be) << std::endl;
#endif
                    std::vector<HitDef> entity_hits;
                    trim_blast_hits( entity_hits, hits, *fe, keep_all );
#ifdef DEBUG
std::cerr << entity_hits.size() << " hits for entity " << *fe << std::endl;
#endif
                    if( entity_hits.size() > 0 ) {
#ifdef DEBUG
std::cerr << "Updating saveframe" << std::endl;
#endif

                        if( ! update_savefame( star, *fe, entity_hits ) )
                            update_query_date( star, *fe );
                    }
    		} // endif *fe == *be
    	    } // endfor blast entities
        } // endif got hits
    } // endfor file entities
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
