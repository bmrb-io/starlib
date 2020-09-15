
#ifndef _HIT_LIST_H_
#define _HIT_LIST_H_

#include <set>
#include <ostream>
#include <vector>
#include <string>
#include "HitDef.h"

class HitDef;

class HitList {
    friend std::ostream & operator<<( std::ostream &, const HitList & );

    public:
    	static const float QSUB_CUTOFF = 50.0F;
    	static const float IDENT_CUTOFF = 98.0F;
/*
    	HitList() :
    		fIdentCutoff( IDENT_CUTOFF ),
    		fQsubCutoff( QSUB_CUTOFF )
    		{};
*/
    	HitList( float ident_cutoff = IDENT_CUTOFF, float qsub_cutoff = QSUB_CUTOFF ) :
    		fIdentCutoff( ident_cutoff ),
    		fQsubCutoff( qsub_cutoff )
    		{}
    	~HitList() { fData.clear(); };
    	bool add( const HitDef & );
    	bool getEntityIds( std::vector<std::string> & );
    	bool eidWasQueried( const std::string & eid ) {
#ifdef DEBUG
std::cerr << "eidWasQueried( " << eid << " ) : eids are" << std::endl;
{
for( std::set<std::string>::iterator pos = fEntityIds.begin(); pos != fEntityIds.end(); pos++ )
    std::cerr << (*pos) << std::endl;
}
#endif
    	    std::set<std::string>::iterator pos = fEntityIds.find( eid );
#ifdef DEBUG
std::cerr << "eidWasQueried( " << eid << " ) : found " << (*pos) << std::endl;
std::cerr << "returning " << ((pos == fEntityIds.end()) ? "false" : "true") << std::endl;
#endif
    	    if( pos == fEntityIds.end() ) return false;
    	    return true;
    	}
    	std::set<HitDef> & getHits() { return fData; }
    	void setIdentCutoff( float cutoff ) { 
#ifdef DEBUG
std::cerr << "!!!! HitList: id cutoff is now " << cutoff << std::endl;
#endif
    	    fIdentCutoff = cutoff; 
    	}
    	void setQsubCutoff( float cutoff ) { fQsubCutoff = cutoff; }

    private:
    	std::set<HitDef> fData;
    	std::set<std::string> fEntityIds;
    	float fIdentCutoff;
    	float fQsubCutoff;
};

#endif
