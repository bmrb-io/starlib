/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef ASTGUARD
#define ASTGUARD

//--------------------------------------------------
//                               AST
//
// The AST classes are used to manipulate star files' representations
// in memory.  I do not know what AST stands for.  I assume it is an
// abbreviation for something.
//--------------------------------------------------

//--------------------------------------------------
// RCS COMMENTS: 
//  HISTORY LOG:
//  ------------
/*********************************************************************
//  $Log: ast.h,v $
//  Revision 1.14  2010/03/31 21:23:22  madings
//  *** empty log message ***
//
//  Revision 1.13  2010/02/25 20:37:57  testadit
//  added ability to suppress the stop at the outermost level of the loop,
//  so the loop looks like how CIF likes it.
//
//  Revision 1.12  2010/01/30 00:28:44  testadit
//  added precomment ability to loop row nodes
//
//  Revision 1.11  2009/10/06 21:17:05  madings
//  *** empty log message ***
//
//  Revision 1.10  2008/08/14 18:46:30  madings
//  *** empty log message ***
//
//  Revision 1.9  2008/05/08 22:12:06  madings
//  Added some casts for g++ 2.4 warnings about signed/unsigned comparasins
//
//  Revision 1.8  2007/02/13 22:26:57  madings
//  *** empty log message ***
//
//  Revision 1.7  2006/03/23 06:20:19  madings
//  *** empty log message ***
//
//  Revision 1.6  2005/11/07 23:30:29  madings
//  *** empty log message ***
//
//  Revision 1.5  2003/02/28 03:16:27  madings
//  Fixed formatNMRSTAR that was failing after the
//  ability to store comments at any level of the
//  AST tree had been taken away.
//
//  Revision 1.4  2001/12/04 20:58:16  madings
//  Major changes  - see doc/changes.html for details:
//     - setstring / getstring interface at the  loop level
//     - implement #<START-SKIP> ... #<END-SKIP> directives
//     - fix compiler errors related to use of iterators and const
//          on newer versions of gcc.
//     - ValCache usage, with LoopRows containing one string for the
//          whole row instead of seperate data value nodes.
//     - change the type of the *os file stream because the ability to
//          construct from a unix fileno() handle has been depreciated in
//  	newer C++ stream libraries.
//
//  Revision 1.3  2001/11/13 20:30:28  madings
//  This update includes several changes:
//  (See doc/changes.html for the full explanation)
//     1- The previous compilers we had been using had allowed us to get
//         away with some mis-use of the const specifier without flagging
//         it as a warning or error.  That is no longer the case, so we had
//         to alter some of the code's use of "const".
//     2- The STL no longer allows vector iterators to be interchangable
//         with pointers, so we lost the ability to use that feature for
//         users of the library.
//     3- The streams library no longer allows you to make an ofstream out of
//         a pipe or standard output.  Callers of this method must now make
//         'os' be a pointer to an ostream instead of ofstream.
//     4- The newer compilers no longer allow a reference to a temporary store
//         that was created just for the duration of the function call.
//
//  Revision 1.2  2001/10/09 19:03:23  madings
//  Modified formatNMRSTAR to return exit codes for errors, and also
//  rolling up some minor changes to the source in terms of formatting.
//
 * Revision 1.1.1.1  2001/04/17  10:00:40  madings
 *
 * Revision 2.7  2000/10/24  21:40:04  madings
 * Previous check-in would not work under Linux because g++ and CC
 * disagree on what the default is when public: or private: has not been
 * explicitly specified.  This version makes it more explicit.
 *
 * Revision 2.6  1999/10/24  06:56:59  madings
 * Fixed problem with line numbers being wrong in starlib.
 * Bug was this: It was supposed to return the line/col number of the
 * parent LoopRowNode when a DataValueNode->get{Line|Col}Num() call is
 * made and the NO_POSBITS_DATAVALUENODE define was on.  But it didn't,
 * it returned -1.  The problem was that the parsing in the YACC file
 * lost the line number info while still building the list of DataValueNodes
 * and by the time they got transform()ed into LoopRowNodes, the position
 * info was already gone.  To fix this I made a new type "DVNWithPos" that
 * is a DataValueNode that always has position info even when
 * NO_POSBITS_DATAVALUENODE is defined.  This is the type that now temporarily
 * stores the values before they are transformed into LoopRowNodes.  So the
 * position info is not lost until after loop transformation now.
 *
 * Revision 2.5  1999/10/18  23:00:20  madings
 * Added formatNMRSTAR.cc for the first time.
 *
 * Revision 2.4  1999/08/26  06:44:16  madings
 * More changes to attempt to trim the memory footprint of this
 * program down:  Added the ability to compile the code with
 * some of the line-number and column-number information turned off
 * in DataValueNodes.  It didn't seem to help any.
 *
 * Revision 2.3  1999/04/28  02:29:00  madings
 * Made the searchByTag and searchByTagValue functions case-insensitive
 * for tag-names, datablock names, and saveframe names (but not data values).
 *
 * Revision 2.2  1999/03/19  00:59:00  madings
 * Added setTabFlag/getTabFlag functions to the loops, and altered the parser
 * so it remembers the tabulation format and preserves it.
 *
 * Revision 2.1  1999/03/04  20:32:40  madings
 * Added the ability to find some warning-level errors.
 * and made the new program called "star_syntax"
 *
 * Revision 2.0  1999/01/23  03:34:32  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.13  1999/01/22  23:58:27  madings
 * Golly, where should I begin?  Okay, There was a port to linux being
 * done in this version, which in turn meant a port to egcs instead of
 * making code that can only compile on a CFront style compiler like SGI
 * uses.  This port meant a lot of tiny changes all over the place to
 * accomodate the differences in the way templates get linked.  Also,
 * a bug was discovered in the way rows were deleted in the LoopTableNode's
 * erase() operator.  The bug was there all along and it surfaced as part
 * of the portation (what was just bad data in SGI was causing coredumps
 * in Linux.)  Hopefully now that it compiles under egcs, it will be much
 * more portable and I can begin getting it out to the public.
 *
 * Revision 1.12  1998/12/19  06:53:58  madings
 * Added multiple error handing (recovery) and caller-controlled
 * error handling.
 *
 * Revision 1.11  1998/10/08  03:45:33  madings
 * Two changes:
 *    1 - Most signifigant change is that the library now has
 *    doc++ comments in it.
 *    2 - Also, changed so that when using the copy constructor,
 *    it remembers line numbers and column numbers in the newly
 *    copied tree (I hope, this is untested.)
 *
 * Revision 1.10  1998/09/04  20:36:30  madings
 * Fixed an error when compiling on the older mips2 output compiler.
 * (It didn't understand that classes can see each other's protected
 * members if they are the same type of class.  I had to insert
 * the silly "friend class iterator" inside class iterator itself to
 * fix this.  Which is silly when you think about it.)
 *
 * Revision 1.9  1998/09/04  07:02:03  madings
 * There were several bug fixes with this version.
 *
 * However, the erasing probably still needs some work.
 *
 * Revision 1.8  1998/08/29  08:45:59  madings
 * This one was a rather large change.  I haven't finished
 * documenting and testing all of it, so expect another
 * revision or two soon.
 *
 * This revision creates a new way of dealing with loops.  Now
 * the caller can use the loops like 2-D vectors, but with some
 * extra magic behind the scenes to ensure that no insertions or
 * deletions that can leave the tree in an invalid state will
 * be allowed.  (For example, you cannot remove a value from the
 * loop without also removing the tagname for it and all the other
 * values in that column.)
 *
 * Also in this revision is a fix to the previous revision's method
 * of making List and ASTlist be wrappers around STL vector.  The
 * previous version of the library broke when the STL library decided
 * to move the vector to a new location in memory (realloc), and the
 * myCurrent interator became invalid.  Now myCurrent is an integer
 * index offset instead, so it moves with the vector.
 *
 * Also in this revision are some fixes to a longstanding problem that
 * made the library incapable of parsing nested loops unless the nested
 * loops were fully populated.  (In other words if an inner loop for any
 * row was null, because the input file simply has a "stop_" and no
 * data for the inner loop, it used to crash on that.)  Now it works and
 * empty inner loops are represented to the caller as null pointers (so
 * don't blindly follow inner loop pointers that are handed to you - check
 * them first.)
 *
 * Note that this marks my first attempt to make a class that implements
 * the STL vector interface, so I expect to find some bugs to fix in the
 * next revision or two.  There's some strange C++ "magic" going on here
 * to get this to work.
 *
 * Revision 1.7  1998/08/14  00:01:15  madings
 * Added ability to define which loops are to be output many-rows to the
 * line, and the ability to specifcy that a loop's values should be printed
 * up against the margin and not indented.
 *
 * Revision 1.6  1998/07/17  01:41:28  madings
 * Two changes related to Unparse() this revision:
 *   1 - Added the ability to programtically insert comments with
 *           the get/setPreComment functions.
 *   2 - Make Unparse() somewhat more sophisticated to handle formatting
 * 	  values so they line up with each other.
 *
 * Revision 1.5  1998/06/01  21:14:59  madings
 * Changed FlattenNestedLoop so it returns a List<> instead of an ASTlist<>.
 * ASTlist<> should NEVER, EVER, EVER be used to hold a list that is outside
 * the normal star tree.  (ASTlist<> changes the parent pointers of the
 * nodes you add to it, so if you use it to hold pointers to nodes in a
 * star tree, you change the parent pointers of those nodes.)
 *
 * Revision 1.4  1998/05/29  06:51:14  madings
 * Added the ability to search for all objects of a particular AST type.
 * (i.e. all SaveFrameNodes, or all DataItemNodes, etc...)
 * See the method called "searchForType()", which was added to do this.
 *
 * Revision 1.3  1998/05/21  22:20:46  madings
 * Made some minor changes from string references to string-by-value
 * in a few function templates.
 *
 * Revision 1.2  1998/05/08  20:27:23  informix
 * Made a change to handle the C++ STL instead of gcc's libg++.
 * Now all strings are of type 'string' instead of 'String' (notice
 * the uppercase/lowercase change.)  Applications using this library
 * will have to change also, unfortunately, to use the new string style.
 *
 * Revision 1.1.1.1  1998/02/12  20:59:00  informix
 * re-creation of starlib project, without sb_lib stuff.  sb_lib has
 * been moved to it's own project.
 *
 * Revision 1.1.1.1  1997/11/07  01:49:05  madings
 * First check-in of starlib and sb_lib in library form.
 *
 * Revision 1.1.1.1  1997/10/31  01:04:47  madings
 * First check-in.
 *
// Revision 1.6  1997/10/28  21:40:03  bsri
//   Added a function to rename a saveframe. This function is called
//   SaveFrameNode::changeName().
//
// Revision 1.5  1997/10/25  02:59:25  madings
//  2 Things in this check-in:
//    -1- "Parallel copy".  Addded a new way to do a copy constructor
//        such that it will copy with a parallel link back to the
//        original tree it was copied from.  Any node in either tree
//        can refer to the corresponding node in the new tree.
//        See "parallel.html" for more detail.
//
//    -2- Bug fixes to copy constructors that did not properly deep-copy
//        all the way down into the LoopTableNode values.
//
// Revision 1.4  1997/10/10  18:34:20  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.3  1997/09/16  22:03:18  madings
// Changed searchByTag() and searchByTagValue() so that they will
// return lists instead of returning single hits, and extended
// them to levels of the star tree above "saveframe" so it is now
// possible to use them on the whole star file.
// (Also changed the history log comments of several header files
// to use slash-star comment style instead of '//' style.)
//
// Revision 1.2  1997/09/15  22:28:21  madings
// Split list.h into list.h and astlist.h.  DId the same with list.cc
//
// Revision 1.1.1.1  1997/09/12  22:54:39  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.6  1997/07/30  21:34:26  madings
//check-in before switching to cvs
//
//Revision 1.5  1997/07/11  22:40:00  madings
//Finally seems to work - just waiting for
//new dictionaries.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:58:00  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:47  madings
//I Added the RCS log and ID variables.
**********************************************************************/
//
//
//--------------------------------------------------


#ifdef IS_GCC
#   pragma interface
#endif

#include <limits.h>
#include <stdio.h>
#include "portability.h"
#include "list.h"
#include "astnode.h"
#include "astlist.h"

#ifndef STAR_SYNTAX_PRINT_DEF
#define STAR_SYNTAX_PRINT_DEF
extern bool star_syntax_print;
#endif

#ifndef STAR_WARNING_CHECK_DEF
#define STAR_WARNING_CHECK_DEF
extern bool star_warning_check;
#endif

#ifndef STAR_TAGS_TO_DISABLE_ROW_COUNTING
#define STAR_TAGS_TO_DISABLE_ROW_COUNTING
extern vector<string> g_tags_to_disable_row_counting;
extern bool one_row_count_error_per_loop_only;
#endif


// This highly unsafe cast effectively removes the
// const access restriction on a pointer, by casting it
// to void and then back to the real type again.
// This became necessary when I switched from gcc 2.65
// to gcc 2.81, to accomodate the fact that the newer
// gcc treats all references to "this" as if they were
// const.  This is completely unnaceptable given that this
// broke all of my iterator methods (which frequently need
// to remember the address of the object, so they assign
// a pointer to point at "this" - which is a violation if
// "this" is a const.  I didn't *ask* for this to be const,
// and this change in gcc was to accomodate a feature I
// don't even use - so I'm a bit miffed about this.  To see
// why the change was made, scan the gcc manpage for
// "-fthis-is-variable" and read the paragraph there.  Note that
// all they were trying to prevent was something like:
//      this = new_ptr_value;
// But by making it const they also ended up overzealously
// preventing valid uses of "this" when they did that, like:
//      remember_for_my_cache = this;
//
#define UNDO_CONST_PTR(sometype,var) (sometype*)(void*)var

/** Performs a case-insensitive string compare.<BR>
  * Behaves like the standard C <tt>strcmp()</tt> function,
  * returning -1 for less than, 0 for equals, and +1 for
  * greater than.
  */
int strCmpInsensitive( const string &st1, const string &st2 );

// **********************************************************************
// subclasses of ASTnode 
// **********************************************************************


class LoopRowNode;
class LoopTableNode;

  //--------------------------------------------------
 //  class HeadingNode  //
//--------------------------------------------------
/** HeadingNode is a type that holds a simple header for one of the
  * other types of nodes.  It has several subtypes depending on what
  * kind of heading it is.  Typically a heading is just a string.
  */
class HeadingNode : public ASTnode, private ASTnodeWithPosBits
{
    ///
 public:
  HeadingNode(HeadingNode & h);   /// Copy constructor - recurses down

  /** Copy with parallel link.  Set "link" to true to create
    * a copy with a parallel link, or set it to false to create
    * a copy without a parallel link.  See the external documentation
    * for more details on parallel copies.
    */
  HeadingNode(bool link, HeadingNode & h);

  /// Constructor - give it a string to copy into the name.
  HeadingNode(const string & h);  
  /// Returns the name.
  inline string myName() const {return myHeading;};  // return the heading name.
  /// Sets the name.
  void changeName( const string &name);
  /// 
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );


  virtual void Unparse(int indent);

  ///
 protected:
  string myHeading;

  ///
public:
  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it is just the length of the heading.)
    */
  virtual int myLongestStr( void ) const { return myHeading.length(); };

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

  ///
private:
    virtual bool copyFrom( HeadingNode &fromThis,  // copy this node
	                   bool linkWith ); /// true = try to make parallel link

};

//------------------------
//  class DataValueNode
//------------------------
/** This is the type that holds a single string value from the 
  * star file, and the delimiter type that is used to quote it
  */
class DataValueNode: public ASTnode
#ifndef NO_POSBITS_DATAVALUENODE
                    , private ASTnodeWithPosBits
