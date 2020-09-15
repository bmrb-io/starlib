#ifndef _HIT_DEF_H_
#define _HIT_DEF_H_

#include <ostream>
#include <string>
#include <math.h>

#include "BlastParser.h"

class HitDef {

    friend class BlastParser;
    friend std::ostream & operator<<( std::ostream &, const HitDef & );
    
    public :
	
	static const int NUM_DBS = 12;
	static const std::string DB_IDS[];	///< simple BLAST ID to BMRB DB name map
	static const std::string DB_NAMES[];

	HitDef() { reset(); }

	HitDef( const HitDef & other ) :
	    fQueryDbName( other.fQueryDbName ),
	    fQueryDbId( other.fQueryDbId ),
	    fQueryDbLoc( other.fQueryDbLoc ),
	    fQueryMolName( other.fQueryMolName ),
	    fQueryLength( other.fQueryLength ),
	    fHitDbName( other.fHitDbName ),
	    fHitDbId( other.fHitDbId ),
	    fHitDbLoc( other.fHitDbLoc ),
	    fHitMolName( other.fHitMolName ),
	    fHitLength( other.fHitLength ),
	    fBitScore( other.fBitScore ),
	    fHspScore( other.fHspScore ),
	    fExp( other.fExp ),
	    fQueryFrom( other.fQueryFrom ),
	    fQueryTo( other.fQueryTo ),
	    fHitFrom( other.fHitFrom ),
	    fHitTo( other.fHitTo ),
	    fIdent( other.fIdent ),
	    fPosit( other.fPosit ),
	    fAlignLength( other.fAlignLength )
	{}

	HitDef( const std::string & query_id, int query_length ) :
	    fQueryLength( query_length ) {
	    setQueryId( query_id );
	    reset();
	}

	~HitDef() {}

	bool operator == ( const HitDef & ) const;
	bool operator != ( const HitDef & ) const;
	bool operator < ( const HitDef & ) const;
	bool operator > ( const HitDef & ) const;

	void setQueryId( const std::string & );
	void setHitDbName( const std::string & );
	void setHitDbId( const std::string & );
	void setHitDescription( const std::string & );
	void reset();

	std::string queryDbName() const { return fQueryDbName; }
	std::string queryDbId() const { return fQueryDbId; }
	std::string queryDbSubid() const { return fQueryDbLoc; }
	std::string queryDescription() const { return fQueryMolName; }
	int queryFrom() const { return fQueryFrom; }
	int queryTo() const { return fQueryTo; }
	int queryLength() const { return fQueryLength; }
	std::string hitDbName() const { return fHitDbName; }
	std::string hitDbId() const { return fHitDbId; }
	std::string hitDbSubid() const { return fHitDbLoc; }
	std::string hitDescription() const { return fHitMolName; }
	int hitFrom() const { return fHitFrom; }
	int hitTo() const { return fHitTo; }
	int hitLength() const { return fHitLength; }
	float bitScore() const { return fBitScore; }
	int hspScore() const { return fHspScore; }
	double expVal() const { return fExp; }
	int alignLength() const { return fAlignLength; }
	int numIdentities() const { return fIdent; }
	int numPositives() const { return fPosit; }
	
	float queryToSubPercent() const {
	    float tmp = roundf( 10000.0F * fAlignLength / fQueryLength );
	    return (float) tmp / 100.0F;
	}
	float pctIdentities() const {
	    float tmp = roundf( 10000.0F * fIdent / fAlignLength );
	    return (float) tmp / 100.0F;
	}

	float pctPositives() const {
	    float tmp = roundf( 10000.0F * fPosit / fAlignLength );
	    return (float) tmp / 100.0F;
	}

    private:
    
	std::string fQueryDbName;	///< query DB name
	std::string fQueryDbId;	///< query DB ID
	std::string fQueryDbLoc;	///< query DB subid
	std::string fQueryMolName;	///< query DB description
	int fQueryLength;	///< query length
	std::string fHitDbName;	///< hit DB name
	std::string fHitDbId;	///< hit DB ID
	std::string fHitDbLoc;	///< hit DB subid
	std::string fHitMolName;	///< hit DB description (molecule name)
	int fHitLength;		///< hit length
	float fBitScore;	///< bit score
	int fHspScore;		///< HSP score
	double fExp;		///< expectation value
	int fQueryFrom;		///< query match start
	int fQueryTo;		///< query match end
	int fHitFrom;		///< hit match start
	int fHitTo;		///< hit match end
	int fIdent;		///< num identities
	int fPosit;		///< num positives
	int fAlignLength;	///< alignment length

};

#endif
