#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
   char infname[255];
   char outfname[255];
   char userInput[255];
   FILE *inF, *outF;
   int  ch;
   int  ln = 1;

   int  virtual_col = 0; // current column if you assume
                         // tabstops are at every 8 columns.
			 // Column count starts at zero on the
			 // first column (so modulo arithemetic
			 // works nicer.)

   if( argc != 3 )
   {
       fprintf( stderr, "Error: needs 2 arguments: inputfile  outputfile\n" );
       exit(1);
   }
   strcpy( infname, argv[1] );
   strcpy( outfname, argv[2] );


   inF = fopen( infname, "r" );
   if( inF == NULL )
   {
       fprintf( stderr, "Error opening '%s': %s\n", infname, strerror(errno) );
       exit(1);
   }
   outF = fopen( outfname, "w" );
   if( outF == NULL )
   {
       fprintf( stderr, "Error opening '%s': %s\n", outfname, strerror(errno) );
       exit(1);
   }
   while( (ch=fgetc( inF ) ) != EOF )
   {
      if( ch == '\n' )
      {
	  ln++;
	  virtual_col = 0;
      }
      if( ch == '\t' )
      {
          ch = ' '; // change this tab to a space for when it will be
	            // printed later.

	  // Print out the padded preceeding spaces:
	  while( virtual_col % 8 )
	  {   fputc( ' ', outF );
	      virtual_col++;
	  }
      }
      if( ch > 127 )
      {
          printf( "Found ASCII CODE = %d CHAR = ('%c')", ch,ch );
	  printf( " in line %d: ", ln ); 
	  printf( "Replace with ->" );
	  fgets( userInput, 255, stdin );
	  ch = userInput[0];
      }
      fputc( ch, outF );
      virtual_col++;
   }
}