#endif
{
 friend class LoopRowNode;
 friend class LoopTableNode;
 ///
 public:
  /// ValType - the type of value this is
  enum ValType
  {
      /// Skipping an enum of zero, as that will indicate
      /// an undefined value.
      /// not quoted
      NON = 1,  
      /// double-quotes
      DOUBLE = 2, 
      /// single-quotes
      SINGLE = 3,  
      /// semicolon-delimited
      SEMICOLON = 4,
      /// framecode (puts a dollar sign in front)
      FRAMECODE = 5
  };

  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool    NotVirtualIsOfType( ASTtype T );

    //-------------------------------
   //  myParellelCopy()
  //--------------------------------
  /** Overrides the virtual method of the same name for
    * ASTnode.  DataValueNodes inside loops need some
    * special work behind the scenes to handle the
    * parallel copy.  On the surface, this method should
    * operate the same as for all other ASTnodes
    */
  virtual ASTnode *myParallelCopy( void );

    //--------------------------------------------------
   //   myDelimType()    //
  //--------------------------------------------------
  /** Returns the type of the DataValueNode - indicating what
    * kind of string delimiter it has.
    */
  ValType  myDelimType( void ) const { return (ValType) bitFields.delimType; } ;

    //--------------------------------------------------
   //  setDelimType()  //
  //--------------------------------------------------
  /** Sets the type of string value this is, and consequently,
    * what kind of delimiter to use when printing this node out.
    */
  void     setDelimType( ValType setTo );

    //--------------------------------------------------
   //  myValue() //
  //--------------------------------------------------
  /** Returns the string value of this node.  This is
    * identical to myName(), and is included just for
    * completeness.
    */
  string myValue( void ) const { return myStrVal; };

    //--------------------------------------------------
   // setValue() //
  //--------------------------------------------------
  //@{
      ///
      void setValue( const string &newVal );
      ///
      void setValue( const char *newVal );
      /** Changes the value of this string.  *DO NOT* include
	* delimiters when using this function.  Use setType()
	* to set the delimiter style instead.  For example,
	* to set the value to a framecode (dollar-sign on the front),
	* make two seperate function calls like so:
	* <pre>
	*     val->setValue( "new_value" );
	*     val->setType( DataValudeNode::FRAMECODE );
	* </pre>
	* Do *NOT* do this:
	* <pre>
	*     val->setValue( "$new_value" );
	* </pre>
	*     There are two overloaded versions: one for string and
	*     one for char *
	*/
  //@}

    //--------------------------------------------------
   //  Constructors    //
  //--------------------------------------------------
  DataValueNode(const string &str);
  ///
  DataValueNode(const char   *str);
  ///
  DataValueNode(const string &str, ValType type );
  ///
  DataValueNode(const char   *str, ValType type );
  /// Copy constructor.
  DataValueNode( DataValueNode &copy );

  /** Copy with parallel link.  Set "link" to true to create
    * a copy with a parallel link, or set it to false to create
    * a copy without a parallel link.  See the external documentation
    * for more details on parallel copies.
    */
  DataValueNode( bool link, DataValueNode &copy );  // copy constructor

  ///
  string myName() const {return myStrVal;}
  ///
  virtual void Unparse(int indent) { Unparse( indent, 0, -1, false ); };
  /** This function is public for reasons of technical difficulty.
    * You should not need to call it yourself.  Instead call the
    * other version of Unparse:  Unparse(int).
    */
  virtual void Unparse(int indent, int presize, int postsize, bool allNonQuoted);

    //--------------------------------------------------
   //  searchForType  //
  //--------------------------------------------------
  virtual List<ASTnode*> * searchForType(
	     ASTtype    type,
	     int        delim
	     );

    //--------------------------------------------------
   // operators ==, !=, <, >, <=, >=.   //
  //--------------------------------------------------
  /** For comparing values as strings.
    * (Gripe to Mr. B. Stroustrup: Why did you make me have to define
    * these operations seperately!?  Isn't it enough for me to define
    * "==" and "<", and then the compiler can derive code for the rest
    * of them from just those two?  Like so:
    * <pre>
    *      a != b   <==>  !( a == b )
    *      a >= b   <==>  !( a < b )
    *      a > b    <==>  !( a < b ) && !( a == b )
    *      ... etc ...
    * </pre>
    * End Gripe.)
    */
  virtual bool operator== (const string &str);
  ///
  virtual bool operator== (const char *str);
  ///
  virtual bool operator< (const string &str);
  ///
  virtual bool operator> (const string &str);
  ///
  virtual bool operator<=(const string &str);
  ///
  virtual bool operator>=(const string &str);
  ///
  virtual bool operator< (const char *str);
  ///
  virtual bool operator> (const char *str);
  ///
  virtual bool operator<=(const char *str);
  ///
  virtual bool operator>=(const char *str);

 ///
 protected:
  string   myStrVal;

  ///
public:
  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it just returns the length of the longest
    * string, plus whatever extra the quotation marks might need.)
    */
  virtual int myLongestStr( void );

  /** changeToAppropriateDelimiter():  Examine the string value
    * and determine what the appropriate delimiter type should
    * be, and apply it.
    */
  virtual void changeToAppropriateDelimiter( void );

  /** Find the delimiter type that would be appropriate for
    * the given string.  This is a static method, using none
    * of the data in a DataValueNode.  It is kept here in
    * this class for organizational purposes only.
    */
  static ValType findAppropriateDelimiterType( const string &val );

#ifndef NO_POSBITS_DATAVALUENODE
  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };
#endif

  ///
private:
    virtual bool copyFrom( DataValueNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

//------------------------
//  class DVNWithPos
//------------------------
/** This is identical to DataValueNode, but with one exception.
  * In the case where NO_POSBITS_DATAVALUENODE is defined, a
  * DataValueNode will have no positional information while this
  * class does.  So in other words, this class is identical to
  * DataValueNode, except that where the positional info (line/col)
  * is removable with a #define in DataValueNode, it is not removable
  * here.
  */
#ifdef NO_POSBITS_DATAVALUENODE
class DVNWithPos: public DataValueNode , private ASTnodeWithPosBits
{
public:
  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

    //--------------------------------------------------
   //  Constructors    //
  //--------------------------------------------------
  DVNWithPos(const string &str) : DataValueNode( str )
  { };
  //
  DVNWithPos(const char   *str) : DataValueNode( str )
  { };
  //
  DVNWithPos(const string &str, ValType type ) : DataValueNode( str, type )
  { };
  //
  DVNWithPos(const char   *str, ValType type ) : DataValueNode( str, type )
  { };
  // Copy constructor.
  DVNWithPos( DVNWithPos &copy ) :
      DataValueNode( copy.myValue(), copy.myDelimType() )
  {
      setParent( copy.myParent() );
      setPeer( copy.myParallelCopy() );
      setLineNum( copy.getLineNum() );
      setColNum( copy.getColNum() );
  };
};
#else
    typedef DataValueNode DVNWithPos;
#endif

  //--------------------------------------------------
 //  class DataNode     //
//--------------------------------------------------
// DataNode consists of any sort of single piece of data in the star file
// format.  It can be either a single item, a loop, or a save frame.
// DataNode cannot be used as a terminal class - it is virtual.  To
// instantiate a DataNode, one must use one of DataNode's derived
/// classes: DataItemNode, Unparse, or SaveFrameNode.
class DataNode: public ASTnode, private ASTnodeWithPosBits
{
    ///
public :

  ///
  virtual void Unparse(int indent) { Unparse( indent, 0 ); };
  /** This function is public for reasons of technical difficulty.
    * You should not need to call it yourself.  Instead call the
    * other version of Unparse:  Unparse(int).
    */
  virtual void Unparse(int indent, int padsize )=0;
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );
  bool  NotVirtualIsOfType( ASTtype T );
  ///
public:


  /** return the name of this node (tagname or saveframe name or the
    * name of the loop's first tag if this is a DataLoopNode)
    */
  virtual string myName() const = 0;
               
  /** <EM>DEPRECIATED</EM>:
    * Return the values in the loop that go with the given tag name.
    * Only works with the outermost loop level.
    * Only makes sense if this DataNode is a DataLoopNode, else it returns
    * an empty list.
    */
  virtual ASTlist<DataValueNode *>* returnLoopValues(const string & tagName) = 0;

  ///
public:
  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case, it does nothing - this is an incomplete virtual class.)
    */
  virtual int myLongestStr( void ) const =0;

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

};

class StarListNode;  // Stub - will be fleshed out further down.
class SaveFrameNode;  // Stub - will be fleshed out further down.
class BlockNode;  // Stub - will be fleshed out further down.
class DataFileNode;  // Stub - will be fleshed out further down.

  //--------------------------------------------------
 //  class StarFileNode  //
//--------------------------------------------------
/** The Mother Of All Star Nodes (literally).
  * This node is the root of the star tree for a
  * whole star file.  If a star file is parsed in with
  * yyparse(), it produces an object of this type, which
  * in turn contains the rest of the star tree.
  */
class StarFileNode: public ASTnode, private ASTnodeWithPosBits
{
    ///
 public:
  /** This status is sometimes returned by functions to report
    * errors.
    */
  enum Status {
      ///
      OK,
      ///
      ERROR
  };

//@{   /** <B>Constructors:</B> */

  /// Make a copy of the star file tree (recurses down).
  StarFileNode(StarFileNode &n);

  /// Make a trivial shallow copy.
  StarFileNode(StarListNode *n);

  /// Make an empty star file tree:
  StarFileNode();

  /** Copy with parallel link.  Set "link" to true to create
    * a copy with a parallel link, or set it to false to create
    * a copy without a parallel link.  See the external documentation
    * for more details on parallel copies.
    */
  StarFileNode( bool link, StarFileNode &n); 
//@}

  ///
  virtual ~StarFileNode();

  // Unparse():
  // ---------- 
  /** Print the whole star file out to (*os), indenting it to the level
    * given in the parameter.  (i.e. 4 = 4 spaces indented.)
    */
  virtual void Unparse(int indent);
                                     
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
  ///
public:


  // ReturnDataBlockDataNode():
  // --------------------------
  /** <em>DEPRECIATED - see searchByTag instead.</em>: <br>
    * Returns the save frame matching the name given in the data block name given:
    * @see searchByTag
    */
  DataNode*  ReturnDataBlockDataNode(string datablockName, string saveframeName);

  // RemoveSaveFrame():
  // ------------------
  /** <em>DEPRECIATED - Use the delete operator insread, or see List::erase (STL vector erase)</em>: <br> 
    * Deletes the save frame name given in the data block name given:
    * @see List::erase
    */
  void  RemoveSaveFrame(string datablockName, string saveframeName);

  // AddSaveFrameToDataBlock():
  // --------------------------
  /** <em>DEPRECIATED - Use the vector insert operator of ASTlist instead. </em><br>
    * Given a save frame, adds it to the datablock name given.  The 
    * save frame is passed as a generic DataNode, rather than a SaveFrameNode.
    * The DataNode given is hooked right into the star tree, no copied, so
    * don't delete it after adding it.
    * @see ASTlist::insert
    */
  void AddSaveFrameToDataBlock(string datablockName, DataNode* newNode);

  // GiveMyDataBlockList():
  // ----------------------
  /**
    * Returns a list of all blocks in the star tree.
    * This list can be manipulated directly to insert, erase, or iterate
    * over items from the tree, using the STL vector operators.
    */
  ASTlist<BlockNode*>* GiveMyDataBlockList();

  // AddDataBlock():
  // ---------------
  /** <em>DEPRECIATED - Use the vector insert operator of ASTlist instead. </em><br>
    * Add a new datablock (empty) to the star file, labelling it with the
    * name given.  Adds at the end.
    * @see ASTlist::insert
    */
  StarFileNode::Status AddDataBlock(const string & name);

  // AddSaveFrame():
  // ---------------
  /** <em>DEPRECIATED - Use the vector insert operator of ASTlist instead. </em><br> 
    * Add an empty save frame to the star file, labelling it eith the
    * name given. Adds at the end.
    * @see ASTlist::insert
    */
  StarFileNode::Status AddSaveFrame(const string & name);

  // AddSaveFrameDataItem():
  // -----------------------
  /** <em>DEPRECIATED - Use the vector insert operator instead. </em><br> 
    * Add a data item to the save frame, with the name/value pair
    * given. Adds to the last save frame in the star file tree.
    * returns ERROR if the last thing in the star file is not
    * a save frame.
    * @see ASTlist::insert
    */
  StarFileNode::Status AddSaveFrameDataItem( const string & name,
	                                     const string & value,
					     DataValueNode::ValType type);

  // AddSaveFrameLoop():
  // -------------------
  /** <em>DEPRECIATED - Use the vector insert operator instead. </em><br>
    * Add a loop to the last save frame in the star file.
    * returns ERROR if the last thing in the star file is not
    * a save frame.
    * @see ASTlist::insert
    */
  StarFileNode::Status AddSaveFrameLoop();

  // AddSaveFrameLoopDataName():
  // ---------------------------
  /** <em>DEPRECIATED - Use the vector insert operator instead.</em><br>
    * Add a new name to the loop at the end of the last save frame
    * of the file.  returns ERROR if the last thing in the save frame
    * is not a loop, or if the last thing in the AST is not a save
    * frame.
    * @see ASTlist::insert
    */
  StarFileNode::Status AddSaveFrameLoopDataName(const string & name);

  // AddSaveFrameLoopDataValue():
  // ----------------------------
  /** <em>DEPRECIATED  - Use the vector insert operator instead. </em><br>
    * Add a new value to the end of the loop at the end of the last 
    * save frame of the file.  returns ERROR if the last thing in
    * the save frame is not a loop, or if the last thing in the
    * AST is not a save frame.
    * @see ASTlist::insert
    */
  StarFileNode::Status AddSaveFrameLoopDataValue( const string & value,
						  DataValueNode::ValType type );
  
    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /** Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    *
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    *
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param searchFor - Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag(
	  const string  &searchFor
	  );

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type - the type to search for
    * @param delim - the delimiter type to search for if this is a search for
    *                DataValueNodes (enum DataValueNode::ValType)  Leave it off
    *                if you don't care about delimiter type or if this is a search 
    *                for something other than a DataValueNode
    */
  virtual List<ASTnode*> * searchForType(
	  ASTtype  type,      // type to search for.
	  int      delim = -1 // The delimiter type (the enum DataValueNode::ValType)
	  );


    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /** Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    *
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    *
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    *
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param tag - Look for this tag.
    * @param value - Where it has this value.
    */
  virtual List<ASTnode*> * searchByTagValue(
	  const string &tag,
	  const string &value
	  );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**  Unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    */
  virtual bool unlinkChild( ASTnode *child );

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

 ///
 private:
  ///
  StarListNode *myStarList;
  vector<string> skipTexts;
  vector<int> skipTextLineNums;

 public:
  ///
  void addSkipText( int line, string s )
  {   skipTexts.insert( skipTexts.end(), s );
      skipTextLineNums.insert( skipTextLineNums.end(), line );
  };
  /// Get the skip text at the index given.
  string getSkipText( int idx )
  {   return skipTexts[idx];
  };
  /** Get the first text that lays between line1 and line2, inclusive.
    * return: null if no such text.
    * return: ptr to the text if it exists, and retPos is the line
    * number of the text.
    */
  string getSkipTextBetween( int line1, int line2, int &retLine );

  /// how many skipped text blocks are there?
  int  numSkipTexts( void )
  {   return skipTexts.size();
  };

  /// copy from another vector:
  void setSkipTexts( vector<string> &vs, vector<int> &vi )
  {   skipTexts = vs;
      skipTextLineNums = vi;
  }


