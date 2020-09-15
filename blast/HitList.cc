
#include <algorithm>
#include "HitList.h"

std::ostream & operator<<( std::ostream & out, const HitList & list ) {
    for( std::set<HitDef>::iterator i = list.fData.begin(); i != list.fData.end(); i++ ) {
	out << "* Hit" << std::endl;
	out << *i << std::endl;
    }
    return out;
}
//
// returns false if hit was not added (duplicate and worse that existing one)
//
bool HitList::add( const HitDef & hit ) {
#ifdef DEBUG
std::cerr << "HitList: new " << hit << std::endl;
std::cerr << " id " << hit.pctIdentities();
std::cerr << " q2s " << hit.queryToSubPercent();
std::cerr << " size " << fData.size() << std::endl;
#endif
// add to list of queried entity IDs
    fEntityIds.insert( hit.queryDbSubid() );
// don't store hits w/ < 98% identity score
#ifdef DEBUG
std::cerr << "!!! HitList: hit id score " << hit.pctIdentities() << ", cutoff " << fIdentCutoff << std::endl;
#endif
    if( hit.pctIdentities() < fIdentCutoff ) return false;
// or <= half the query length
#ifdef DEBUG
std::cerr << "!!! HitList: hit qsub score " << hit.queryToSubPercent() << ", cutoff " << fQsubCutoff << std::endl;
#endif
    if( hit.queryToSubPercent() < fQsubCutoff ) return false;
// don't store hits to self
    if( (hit.queryDbName() == hit.hitDbName()) 
     && (hit.queryDbId() == hit.hitDbId()) ) 
	return false;

    std::pair<std::set<HitDef>::iterator, bool> ret;
    if( fData.empty() ) {
	ret = fData.insert( hit );
	return( ret.second );
    }
// find hit in set: log(size)
#ifdef DEBUG
std::cerr << "HitList: looking for hit" << std::endl;
#endif
    std::set<HitDef>::iterator pos = fData.find( hit );
    if( pos == fData.end() ) {
#ifdef DEBUG
std::cerr << "HitList: not found" << std::endl;
#endif
	ret = fData.insert( hit );
	return( ret.second );
    }
#ifdef DEBUG
std::cerr << "HitList: found, comparing" << std::endl;
#endif
// found: keep best hit
    if( pos->pctIdentities() < hit.pctIdentities() ) {
#ifdef DEBUG
std::cerr << "HitList: better ident%" << std::endl;
#endif
	fData.erase( pos );
#ifdef DEBUG
std::cerr << "HitList: old rec. erased" << std::endl;
#endif
	fData.insert( pos, hit );
#ifdef DEBUG
std::cerr << "HitList: returning true" << std::endl;
#endif
	return true;
    }
    else if( pos->pctIdentities() == hit.pctIdentities() ) {
	if( pos->queryToSubPercent() < hit.queryToSubPercent() ) {
#ifdef DEBUG
std::cerr << "HitList: better qsub%" << std::endl;
#endif
	    fData.erase( pos );
	    fData.insert( pos, hit );
	    return true;
	}
	else if( pos->queryToSubPercent() == hit.queryToSubPercent() ) {
// can use bit score, hsp score (hgher), or expectation value (lower)
// use hsp score because it's an int
	    if( pos->hspScore() < hit.hspScore() ) {
#ifdef DEBUG
std::cerr << "HitList: better hsp score" << std::endl;
#endif
		fData.erase( pos );
		fData.insert( pos, hit );
		return true;
	    }
	}
    }
// else new hit is no better, ignore it
#ifdef DEBUG
std::cerr << "HitList: keeping old hit" << std::endl;
#endif
    return false;
}

bool HitList::getEntityIds( std::vector<std::string> & eids ) {
    std::string lastid;
    for( std::set<HitDef>::iterator i = fData.begin(); i != fData.end(); i++ ) {
	if( (lastid == "") || (lastid != i->queryDbSubid()) ) {
	    lastid = i->queryDbSubid();
	    eids.push_back( lastid );
	}
    }
    if( eids.size() < 1 ) return false;
    return true;
}
