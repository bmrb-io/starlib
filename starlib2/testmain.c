
#include "sb_lib.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main( int argc, char **argv )
{
    char    scope[82];
    char    key_tag[82];
    char    key_value[82];
    char    tag[82];
    int     choice;
    char    **strValues;
    double  *doubleValues;
    int     *flags;
    int     count;
    int     idx;

    void *star_handle;

    star_handle = init_star( "test.star" );
    if( star_handle == NULL )
    {
	fprintf( stderr, "%s: 'test.star': \n", argv[0], strerror(errno) );
	exit(1);
    }

    choice = 1;
    while( choice != 0 )
    {
	printf( "Choose a search type:\n"
		"   0 = quit\n"
		"   1 = string search\n"
		"   2 = numeric search\n"
		"   3 = string keyed search\n"
		"   4 = numeric keyed search\n"
		"   choice? " );
	scanf( "%d", &choice );
	
	if( choice == 0 )
	    continue;

	if( choice == 1 || choice == 2 )
	{
	    printf( "Scope? " );
	    scanf( "%s", scope );
	}
	else
	{
	    printf( "Key Tag? " );
	    scanf( "%s", key_tag );
	    printf( "Key Value? " );
	    scanf( "%s", key_value );
	}

	printf( "Tag to get values for? " );
	scanf( "%s", tag  );

	switch( choice )
	{
	    case 1:
	    case 3:
		if( choice == 1 )
		    star_get_string_tag( star_handle, scope, tag,
					 &strValues, &count );
		else
		    star_get_keyed_string_tag( star_handle, key_tag,
			                       key_value, tag,
					       &strValues, &count );
		for( idx = 0 ; idx < count ; idx++ )
		    printf( "val %d is: %s\n", idx, strValues[idx] );
		for( idx = 0 ; idx < count ; idx++ )
		    free( strValues[idx] );
		free( strValues );

		break;

	    case 2:
	    case 4:
		if( choice == 2 )
		    star_get_numeric_tag( star_handle, scope, tag,
					  &doubleValues, &flags, &count );
		else
		    star_get_keyed_numeric_tag( star_handle, key_tag,
			                  key_value, tag, &doubleValues,
					  &flags, &count );
		for( idx = 0 ; idx < count ; idx++ )
		{
		    printf( "val %d is: %lf (%s)\n", idx, doubleValues[idx],
			    (flags[idx] == OK_FLAG) ? "OK" :
			        (flags[idx] == NULL_FLAG) ? "NULL" :
				    "ISN'T A NUM" );
		}
		free( strValues );
		free( flags );

		break;

	    default:  break;
	}
    }

    shut_star( star_handle );

}