  ///
private:
    virtual bool copyFrom( StarFileNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

//-------------------------
// class DataListNode
//-------------------------
/** Holds a list of the kinds of things that can be in a DataBlockNode.
  *
  * (This means SaveFrameNodes, DataLoopNodes, and DataItemNodes)
  */
class DataListNode: public ASTnode
{
    ///
 public:
  //@{   /// <u>Constructors:</u>
      /// Recursive deep copy - use this for most copying needs.
      DataListNode( DataListNode &L);

      /** Shallow dumb copy - don't use this unless you really
        * know the insides of the library well.
	*/
      DataListNode(ASTlist<DataNode *> *L); 

      /// Make an empty list.
      DataListNode();

      /** Copy with parallel link.  Set "link" to true to create
        * a copy with a parallel link, or set it to false to create
        * a copy without a parallel link.  See the external documentation
        * for more details on parallel copies.
	*/
      DataListNode( bool link, DataListNode &L );
  //@}

//###############################################################

  /** Destructor - Recursively destroys the items inside the node
    * as well.
    */
  virtual ~DataListNode();

  /** Just like the vector method of the same name */
  virtual void reserve( size_t initSize ) { myKids->reserve( initSize); } ;
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
  ///
public:


  // GiveMyDataList():
  // -----------------
  /** Returns a list of all DataNodes in the data list.
    * This list can then be used like an STL vector to
    * manipulate the items inside the list to add
    * new nodes, erase nodes, or iterate over nodes.
    */
  ASTlist<DataNode*>* GiveMyDataList( void ); 

  // RemoveSaveFrame():
  // -----------------
  /** <em>DEPRECIATED - use STL vector erase() on the list returned by
    * GiveMyDataList instead.</em><br>
    * Remove the save frame matching the given name.
    * @see GiveMyDataList
    */
  void RemoveSaveFrame(string saveframe);

  // ReturnDataBlockDataNode():
  // --------------------------
  /** <em>DEPRECIATED - use searchByTag() instead.</em><br>
    * Return the datanode (usually a saveframe) that matches the
    * name given.)
    * @see searchByTag
    */
  DataNode*  ReturnDataBlockDataNode(string saveframeName);

  // AddSaveFrameToDataBlock():
  // --------------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Given a save frame, adds it to the datablock name given.  The 
    * save frame is passed as a generic DataNode, rather than a SaveFrameNode.
    * The DataNode given is hooked right into the star tree, no copied, so
    * don't delete it after adding it.
    * @see GiveMyDataList
    */
  void AddSaveFrameToDataBlock(DataNode* newNode);


  ///
  virtual void Unparse(int indent);


  // AddSaveFrame():
  // ---------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add an empty save frame to the star file, labelling it eith the
    * name given. Adds at the end.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrame(const string & name);

  // AddSaveFrameDataItem():
  // -----------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a data item to the save frame, with the name/value pair
    * given. Adds to the last save frame in the star file tree.
    * returns ERROR if the last thing in the star file is not
    * a save frame.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameDataItem( const string & name,
	                                     const string & value,
					     DataValueNode::ValType type);

  // AddSaveFrameLoop():
  // -------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a loop to the last save frame in the star file.
    * returns ERROR if the last thing in the star file is not
    * a save frame.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameLoop();

  // AddSaveFrameLoopDataName():
  // ---------------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a new name to the loop at the end of the last save frame
    * of the file.  returns ERROR if the last thing in the save frame
    * is not a loop, or if the last thing in the AST is not a save
    * frame.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameLoopDataName(const string & name);

  // AddSaveFrameLoopDataValue():
  // ----------------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a new value to the end of the loop at the end of the last 
    * save frame of the file.  returns ERROR if the last thing in
    * the save frame is not a loop, or if the last thing in the
    * AST is not a save frame.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameLoopDataValue( const string & value,
						  DataValueNode::ValType type );
  
    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /**
    * Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    *
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    *
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param searchFor Look for this string as the tag name
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor);

    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /**
    * Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    *
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    *
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param tag Look for this tag
    * @param value Where it has this value
    */
  virtual List<ASTnode*> * searchByTagValue( const string &tag, const string &value );

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /**
    * This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type Look for this type of node
    * @param delim If the node is a DataValueNode, then look specificly
    *              for values with this kind of delimiter.  The default
    *              if it is left off is "dont-care".
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int  delim = -1 );
    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**
    *  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

 ///
  ///
 private:
  ///
  ASTlist<DataNode *> *myKids;

 ///
 public:
  /**
    * Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * Basicly it calculates the longest string in the list of kids.
    */
  virtual int myLongestStr( void );

  ///
private:
    virtual bool copyFrom( DataListNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

//------------------------
//  class BlockNode
//------------------------
/** A single block in the file - this can be either a
  * data block or a global block - they are both handled
  * the same way.
  */
class BlockNode: public ASTnode, private ASTnodeWithPosBits
{
 ///
 public:
  //@{
      /** <b>Constructors:</b> */
      ///
      BlockNode(HeadingNode *h, DataListNode *n);
      /// give the name to call the block.
      BlockNode(const string & name);
      /// deep copy constructor
      BlockNode( BlockNode &n );

      // Copy with parallel link.  Set "link" to true to create
      // a copy with a parallel link, or set it to false to create
      // a copy without a parallel link.  See the external documentation
      /// for more details on parallel copies.
      BlockNode( bool link, BlockNode &n );
  //@}

  /// Destructor.  This will destruct everything inside the block as well.
  virtual ~BlockNode();

  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );
  bool  NotVirtualIsOfType( ASTtype T );
///
public:

  //-------------------------------------------
  // getPreComment() / setPreComment()
  //-------------------------------------------
  //@{
      ///
      virtual const string  &getPreComment( void )
      {
	  return preComment;
      };
      ///
      virtual void          setPreComment( const string &cmt )
      {
	  preComment = string(cmt);
      };
      /** These functions are used to give some nodes in the
	* AST tree the ability to remember a comment to be
	* pasted into the file in front of that node.  This
	* is useful if you want to insert header comments of
	* some sort into the output produced by Unparse().
	* As of this writing, no provisions are being made
	* to handle the parsing of comments from the original
	* file and storing them via these functions.  The
	* grammar to do that would be rather convoluted.
	* These functions are only intended to be used by programs
	* insterting their own comments after the file has
	* been read.
	* <p>
	* In the interest of saving memory, the preComment ability
	* only exists at the level of saveframenodes and blocknodes.
	* If you attempt to use these functions at other levels,
	* nothing will happen (the comment will always be empty).
	* <p>
	* The string must contain the comment characters embedded
	* inside, like so: "# this is a\n# multiline comment.",
	* not like this: "this is a\nmultiline comment."  This is
	* so that the caller is allowed to have the comment contain
	* blank lines like this:
	* <pre>
	*      # This is an example comment.
	*
	*      # The comment has some blank
	*
	*      # lines in it.
	* </pre>
	* If the Unparse() function were designed to insert the comment
	* characters (#) itself, then such a comment block would be impossible
	* to create.
	* <p>
	* Note that the comment lines are not syntax-checked in any way, so
	* using these functions it is entirely possible to create invalid
	* STAR files, since these "comments" can really be strings with
	* anything at all in them - so be careful.
	* <p>
	* To get rid of the preComment if you change your mind, set it to
	* a zero-length string with setPreComment().
	*/
  //@}


  // GiveMyDataList():
  // -----------------
  /** Return the list of data nodes that reside inside this block node:
    * This list is a vector and can be used like the STL vector to
    * add/delete/iterate over the the things in the block.
    */
  ASTlist<DataNode*>* GiveMyDataList(void); 

  // myName():
  // ---------
  /** Return the label of this block node, eg "data_foo" :
    */
  string myName() const {return myHeading->myName();};

  // ReturnDataBLockDataNode():
  // --------------------------
  /**
    * Return the data node matching the name given
    * (A save frame, data item, or loop).
    */
  DataNode* ReturnDataBlockDataNode(string saveframeName);

  // RemoveSaveFrame():
  // ------------------
  /** <em>DEPRECIATED - use STL vector erase() on the list returned by
    * GiveMyDataList instead.</em><br>
    * Remove the data item matching the name given (usually
    * a save frame).
    * @see GiveMyDataList
    */
  void RemoveSaveFrame(string saveframeName);

  // AddSaveFrameToDataBlock():
  // --------------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Attach the given data node (usually a save frame) to the
    * AST.  Note that it just makes a link to the object, not a
    * copy.  Do not delete the object after calling this.
    * @see GiveMyDataList
    */
  void  AddSaveFrameToDataBlock(DataNode* newNode);

  ///
  virtual void Unparse(int indent);

  // AddSaveFrame():
  // ---------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add an empty save frame to the end of the block, using the name
    * given:
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrame(const string & name);

  // AddSaveFrameDataItem():
  // -----------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a new item (tag/value pair) to the block in the last saveframe
    * of he block.  If the last thing in the block is not a save frame,
    * returns ERROR
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameDataItem( const string & name,
	                                     const string & value,
					     DataValueNode::ValType type);

  // AddSaveFrameLoop():
  // -------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a new empty loop to the last saveframe in the block.  Returns
    * ERROR if the last thing in the block is not a save frame.  Note
    * that no name is given, because the name of a loop is derived
    * from its first tag name - and an empty loop has no first tag.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameLoop();

  // AddSaveFrameLoopDataName():
  // ---------------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a tag name to the last loop of the last save frame in the
    * block.  Returns ERROR if the last thing in the block is not
    * a save frame, or if the last thing in the save frame is not a loop.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameLoopDataName(const string & name);

  // AddSaveFrameLoopDataValue():
  // ----------------------------
  /** <em>DEPRECIATED - use STL vector insert() on the list returned
    * by GiveMyDataList instead.</em><br>
    * Add a value to the last loop of the last save frame in the
    * block.  Returns ERROR if the last thing in the block is not
    * a save frame, or if the last thing in the save frame is not a loop.
    * @see GiveMyDataList
    */
  StarFileNode::Status AddSaveFrameLoopDataValue( const string & value,
						  DataValueNode::ValType type );
  
    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /**
    * Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param searchFor Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor);

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /**
    * This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    *
    * @param type type to search for
    * @param delim the delimiter type if searching for a DataValueNode.  The
    *              default is "dont-care".
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int  delim = -1 );

    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /**
    * Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    *
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    * <P>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param tag Look for this tag name...
    * @param value Where it has this value
    */
  virtual List<ASTnode*> * searchByTagValue( const string &tag, const string &value );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**
    *  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

  ///
  protected :
  ///
  DataListNode *myDataList;
  ///
  HeadingNode  *myHeading;
  string       preComment;

  ///
public:
  /**
    * Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * In this case it just returns the longest of the heading node or
    * the data list node:
    */
  virtual int myLongestStr( void );

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

  ///
private:
    virtual bool copyFrom( BlockNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

//-----------------------
//  class StarListNode
//-----------------------
/** This class holds a list of the nodes that can be inside a
  * star file. (data blocks are all that is allowed)
  */
class StarListNode: public ASTnode
{
 ///
 public:
    //@{ /** <u>Constructors:</u> */
         /// Recursive deep-copy constructor.
	 StarListNode( StarListNode &L );

         /** Dumb shallow copy constructor - don't use unless you really
	   * understand the Star library well.
	   */
	 StarListNode(ASTlist<BlockNode *> *L);

	 /// Make an empty list:
	 StarListNode();

	 /** Copy with parallel link.  Set "link" to true to create
	   * a copy with a parallel link, or set it to false to create
	   * a copy without a parallel link.  See the external documentation
	   * for more details on parallel copies.
	   */
	 StarListNode( bool link, StarListNode &L );
    //@}

  /// Destructor - also destructs everything in this list.
  virtual ~StarListNode();

  /// Just like the vector method of the same name.
  virtual void reserve( size_t initSize ) { myKids->reserve( initSize ); };
  ///
  virtual void Unparse(int indent);
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
///
public:

      /** Returns a list of nodes that are in the list.
	* This list is directly manipulatable such that
	* it can be used like an STL vector to insert/erase,
	* and iterate over the items in the list:
	*/
      ASTlist<BlockNode*>* GiveMyDataBlockList();

  //@{
      /** <font size=+1><em>
	* THESE FOLLOWING METHODS ARE DEPRECIATED - use STL vector
	* operations on the list returned by GiveMyDataBlockList
	* instead.</em><br></font>
	*/
      /// <em>DEPRECIATED</em>
      DataNode* ReturnDataBlockDataNode(string datablockName, string saveframeName);
      /// <em>DEPRECIATED</em>
      void  RemoveSaveFrame(string datablockName, string saveframeName);
      /// <em>DEPRECIATED</em>
      void AddSaveFrameToDataBlock(string datablockName, DataNode* newNode);
      /// <em>DEPRECIATED</em>
      StarFileNode::Status AddDataBlock(const string & name);
      /// <em>DEPRECIATED</em>
      StarFileNode::Status AddSaveFrame(const string & name);
      /// <em>DEPRECIATED</em>
      StarFileNode::Status AddSaveFrameDataItem( const string & name,
						 const string & value,
						 DataValueNode::ValType type );
      /// <em>DEPRECIATED</em>
      StarFileNode::Status AddSaveFrameLoop();
      /// <em>DEPRECIATED</em>
      StarFileNode::Status AddSaveFrameLoopDataName(const string & name);
      /// <em>DEPRECIATED</em>
      StarFileNode::Status AddSaveFrameLoopDataValue( const string & value,
						      DataValueNode::ValType type );
  //@}

    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /**
    * Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    *
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    *
    * @param searchFor Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor );

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /**
    * This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type the type to search for
    * @param delim the delimiter type if this is a search for a
    *              DataValueNode.  The default is -1 which means "dont-care"
    */
  virtual List<ASTnode*> * searchForType(
	  ASTtype  type,
	  int      delim = -1
	  );

    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /**
    * Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    * <P>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param tag the tag name to search for
    * @param value find it where it has this value.
    */
  virtual List<ASTnode*> * searchByTagValue( const string &tag, const string &value );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**
    *  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

 ///
 private:
  /// list of kids
  ASTlist<BlockNode *> *myKids;

  ///
public:
  /**
    * Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it is the longest of the myLongestStr()'s in
    * the list of objects inside this node.)
    */
  virtual int myLongestStr( void );

  ///
private:
    virtual bool copyFrom( StarListNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

//----------------------------
//  class GlobalBlockNode
//----------------------------
/** A BlockNode speficly for holding the "global_" block.
  */
class GlobalBlockNode: public BlockNode
{
 ///
 public:
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
///
public:

  //@{  /** <u>Constructors</u>: */
      ///
      GlobalBlockNode(HeadingNode *h, DataListNode *n);

      /// Copy constructor:
      GlobalBlockNode( GlobalBlockNode &N );
      /// Copy constructor - parallel version:
      GlobalBlockNode( bool link, GlobalBlockNode &N );
  //@}

};

//---------------------------
//  class DataBlockNode
//---------------------------
/** A data block of the form data_...
  */
class DataBlockNode: public BlockNode
{
 ///
 public:
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );
  
  bool  NotVirtualIsOfType( ASTtype T );

  ///
  /// Will set the datablock name.  If your name
  /// does not start with "data_" it will prepend
  //. "data_" to it.
  virtual void setName(const string & newName);
///
public:

  //@{ /** <u>Constructors</u>: */
      ///
      DataBlockNode(HeadingNode *h, DataListNode *n);
      ///
      DataBlockNode(const string & name);

      /// Copy constructor:
      DataBlockNode( DataBlockNode &N );
      /// Copy constructor - parallel version:
      DataBlockNode( bool link, DataBlockNode &N );
  //@}

};

//-------------------------
//   class DataNameNode
//-------------------------
/** Holds a name of either a tag/value pair or a loop tag name.
  */
class DataNameNode: public ASTnode, private ASTnodeWithPosBits
{
  friend class LoopNameListNode;  // needed so LoopNameListNode's erase() can
                                  // see my bitFields.isDeleting flag.
    ///
 public:
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
///
public:

  ///
  DataNameNode(const string &str);
  
  /// Parallel copy constructor:
  DataNameNode(bool link, DataNameNode &fromThis );

  /// Destructor:
  ~DataNameNode();

  /// unlinkMe();

  /// Returns the name of this tag (includes the leading underscore).
  string myName() const {return myStrVal;}
  /// Sets the name of this tag (caller must provide the leading underscore).
  void setName( const string &newName ){ myStrVal = newName; }
  ///
  void Unparse(int indent) { Unparse( indent, 0 ); };
  ///
  void Unparse(int indent, int padsize);

 ///
 private:
  ///
  string myStrVal;

  ///
public:
  /**
    * Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it is just the length of the name itself:)
    */
  virtual int myLongestStr( void )  const { return myStrVal.length(); }

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };
};

//--------------------------
//  class DataItemNode
//--------------------------
/** Holds a single tag name / tag value pair
  */
class DataItemNode: public DataNode
{
    ///
 public:
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
///
public:

  //@{  /** <u>Constructors</u>: */

      /** Make a new item given the name and value.
	* Note that these pointers refer to memory that you are
	* "giving" to the node (don't delete them afterward, and
	* don't pass addresses to stack variables - only addresses
	* to heap variables.)
	*/
      DataItemNode(DataNameNode *name, DataValueNode *value);

      /** Make a new item given the name and value as strings.
	* The delimiter type will be chosen heuristicly as one
	* that is legal for the value given.
	*/
      DataItemNode(const string & name, const string & value);

      /** Make a new item given the name and value and delimiter
	* type.  Note that no checking is done to ensure that the
	* delimiter type given is appropriate for the value given.
	* (It is possible to make a value with spaces and tell
	* it to be nonquoted, for example.)
	*/
      DataItemNode(const string & name, const string & value, DataValueNode::ValType type);

      /** Copy Constructor. */
      DataItemNode(DataItemNode & D);

      /** Copy with parallel link.  Set "link" to true to create
        * a copy with a parallel link, or set it to false to create
        * a copy without a parallel link.  See the external documentation
        * for more details on parallel copies.
	*/
      DataItemNode( bool link, DataItemNode & D);
  //@}

  /// Destructor.
  virtual ~DataItemNode();


  //-------------------------------------------
  // getPreComment() / setPreComment()
  //-------------------------------------------
  //@{
      ///
      virtual const string  &getPreComment( void )
      {
          return preComment;
      };
      ///
      virtual void          setPreComment( const string &cmt )
      {
          preComment = string(cmt);
      };

  /// Return the name of the tag in this item.
  virtual string myName() const { return (myDataName->myName());};

  /// Give the dataNameNode pointer out of the tree, not just the string value.
  virtual DataNameNode *myNameNodePtr() const { return (myDataName);};

  /// Return the value of this item.
  string myValue() const {return myDataValue->myName();};

  /// Give the dataNameNode pointer out of the tree, not just the string value.
  virtual DataValueNode *myValueNodePtr() const { return (myDataValue);};


  /** Dummy do-nothing function needed to handle the fact that this
    * is derived from DataNode, which defines this function as
    * virtual.
    */
  virtual ASTlist<DataValueNode *>* returnLoopValues(const string & tagName);


  ///
  virtual void Unparse(int indent) { Unparse( indent, 0 ); };
  ///
  virtual void Unparse(int indent, int padsize);

    //--------------------------------------------------
   //  myDelimType()   //
  //--------------------------------------------------
  /// Returns the delimiter type of the value of this Data Item.
  DataValueNode::ValType myDelimType(void) const { return myDataValue->myDelimType();};

    //--------------------------------------------------
   // setDelimType() //
  //--------------------------------------------------
  /// Sets the delimiter type of the value of this Data Item.
  void setDelimType( DataValueNode::ValType );

    //--------------------------------------------------
   // setValue()   //
  //--------------------------------------------------
  //@{
      ///
      void setValue( const string &val );
      ///
      void setValue( const char  *val );
      /**
	* Sets the string value of this node.  Do not use delimiters,
	* instead use setDelimType to chose the delimiter style.
	* just use the raw string here.
	*/
  //@}

    //--------------------------------------------------
   // setName()   //
  //--------------------------------------------------
  //@{
      ///
      void setName( const string &name );
      ///
      void setName( const char   *name );
      /** Both these functions set the name of the tag.
	* You should provide the leading undescore.
	*/
  //@}

    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /** Included for orthogonality with ASTnode.  Not really
    * very useful at this scope.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor );

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** Included for orthogonality with ASTnode.  Not really
    * very useful at this scope.
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int delim = -1 );

    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /** Included for orthogonality with ASTnode.  Not really
    * very useful at this scope.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    */
  virtual List<ASTnode*> * searchByTagValue( const string &tag, const string &value );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**
    *  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

  
  ///
 private:
  ///
  DataNameNode *myDataName;
  ///
  DataValueNode *myDataValue;
  string preComment;

  ///
public:
  /**
    * Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case, it just returns the length of the tagname,
    * not the data value)
    */
  virtual int myLongestStr( void )  const { return myDataName->myLongestStr(); };

///
private:
    virtual bool copyFrom( DataItemNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};


/// DEPRECIATED name for LoopNameListNode for backward compatability:
#define LoopDefListNode  LoopNameListNode

//-------------------------------
//   class LoopNameListNode
//-------------------------------
/** A list of tag names representing one single nesting level
  * of a loop's definition.  This class is designed to look
  * somewhat like an STL vector, implementing many of the same
  * methods as STL vector.  The caller is <b>not</b> given access
  * to the interior ASTlist of names because there needs to be
  * some automated sanity checking to prevent people from deleting
  * names without deleting the associated values in the loop.  The
  * same is true for inserting new names.  If a new name is inserted,
  * new values need to be inserted into the associated loop's
  * values so the result is still valid STAR syntax.  The methods
  * in this class that are made to look like STL vector functions
  * perform these tasks automatically when things are inserted or
  * deleted from the list of names.
  */
class LoopNameListNode : public ASTnode, private ASTnodeWithPosBits
{
///
public:
  //@{ /** <u>Constructors</u>: */
      /// Deep-Copy Constructor
      LoopNameListNode( LoopNameListNode &L);
      /**
        * Copy with parallel link.  Set "link" to true to create
        * a copy with a parallel link, or set it to false to create
        * a copy without a parallel link.  See the external documentation
        * for more details on parallel copies.
	*/
      LoopNameListNode( bool link, LoopNameListNode &L);
      ///
      LoopNameListNode(ASTlist<DataNameNode *> *L);
      ///
      LoopNameListNode(vector<DataNameNode *> &L);
      /// Makes an empty list:
      LoopNameListNode();
  //@}

  /// Print out debugging info detailing the layout of this list:
  void debugPrint( void );

    //@{
	typedef DataNameNode *value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef size_t difference_type;

	typedef ASTlist<DataNameNode*>::iterator iterator;
	typedef ASTlist<DataNameNode*>::const_iterator const_iterator;
	typedef ASTlist<DataNameNode*>::reverse_iterator reverse_iterator;
	typedef ASTlist<DataNameNode*>::const_reverse_iterator const_reverse_iterator;
    //@}

// TODO - delete this commented section out once I'm convinced that the
// above typedefs work:
#ifdef BOGUS_NAME_NOT_DEFINED_SO_THIS_GETS_COMMENTED_OUT

    class iterator;
    class const_iterator;

    /** This is designed to behave like the STL vector iterator.
      * it is not documented in detail because the reader is
      * expected to have access to the STL documentation describing
      * vectors.
      * Note that reverse_iterator and const_reverse_iterator are
      * not implemented.
      */

    class iterator
    {
	friend class LoopNameListNode;

	friend class const_iterator;

	///
	protected:
	    ///
	    ASTlist<DataNameNode*>::iterator  vIter;
	///
	public:
	    ///
	    iterator()
	    {
	    }
	    ///
	    iterator( const ASTlist<DataNameNode*>::iterator  &copyMe )
	    {
		vIter = copyMe;
	    }
	    ///
	    //iterator( value_type  &copyMe )
	    //{
	    //	vIter = &copyMe;
	    //}
	    ///
	    ~iterator()
	    {
	    }
	    ///
	    bool operator==( const iterator &x ) const
	    {   return vIter == x.vIter; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return vIter != x.vIter; 
	    }
	    ///
	    reference operator*() const { return *vIter; }
	    ///
	    value_type operator->() const{ return *vIter; }
	    ///
	    iterator &operator++() 
		{ ++vIter; return *this; }
	    ///
	    iterator &operator++(int) 
		{   iterator retVal;
		    retVal = *this; ++vIter; return retVal; }
	    ///
	    iterator &operator--() 
		{ --vIter; return *this; }
	    ///
	    iterator &operator--(int) 
		{   iterator retVal;
		    retVal = *this; --vIter; return retVal; }
	    ///
	    iterator &operator+=(size_type n) 
		{ vIter += n; return *this; }
	    ///
	    iterator &operator-=(size_type n) 
		{ vIter -= n; return *this; }
    };

    ///
    class const_iterator
    {
	friend class LoopNameListNode;

	protected:
	    ///
	    ASTlist<DataNameNode*>::const_iterator  vIter;
	///
	public:
	    ///
	    const_iterator()
	    {
	    }
	    ///
	    const_iterator( const ASTlist<DataNameNode*>::const_iterator  &copyMe )
	    {
		vIter = copyMe;
	    }
	    ///
	    //const_iterator( const value_type  &copyMe )
	    //{
	    //	vIter = &copyMe;
	    //}
	    ///
	    ~const_iterator()
	    {
	    }
	    ///
	    bool operator==( const iterator &x ) const
	    {   return vIter == x.vIter; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return vIter != x.vIter; 
	    }
	    ///
	    const_reference operator*() const { return *vIter; }
	    ///
	    const value_type operator->() const{ return *vIter; }
	    ///
	    const_iterator &operator++() 
		{ ++vIter; return *this; }
	    ///
	    const_iterator &operator++(int) 
		{   const_iterator retVal;
		    retVal = *this; ++vIter; return retVal; }
	    ///
	    const_iterator &operator--() 
		{ --vIter; return *this; }
	    ///
	    const_iterator &operator--(int) 
		{   const_iterator retVal;
		    retVal = *this; --vIter; return retVal; }
	    ///
	    const_iterator &operator+=(size_type n) 
		{ vIter += n; return *this; }
	    ///
	    const_iterator &operator-=(size_type n) 
		{ vIter -= n; return *this; }
    };
// ENDIF BOGUS_NAME_NOT_DEFINED_SO_THIS_GETS_COMMENTED_OUT
#endif


    //@{
	/** These are for use with STL iterator algorithms:
	  * The following allows the caller to use LoopNameListNode like
	  * it was an STL vector<DataNameNode>, with the extra stipulation
	  * that deletion or insertion operations will automatically cause
	  * the values in the same loop to be altered so that the loop is
	  * still valid STAR syntax.  (Values added or deleted as appropriate.)
	  */

	///
	iterator begin() { return myKids->begin(); }
	///
	iterator end() { return myKids->end(); }
	///
	const_iterator begin() const { return myKids->begin(); }
	///
	const_iterator end() const { return myKids->end(); }
	///
	size_type  size() const { return myKids->size(); }
	///
	bool empty() const { return myKids->empty(); }
	///
	reference operator[]( size_type n ) { return (*myKids)[n]; }
	///
	const_reference operator[]( size_type n) const { return (*myKids)[n];}
	///
	reference front() { return myKids->front(); }
	///
	const_reference back() const { return myKids->back(); }
	///
	const_reference front() const { return myKids->front(); }
	///
	reference back() { return myKids->back(); }
    //@}

    //@{
    /** Convert from a pointer-to-DataNameNode into an
      * iterator for this LoopNameListNode class.  This is an operation
      * that takes linear time, so use it with care.  If the
      * object refered to is not here, then the end iterator is 
      * returned.
      */
    iterator iteratorFor( const DataNameNode *ptr );
    //@}

   //@{
      //--------------------------------------------------
     // insert() and erase()  //
    //--------------------------------------------------
    /**
      *   The following functions are all designed to make sure things
      *   stay correct within the STAR file rules.  Thus they automatically
      *   insert and delete the appropriate data values to go along with
      *   the data names if this LoopNameListNode is inside a DataLoopNode.
      *   <P>
      *   If this LoopNameListNode has not yet been attached to the rest of
      *   a DataLoopNode yet, such that there are no associated data values
      *   to go with these names, then nothing special happens - the name(s)
      *   is(are) inserted and that's all.
      *   <P>
      *   Here is the more precise rules:<BR>
      *   -------------------------------<BR>
      *       <UL><LI>Any time any new tagnames are INSERTED:
      *           <UL><LI>If this LoopNameListNode is inside a DataLoopNode:
      *               <UL><LI>If that DataLoopNode has any rows of values at the
      *                       nesting level where this inserted name is going in:
      *                   <UL><LI>For each such row, a new DataValueNode is inserted
      *                   under this new tag name.  The value is STAR-null (a
      *                   period), or some other constant value provided by
      *                   the caller of the insert function.
      *    </UL></UL></UL></UL>
      * <P><UL><LI>Any time any new tagnames are DELETED:
      *       <UL><LI>If this LoopNameListNode is inside a DataLoopNode:
      *           <UL><LI>If that DataLoopNode has any rows of values at the
      *                   nesting level where this inserted name is going in:
      *               <UL><LI>
      *                   For each such row, the DataValueNodes associated
      *                   with the name(s) that is(are) deleted are also
      *                   deleted so the loop is still syntacticly correct.
      *               </UL>
      *           </UL>
      *      </UL>
      *   </UL>
      * <P>
      * <P>
      *   Insert - Insert before 'pos', the Name given.  All versions make
      *   copies of the Name rather than just pointers to it:
      * <P>
      * <P>
      * When new values need to be inserted to go with the new name,
      * this version defaults the values to insert to the STAR-null value,
      * a single nonquoted dot (.):
      */
    /// This version copies the DataNameNode into the list:
    void insert( iterator pos, const DataNameNode &v );

    /** This version makes a link to the DataNameNode in the list:
      * (Don't delete the DataNameNode after you call this):
      */
    void insert( iterator pos, DataNameNode *v );

    /**
      * This version allows the caller to specify the value to insert:
      *
      * Make-a-copy of DataNameNode version:
      */
    void insert( iterator       pos,
	         const DataNameNode   &v,
		 const DataValueNode  &val );

    /**
      * This version allows the caller to specify the value to insert:
      *
      * Make-a-link of DataNameNode version:
      */
    void insert( iterator       pos,
	         DataNameNode   *v,
		 const DataValueNode  &val );

    /**
      *   erase - Remove the value given.
      */
    void erase( iterator pos );
    ///  erase - Remove the values in the range given as [from,to)
    void erase( iterator from, iterator to );

  //@}

    /**
      * isInLoop: Returns true if this LoopNameListNode is inside a DataLoopNode,
      * or false if it is not.  (In a complete STAR tree it is always true,
      * but when building up a loop piece by piece an LoopNameListNode might
      * not have been attached to a loop yet.)
      * <P>
      * The value returned is an integer.  It is a count of how many nesting
      * levels deep this is in the loop that it was found it.  Thus if
      * it is zero (false) it was not found in a loop, and if it is 1 then
      * it was found in nesting level 1 of a loop, 2 = nesting level 2, etc.
      */
    int isInLoop(void);


  /// Destructor
  virtual ~LoopNameListNode();

  ///
  virtual void Unparse(int indent);

  virtual void Unparse(int indent, bool isEmpty );
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );

///
public:


  /// <em>DEPRECIATED</em> - Use STL vector notation instead
  void reset();

  /// <em>DEPRECIATED</em> - Use STL vector notation instead
  void getNextTag();

  /// <em>DEPRECIATED</em> - Use STL vector notation instead
  bool atEnd();

  /// <em>DEPRECIATED</em> - Use STL vector notation instead
  bool atStart();

  /** Return the name of the first tag in my list.  By convention, that
    * will be my "name" for the purposes of those methods that
    * insist that everything in the tree have a name
    */
  string myName() const {myKids->Reset(); return(myKids->Current()->myName());};

  /// <em>DEPRECIATED</em>
  void FlattenNestedLoop(List<DataNameNode*>* L);

  /** The postion of a tag in the list (returns an integer index).
    * The return value starts counting at zero, like arrays do.
    * (A negative number indicates it was not found.)
    * Note that this is a case-insensitive search. A tag of "_tag1" will
    * count as a match when searching for "_TAG1" or "_Tag1".
    */
  int tagPosition(const string & tagName);

  /// <em>DEPRECIATED</em> - Use STL vector notation instead
  StarFileNode::Status AddDataName(const string & name);

  /// <em>DEPRECIATED</em> - Use STL vector notation instead
  StarFileNode::Status RemoveDataName(const string & name, int *ele = (int*)NULL  );

  ///
  StarFileNode::Status ChangeName( const string &oldName, const string &newName );


    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /** Find the name given in this name list.
    * <P>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    *
    * @param searchFor Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor );
  
    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /**
    * This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type type to search for
    * @param delim the delimiter type (not relevant for this scope).
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int  delim = -1 );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**
    *  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

 ///
 private:
  /// list of kids
  ASTlist<DataNameNode *> *myKids;

///
public:
  /**
    * Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it returns the longest tag name in the list of names.)
    */
  virtual int myLongestStr( void );

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

///
private:
    virtual bool copyFrom( LoopNameListNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

class LoopValListNode;

//-------------------------------------
// class DataLoopValListNode
//-------------------------------------
// This is Depreciated and therefore not documented with doc++
//
class DataLoopValListNode : public ASTnode, private ASTnodeWithPosBits
{
 public:
  enum Status { END, STOP, CONTINUE };

  DataLoopValListNode(ASTlist<LoopValListNode *> *L);
  virtual ~DataLoopValListNode();

  Status getNextValue(DVNWithPos* &value);
  void reset();

  // Print out debugging info detailing the layout of this list:
  void debugPrint( void );

  //----------------------------------------------------------
  // Functions needed because this is a derivative of ASTnode:
  //----------------------------------------------------------
  virtual void     Unparse(int indent);
  virtual ASTtype  myType(void);
  virtual bool     isOfType( ASTtype T );
  virtual bool     unlinkChild( ASTnode *child );
  bool             NotVirtualIsOfType( ASTtype T );

 private:
  ASTlist<LoopValListNode *> *myKids;

public:
  // Returns the length of the longest string in this object.
  // Used by Unparse() at various levels of the AST tree.
  // (In this case it is just the longest datavalue in the list: )
  virtual int myLongestStr( void );

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };
};

//---------------------------------
// class LoopValListNode
//---------------------------------
// This is Depreciated and therefore not documented with doc++
//
class LoopValListNode : public ASTnode, private ASTnodeWithPosBits
{
public:
  LoopValListNode(ASTlist<DVNWithPos *> *L);
  LoopValListNode();
  virtual ~LoopValListNode();

  void reset();
  DataLoopValListNode::Status getNextValue(DVNWithPos* & value);

  // Print out debugging info detailing the layout of this list:
  void debugPrint( void );

  //-------------------------------------------------------------
  // Functions nessacery because this s a derivative of ASTnode:
  //-------------------------------------------------------------
  virtual void     Unparse(int indent);
  virtual ASTtype  myType(void);
  virtual bool     isOfType( ASTtype T );
  virtual bool     unlinkChild( ASTnode *child );
  bool             NotVirtualIsOfType( ASTtype T );

 private:
  ASTlist<DVNWithPos *> *myKids;

public:
  // Returns the length of the longest string in this object.
  // Used by Unparse() at various levels of the AST tree.
  // (In this case, this is the length of the longest value in
  // the list.)
  virtual int myLongestStr( void );

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };
};


/// Depreciated name for DataLoopNameListNode for backward compatability:
#define DataLoopDefListNode DataLoopNameListNode

//-----------------------------------
//  class DataLoopNameListNode
//-----------------------------------
/** @memo This is a list of lists of names in a loop.  The first
  *       list of names is the list of names for the outermost loop.
  *       the second list of names is the list of names for the next
  *       nesting level in, and so on.
  */
class DataLoopNameListNode : public ASTnode, private ASTnodeWithPosBits
{
///
public:
  ///
  enum Status {
      ///
      ERROR,
      ///
      STOP,
      ///
      NEW_ITER,
      ///
      SAME_ITER };

    //@{ /** <u>Constructors</u>: */
      /// Copy constructor with a deep recursive copy
      DataLoopNameListNode( DataLoopDefListNode &L);
      /// Parallel copy constructor:
      DataLoopNameListNode( bool link, DataLoopDefListNode &L);
      /// Also a Copy Constructor.
      DataLoopNameListNode(const DataLoopDefListNode& D);
      /// 
      DataLoopNameListNode(ASTlist<LoopDefListNode *> *L);
      /// Create an empty list of lists of names.
      DataLoopNameListNode();
    //@}

  /// Destructor - recursively destroys the whole list of names.
  virtual ~DataLoopNameListNode();

  /// Print out debugging info detailing the layout of this list:
  void debugPrint( void );


  //@{ /// These are for use with the STL vector-like functions:
	///
	typedef LoopNameListNode *value_type;
	///
	typedef value_type* pointer;
	///
	typedef const value_type* const_pointer;
	///
	typedef value_type& reference;
	///
	typedef const value_type& const_reference;
	///
	typedef size_t size_type;
	///
	typedef size_t difference_type;

	typedef ASTlist<LoopNameListNode*>::iterator iterator;
	typedef ASTlist<LoopNameListNode*>::const_iterator const_iterator;
	typedef ASTlist<LoopNameListNode*>::reverse_iterator reverse_iterator;
	typedef ASTlist<LoopNameListNode*>::const_reverse_iterator const_reverse_iterator;
  //@}

// TODO - delete this commented section out once I'm convinced that the
// above typedefs work:
#ifdef BOGUS_NAME_NOT_DEFINED_SO_THIS_GETS_COMMENTED_OUT

    class iterator;
    class const_iterator;

    /** This is supposed to behave like the iterator class
      * in an STL vector.  It is not documented because the
      * reader is expected to have access to the STL vector
      * documentation, and a treatise on how to use STL vectors
      * is beyond the scope of this documentation.
      * Note that reverse_iterator and const_reverse_iterator are
      * not implemented.
      */
    class iterator
    {
	friend class DataLoopNameListNode;

	friend class const_iterator;

	///
	protected:
	    ///
	    ASTlist<LoopNameListNode*>::iterator  vIter;
	///
	public:
	    ///
	    iterator()
	    {
	    }
	    ///
	    iterator( const ASTlist<LoopNameListNode*>::iterator  &copyMe )
	    {
		vIter = copyMe;
	    }
	    ///
	    // iterator( value_type  &copyMe )
	    // {
	    // 	vIter = &copyMe;
	    // }
	    ///
	    ~iterator()
	    {
	    }
	    ///
	    bool operator==( const iterator &x ) const
	    {   return vIter == x.vIter; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return vIter != x.vIter; 
	    }
	    ///
	    reference operator*() const { return *vIter; }
	    ///
	    value_type operator->() const{ return *vIter; }
	    ///
	    iterator &operator++() 
		{ ++vIter; return *this; }
	    ///
	    iterator &operator++(int) 
		{   iterator retVal;
		    retVal = *this; ++vIter; return retVal; }
	    ///
	    iterator &operator--() 
		{ --vIter; return *this; }
	    ///
	    iterator &operator--(int) 
		{   iterator retVal;
		    retVal = *this; --vIter; return retVal; }
	    ///
	    iterator &operator+=(size_type n) 
		{ vIter += n; return *this; }
	    ///
	    iterator &operator-=(size_type n) 
		{ vIter -= n; return *this; }
    };
    /** This is supposed to behave like the iterator class
      * in an STL vector.  It is not documented because the
      * reader is expected to have access to the STL vector
      * documentation, and a treatise on how to use STL vectors
      * is beyond the scope of this documentation.
      * Note that reverse_iterator and const_reverse_iterator are
      * not implemented.
      */
    class const_iterator
    {
	friend class DataLoopNameListNode;
	///
	protected:
	    ///
	    ASTlist<LoopNameListNode*>::const_iterator  vIter;
	///
	public:
	    ///
	    const_iterator()
	    {
	    };
	    ///
	    const_iterator( const ASTlist<LoopNameListNode*>::const_iterator  &copyMe )
	    {
		vIter = copyMe;
	    }
	    ///
	    // const_iterator( const value_type  &copyMe )
	    // {
	    // 	vIter = &copyMe;
	    // }
	    ///
	    ~const_iterator()
	    {
	    }
	    bool operator==( const iterator &x ) const
	    {   return vIter == x.vIter; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return vIter != x.vIter; 
	    }
	    ///
	    const_reference operator*() const { return *vIter; }
	    ///
	    const value_type operator->() const{ return *vIter; }
	    ///
	    const_iterator &operator++() 
		{ ++vIter; return *this; }
	    ///
	    const_iterator &operator++(int) 
		{   const_iterator retVal;
		    retVal = *this; ++vIter; return retVal; }
	    ///
	    const_iterator &operator--() 
		{ --vIter; return *this; }
	    ///
	    const_iterator &operator--(int) 
		{   const_iterator retVal;
		    retVal = *this; --vIter; return retVal; }
	    ///
	    const_iterator &operator+=(size_type n) 
		{ vIter += n; return *this; }
	    ///
	    const_iterator &operator-=(size_type n) 
		{ vIter -= n; return *this; }
    };
// ENDIF BOGUS_NAME_NOT_DEFINED_SO_THIS_GETS_COMMENTED_OUT
#endif

    ///
    iterator begin() { return myKids->begin(); }
    ///
    iterator end() { return myKids->end(); }
    ///
    const_iterator begin() const { return myKids->begin(); }
    ///
    const_iterator end() const { return myKids->end(); }
    ///
    size_type  size() const { return myKids->size(); }
    ///
    bool empty() const { return myKids->empty(); }
    ///
    reference operator[]( size_type n ) { return (*myKids)[n]; }
    ///
    const_reference operator[]( size_type n) const { return (*myKids)[n]; }
    ///
    reference front() { return myKids->front(); }
    ///
    const_reference back() const { return myKids->back(); }
    ///
    const_reference front() const { return myKids->front(); }
    ///
    reference back() { return myKids->back(); }

    //@{
    /** Convert from a pointer-to-LoopNameListNode into an
      * iterator for this DataLoopNameListNode class.  This is an operation
      * that takes linear time, so use it with care.  If the
      * object refered to is not here, then the end iterator is 
      * returned.
      */
    iterator iteratorFor( const LoopNameListNode *ptr );
    //@}

      //--------------------------------------------------
     // insert() and erase()  //
    //--------------------------------------------------
    //@{  
	/**
	  * Insertions/Deletions.  Inserting new loop levels' names,
	  * and deleting a level of loop names. <p>
	  * <u>Insert</u> - Insert before 'pos', the row of tagnames given by
	  *          'v'.  Note that what this means is that you are
	  *          inserting a new nesting level to the loop.  If this
	  *          is already inside a DataLoopNode, then you are only
	  *          allowed to add new nesting levels at the deepest level,
	  *          i.e. 'pos' must == 'end()' or it will refuse to do it
	  *          and return false.  This is because if you inserted a
	  *          new nesting level in the middle, then values that are
	  *          at the levels deeper than that are no longer associated
	  *          with the higher-level row that they are supposed to
	  *          "hang" off from.
	  *          <p>
	  *          If this DataLoopNameListNode is not inside a DataLoopNode,
	  *          then you can insert and remove nesting levels at will.
	  *          <p>
	  * the passed vector itself (in other words, you can delete
	  * it after calling (and the DataNameNodes it points to).
	  */

	/// Make a copy of the vector version:
	bool insert( iterator               pos,
		     vector<DataNameNode*>  &v );

	/** <u>Insert</u> - Insert before 'pos', the LoopNameListNode given.
	  * <p>
	  * (This function performs a DEEP copy of the given name def list
	  * so everything in the passed argument can be deleted after
	  * calling, if it is appropriate.)
	  */
	bool insert( iterator pos, LoopNameListNode &v );

	/** Make a link to the node given (NOT A COPY - don't delete the node
	  * pointer that was passed).
	  */
	bool insert( iterator pos, LoopNameListNode *v );

	/** erase - Remove the row of tagnames given.  If this is
	  *         inside a DataLoopNode, then calling this method
	  *         also removes all the values from the body of the
	  *         loop that were associated with these names.
	  *         <p>
	  *         In order to keep the STAR syntax correct,
	  *         deleting a row of names also results in all
	  *         tagnames for levels nested AFTER that row
	  *         being deleted also.  (If there are three nested
	  *         levels in a loop and you delete the middle level,
	  *         then the inner level goes away too.)
	  *         <p>
	  *         That is why there is no erase() method for erasing
	  *         a range of rows of names.  There would be no point,
	  *         seeing as how once one is deleted, the rest under it
	  *         have to be deleted too.
	  */
	void erase( iterator pos );
  //@}

  ///
  virtual void Unparse(int indent);
  virtual void Unparse(int indent, bool isEmpty);
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
  ///
public:


  /// <em>DEPRECIATED - use the STL vector notation instead: </em>
  StarFileNode::Status AddDataName(const string & name);
  /// <em>DEPRECIATED - use the STL vector notation instead: </em>
  StarFileNode::Status RemoveDataName( const string & name,
	                               int   *nest = (int*)NULL,
				       int   *ele = (int*)NULL  );

  /// <em>DEPRECIATED - use the STL vector notation instead: </em>
  StarFileNode::Status ChangeName( const string &oldName, const string &newName );
  /// <em>DEPRECIATED - use the STL vector notation instead: </em>
  void reset();
  /// <em>DEPRECIATED - use the STL vector notation instead: </em>
  Status getNextTag(DataLoopValListNode::Status vstat,
		    int & row);
  /** Return the 'name' of this node (the name of the first tag in the
    * outermost nesting level is arbitrarily chosen as the "name" of
    * this node.
    */
  string myName() const {myKids->Reset(); return (myKids->Current()->myName());};
  /// <em>DEPRECIATED - use the STL vector notation instead: </em>
  void FlattenNestedLoop(List<DataNameNode*>* L);

  /** <em>DEPRECIATED - use tagPositionDeep instead</em><br>
    * Get the index of the given tag, if it resides in the outermost
    * loop only (is incapable of searching into nested tags.)
    * (This search is case-insensitive).
    * @see tagPositionDeep
    */
  int tagPosition(const string & TagName);

  /** Get the index of the given name.  Returns the
    * nest depth and the column index within that nest depth.
    * (indexes start counting at zero, negative numbers returned
    * mean the tag was not found.)
    * The search is case-insensitive.
    */
  void tagPositionDeep( string tagName, int *nestLevel, int *column );




    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /**
    * Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <p>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param searchFor Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor );

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type type to search for
    * @param delim the delimiter type (not relevant at this scope )
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int delim = -1 );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

 ///
 private:
  ///
  ASTlist<LoopNameListNode *> *myKids;

  int currRow;

///
public:
  /**
    * Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it takes the longest tagname from the name list)
    */
  virtual int myLongestStr( void );

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };
///
private:
    virtual bool copyFrom( DataLoopNameListNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

// depreciated name for LoopTableNode for backward compatibility:
#define LoopIter LoopTableNode


// -------------------
// globalSetTabFlag()
// -------------------
//  This is an ugly hack to get around the fact that I cannot call
//  LoopTableNode::setTabFlag() directly from inside LoopRowNode.
//  (I can't because at that point in the file, LoopTableNode is
//  still an incomplete class defnition and its methods have not been
//  declared yet.  But LoopTableNode needs to call LoopRowNode's methods
//  too, so no matter which comes first in the file it won't work.  This
//  global function gets around the problem.)
//
void globalSetTabFlag( LoopTableNode *tbl, bool value );

/// DEPRECIATED name for LoopRowNode for backward compatability:
#define IterNode LoopRowNode

class LoopRowCol
{
    friend class LoopRowNode;
public:
    LoopRowNode *lrn;
    short       col;
    bool operator()( const LoopRowCol a, const LoopRowCol b ) const
    {
	if( a.lrn == b.lrn )
	{
	    // Special case - lie and claim that if a or b is -1,
	    // then the two are equal.  This renders -1 into
	    // a "wildcard" value when trying to search for
	    // LoopRowCols in an STL map.  (A search for lrn = foo
	    // and col = -1 will find ALL values where lrn = foo.)
	    if( a.col == -1 || b.col == -1 )
		return false;
	    // Normal case: compare columns.
	    return a.col < b.col;
	}
	else
	    return a.lrn < b.lrn;
    }
    // This operator will lie by claiming that a LoopRowCol with
    // a column value of -1 is a "don't-care" value that matches
    // all columns.  (I.E. an attempt to compare a LoopRowCol where
    // lrn=0x1234abcd, and col = -1 will be a match for *any* column
    // in that row.
    // Also, this operator will lie by checking to see if both columns
    // are past the end of the loop row node.  If they are, it will
    // claim they are equal.  This allows the iterator end() operator
    // to work inside LoopRowNode.  (As soon as an iterator goes past
    // the end of the row, it will match any other 'past end' iterator,
    // even if one is, say, 1 past the end and the other is 100 past the
    // end.  For the purpose of iterators, they are both equal to 'end'.)
    bool operator==( const LoopRowCol compareTo ) const;

    LoopRowCol()
    {
	// Default to an invalid value:
	col = SHRT_MAX;
    };
    LoopRowCol( LoopRowNode *r, short c )
    {
	lrn = r;
	col = c;
    };
};

/** ValCache is a global class, with static members.
  * It is designed to hold a cache of data value nodes for
  * loop rows in all tables.
  */
class ValCache
{
friend class LoopTableNode;
friend class LoopRowNode;
friend class DataValueNode;
friend class DataLoopNode;

private:
   static map<const LoopRowCol, DataValueNode*, LoopRowCol>  cache;

protected:
  /** Add a datavaluenode to the value cache for the given row/col.
    * if it already exists in the cache, this will overwrite the
    * pointer and orphan the old one!*/
  static void addToValCache( LoopRowNode *lrn, short col, DataValueNode *val );

  /** Move an item if the ValCache to a new key.
    */
  static void moveInValCache( LoopRowNode *lrnFrom, short colFrom,
	               LoopRowNode *lrnTo, short colTo );
  /** Get a datavaluenode from the value cache or return NULL if none
    * has been inserted at that point yet.
    */
  static DataValueNode *&getFromValCache( LoopRowNode *lrn, short col );

  static void removeFromValCache( LoopRowNode *lrn, short col );

  static void clear( void ) { ValCache::cache.clear(); };


public:
  /** Flush all cached DataValueNodes in all star trees
    * in the library.  Once called, no DataValueNode pointer
    * variables held by the caller (you) should be trusted
    * to be valid anymore.  They are all pointing to deleted
    * objects that don't "officially" exist anymore.
    * The starlib2 maintains a cache of DataValueNodes
    * generated as needed.  Behind the scenes, there are no real
    * DataValueNodes in loops - they are generated from a
    * more compact internal format as they get referenced.
    * Because C++ has no garbage collection and no way for
    * a library to tell when a pointer or reference in the
    * calling code goes out of scope, the library must rely
    * on the calling code to occasionally call this function
    * when all DataValueNode pointers are done being used.
    */
  static void flushValCache( void );
  /// dump the whole cache to stdout:
  static void debugDump( void );
};

//--------------------------
//   class LoopRowNode
//--------------------------
/** This class is a single row of values in a loop.
  * <p>
  * A single row of values in the loop is a list (STL vector) of
  * DataValueNodes.  In addition each loop row can have a table
  * of values hanging "under" it at the next nesting level. And the
  * rows of that table can have other tables under them, and so on.
  */
class LoopRowNode : public ASTnode, private ASTnodeWithPosBits
{
    friend class LoopTableNode;
    friend class DataLoopNode;
    friend class LoopNameListNode;
    friend class ValCache;
    friend class LoopRowCol;

///
public :
  //-------------------------------------------
  // getPreComment() / setPreComment()
  //-------------------------------------------
  //@{
      ///
      virtual const string  &getPreComment( void )
      {
          return preComment;
      };
      ///
      virtual void          setPreComment( const string &cmt )
      {
          preComment = string(cmt);
      };


    //@{
          /** These types are for use with the STL vector-like
            * methods of this class.
            */
	///
	typedef DataValueNode *value_type;
	///
	typedef value_type* pointer;
	///
	typedef const value_type* const_pointer;
	///
	typedef value_type& reference;
	///
	typedef const value_type& const_reference;
	///
	typedef size_t size_type;
	///
	typedef size_t difference_type;

    //@}


    class iterator;
    class const_iterator;

    /** This iterator class is meant to behave just like the
      * iterator class inside an STL vector.  This is not
      * documented because the STL vector interface is large and
      * beyond the scope of this document.  It is expected that
      * the reader will have access to STL vector documentation,
      * in which case the way to use these functions will be
      * immediately apparent, as they share the same name as their
      * STL vector counterparts:
      * Note that reverse_iterator and const_reverse_iterator are
      * not implemented.
      */
    class iterator
    {
	friend class LoopRowNode;

	friend class const_iterator;

	///
	protected:
	    ///
	    LoopRowCol  here;
	///
	public:
	    ///
	    iterator()
	    {
	    };
	    ///
	    iterator( const LoopRowCol  &copyMe )
	    {
		here = copyMe;
	    }
	    // This was removed because it doesn't work with the cache.
	    // iterator( value_type  &copyMe )
	    // {
	    //	   here = &copyMe;
	    // }
	    //
	    ~iterator()
	    {
	    }
	    ///
	    bool operator==( const iterator &x ) const
	    {   return here == x.here; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return !(here == x.here); 
	    }
	    ///
	    reference operator*() const
	    { return here.lrn->elementAt(here.col); }
	    ///
	    value_type operator->() const
	    { return here.lrn->elementAt(here.col); }
	    ///
	    iterator &operator++()
	    {   ++(here.col);
		if( here.col >= (signed) here.lrn->size() )
		{   here.col = here.lrn->size() ;
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    iterator &operator++(int)
	    {   iterator retVal;
		retVal = *this;
		++(here.col);
	        if(  here.col >= (signed) here.lrn->size() )
		{   here.col = here.lrn->size() ;
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    iterator &operator--()
	    {   --(here.col);
	        if(  here.col >= (signed) here.lrn->size() )
		{   here.col = here.lrn->size() ;
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    iterator &operator--(int)
	    {   iterator retVal;
		retVal = *this;
		--(here.col);
	        if(  here.col >= (signed) here.lrn->size() )
		{   here.col = here.lrn->size() ;
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    iterator &operator+=(size_type n)
	    { here.col += n;
	        if( here.col >= (signed) here.lrn->size() )
		{   here.col = here.lrn->size() ;
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    iterator &operator-=(size_type n)
	    {   here.col -= n;
	        if( here.col >= (signed) here.lrn->size() )
	        {   here.col = here.lrn->size() ;
 		    return *this;
	        }
	        else
	 	    return *this;
	    }
    };
    /** This iterator class is meant to behave just like the
      * iterator class inside an STL vector.  This is not
      * documented because the STL vector interface is large and
      * beyond the scope of this document.  It is expected that
      * the reader will have access to STL vector documentation,
      * in which case the way to use these functions will be
      * immediately apparent, as they share the same name as their
      * STL vector counterparts:
      * Note that reverse_iterator and const_reverse_iterator are
      * not implemented.
      */
    class const_iterator
    {
	friend class LoopRowNode;
	///
	protected:
	    ///
	    LoopRowCol here;
	///
	public:
	    ///
	    const_iterator()
	    {
	    };
	    ///
	    const_iterator( const LoopRowCol &copyMe )
	    {
		here = copyMe;
	    }
	    // This was removed because it doesn't work with the cache.
	    // iterator( const value_type  &copyMe )
	    // {
	    //	   here = &copyMe;
	    // }

	    ///
	    ~const_iterator()
	    {
	    }
	    ///
	    bool operator==( const iterator &x ) const
	    {   return here == x.here; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return !( here == x.here );  // Do it this way because only the
		                             // == operator is implemented for
					     // LoopRowCol.
	    }
	    ///
	    const_reference operator*() const
	    { return here.lrn->elementAt(here.col); }
	    ///
	    const value_type operator->() const
	    { return here.lrn->elementAt(here.col); }
	    ///
	    const_iterator &operator++()
	    { ++(here.col);
	        if( here.col >= (signed) here.lrn->size() )
		   
		{   here.col = here.lrn->size();
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    const_iterator &operator++(int)
	    {   const_iterator retVal;
		retVal = *this;
		++(here.col);
	        if( here.col >= (signed) here.lrn->size() )
		{   here.col = here.lrn->size();
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    const_iterator &operator--()
	    { --(here.col);
	        if( here.col >= (signed) here.lrn->size() )
		{   here.col =  here.lrn->size();
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    const_iterator &operator--(int)
	    {   const_iterator retVal;
		retVal = *this;
		--(here.col);
	        if( here.col >= (signed) here.lrn->size() )
		{   here.col = here.lrn->size();
		    return *this;
		}
		else
		    return *this;
	    }
	    ///
	    const_iterator &operator+=(size_type n)
		{   here.col += n;
		    if( here.col >= (signed) here.lrn->size() )
		    {   here.col = here.lrn->size();
			return *this;
		    }
		    else
			return *this;
		}
	    ///
	    const_iterator &operator-=(size_type n)
		{   here.col -= n;
		    if( here.col >= (signed) here.lrn->size() )
		    {   here.col = here.lrn->size(); 
			return *this;
		    }
		    else
			return *this;
		}
    };

    ///
    iterator begin()
    {   iterator retVal;
	retVal.here.lrn = this;
	retVal.here.col = 0;
	return retVal;
    };
    ///
    iterator end()
    {   iterator retVal;
	retVal.here.lrn = this;
	retVal.here.col = size();
	return retVal;
    }
    ///
    const_iterator begin() const
    {   const_iterator retVal;
	retVal.here.lrn = UNDO_CONST_PTR(LoopRowNode,this);
	retVal.here.col = 0;
	return retVal;
    };
    ///
    const_iterator end() const
    {   const_iterator retVal;
	retVal.here.lrn = UNDO_CONST_PTR(LoopRowNode,this);
	retVal.here.col = size();
	return retVal;
    }
    ///
    size_type  size() const { return myStarts.size(); }
    ///
    bool empty() const { return (myStarts.size() == 0); }
    ///
    reference operator[]( size_type n ) { return elementAt(n); }
    ///
    // const_reference operator[]( size_type n) const { return elementAt(n); }

    // deleted front/back methods - nobody was using them, and they were
    // redundant.

    //@{
    /** Convert from a pointer-to-DataValueNode into an
      * iterator for this LoopRowNode class.  This is an operation
      * that takes linear time, so use it with care.  If the
      * object refered to is not here, then the end iterator is 
      * returned.
      */
    iterator iteratorFor( const DataValueNode *ptr );
    //@}

    //@{
	/** <pre>
	  *--------------------------------------------------
	  * insert() and erase()    *
	  *--------------------------------------------------
	  * The following functions will only work if this LoopRowNode has not
	  * yet been attached to a loop in a STAR tree.  if this LoopRowNode
	  * *HAS* been attached to a loop in the STAR tree already, then these
	  * methods will be disallowed because that would cause a mismatch
	  * between the number of values in the loop row and the number of
	  * tagnames for the loop.
	  *
	  *    Thus this can work:
	  *           DataLoopNode  some_loop;
	  *           LoopRowNode  some_row;
	  *           ...
	  *           some_row.insert( some_row.end(),
	  *                            new DataValueNode( "hello" );
	  *           some_row.insert( some_row.end(),
	  *                            new DataValueNode( "world" );
	  *           some_loop.getVals.insert( begin(), some_row );
	  *               * (it only works assuming that two values per row is
	  *               * the proper amount for some_loop, else it's an error.)
	  *
	  *    But this will not work:
	  *           DataLoopNode  some_loop;
	  *           LoopRowNode  some_row;
	  *           ...
	  *           ...
	  *           some_loop.getVals[0].insert( end(),
	  *                                        new DataValueNode( "hello " );
	  *               * (You cannot insert or delete values from a row
	  *               * of data that is already in a loop.  You can only
	  *               * insert or delete entire rows at a time from an
	  *               * existing loop.)
	  *

	  *
	  *   Insert - Insert before 'pos', the data value given by
	  *            'v'.  Returns true if it worked or false otherwise.
	  *            (Will fail if the number of values does not match
	  *            what is in the loop's tagname list.)
	  *
	  * <pre>
	  */

	/**
	  * This allocates a copy of the data, rather than a pointer to it,
	  * so if the caller is passing a heap-allocated DataValueNode, the
	  * caller needs to delete it later.
	  */
	bool insert( iterator pos, DataValueNode &v );
	/** This version is used when the caller wants to GIVE a
	  * newly created DataValueNode to the library, meaning that
	  * instead of making a copy of the DataValueNode in the
	  * loop row, the actual DataValueNode passed in will be linked
	  * directly into the row.  The caller should not delete the
	  * node after using this version.
	  */
	bool insert( iterator pos, DataValueNode *v );

	/// erase - Remove the value given.
	bool erase( iterator pos );

	// commented out - too much work, and nobody uses it:
	//   (erases the range [start,finish) (not including finish). )
	// bool erase( iterator start, itareator finish );
    //@}
    
    //@{
	/** elementAt() - get the DataValueNode at the position given.
	  * Counting starts at zero as the first element.
	  */
	DataValueNode *&elementAt( const short pos );
        /** insertElementAt() - insert a new value at the position
	  * given (shifting all other values forward).
	  */
	void insertElementAt( const short pos, DataValueNode &v );
	/** removeElementAt() - erase a value at the position given
	  * (shifting all other values downward).
	  */
	void removeElementAt( const short pos );
	/** Set the value in-place at a particular column in the row
	  */
	void setElementAt( const short pos, DataValueNode &v );


	/** stringAt() - gets the string value at the position
	  * given rather than the entire DataValueNode.
	  * (The name "stringElementAt" was chosen because C++ cannot
	  * differentiate overloaded methods that differ only by their
	  * return type.)
	  */
	string stringAt( const short pos );
        /** insertStringAt() - insert a new value at the position
	  * given (shifting all other values forward).
	  * The delimiter type will be chosen by examining the contents
	  * of the string to determine if any delimiter is required.
	  */
	void insertStringAt( const short pos, const string &val );
	/** Set the value in-place at a particular column in the row.
	  * The delimiter type will be chosen by examining the contents
	  * of the string to determine if any delimiter is required.
	  */
	void setStringAt( const short pos, const string &val );
    //@}

    /** isInLoop: Returns true if this LoopRowNode is inside a DataLoopNode,
      * or false if it is not.  (In a complete STAR tree it is always true,
      * but when building up a loop piece by piece an LoopRowNode might not
      * have been attached to a loop yet.)
      * <p>
      * The value returned is an integer.  It is a count of how many nesting
      * levels deep this is in the loop that it was found it.  Thus if
      * it is zero (false) it was not found in a loop, and if it is 1 then
      * it was found in nesting level 1 of a loop, 2 = nesting level 2, etc.
      * <p>
      * Note that this is off-by-one with the index used in the [] operator,
      * which starts counting at zero.  This was done so that this method
      * could be used like a boolean. (zero means not found, nonzero means
      * found).
      */
    int isInLoop(void);

      //--------------------------------------------------
     //   myLoop() //
    //--------------------------------------------------
    /**  Returns a pointer to the inner loop that is under this
      *  row.  If no such loop, a NULL is returned.  Note that
      *  even if a loop is returned, that this loop might have
      *  zero rows in it, which should be treated the same as
      *  not having an inner loop at all.
      */
    LoopTableNode    *myLoop( void ) { return innerLoop; };

    /** Sets the interior loop under this row.  Not to be
      * used outside the parser, but it needs to be made
      * public anyway due to technical difficulties in the
      * bison parser - there is no guarantee that this
      * routine will work correctly in the future.
      * @param tbl LoopTableNode to place under this row as an
      *        inner table.
      */
    void setMyLoop( LoopTableNode *tbl );


    //@{  /** <u>Constructors</u>: */
      /** @param tflag true = loop is tabular, false = loop is linear
        *              (only important when outputting with Unparse()).
	*/
      LoopRowNode( const bool tflag );
      /** @param tflag true = loop is tabular, false = loop is linear
        *              (only important when outputting with Unparse()).
	* @param initSize = the initial allocated capacity of the vector.
	*    (Not the actual size of the vector)
	*/
      LoopRowNode( const bool tflag, size_t initSize );

      /// Copy Constructor.
      LoopRowNode( IterNode& N);

      /** Copy with parallel link.  Set "link" to true to create
        * a copy with a parallel link, or set it to false to create
        * a copy without a parallel link.  See the external documentation
        * for more details on parallel copies.
	*/
      LoopRowNode( bool link, IterNode& N);

      /** Make a new Iternode with the values given in the vector.
        * (ASTlist is derived from vector, so an ASTlist can be used
        * as the argument as well.)
	*/
      LoopRowNode( vector<DataValueNode*> &v, const bool tflag = false );
    //@}

  /// Destructor - recursively destructs the values in the loop too.
  virtual ~LoopRowNode();

/// Print the output of this row of the list:
virtual void Unparse(int indent) { Unparse( indent, NULL, NULL, NULL ); };

/** This version is for internal use only, but had to be made public
  * for technical reasons - don't call it.  Call the other version of
  * Unparse()
  */
virtual void Unparse(int indent, List<int> *presizes, List<int> *postsizes,
		    List<int> *allNonQuoteds );
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );

///
public:

  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void addValue(DVNWithPos *value);

  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void addInnerLoopValue(const DataLoopNameListNode::Status dstat,
			 const int row,
			 const int currRow,
			 DVNWithPos *value);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void FlattenNestedLoop(List<DataValueNode*>* M);

  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void reset();
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  DataLoopValListNode::Status returnNextLoopElement(DataValueNode* & v);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  DataValueNode* returnLoopValue(unsigned position);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status  RemoveColumnValues(
	int          nest,
	int          ele
	);
/* end of COMMENTING_OUT */

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * @param type type to search for.
    * @param delim the delimiter type to search for if searching for
    *              DataValueNode's (default is "dont-care").
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int  delim = -1 );

    //--------------------------------------------------
   //  setTabFlag() / getTabFlag()  //
  //--------------------------------------------------
  //@{
      ///
      void setTabFlag( bool setTo )
      {   bitFields.tFlag = setTo;
	  if( innerLoop != NULL )
	      globalSetTabFlag( innerLoop, bitFields.tFlag );
      }
      ///
      bool getTabFlag( void ) { return bitFields.tFlag; };

      /** Sets (or gets) the tabulation flag for this row, and
        * all nested loops inside it.  true = tab, false = linear.
	*/
  //@}


    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

///
private :
  ///
  /// Was this in version 1: ASTlist <DataValueNode *> myVals;
  ///
  string       myVal;
  short        cur;
  vector<short>  myStarts;
  ///
  LoopTableNode *innerLoop;
  ///
  string preComment;

///
public:
  void debugDump( void );

  /** Flushes the value cache for this row.  Call this routine
    * whenever you are done using the values in this row.
    * Failure to do so will result in an excessive memory footprint
    * for this program if you iterate over large loops, but doing
    * so to often merely results in slow (but correct) operation
    * as the same value nodes are regenerated and thrown away
    * multiple times.  When in doubt, do it a lot.
    * Note that after calling flushValCache(), any DataValueNode
    * pointers you are holding onto from this row will become freed
    * and invalid.
    * <TABLE BORDER=2 WIDTH=70%>
    *     <TR><TH>VALID USAGE</TH>
    *         <TD>
    *             <PRE>
    *    LoopRowNode *aRow = (*myTable)[3]; //Fourth row in some table
    *    // Concatenate first two columns, put result in the first one.
    *    DataValueNode firstVal = (*aRow)[0];
    *    DataValueNode secondVal = (*aRow)[1];
    *    string concat = firstVal->myValue() + secondVal->myValue();
    *    firstVal->setValue( concat );
    *    aRow->flushValCache();
    *             </PRE>
    *         </TD>
    *     </TR>
    *     <TR>
    *     <TH>INVALID USAGE!</TH>
    *         <TD>
    *             <PRE>
    *    LoopRowNode *aRow = (*myTable)[3]; //Fourth row in some table
    *    // Concatenate first two columns, put result in the first one.
    *    DataValueNode firstVal = (*aRow)[0];
    *    DataValueNode secondVal = (*aRow)[1];
    *    string concat = firstVal->myValue() + secondVal->myValue();
    *    aRow->flushValCache();
    *       // At this point firstVal is invalid, so it
    *       // should not be used like this anymore:
    *    firstVal->setValue( concat );
    *             </PRE>
    *         </TD>
    *     </TR>
    * </TABLE>
    */
  virtual void flushValCache(void);

  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it is the longest datavalue from the list: )
    */
  virtual int myLongestStr( void );

  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

///
private:
    virtual bool copyFrom( LoopRowNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link

    /** Generate the new DataValueNode given the field column in
      * this row.  Note that this does not put it in the map
      */
    DataValueNode *generateVal( short col );
    void privateSetElementAt( short col, DataValueNode &dvn );
    void privateInsertElementAt( short col, DataValueNode &dvn );
    void internalInsertElementAt( short col, DataValueNode &dvn,
	                          bool internalFlag );
    void privateRemoveElementAt( short col );
    void internalRemoveElementAt( short col, bool internalFlag );
};


//---------------------------
//   class LoopTableNode
//----------------------------
/** A LoopTableNode is a table of rows in a DataLoopNode.
  * <p>
  * You can think of a loop in a STAR file as being a table of
  * values, with each iteration of the loop being a row on the
  * table.  This is the view taken by the starlib.  Things are 
  * further complicated by the fact that each row of the table can
  * have another table under it (another nesting level), but even
  * then, those tables are the same structure as the outermost one.
  * <p>
  * Thus, this class, LoopTableNode, stores a table at some arbitrary
  * nesting level in the loop.  A simple singly nested loop will
  * have only one loop table node, but a multiply nested loop will
  * have a whole tree of loop tables.
  */
class LoopTableNode : public ASTnode, private ASTnodeWithPosBits
{
    friend class LoopRowNode;
    friend class LoopNameListNode;
    friend class DataLoopNameListNode;
    friend class DataLoopNode;
///
public :

    //@{
          /** These types are used with the STL vector look-alike
            * functions:
	    */
	///
	typedef LoopRowNode *value_type;
	///
	typedef value_type* pointer;
	///
	typedef const value_type* const_pointer;
	///
	typedef value_type& reference;
	///
	typedef const value_type& const_reference;
	///
	typedef size_t size_type;
	///
	typedef size_t difference_type;

	typedef ASTlist<LoopRowNode*>::iterator iterator;
	typedef ASTlist<LoopRowNode*>::const_iterator const_iterator;
	typedef ASTlist<LoopRowNode*>::reverse_iterator reverse_iterator;
	typedef ASTlist<LoopRowNode*>::const_reverse_iterator const_reverse_iterator;
    //@}
// TODO - delete this commented section out once I'm convinced that the
// above typedefs work:
#ifdef BOGUS_NAME_NOT_DEFINED_SO_THIS_GETS_COMMENTED_OUT

    class iterator;
    class const_iterator;

    /** This iterator class is meant to behave just like the
      * iterator class inside an STL vector.  This is not
      * documented because the STL vector interface is large and
      * beyond the scope of this document.  It is expected that
      * the reader will have access to STL vector documentation,
      * in which case the way to use these functions will be
      * immediately apparent, as they share the same name as their
      * STL vector counterparts:
      * Note that reverse_iterator and const_reverse_iterator are
      * not implemented.
      */
    class iterator
    {
	friend class LoopTableNode;

	friend class const_iterator;

	///
	protected:
	    ///
	    ASTlist<LoopRowNode*>::iterator  vIter;
	///
	public:
	    ///
	    iterator()
	    {
	    };
	    ///
	    iterator( const ASTlist<LoopRowNode*>::iterator  &copyMe )
	    {
		vIter = copyMe;
	    }
	    ///
	    // iterator( value_type  &copyMe )
	    // {
	    //	vIter = &copyMe;
	    // }
	    ///
	    ~iterator()
	    {
	    }
	    ///
	    bool operator==( const iterator &x ) const
	    {   return vIter == x.vIter; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return !(vIter == x.vIter); 
	    }
	    ///
	    reference operator*() const { return *vIter; }
	    ///
	    value_type operator->() const{ return *vIter; }
	    ///
	    iterator &operator++() 
		{ ++vIter; return *this; }
	    ///
	    iterator &operator++(int) 
		{   iterator retVal;
		    retVal = *this; ++vIter; return retVal; }
	    ///
	    iterator &operator--() 
		{ --vIter; return *this; }
	    ///
	    iterator &operator--(int) 
		{   iterator retVal;
		    retVal = *this; --vIter; return retVal; }
	    ///
	    iterator &operator+=(size_type n) 
		{ vIter += n; return *this; }
	    ///
	    iterator &operator-=(size_type n) 
		{ vIter -= n; return *this; }
    };
    /** This iterator class is meant to behave just like the
      * iterator class inside an STL vector.  This is not
      * documented because the STL vector interface is large and
      * beyond the scope of this document.  It is expected that
      * the reader will have access to STL vector documentation,
      * in which case the way to use these functions will be
      * immediately apparent, as they share the same name as their
      * STL vector counterparts:
      * Note that reverse_iterator and const_reverse_iterator are
      * not implemented.
      */
    class const_iterator
    {
	friend class LoopNode;
	///
	protected:
	    ///
	    ASTlist<LoopRowNode*>::const_iterator  vIter;
	///
	public:
	    ///
	    const_iterator()
	    {
	    };
	    ///
	    const_iterator( const ASTlist<LoopRowNode*>::const_iterator  &copyMe )
	    {
		vIter = copyMe;
	    }
	    ///
	    // const_iterator( const value_type  &copyMe )
	    // {
	    //	vIter = &copyMe;
	    // }
	    ///
	    ~const_iterator()
	    {
	    }
	    ///
	    bool operator==( const iterator &x ) const
	    {   return vIter == x.vIter; 
	    }
	    ///
	    bool operator!=( const iterator &x ) const
	    {   return !(vIter == x.vIter); 
	    }
	    ///
	    const_reference operator*() const { return *vIter; }
	    ///
	    const value_type operator->() const{ return *vIter; }
	    ///
	    const_iterator &operator++() 
		{ ++vIter; return *this; }
	    ///
	    const_iterator &operator++(int) 
		{   const_iterator retVal;
		    retVal = *this; ++vIter; return retVal; }
	    ///
	    const_iterator &operator--() 
		{ --vIter; return *this; }
	    ///
	    const_iterator &operator--(int) 
		{   const_iterator retVal;
		    retVal = *this; --vIter; return retVal; }
	    ///
	    const_iterator &operator+=(size_type n) 
		{ vIter += n; return *this; }
	    ///
	    const_iterator &operator-=(size_type n) 
		{ vIter -= n; return *this; }
    };
// ENDIF BOGUS_NAME_NOT_DEFINED_SO_THIS_GETS_COMMENTED_OUT
#endif

    ///
    iterator begin() { return myIters.begin(); }
    ///
    iterator end() { return myIters.end(); }
    ///
    const_iterator begin() const { return myIters.begin(); }
    ///
    const_iterator end() const { return myIters.end(); }
    ///
    size_type  size() const { return myIters.size(); }
    ///
    bool empty() const { return myIters.empty(); }
    ///
    reference operator[]( size_type n ) { return (myIters[n]); }
    ///
    const_reference operator[]( size_type n) const { return (myIters[n]); }
    ///
    reference front() { return myIters.front(); }
    ///
    const_reference back() const { return myIters.back(); }
    ///
    const_reference front() const { return myIters.front(); }
    ///
    reference back() { return myIters.back(); }
    ///
    void reserve( size_type n ) { myIters.reserve( n ); } ;

    //@{
    /** Convert from a pointer-to-LoopRowNode into an
      * iterator for this LoopTableNode class.  This is an operation
      * that takes linear time, so use it with care.  If the
      * object refered to is not here, then the end iterator is 
      * returned.
      */
    iterator iteratorFor( const LoopRowNode *ptr );
    //@}

    //@{
	/**
	  * <u>Insertions/Deletions</u>.  These cannot be done in exactly the
	  * same syntax as vector<> because we need to check for things that
	  * are illegal in STAR and disallow them.  However, it should look
	  * quite familiar to someone who is fluent in the STL vector class.
	  */

	/** <pre>
	  * Insert - Insert before 'pos', the row of data given by
	  *            'v'.  Returns true if it worked or false otherwise.
	  *            (Will fail if the number of values does not match
	  *            what is in the loop's tagname list.)
	  * </pre>
	  * (This function makes a copy of the passed vector, it does not use
	  * the passed vector itself (in other words, you can delete
	  * it after calling (and the DataValueNodes it points to).
	  */
	bool insert( iterator                pos,
		     vector<DataValueNode*>  &v,
		     const bool              tflag );

	/** <pre>
	  * Insert - Insert before 'pos', the LoopRowNode given.  If the
	  *            LoopRowNode given has the wrong number of values, it
	  *            will fail and return false, else it returns true.
	  * <pre>
	  * (This function makes a copy of the passed LoopRowNode, it does not use
	  * the passed LoopRowNode itself (in other words, you can delete
	  * it after calling (The LoopRowNode and the DataValueNodes IN the
	  * LoopRowNode).)
	  *
	  * This version makes a deep copy of the node given:
	  */
	bool insert( iterator pos, LoopRowNode &v );
	/** This version makes a link to the node given, so don't
	  * delete it afterword:  (You are "giving" the new node to
	  * the class after having allocated it yourself.)
	  */
	bool insert( iterator pos, LoopRowNode *v );

	/** <pre>
	  * Insert - Insert before 'pos', the table of data given
	  *            by the LoopTableNode.  If the layout of values is not
	  *            consistent with the layout defined by the names
	  *            in the DataLoopNode, it will fail and return false,
	  *            else it returns true.
	  * </pre>
	  * (This function performs a DEEP COPY of the given loop node, so
	  * everything in the passed argument can be deleted after calling,
	  * if it is appropriate.)
	  */
	bool insert( iterator pos, LoopTableNode &t );
	/** This version makes a shallow link instead of a deep copy, so
	  * do not delete the node after passing it.  (You are "giving"
	  * the node to the library, after having allocated it yourself.)
	  */
	bool insert( iterator pos, LoopTableNode *t );

	/**
	  *   erase - Remove the row (LoopRowNode) given.  If there is any nested
	  *           table under it, it gets removed as well.
	  *
	  */
	void erase( iterator pos );

	/**   erase - Remove the range of rows given, using typical STL range
	  *           conventions: The range is inclusive of the start, but
	  *           exclusive of the end:  [from,to)
	  */
	void erase( iterator from, iterator to );
    //@}

    //@{
        /** These methods provide a shortcut to get to the data inside
	  * the loop:
	  */

	/** This method retrieves the value at the specified row and
	  * column (indeces start at zero).  If no such row or column
	  * exists in the table, then a null-length string is returned.
	  */
	string getString( int row, int col );

	/** This method sets the value at the specified row and column
	  * to the given value.  The delimiter type is not passed, but
	  * is calculated based on the contents of the string.
	  */
	void setString( int row, int col, const string &val );
    //@}

    /** Gets the number of columns in this LoopTableNode, by looking up
      * at the list of names for this loop.  If this LoopTableNode is not
      * inside a DataLoopNode yet, then a zero is returned.
      */
    int  getNumCols( void );


    //@{   /// <u>Constructors</u>:
      ///
      LoopTableNode(const string &tflag, bool indentFl = true, int rowsPerLn = 1 );

      /// call with tflag = true for "tabulate" or false for "linear"
      LoopTableNode( bool tflag );

      /// Copy Constructor.
      LoopTableNode( LoopIter& I);

      /** Copy with parallel link.  Set "link" to true to create
        * a copy with a parallel link, or set it to false to create
        * a copy without a parallel link.  See the external documentation
        * for more details on parallel copies.
	*/
      LoopTableNode( bool link, LoopIter& I);
    //@}

  ///
  virtual ~LoopTableNode();
  ///
  virtual void Unparse(int indent);
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );

    //--------------------------------------------------
   //  setIndentFlag() / getIndentFlag()  //
  //--------------------------------------------------
  //@{
      ///
      void setIndentFlag( bool setTo ) { bitFields.indFlag = setTo; };
      ///
      bool getIndentFlag( void ) const { return bitFields.indFlag; };
      /** Sets (or gets) the boolean flag for whether or not to
	* indent the values in this loop.  When a loop is first
	* constructed, the default is "true".
	*/
  //@}

    //--------------------------------------------------
   //  setRowsPerLine() / getRowsPerLine()  //
  //--------------------------------------------------
  //@{
      ///
      void setRowsPerLine( int setTo ) { rowsPerLine = setTo; };
      ///
      bool getRowsPerLine( void ) const { return rowsPerLine; };

      /** Sets (or gets) the number of loop iterations to print on
	* one line of text when unparsing.  This only has meaning
	* when the loop is tabulated, and is ignored when the loop
	* is linearly printed.  The default is 1.
	*/
  //@}

    //--------------------------------------------------
   //  setTabFlag() / getTabFlag()  //
  //--------------------------------------------------
  //@{
      ///
      void setTabFlag( bool setTo ) ;
      ///
      bool getTabFlag( void );

      /** Sets (or gets) the tabulation flag for this loop, and
        * all nested loops inside it.  true = tab, false = linear.
	*/
  //@}

  /** Test the Loop Table to see if there are any lines where the
    * values on the line don't "align" with the names in the taglist.
    * For a loop row to be "aligned", it must match the following
    * criteria:<br>
    *   - If the loop is linear, then all rows are aligned no matter
    *   what.<br>
    *   - If the loop is tabular, then each LoopRowNode must have
    *   had all its values on the same text line in the original file
    *   for the loop to be aligned.  (Exception: If any of the values in
    *   the LoopRowNode is a semicolon string, then give up on trying
    *   to check for alignment and assume the row is aligned.)
    * <p>
    * Notes: <br>
    * - You need to delete the List<int> returned when done with it.
    * - The List<int> returned will be of zero size if no misalignments
    * were found.  It never returns as NULL.
    */
  List<int> *checkValueAlignment(void);
///
public:


  //-------------------------------------------
  // getPreComment() / setPreComment()
  //-------------------------------------------
  //@{
      ///
      virtual const string  &getPreComment( void )
      {
          return preComment;
      };
      ///
      virtual void          setPreComment( const string &cmt )
      {
          preComment = string(cmt);
      };

  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void addValue(const DataLoopNameListNode::Status dstat,
		const int row,
		const int currRow,
		DVNWithPos *value);
/* end of COMMENTING_OUT */
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void FlattenNestedLoop(List<DataValueNode*>* M);

  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void reset();
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  DataLoopValListNode::Status returnNextLoopElement(DataValueNode* & v);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  ASTlist<DataValueNode *> * returnLoopValues(unsigned position);
  
  /** <em>DEPRECIATED - use the STL vector operations instead.</em><br>
    * (Removing the tagname from the name list with the erase() operator
    * causes it to remove the values under that name.)
    */
  StarFileNode::Status  RemoveColumnValues(
	int          nest,
	int          ele
	);

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type type to search for
    * @param delim the delimiter type
    */
  virtual List<ASTnode*> * searchForType(
	  ASTtype  type,      // type to search for.
	  int      delim = -1 // The delimiter type (the enum DataValueNode::ValType)
	  );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  This function MUST be overridden for each subclass of ASTnode.
    *  @return  true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    */
  virtual bool unlinkChild( ASTnode *child );

///
private :
  ///
  ASTlist <LoopRowNode *> myIters;
  ///
  int               rowsPerLine;
  string preComment;

///
public:
  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case, it is the longest value in all the lists: )
    */
  virtual int myLongestStr( void );


  /* standard boilerplate for nodes that have line/col info stored: */
  virtual int  getLineNum(void) const    {  return posBitsGetLineNum(); };
  virtual void setLineNum(const int num) {  posBitsSetLineNum(num); };
  virtual int  getColNum(void) const     {  return posBitsGetColNum(); };
  virtual void setColNum(const int num)  {  posBitsSetColNum(num); };

  /** Flush the cache that builds up when values in this loop
    * have been examined.  This routine is needed because there is
    * no garbage collection in C.  There is no way for the
    * library to tell when the calling programmer is done using
    * a pointer.  This routine explicitly tells the library that it
    * is now safe to flush the value cache, which has the effect of
    * invalidating all DataValueNode pointers that were held by
    * the calling program.  If the value cache is not flushed as soon
    * as you are done with your data value node pointers, the memory
    * footprint of this library will be very large when processing
    * STAR files that contain long loops.
    * Calling flushValCache at the LoopNode level flushes all data
    * value node pointers within that loop.  It is also possible to
    * flush the value cache at the row and value level to just flush
    * parts of it.
    * Please be warned that any call to "Unparse" will automaticly
    * flush the value cache, and invalidate any data value node pointers
    * you may have been hanging on ot.
    */
  void flushValCache( void );

///
protected:
  virtual void calcPrintSizes(
	  List<int> **presizes,  // out: list of longest string sizes
	  List<int> **postsizes,  // out: largest number of digits after the decimal.
	  List<int> **allNonQuoteds   // out: flag for each column - true if
	                               // the entire column is all nonquoted
				       // data value nodes.  If so much as one
				       // value in the column has a delimiter,
				       // this flag is false.
	  );

///
private:
    virtual bool copyFrom( LoopTableNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
    
    //  Insert the given value at the postions shown.  Can violate the
    //  name-value matching of a loop, so it is restricted to only me
    //  and my friends.
    void insertAtPos(
	int            nest,    // Nesting level to insert at.
	int            rowPos,  // index into the row to insert before.
	const DataValueNode  &val );   // Value to insert.
    
    //  Erase the given value at the postions shown.  Can violate the
    //  name-value matching of a loop, so it is restricted to only me
    //  and my friends.
    void eraseAtPos(
	int                  nest,    // Nesting level to insert at.
	int                  rowPos); // index into the row to erase.
  
    //  Get the depth of nesting of this loop table.
    //  -1 = this loop table is not inside a DataLoopNode at this time.
    //   0 = this loop table is the outermost level for the loop.
    //   1 = this loop table is the next level in of nesting for the loop.
    //   2 = this loop table is the next level in of nesting.
    //   ...etc...
    virtual int getMyDepth( void );

    //  Get the deepest depth of nesting of the entire loop.
    //  -1 = this is not in a loop, so the question is meaningless.
    //   0 = only outermost level of nesting exists.
    //   1 = two levels of nesting
    //   2 = three levels of nesting
    //   ...etc...
    virtual int getMaxDepth( void );

    //  Make a new nesting level at the deepest point, by inserting
    //  dummy empty loops at those points where the nesting is deepest.
    //  (where the innerloops are currently NULL).
    virtual void  makeNewNestLevel( void );

    //  Truncate the loop to get rid of all nesting levels at and under
    //  the level passed (outermost level is counted as zero.)
    virtual void  truncateNestLevel( int depth );

};

  //--------------------------------------------------
 //    DataLoopNode()     //
//--------------------------------------------------
/** A DataLoopNode is a list of tagnames and a list of values
  * for those names.  It corresponds to a 'loop' in a STAR file.
  */
class DataLoopNode: public DataNode
{
  ///
  public:
    //@{  /// <u>Constructors</u>:
      ///
      DataLoopNode(DataLoopNameListNode *d, DataLoopValListNode *l,
		   const string &tflag = string("tabulate"));
      ///
      DataLoopNode(const string &tflag);

      /// Copy Constructor
      DataLoopNode( DataLoopNode& D);

      /** Copy with parallel link.  Set "link" to true to create
        * a copy with a parallel link, or set it to false to create
        * a copy without a parallel link.  See the external documentation
        * for more details on parallel copies.
	*/
      DataLoopNode( bool link, DataLoopNode& D);

      ///
      DataLoopNode( DataLoopNameListNode* D,  LoopTableNode &L);
    //@}

  /// Destructor:
  virtual ~DataLoopNode();

  //-------------------------------------------
  // getShowStop() / setShowStop()
  //-------------------------------------------
  //@{
      ///
      virtual bool &getShowStop( void )
      {
          return showStop;
      };
      ///
      virtual void          setShowStop( bool s )
      {
          showStop  = s;
      };

  //-------------------------------------------
  // getPreComment() / setPreComment()
  //-------------------------------------------
  //@{
      ///
      virtual const string  &getPreComment( void )
      {
          return preComment;
      };
      ///
      virtual void          setPreComment( const string &cmt )
      {
          preComment = string(cmt);
      };
    //--------------------------------------------------
   //  setIndentFlag() / getIndentFlag()  //
  //--------------------------------------------------
  //@{
      ///
      void setIndentFlag( bool flg ) { myValList.setIndentFlag( flg ); };
      ///
      bool getIndentFlag( void ) const { return myValList.getIndentFlag(); };
      /** sets or gets the indention flag for the loop.
	* If it is true then the loop's values are indented
	* normally, if it is false, then the values are
	* not indented from the margin.  This only affects
	* how the loop will be printed when calling Unparse().
	* The default value made by the constructor is true.
	*/
  //@}

    //--------------------------------------------------
   //  setRowsPerLine() / getRowsPerLine()  //
  //--------------------------------------------------
  //@{
      ///
      void setRowsPerLine( int setTo ) { myValList.setRowsPerLine(setTo); };
      ///
      bool getRowsPerLine( void ) const { return myValList.getRowsPerLine(); };
      /** Sets (or gets) the number of loop iterations to print on
	* one line of text when unparsing.  This only has meaning
	* when the loop is tabulated, and is ignored when the loop
	* is linearly printed.  The default is 1.  Here is an example
	* of what it would look like if it were set to 3:
	* <pre>
	*     loop_
	*         _foo
	*         _bar
	*
	*         fooval1  barval1   fooval2  barval2   fooval3  barval3
	*         fooval4  barval4   fooval5  barval5   fooval6  barval6
	*     stop_
	* </pre>
	*/
  //@}

    //--------------------------------------------------
   //  setTabFlag() / getTabFlag()  //
  //--------------------------------------------------
  //@{
      ///
      void setTabFlag( bool setTo ) { myValList.setTabFlag(setTo); };
      ///
      bool getTabFlag( void ) { return myValList.getTabFlag(); };
      /** Sets (or gets) the value of the tab flag for this loop.
	* The tab flag determines if the loop prints out in tabular
	* format or linear format (tabular = row all on one line,
	* linear = one value per text line.)  This only affects the
	* unparsing of the loop and nothing else.
	* (true = tabular, false = linear)
	*/
  //@}


  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );
  ///
  bool  NotVirtualIsOfType( ASTtype T );
  ///
public:


  /** Return the name of the loop (arbitrarily chosen to be
    * the name of the first tag in the loop's names.
    */
  virtual string myName() const { return myDefList->myName();}
  /** <em>DEPRECIATED - instead use the STL vector-like
    * methods outlined in the documentation for DataLoopNameListNode
    * and LoopTableNode.</em>
    */
  void FlattenNestedLoop(
			 List<DataNameNode*>* &L,  
			 List<DataValueNode*>* &M);
	
  /** <em>DEPRECIATED - instead use the STL vector-like
    * methods outlined in the documentation for DataLoopNameListNode
    * and LoopTableNode.</em>
    */
  DataLoopValListNode::Status returnNextLoopElement(DataValueNode* & v);
  /** <em>DEPRECIATED - instead use the STL vector-like
    * methods outlined in the documentation for DataLoopNameListNode
    * and LoopTableNode.</em>
    */
  virtual ASTlist<DataValueNode *> * returnLoopValues(const string & tagName);
  /** <em>DEPRECIATED - instead use the STL vector-like
    * methods outlined in the documentation for DataLoopNameListNode
    * and LoopTableNode.</em>
    */
  void reset();
  
  ///
  virtual void Unparse(int indent);
  ///
  virtual void Unparse(int indent, int ) { Unparse(indent); };

  /** Get the integer indexes that tell where the given tagname
    * is located.  For example, the first tag in the outermost
    * loop would be returned as nestLevel = 0, column = 0.  The
    * third tag in the second nesting level would be returned
    * as nestLevel = 1, column = 2.  (values start counting at
    * zero, like array indeces).  Negative values are returned
    * if the tag was not found in this loop.
    * The search is case-insensitive.
    */
  void tagPositionDeep( string tagName, int *nestLevel, int *column );


  /** Removes a column given it's tag name:
    */
  StarFileNode::Status RemoveColumn( const string name );

  /** <em>DEPRECIATED - instead use the STL vector-like
    * methods outlined in the documentation for DataLoopNameListNode
    * and LoopTableNode.</em>
    */
  StarFileNode::Status AddDataName(const string & name);
  /** <em>DEPRECIATED - instead use the STL vector-like
    * methods outlined in the documentation for DataLoopNameListNode
    * and LoopTableNode.</em>
    */
  StarFileNode::Status AddDataValue( const string & value,
				     DataValueNode::ValType type );

  /// Find the old tag name in the loop and change it to the new name.
  StarFileNode::Status ChangeName(const string &oldName, const string &newName );
  

  /** Although this method is public, you should not use it.
    * It was made public merely because of a technical difficulty
    * in the parser that prevented it from working any other way.
    */
  void setVals( LoopTableNode &L );
  // The following require friend access into LoopTableNode:

  /** Although this method is public, you should not use it.
    * It was made public merely because of a technical difficulty
    * in the parser that prevented it from working any other way.
    */
  void setNames( DataLoopNameListNode &N )
  {
      if( myDefList != NULL )
	  delete myDefList;
      myDefList = new DataLoopNameListNode( N );
      myDefList->setParent(this);
  }

    //--------------------------------------------------
   //  getVals(), getValsPtr  //
  //--------------------------------------------------
  //@{
      ///
      LoopTableNode &getVals( void ) { return myValList; };
      ///
      LoopTableNode *getValsPtr( void ) { return &myValList; };
      /** getVals returns a reference or pointer to the LoopTableNode inside this
	* DataValueNode.
	* The loopIter returned is the outermost loop in the node.
	* The LoopTableNode returned can be manipulated directly.
	* This is the intended way to add/modify/read/delete the data in the
	* loop.  If the layout of the loop needs to be changed, however, by
	* adding or removing a tagname, then the getNames() method needs to
	* be used instead to manipulate that information first.
	* For more information, see the LoopTableNode class.
	*/
  //@}

    //-------------------------------------------------
   // getOneVal(), setOneVal()
  //-------------------------------------------------
  //@{
      /** Get one value out of the loop, from an arbitrary nesting level.
        * NOTE: The parameters are in row-major order.  This might be reversed
	* from how many scientific programmers think of things.
	* @param row - The row number to retreive from.  Rows start numbering
	* at zero.
	* @param col - The column number to retreive from.  Columns start
	* numbering at zero.
        * @param nest - The nesting level to retrieve for.  Zero means the
	* outermost level.  Defaults to zero if left off.
	* @returns TODO
	*/
  //@}

    //--------------------------------------------------
   //  getNames(), getNamesPtr() //
  //--------------------------------------------------
  //@{
      ///
      DataLoopNameListNode &getNames( void ) { return *myDefList; };
      ///
      DataLoopNameListNode *getNamesPtr( void ) { return myDefList; };
      /** getNames returns a reference or pointer to the DataLoopNameListNode
	* inside this DataValueNode.
	* It can be manipulated directly to alter the tags
	* in the loop.  In general, when tags are altered, new information
	* is automatically inserted into the loop or deleted from the loop
	* so that the STAR syntax is not violated.  For more information,
	* see the DataLoopNameListNode class.
	*/
  //@}

    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /** Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <p>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param searchFor Look for this strign as the tag name
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor );

    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /** Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    * <p>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param tag Look for this tag name...
    * @param value ...where it has this value.
    */
  virtual List<ASTnode*> * searchByTagValue( const string &tag, const string &value );
  
    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type type to search for
    * @param delim the delimiter type if searching for a DataValueNode.  The
    *              default if it is left off is "dont-care".
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int  delim = -1 );

    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    */
  virtual bool unlinkChild( ASTnode *child );

  
///
private :
  StarFileNode::Status transform(DataLoopValListNode *l);

  // Remove a column name heading.  Does not remove the data for
  // the heading.  The integers returned explain where the tag
  // was found.  returns StarFileNode::OK if something was
  // found and deleted, or StarFileNode::ERROR if the tag was
  // not found.
  StarFileNode::Status RemoveColumnName( const string name,  // IN: the tag name to look for
	                                 int   *nest,        // OUT: the nesting level it was found at.
					 int   *ele );       // OUT: the element # within its nesting
                                                             ///      level where it was found.


  /// This is private, but doc++ doesn't realize that.
  DataLoopNameListNode *myDefList;
  /// This is private, but doc++ doesn't realize that.
  LoopTableNode        myValList;
  string    preComment;

  bool      showStop;
///
public:
  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * In this case it returns the longest tagname, not the longest
    * data value.
    */
  virtual int myLongestStr( void )  const { return myDefList->myLongestStr(); };

  /**  Get the deepest depth of nesting of the entire loop.
    *  -1 = this is not in a loop, so the question is meaningless.
    *   0 = only outermost level of nesting exists.
    *   1 = two levels of nesting
    *   2 = three levels of nesting
    *   ...etc...
    */
  virtual int getMaxDepth( void );

///
private:
    virtual bool copyFrom( DataLoopNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

//------------------------------
//   class GlobalHeadingNode
//------------------------------
/** Contains nothing more than the name "global_" - this class
  * exists more for orthagonoality more than utility.
  */
class GlobalHeadingNode: public HeadingNode
{
 ///
 public:
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );
  ///
  bool  NotVirtualIsOfType( ASTtype T );
///
public:

  ///
  GlobalHeadingNode(const string & myHeading);

  ///
  GlobalHeadingNode( GlobalHeadingNode &N );  // copy constructor
  ///
  GlobalHeadingNode( bool link, GlobalHeadingNode &N );  // parallel version

};

//------------------------------
//   class DataHeadingNode
//------------------------------
/** Contains nothing more than the name "data_..." - this class
  * exists more for orthagonoality more than utility.
  */
class DataHeadingNode: public HeadingNode
{
 ///
 public:
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );
  ///
  bool  NotVirtualIsOfType( ASTtype T );
///
public:

  ///
  DataHeadingNode(const string & myHeading);

  ///
  DataHeadingNode( DataHeadingNode &N );  // copy constructor
  ///
  DataHeadingNode( bool link, DataHeadingNode &N );  // parallel version
  
};

//------------------------------
//   class SaveHeadingNode
//------------------------------
/** Contains nothing more than the name "save_..." - this class
  * exists more for orthagonoality more than utility.
  */
class SaveHeadingNode: public HeadingNode
{
 ///
 public:
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );
  ///
  bool  NotVirtualIsOfType( ASTtype T );
///
public:

  ///
  SaveHeadingNode(const string & myHeading);

  ///
  SaveHeadingNode( SaveHeadingNode &N );  // copy constructor
  ///
  SaveHeadingNode( bool link, SaveHeadingNode &N );  // parallel version

};

  //--------------------------------------------------
 //   class SaveFrameListNode   //
//--------------------------------------------------
/** Contains the things that are inside of a saveframe.
  */
class SaveFrameListNode: public ASTnode
{
 ///
 public:
  //@{  /// <u>Constructors</u>:
      ///
      SaveFrameListNode(ASTlist<DataNode *> *L);
      /// Make an empty list:
      SaveFrameListNode();
      ///  Copy Constructor
      SaveFrameListNode(SaveFrameListNode &L);
      // Copy with parallel link.  Set "link" to true to create
      // a copy with a parallel link, or set it to false to create
      // a copy without a parallel link.  See the external documentation
      /// for more details on parallel copies.
      SaveFrameListNode( bool link, SaveFrameListNode &L);
  //@}
  ///
  virtual ~SaveFrameListNode();

  /** Given a name of a DataItemNode or the first tag of a
    * DataLoopNode, returns the matching DataNode.
    */
  DataNode*  ReturnSaveFrameDataNode(string dataItemName);

  /** Returns the list (STL vector) of the insides of the
    * save frame.  This list is directly manipulatable such
    * that if you add or erase from it it changes the insides
    * of the save frame.
    */
  ASTlist<DataNode*>* GiveMyDataList();

  ///
  virtual void Unparse(int indent);
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
  ///
public:

   
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddDataItem(const string & name, const string & value, DataValueNode::ValType type);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddLoop(const string &tflag);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddLoopDataName(const string & name);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddLoopDataValue( const string & value,
					 DataValueNode::ValType type );
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void AddItemToSaveFrame(DataNode* S);

  /// <em>DEPRECIATED - use searchByTag instead @see searchByTag</em>
  DataNode* ReturnSaveFrameDeepLook( string dataItemName, int *nest, int *col );
  
    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /** Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <p>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param searchFor Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor );

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type the type to search for
    * @param delim the delimiter to search for if searching for
    *               DataValueNodes.  The default is "dont-care".
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int  delim = -1 );

    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /** Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    * <p>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param tag Look for this tag...
    * @param value ...where it has this value.
    */
  virtual List<ASTnode*> * searchByTagValue( const string &tag, const string &value );
  
    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  /**  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free it's space.
    *  RETURNS: true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    *  This function MUST be overridden for each subclass of ASTnode.
    */
  virtual bool unlinkChild( ASTnode *child );

///
 private:
  ///
  ASTlist<DataNode *> *myKids;

///
public:
  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case it is the longest tagname of a DataItem (not loop): )
    */
  virtual int myLongestStr( void );

///
private:
    virtual bool copyFrom( SaveFrameListNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};

  //--------------------------------------------------
 //   class SaveFrameNode     //
//--------------------------------------------------
/** This is a save frame from the STAR file.
  */
class SaveFrameNode: public DataNode
{
 ///
 public:
   //@{  /// <u>Constructors</u>:

      /// Make a new empty save frame with the given name.
      SaveFrameNode(const string & name);

      /// Copy constructor.
      SaveFrameNode( SaveFrameNode &S );

      /** Copy with parallel link.  Set "link" to true to create
        * a copy with a parallel link, or set it to false to create
        * a copy without a parallel link.  See the external documentation
        * for more details on parallel copies.
	*/
      SaveFrameNode( bool link, SaveFrameNode &S );

      /// 
      SaveFrameNode(SaveHeadingNode *h, SaveFrameListNode *l);
  //@}

  /// Destructor.
  virtual ~SaveFrameNode();
  /// Return the name of this saveframe.  (includes the "save_" prefix
  string myName() const {return myHeading->myName();};
  /** Set the name of this saveframe.  (Mus inculde the "save_" prefix
    * manually.
    */
  void changeName(const string &name);

  //-------------------------------------------
  // getPreComment() / setPreComment()
  //-------------------------------------------
  //@{
      ///
      virtual const string  &getPreComment( void )
      {
          return preComment;
      };
      ///
      virtual void          setPreComment( const string &cmt )
      {
          preComment = string(cmt);
      };
      /** These functions are used to give some nodes in the
	* AST tree the ability to remember a comment to be
	* pasted into the file in front of that node.  This
	* is useful if you want to insert header comments of
	* some sort into the output produced by Unparse().
	* As of this writing, no provisions are being made
	* to handle the parsing of comments from the original
	* file and storing them via these functions.  The
	* grammar to do that would be rather convoluted.
	* These functions are only intended to be used by programs
	* insterting their own comments after the file has
	* been read.
	* <p>
	* In the interest of saving memory, the preComment ability
	* only exists at the level of saveframenodes and blocknodes.
	* If you attempt to use these functions at other levels,
	* nothing will happen (the comment will always be empty).
	* <p>
	* The string must contain the comment characters embedded
	* inside, like so: "# this is a\n# multiline comment.",
	* not like this: "this is a\nmultiline comment."  This is
	* so that the caller is allowed to have the comment contain
	* blank lines like this:
	* <pre>
	*      # This is an example comment.
	*
	*      # The comment has some blank
	*
	*      # lines in it.
	* </pre>
	* If the Unparse() function were designed to insert the comment
	* characters (#) itself, then such a comment block would be impossible
	* to create.
	* <p>
	* Note that the comment lines are not syntax-checked in any way, so
	* using these functions it is entirely possible to create invalid
	* STAR files, since these "comments" can really be strings with
	* anything at all in them - so be careful.
	* <p>
	* To get rid of the preComment if you change your mind, set it to
	* a zero-length string with setPreComment().
	*/
  //@}
  /// Given a name of a DataItem, return it (or NULL if it can't be found).
  DataNode* ReturnSaveFrameDataNode(string dataItemName);

  /** Return the list (STL vector) of the DataItemNodes and DataLoopNodes
    * in this saveframe.  Manipulations such as insert() and erase()
    * on this list will directly insert or delete things from the saveframe
    * itself.
    */
  ASTlist<DataNode*>* GiveMyDataList();

  ///
  virtual void Unparse(int indent, int padsize);
  ///
  virtual void Unparse(int indent) { Unparse( indent, 0 ); };
  ///
  virtual ASTnode::ASTtype myType(void);
  ///
  virtual bool isOfType( ASTtype T );

  bool  NotVirtualIsOfType( ASTtype T );
  ///
public:

   

  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  virtual ASTlist<DataValueNode *>* returnLoopValues(const string & tagName);

  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  void AddItemToSaveFrame(DataNode* S);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddDataItem(const string & name, const string & value, DataValueNode::ValType type );
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddLoop(const string &tflag = string("tabulate"));
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddLoopDataName(const string & name);
  /// <em>DEPRECIATED - use the STL vector operations instead.</em>
  StarFileNode::Status AddLoopDataValue( const string & value,
					 DataValueNode::ValType type );
  
  /// <em>DEPRECIATED - use searchByTag instead @see searchByTag</em>
  DataNode* ReturnSaveFrameDeepLook( string dataItemName, int *nest, int *col );

    //--------------------------------------------------
   //  searchByTag()   //
  //--------------------------------------------------
  /** Given a tag name, find the AST object it resides in.  It returns
    * a pointer to the lowest level AST object that the tag resides in.
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <p>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param searchFor Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag( const string  &searchFor );

    //--------------------------------------------------
   //  searchForType()       //
  //--------------------------------------------------
  /** This method returns a list of all the nodes of the given type
    * that are inside this node, or children of this node, or children
    * of children of this node, etc all the way down to the leaf nodes.
    * @param type the type to search for
    * @param delim the delimiter to search for if searching for
    *              DataValueNodes.  The default is "dont-care".
    */
  virtual List<ASTnode*> * searchForType( ASTtype  type, int  delim = -1 );

    //--------------------------------------------------
   //  searchByTagValue()   //
  //--------------------------------------------------
  /** Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * (However, the values are case-sensitive.  A search for a
    * tag of <TT>_t1</TT> is identical to a search for a tag of <TT>_T1</TT>,
    * but a search for a <b>value</b> of "V1" is different from a search for
    * a value of "v1".)
    * <p>
    * WARNING: The list returned is allocated in heap space.  It is
    * the caller's responsibility to delete the list after it is no
    * longer needed.
    * @param tag Look for this tag...
    * @param value ...where it has this value.
    */
  virtual List<ASTnode*> * searchByTagValue( const string &tag, const string &value );
  
    //--------------------------------------------------
   // unlinkChild() //
  //--------------------------------------------------
  //  unlinks the given ASTnode from this ASTnode, assuming that
  //  the given ASTnode is a child of this ASTnode.  Does NOT
  //  call the destructor of the child node!! Use this function to
  ///  remove the child from this ASTnode but not free it's space.
  virtual bool unlinkChild( ASTnode *child );

  ///
 private:
  ///
  SaveHeadingNode   *myHeading;
  ///
  SaveFrameListNode *mySaveFrameList;
  ///
  string  preComment;

///
public:
  /** Returns the length of the longest string in this object.
    * Used by Unparse() at various levels of the AST tree.
    * (In this case, it just returns the length of the heading of
    * the heading of this saveframe: )
    */
  virtual int myLongestStr( void )  const { return myHeading->myLongestStr(); };

///
private:
    virtual bool copyFrom( SaveFrameNode &fromThis,  // copy this node
	                   bool linkWith ); // true = try to make parallel link
};



/** This function formats the file according to the format
  * defined in the formatTree passed in (which is a text
  * file defining the commenting and loop spacing format
  * for parts of the star file).
  * <P>
  * This function formats the file using the user-level
  * calls like 'setPreComment()' and 'setRowsPerLine()'
  * to configure the star tree passed in, such that when
  * it is Unparse()'ed later, it will print out in the 
  * right format.
  * <P>
  * TODO - Describe the formatTree syntax here when
  *        it is finally fully fleshed out.
  * <P>
  * @param inTree the StarFileNode to modify.
  * @param formatTree the StarFileNode (already parsed)
  *                   that holds the configuration information
  *                   for this function.  Its format is explained
  *                   above.
  */
void formatNMRSTAR( StarFileNode *inTree, StarFileNode *formatTree );

#endif
