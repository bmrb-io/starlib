#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "BlastParser.h"

const std::string BlastParser::TAG_ITNUM = "Iteration_iter-num";
const std::string BlastParser::TAG_ITQDEF = "Iteration_query-def";
const std::string BlastParser::TAG_QRYLEN = "Iteration_query-len";
const std::string BlastParser::TAG_HIT = "Hit";
const std::string BlastParser::TAG_HITNUM = "Hit_num";
const std::string BlastParser::TAG_HITID = "Hit_id";
const std::string BlastParser::TAG_HITDEF = "Hit_def";
const std::string BlastParser::TAG_HITACC = "Hit_accession";
const std::string BlastParser::TAG_HITLEN = "Hit_len";
const std::string BlastParser::TAG_BITSCORE = "Hsp_bit-score";
const std::string BlastParser::TAG_HSPSCORE = "Hsp_score";
const std::string BlastParser::TAG_EXPVAL = "Hsp_evalue";
const std::string BlastParser::TAG_HITFROM = "Hsp_hit-from";
const std::string BlastParser::TAG_HITTO = "Hsp_hit-to";
const std::string BlastParser::TAG_QRYFROM = "Hsp_query-from";
const std::string BlastParser::TAG_QRYTO = "Hsp_query-to";
const std::string BlastParser::TAG_IDENT = "Hsp_identity";
const std::string BlastParser::TAG_POS = "Hsp_positive";
const std::string BlastParser::TAG_ALEN = "Hsp_align-len";

BlastParser::BlastParser( HitList * hits ) {
    BlastParser::fBuf = "";
    fHits = hits;
    fHit = new HitDef();
    fFirstHit = true;
    fParser = XML_ParserCreate( NULL );
    XML_SetUserData( fParser, this );
    XML_SetElementHandler( fParser, start_element, end_element );
    XML_SetCharacterDataHandler( fParser, chars );        
}

BlastParser::~BlastParser() { 
    delete fHit; 
    XML_ParserFree( fParser );
}

bool BlastParser::parse( const std::string & filename ) {
    char buf[BUFSIZE];
    std::ifstream in;
    in.open( filename.c_str() );
    if( ! in.is_open() ) {
	std::cerr << "Can't open " << filename << " for reading" << std::endl;
	return false;
    }
    while( ! in.eof() ) {
	in.read( buf, BUFSIZE );
	if( in.fail() && ! in.eof() ) {
	    std::cerr << "Read error" << std::endl;
	    return false;
	}
#ifdef DEBUG
std::cerr << "* read |" << buf << "|" << std::endl;
#endif
	if( in.eof() ) {
	    if( XML_Parse( fParser, buf, in.gcount(), 1 ) == XML_STATUS_ERROR ) {
		std::cerr << "XML parse error @ line " << XML_GetCurrentLineNumber( fParser );
		std::cerr << ": " << XML_ErrorString( XML_GetErrorCode( fParser ) ) << std::endl;
		return false;
	    }
	}
	else {
	    if( XML_Parse( fParser, buf, BUFSIZE, 0 ) == XML_STATUS_ERROR ) {
		std::cerr << "XML parse error @ line " << XML_GetCurrentLineNumber( fParser );
		std::cerr << ": " << XML_ErrorString( XML_GetErrorCode( fParser ) ) << std::endl;
		return false;
	    }
	}
	memset( buf, 0, BUFSIZE );
    }
    in.close();
    return true;
}


void BlastParser::endElement( const std::string & name ) {

    std::string tmp = trim( fBuf );

    if( name == TAG_ITNUM )  {
//	if( fHit == NULL ) fHit = new HitDef();
#ifdef DEBUG
std::cerr << "Parser: new iteration " << tmp << std::endl;
#endif
    }
    if( name == TAG_ITQDEF ) fHit->setQueryId( tmp );
    if( name == TAG_QRYLEN ) fHit->fQueryLength = atoi( tmp.c_str() );
    if( name == TAG_HIT ) {
	fHits->add( *fHit );
	if( fFirstHit ) fFirstHit = false;
	else {
	    fHit->reset();
	    fFirstHit = false;
	}
    }
//    if( name == TAG_HITNUM )
#ifdef DEBUG
std::cerr << "Parser: new hit " << tmp << std::endl;
#endif
    if( name == TAG_HITID )  fHit->setHitDbName( tmp );
    if( name == TAG_HITDEF ) fHit->setHitDescription( tmp ); //fHitMolName = tmp;
    if( name == TAG_HITACC ) fHit->setHitDbId( tmp );
    if( name == TAG_HITLEN ) fHit->fHitLength = atoi( tmp.c_str() );
    if( name == TAG_BITSCORE ) fHit->fBitScore = atof( tmp.c_str() );
    if( name == TAG_HSPSCORE ) fHit->fHspScore = atoi( tmp.c_str() );
    if( name == TAG_EXPVAL ) fHit->fExp = strtod( tmp.c_str(), (char **) NULL );
    if( name == TAG_HITFROM ) fHit->fHitFrom = atoi( tmp.c_str() );
    if( name == TAG_HITTO ) fHit->fHitTo = atoi( tmp.c_str() );
    if( name == TAG_QRYFROM ) fHit->fQueryFrom = atoi( tmp.c_str() );
    if( name == TAG_QRYTO ) fHit->fQueryTo = atoi( tmp.c_str() );
    if( name == TAG_IDENT ) fHit->fIdent = atoi( tmp.c_str() );
    if( name == TAG_POS ) fHit->fPosit = atoi( tmp.c_str() );
    if( name == TAG_ALEN ) fHit->fAlignLength = atoi( tmp.c_str() );

    fBuf = "";
}

