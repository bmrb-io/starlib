#include "HitDef.h"

const std::string HitDef::DB_IDS[] = { "gb", "emb", "dbj", "pir", "prf", "sp", "pdb", "ref", "bmrb", "tpd", "tpe", "tpg" };
//const std::string HitDef::DB_NAMES[] = { "GenBank", "EMBL", "DBJ", "PIR", "PRF", "SWISS-PROT", "PDB", "REF", "BMRB" };
const std::string HitDef::DB_NAMES[] = { "GB", "EMBL", "DBJ", "PIR", "PRF", "SP", "PDB", "REF", "BMRB", "TPD", "TPE", "TPG" };

std::ostream & operator<<( std::ostream & out, const HitDef & hit ) {
    out << "HitDef:" << std::endl;
    out << "  qName:      |" << hit.fQueryDbName << "|" << std::endl;
    out << "  qID:        |" << hit.fQueryDbId << "|" << std::endl;
    out << "  qLoc:       |" << hit.fQueryDbLoc << "|" << std::endl;
    out << "  qDescr:     |" << hit.fQueryMolName << "|" << std::endl;
    out << "  qLen:       |" << hit.fQueryLength << "|" << std::endl;
    out << "  hName:      |" << hit.fHitDbName << "|" << std::endl;
    out << "  hID:        |" << hit.fHitDbId << "|" << std::endl;
    out << "  hLoc:       |" << hit.fHitDbLoc << "|" << std::endl;
    out << "  hDescr:     |" << hit.fHitMolName << "|" << std::endl;
    out << "  hLen:       |" << hit.fHitLength << "|" << std::endl;
    out << "  Bit Score:  |" << hit.fBitScore << "|" << std::endl;
    out << "  Hsp Score:  |" << hit.fHspScore << "|" << std::endl;
    out << "  Exp Value:  |" << hit.fExp << "|" << std::endl;
    out << "  qFrom:      |" << hit.fQueryFrom << "|" << std::endl;
    out << "  qTo:        |" << hit.fQueryTo << "|" << std::endl;
    out << "  hFrom:      |" << hit.fHitFrom << "|" << std::endl;
    out << "  hTo:        |" << hit.fHitTo << "|" << std::endl;
    out << "  ident:      |" << hit.fIdent << "|" << std::endl;
    out << "  posit:      |" << hit.fPosit << "|" << std::endl;
    out << "  align. len: |" << hit.fAlignLength << "|" << std::endl;
    out << "  pct q2sub:   " << hit.queryToSubPercent() << std::endl;
    out << "  pct ident:   " << hit.pctIdentities() << std::endl;
    out << "  pct posit:   " << hit.pctPositives() << std::endl;

    return out;
}

//
// 2 hits are equal if they have the same query and hit DBs and IDs
//
// Both hit and query loci are initialized to "" in constructors
//
bool HitDef::operator == ( const HitDef & rval ) const {
    return( (fQueryDbName == rval.fQueryDbName) && (fQueryDbId == rval.fQueryDbId)
	&& (fQueryDbLoc == rval.fQueryDbLoc) && (fHitDbName == rval.fHitDbName)
	&& (fHitDbId == rval.fHitDbId) && (fHitDbLoc == rval.fHitDbLoc) );
}
bool HitDef::operator != ( const HitDef & rval ) const {
    return( ! ((*this) == rval) );
}
//
// Hit sorting: 
//  1st by query DB: BMRB, PDB, then alphabetically,
//    then by query ID, and locus
//  2nd hit DB, ID, and locus, as above
//
bool HitDef::operator < ( const HitDef & rval ) const {
    if( fQueryDbName != rval.fQueryDbName ) {
    	if( fQueryDbName == "BMRB" ) return true;
    	if( rval.fQueryDbName == "BMRB" ) return false;
    	if( fQueryDbName == "PDB" ) return true;
    	if( rval.fQueryDbName == "PDB" ) return false;
    	return( fQueryDbName < rval.fQueryDbName );
    }
    if( fQueryDbId != rval.fQueryDbId ) return( fQueryDbId < rval.fQueryDbId );
    if( fQueryDbLoc != rval.fQueryDbLoc ) return( fQueryDbLoc < rval.fQueryDbLoc );
    if( fHitDbName != rval.fHitDbName ) {
    	if( fHitDbName == "BMRB" ) return true;
    	if( rval.fHitDbName == "BMRB" ) return false;
    	if( fHitDbName == "PDB" ) return true;
    	if( rval.fHitDbName == "PDB" ) return false;
    	return( fHitDbName < rval.fHitDbName );
    }
    if( fHitDbId != rval.fHitDbId ) return( fHitDbId < rval.fHitDbId );
// this adds every chain of multiple-chain PDB entries
//    if( fHitDbLoc != rval.fHitDbLoc ) return( fHitDbLoc < rval.fHitDbLoc );
    return false;
}
bool HitDef::operator > ( const HitDef & rval ) const {
    if( fQueryDbName != rval.fQueryDbName ) {
    	if( fQueryDbName == "BMRB" ) return false;
    	if( rval.fQueryDbName == "BMRB" ) return true;
    	if( fQueryDbName == "PDB" ) return false;
    	if( rval.fQueryDbName == "PDB" ) return true;
    	return( fQueryDbName > rval.fQueryDbName );
    }
    if( fQueryDbId != rval.fQueryDbId ) return( fQueryDbId > rval.fQueryDbId );
    if( fQueryDbLoc != rval.fQueryDbLoc ) return( fQueryDbLoc > rval.fQueryDbLoc );
    if( fHitDbName != rval.fHitDbName ) {
    	if( fHitDbName == "BMRB" ) return false;
    	if( rval.fHitDbName == "BMRB" ) return true;
    	if( fHitDbName == "PDB" ) return false;
    	if( rval.fHitDbName == "PDB" ) return true;
    	return( fHitDbName > rval.fHitDbName );
    }
    if( fHitDbId != rval.fHitDbId ) return( fHitDbId > rval.fHitDbId );
    if( fHitDbLoc != rval.fHitDbLoc ) return( fHitDbLoc > rval.fHitDbLoc );
    return false;
}

