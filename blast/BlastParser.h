
#ifndef _BLAST_PARSER_H_
#define _BLAST_PARSER_H_

#include <iostream>
#include <string>
#include "expat.h"

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "HitDef.h"
#include "HitList.h"

class HitDef;
class HitList;

static inline std::string & ltrim( std::string & s ) {
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), std::not1( std::ptr_fun<int, int>( std::isspace ) ) ) );
    return s;
}

static inline std::string & rtrim( std::string & s ) {
    s.erase( std::find_if( s.rbegin(), s.rend(), std::not1( std::ptr_fun<int, int>( std::isspace ) ) ).base(), s.end() );
    return s;
}

static inline std::string & trim( std::string & s ) {
    return ltrim( rtrim( s ) );
}

class BlastParser {
    public:
	static const int BUFSIZE = 1024;

	static const std::string TAG_ITNUM;
        static const std::string TAG_ITQDEF;
        static const std::string TAG_QRYLEN;
        static const std::string TAG_HIT;
        static const std::string TAG_HITNUM;
        static const std::string TAG_HITID;
        static const std::string TAG_HITDEF;
        static const std::string TAG_HITACC;
        static const std::string TAG_HITLEN;
        static const std::string TAG_BITSCORE;
        static const std::string TAG_HSPSCORE;
        static const std::string TAG_EXPVAL;
        static const std::string TAG_HITFROM;
        static const std::string TAG_HITTO;
        static const std::string TAG_QRYFROM;
        static const std::string TAG_QRYTO;
        static const std::string TAG_IDENT;
        static const std::string TAG_POS;
        static const std::string TAG_ALEN;

	BlastParser( HitList * hits = 0 );
	virtual ~BlastParser();

	bool parse( const std::string & filename );

    protected:
	
	void endElement( const std::string & name );
/*
	{
	    std::cerr << "End element |" << name << "|, chars: |" << fBuf << "|" << std::endl;
	    fBuf = "";
	}

	void characters( const std::string & chars ) {};
*/
/*
	std::string trim( const std::string & str ) {
            std::string rc = str;
	    std::string::size_type start = str.find_first_not_of( "\n \t" );
	    if( start != std::string::npos ) rc = str.substr( start );
	    start = rc.find_last_not_of( "\n \t" );
	    if( start < rc.size() ) rc.erase( start + 1, rc.size() );
    	    return rc;
	}
*/
    private:
	XML_Parser fParser;
	std::string fBuf;
	HitList * fHits;
	HitDef * fHit;
	bool fFirstHit;

	static void start_element( void * usrData, const XML_Char * name, const XML_Char ** attrs ) {
	    static_cast<BlastParser *>( usrData )->fBuf.clear(); // = "";
	}
	static void end_element( void * usrData, const XML_Char * name ) {
	    std::string tmp = name;
	    static_cast<BlastParser *>( usrData )->endElement( tmp );
	}
	static void chars( void * usrData, const XML_Char * characters, int length ) {
	    static_cast<BlastParser *>( usrData )->fBuf.append( characters, length );
	}

};

#endif