void HitDef::setQueryId( const std::string & query_id ) {
// split "gnl|mdb|bmrb15072:1 oscp-nt" into
// fQueryDbName  = BMRB
// fQueryDbId    = 15072
// fQueryDbLoc   = 1
// fQueryMolName = oscp-nt
#ifdef DEBUG
std::cerr << "*** set qid" << std::endl;
#endif
    fQueryDbName = "BMRB"; // always
    std::string::size_type pos;
    std::string tmp = query_id;
    for( int i = 0; i < 2; i++ ) {
    	pos = tmp.find_first_of( "|" );
    	if( pos == std::string::npos ) {
    		fQueryDbId = query_id;
    		fQueryDbLoc = "";
    		return;
    	}
    	tmp = tmp.substr( pos + 1 );
    }
    tmp = tmp.substr( 4 );
    pos = tmp.find_first_of( ":" );
    if( pos == std::string::npos ) {
    	fQueryDbId = tmp;
    	fQueryDbLoc = "";
    	return;
    }
    fQueryDbId = tmp.substr( 0, pos );
#ifdef DEBUG
std::cerr << "qdbid = |" << fQueryDbId << "|, tmp = |" << tmp << "|" << std::endl;
#endif
    tmp = tmp.substr( pos );
#ifdef DEBUG
std::cerr << "tmp now = |" << tmp << "|" << std::endl;
#endif
    if( tmp[0] == ':' ) tmp = tmp.substr( 1 );
#ifdef DEBUG
std::cerr << "tmp now (2) = |" << tmp << "|" << std::endl;
#endif
    pos = tmp.find_first_of( " " );
    if( pos == std::string::npos ) {
    	fQueryDbLoc = tmp;
    	return;
    }
    fQueryDbLoc = tmp.substr( 0, pos );
#ifdef DEBUG
std::cerr << "locus = |" << tmp.substr( 0, pos ) << "|" << std::endl;
#endif
    fQueryMolName = tmp.substr( pos + 1 );
#ifdef DEBUG
std::cerr << "molname = |" << tmp.substr( pos + 1 ) << "|" << std::endl;
#endif
}

void HitDef::setHitDbName( const std::string & hit_id ) {
// either "gnl|mdb|bmrb15072:1" or "gi|XYZ|pdb|BLAH|BLAH"
// fHitDbName  = BMRB or PDB
    std::string::size_type pos;
    std::string tmp = hit_id;
    pos = tmp.find_first_of( "|" );
#ifdef DEBUG
std::cerr << "!!! set HitDbName: >" << hit_id << "<" << std::endl;
#endif
    if( pos == std::string::npos ) {
        fHitDbName = hit_id;
        return;
    }
    std::string id = tmp.substr( 0, pos );
    tmp = tmp.substr( pos + 1 );
#ifdef DEBUG
std::cerr << "!!! set HitDbName tmp(1): >" << tmp << "<" << std::endl;
#endif
    pos = tmp.find_first_of( "|" );
    if( pos == std::string::npos ) {
        fHitDbName = hit_id;
        return;
    }
    tmp = tmp.substr( pos + 1 );
#ifdef DEBUG
std::cerr << "!!! set HitDbName tmp(2): >" << tmp << "<" << std::endl;
#endif
    bool found = false;
    if( id == "gnl" ) fHitDbName = "BMRB"; // always
    if( id == "gi" ) {
    	pos = tmp.find_first_of( "|" );
    	if( pos == std::string::npos ) {
    	    fHitDbName = tmp;
#ifdef DEBUG
std::cerr << "!!! set HitDbName: >" << tmp << "<" << std::endl;
#endif
        }
    	else {
    		for( int i = 0; i < NUM_DBS; i++ ) {
#ifdef DEBUG
std::cerr << "!!! set HitDbName: check >" << tmp.substr( 0, pos ) << "< (pos=" << pos << ") vs " << DB_IDS[i] << std::endl;
#endif
    			if( DB_IDS[i] == tmp.substr( 0, pos ) ) {
    				fHitDbName = DB_NAMES[i];
    				found = true;
    			}
    		}
    		if( ! found ) fHitDbName = tmp; 
#ifdef DEBUG
std::cerr << "!!! set HitDbName (last): >" << tmp << "<" << std::endl;
#endif
    	}
    }
}

void HitDef::setHitDbId( const std::string & hit_id ) {
// either "bmrb15072:1" or "2BO5_A" (pdb)
// fHitDbId  = 15072 if BMRB, 2BO5 if PDB, leave as is for everything else
#ifdef DEBUG
std::cerr << "*** set hid:|" << hit_id << "|, name=" << fHitDbName << std::endl;
#endif
    std::string::size_type pos;
    std::string tmp = hit_id;
    if( fHitDbName == "BMRB" ) {
    	tmp = tmp.substr( 4 );
    	pos = tmp.find_first_of( ":" );
    	if( pos == std::string::npos ) fHitDbId = tmp;
    	else {
    		fHitDbId = tmp.substr( 0, pos );
    		fHitDbLoc = tmp.substr( pos + 1 );
    	}
    }
    else if( fHitDbName == "PDB" ) {
    	pos = tmp.find_first_of( "_" );
    	if( pos == std::string::npos ) fHitDbId = tmp;
    	else {
    		fHitDbId = tmp.substr( 0, pos );
    		fHitDbLoc = tmp.substr( pos + 1 );
    	}
    }
    else fHitDbId = tmp;
}

/***********************************************
 * description is varchar(127) in star db schema
 */
void HitDef::setHitDescription( const std::string & hit_descr ) {
    std::string tmp = hit_descr;
#ifdef _DEBUG
std::cerr << "hit_descr=" << tmp << std::endl;
#endif
// if PDB, remove "Chain N" at the start (if exists)
    if( fHitDbName == "PDB" ) {
	std::string::size_type pos = hit_descr.find( "Chain " );
//std::cerr << "Chain_ at " << pos << " in " << hit_descr << std::endl;
	if( pos == 0 ) {
		tmp = hit_descr.substr( 8 );
		pos = tmp.find_first_not_of( " \t" );
		if( pos != std::string::npos ) tmp = tmp.substr( pos );
	}
    }
#ifdef _DEBUG
std::cerr << "tmp=" << tmp << std::endl;
#endif
    tmp = trim( tmp );
#ifdef _DEBUG
std::cerr << "after trim=" << tmp << std::endl;
#endif
// now the quotes
    if( (tmp[0] == '\'') && (tmp[tmp.length() - 1] == '\'') ) {
        std::string s = tmp.substr( 1, tmp.length() - 2 );
        if( s.find( "'" ) == std::string::npos )
            tmp = trim( s );
    }
    if( (tmp[0] == '"') && (tmp[tmp.length() - 1] == '"') ) {
        std::string s = tmp.substr( 1, tmp.length() - 2 );
        if( s.find( "\"" ) == std::string::npos )
            tmp = trim( s );
    }
#ifdef _DEBUG
std::cerr << "after strip=" << tmp << std::endl;
#endif
    fHitMolName = tmp.substr( 0, 127 );
}

void HitDef::reset() {
// keep query* values, zero out everything else
    fHitDbName = "";
    fHitDbId = "";
    fHitDbLoc = "";
    fHitMolName = "";
    fHitLength = 0;
    fBitScore = 0.0F;
    fHspScore = 0;
    fExp = 0.0;
    fQueryFrom = 0;
    fQueryTo = 0;
    fHitFrom = 0;
    fHitTo = 0;
    fIdent = 0;
    fPosit = 0;
    fAlignLength = 0;
}
