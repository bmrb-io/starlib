/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
#ifndef ASTNODE_GUARD
#define ASTNODE_GUARD
#include "portability.h"
#include "list.h"

/**
 * <pre>
 *     RCS HISTORY:
 *   History:
 * $Log: astnode_working.h,v $
 * Revision 1.3  2001/12/04 20:58:18  madings
 * Major changes  - see doc/changes.html for details:
 *    - setstring / getstring interface at the  loop level
 *    - implement #<START-SKIP> ... #<END-SKIP> directives
 *    - fix compiler errors related to use of iterators and const
 *         on newer versions of gcc.
 *    - ValCache usage, with LoopRows containing one string for the
 *         whole row instead of seperate data value nodes.
 *    - change the type of the *os file stream because the ability to
 *         construct from a unix fileno() handle has been depreciated in
 * 	newer C++ stream libraries.
 *
 * Revision 1.2  2001/11/13 20:30:34  madings
 * This update includes several changes:
 * (See doc/changes.html for the full explanation)
 *    1- The previous compilers we had been using had allowed us to get
 *        away with some mis-use of the const specifier without flagging
 *        it as a warning or error.  That is no longer the case, so we had
 *        to alter some of the code's use of "const".
 *    2- The STL no longer allows vector iterators to be interchangable
 *        with pointers, so we lost the ability to use that feature for
 *        users of the library.
 *    3- The streams library no longer allows you to make an ofstream out of
 *        a pipe or standard output.  Callers of this method must now make
 *        'os' be a pointer to an ostream instead of ofstream.
 *    4- The newer compilers no longer allow a reference to a temporary store
 *        that was created just for the duration of the function call.
 *
 * Revision 1.1.1.1  2001/04/17  10:00:41  madings
 *
 * Revision 2.2  1999/08/26  06:44:16  madings
 * More changes to attempt to trim the memory footprint of this
 * program down:  Added the ability to compile the code with
 * some of the line-number and column-number information turned off
 * in DataValueNodes.  It didn't seem to help any.
 *
 * Revision 2.1  1999/04/28  02:29:00  madings
 * Made the searchByTag and searchByTagValue functions case-insensitive
 * for tag-names, datablock names, and saveframe names (but not data values).
 *
 * Revision 2.0  1999/01/23  03:34:33  madings
 * Since this new version compiles on
 * multiple platforms(well, okay, 2 of them), I
 * thought it warranted to call it version 2.0
 *
 * Revision 1.12  1999/01/22  23:58:28  madings
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
 * Revision 1.11  1998/10/08  03:45:38  madings
 * Two changes:
 *    1 - Most signifigant change is that the library now has
 *    doc++ comments in it.
 *    2 - Also, changed so that when using the copy constructor,
 *    it remembers line numbers and column numbers in the newly
 *    copied tree (I hope, this is untested.)
 *
 * Revision 1.10  1998/09/08  00:39:22  madings
 * My previous fix to the parsing of semicolon delimited strings broke
 * the line count, which zhaohua showed me.  This has been corrected
 * in this version.  (Also, irrelevant changes to comments in astnode.h
 * that don't affect anything are also in this commital)
 *
 * Revision 1.9  1998/09/04  07:02:06  madings
 * There were several bug fixes with this version.
 *
 * However, the erasing probably still needs some work.
 *
 * Revision 1.8  1998/08/29  08:46:04  madings
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
 * Revision 1.7  1998/08/21  02:00:20  informix
 * Fixed some lex rules so that it handles semicolon strings better.
 * (  It used to not parse this example:
 *           ; This value has some text on the same
 *           line as the starting semicolon.
 *           ;
 * )
 *
 * Revision 1.6  1998/07/17  01:41:30  madings
 * Two changes related to Unparse() this revision:
 *   1 - Added the ability to programtically insert comments with
 *           the get/setPreComment functions.
 *   2 - Make Unparse() somewhat more sophisticated to handle formatting
 * 	  values so they line up with each other.
 *
 * Revision 1.5  1998/06/02  06:22:03  madings
 * Made the yacc code that keeps track of lineNum/colNum a bit more
 * sophisticated so that it shows the locations of the beginnings
 * of the objects, not the ends of them.  Also gave the base class
 * 'ASTnode' a copy constructor that will copy the increasing number
 * of variables that are being stored in astnode, like the lineNum/colNum.
 *
 * Revision 1.4  1998/05/29  06:51:16  madings
 * Added the ability to search for all objects of a particular AST type.
 * (i.e. all SaveFrameNodes, or all DataItemNodes, etc...)
 * See the method called "searchForType()", which was added to do this.
 *
 * Revision 1.3  1998/05/25  07:15:15  madings
 * Added preliminary support for keeping track of line numbers and column
 * numbers from the parse.  This way, programs reading the data can give
 * the user actual line/column number references to things found in the
 * STAR file.
 *
 * Revision 1.2  1998/05/08  20:27:24  informix
 * Made a change to handle the C++ STL instead of gcc's libg++.
 * Now all strings are of type 'string' instead of 'String' (notice
 * the uppercase/lowercase change.)  Applications using this library
 * will have to change also, unfortunately, to use the new string style.
 *
 * Revision 1.1.1.1  1998/02/12  20:59:00  informix
 * re-creation of starlib project, without sb_lib stuff.  sb_lib has
 * been moved to it's own project.
 *
 * Revision 1.1.1.1  1997/11/07  01:49:06  madings
 * First check-in of starlib and sb_lib in library form.
 *
 * Revision 1.5  1997/10/25  02:59:26  madings
 *  2 Things in this check-in:
 *    -1- "Parallel copy".  Addded a new way to do a copy constructor
 *        such that it will copy with a parallel link back to the
 *        original tree it was copied from.  Any node in either tree
 *        can refer to the corresponding node in the new tree.
 *        See "parallel.html" for more detail.
 *
 *    -2- Bug fixes to copy constructors that did not properly deep-copy
 *        all the way down into the LoopIter values.
 *
 * Revision 1.4  1997/10/10  18:34:21  madings
 * Changed DataValueNode so that it now no longer has subclasses for
 * single-quote-delimited strings, double-quote delimited strings,
 * semicolon-delimited strings, and so on.  Now there is only one kind
 * of DataValueNode, and it uses a flag to decide on the delimiter type.
 * This also allows for the creation of methods to change a DataValueNode
 * in place, which was not previously possible when different kinds of
 * values had to be stored in different kinds of classes.
 *
 * Revision 1.3  1997/10/08  20:23:18  madings
 * Added "searchForTypeByTag()" and "searchForTypeByTagValue()".
 *
 * (Also fixed bug where it would not find saveframes.)
 *
 * Revision 1.2  1997/09/16  22:03:19  madings
 * Changed searchByTag() and searchByTagValue() so that they will
 * return lists instead of returning single hits, and extended
 * them to levels of the star tree above "saveframe" so it is now
 * possible to use them on the whole star file.
 * (Also changed the history log comments of several header files
 * to use slash-star comment style instead of '//' style.)
 *
 * </pre>
 */

#ifdef IS_GCC
#   pragma interface
#endif

#ifdef IN_AST_CC
    string ASTemptyString;
#else
    extern string ASTemptyString;
#endif

/** This struct is used internally to save some space
  * in each node.  Although bit-fields are often not
  * worth the effort, in this case they are because
  * this data could get repeated millions of times in
  * memory when a large STAR file is parsed. (This
  * information gets repeated for each DataValueNode in
  * a loop, for example.)
  * Any sort of small numeric field used in any derivation
  * of ASTnode should be put into this union.  (This
  * drastic measure became necessery when we found that
  * STAR files more than 5 meg on disk were causing
  * programs to run out of memory.)
  **/
typedef struct
{
    unsigned int tFlag      : 1;  // tab flag for loops: LoopTableNode, and
                                  // LoopRowNode
    unsigned int indFlag    : 1;  // indented loop flag: LoopTableNode
    unsigned int isDeleting : 1;  // boolean: am I being deleted?
    signed   int delimType  : 4;  // Used in DataValueNode only - stores
    				  // delimiter type.
} StarBits;

/** ASTnode class (base class for all other kinds of nodes).
  * --------------------------------------------------------
  * This class defines nothing but the Unparse function, which all AST types
  * must have.  When the root of the AST tree has its Unparse() function
  * called, it will cause a percolation through all the Unparse() functions
  * of all the nodes in the tree.<br>
  * Unparse simply writes the tree out in star format to a file.  The
  * file is hardcoded to be the stream called (*os).
  *
  */

class ASTnode
{ 
  protected :

    ASTnode               *peer;
    ASTnode               *parent;
    string                *preCommentPtr;
    StarBits              bitFields;

  public:
     /** Whenever a new class derived from ASTnode is
       * added to ast.h, it needs to have a name for
       * it added to this enumerated type.
       */
    enum ASTtype {        
	///
	ASTNODE,
	///
	BLOCKNODE,
	///
	DATABLOCKNODE,
	///
	DATAFILENODE,
	///
	DATAHEADINGNODE,
	///
	DATAITEMNODE,
	///
	DATALISTNODE,
	///
	DATALOOPNAMELISTNODE,
	/// <em>DEPRECIATED</em> - Do not use:
	DATALOOPDEFLISTNODE   = 7,
	///
	DATALOOPNODE,
	///
	DATALOOPVALLISTNODE,
	///
	DATANAMENODE,
	///
	DATANODE,
	///
	DATAVALUENODE,
	///
	GLOBALBLOCKNODE,
	///
	GLOBALHEADINGNODE,
	///
	HEADINGNODE,
	///
	LOOPROWNODE,
	/// <em>DEPRECIATED</em> - Do not use:
	ITERNODE              =16,
	///
	LOOPNAMELISTNODE,
	/// <em>DEPRECIATED</em> - Do not use:
	LOOPDEFLISTNODE       =17,
	///
	LOOPTABLENODE,
	/// <em>DEPRECIATED</em> - Do not use:
	LOOPITER              =18,
	/// <em>DEPRECIATED</em> - Do not use:
	LOOPVALLISTNODE,
	///
	SAVEFRAMELISTNODE,
	///
	SAVEFRAMENODE,
	///
	SAVEHEADINGNODE,
	///
	STARLISTNODE,
	///
	STARFILENODE
    };

  protected:

    void printIndent(int indent);
    void printComment( int indent );

  public:


    //-------------------------------------------
   // constructor
  //-------------------------------------------
  /** Sets the parent pointer to be null by default.
    */
  ASTnode( void );

    //-------------------------------------------
   // copy constructor
  //-------------------------------------------
  /// The copy constructor:
  ASTnode( const ASTnode &copyFromMe );

     //-----------
    // copyFrom
   //-----------
   /** The routine that copies low-level values from one
     * ASTnode to another - used by the copy constructor.
     */
  virtual void copyFrom( const ASTnode &copyFromMe );

    //-------------------------------------------
   // destructor
  //-------------------------------------------
  /// The destructor:
  virtual ~ASTnode( void );

  /** Every subtype must provide an unparse operation that
    * outputs the node to a global output stream called "os".
    * The collection of all the unparses of all the nodes is
    * used to print the star file.  If you call the Unparse() of
    * a particular level of nesting, it is supposed to in turn
    * call the Unparse()'s of its children, and thus you can print
    * the entire star file by calling the StarFileNode's Unparse(),
    * or you can print out just one saveframe by calling the
    * saveframe's Unparse(), or you can print out just one loop
    * by calling the DataLoopNode's Unparse(), and so on.
    *
    * @param indent - the Indent level to start printing at. Zero
    *                 means up agaisnt the left margin.
    */
  virtual void Unparse(int indent) = 0;

    //-------------------------------------------
   // myType()
  //-------------------------------------------
  /** Return the type of this ASTnode.  (this function is overridden
    * for each type derived from ASTnode)
    */
  virtual ASTtype myType(void);

    //-------------------------------------------
   // myParent()
  //-------------------------------------------
  /** Return the parent of this ASTnode.  In other words, return the
    * ASTnode object in which this ASTnode is inserted.  The exact
    * type of this object can then be determined by calling its
    * myType() function, at which point it can be casted to the
    * proper type.
    */
  virtual ASTnode *myParent(void) { return parent; }

    //-------------------------------------------
   // setParent()
  //-------------------------------------------
  /** Set the parent of this node to a new value.  Needed when you move
    * this node into a new structure elsewhere.
    */
  virtual void setParent( ASTnode *p );

    //-------------------------------------------
   // isOfType()
  //-------------------------------------------
  /** Call this function when you want to know whether or not a
    * particular ASTnode object is of a certain type
    */
  virtual bool isOfType( ASTtype T );


    //-------------------------------------------
   //  searchByTag()   //
  //-------------------------------------------
  /** Given a tag name, find the AST object it resides in.  It returns
    * a list of pointers to the lowest level AST objects that the tag
    * resides in.<br>
    * <P>
    * This search is case-insensitive.  The names of things, according
    * to the STAR specification, are supposed to be case-insensitive.
    * This is being applied not only to tag names but also to
    * saveframe names and datablock names.
    * <P>
    * The caller of this function needs to use the isOfType() and/or
    * myType() methods to determine what type to cst the object.
    * Returns a NULL if nothing was found.
    * @param searchFor - Look for this string as the tag name
    */
  virtual List<ASTnode*> * searchByTag( string  &searchFor);

  /** <u>Overloaded Version</u>
    * Just calls the above version of this function.  There is no
    * need to override this function in subclasses of ASTnode.
    * @param searchFor - Look for this string as the tag name.
    */
  virtual List<ASTnode*> * searchByTag( char *searchFor);

    //-------------------------------------------
   //  searchByTagValue()   //
  //-------------------------------------------
  //@{
      /** Given a tag name and a value, find the AST object that that
	* particular tag and value pair resides in.  This is like
	* performing an SQL search: WHERE tag = value.
	* <p>
	* Only searches starting at the node it was called from, and
	* its children.  Recurses downward, but does not recurse upward.
	* This function is only capable of returning one answer, so it
	* cannot be called at the same levels where searchByTag() can
	* be called (see above).
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
	* @param tag - Look for this tag...
	* @param value - Where it has this value.
	*/
      virtual List<ASTnode*> * searchByTagValue( string &tag, string &value);

      /** <u>Overloaded Version</u>
	* Just calls the above version of this function.  There is no
	* need to override this function in subclasses of ASTnode.
	* <p>
	* WARNING: The list returned is allocated in heap space.  It is
	* the caller's responsibility to delete the list after it is no
	* longer needed.
	*/
      virtual List<ASTnode*> * searchByTagValue( string &tag, char *value);

      /** <u>Overloaded Version</u>
	* Just calls the above version of this function.  There is no
	* need to override this function in subclasses of ASTnode.
	*/
      virtual List<ASTnode*> * searchByTagValue( char *tag, string &value);

      /** <u>Overloaded Version</u>
	* Just calls the above version of this function.  There is no
	* need to override this function in subclasses of ASTnode.
	*/
      virtual List<ASTnode*> * searchByTagValue( char *tag, char *value);
  //@}

    //-------------------------------------------
   //  searchForType()       //
  //-------------------------------------------
  /** This method returns a list of all the nodes of the given type.
    * It is much like searchByTag() in that it heirarchically walks
    * the STAR tree and calls the searchForType() functions of the subtrees
    * within the tree.  In this way it is possible to call this function
    * at any level of the STAR file.
    * <p>
    * The second parameter is optional and is only useful when you are
    * searching for DATAVALUENODEs.  It determines the kind of
    * DATAVALUENODE you are searching for, by delimiter type.  For
    * example, you could search for only those DATAVALUENODEs that
    * are semicolon-delimited by passing DataValueNode::SEMICOLON
    * as the second argument.  Or you could look for just framecodes
    * by passing DataValueNode::FRAMECODE as the second parameter.
    * Passing a negative number says you want all the DataValueNodes,
    * regardless of their delimiter type.
    * <p>
    * Although the second parameter is an integer, think of it as
    * the enum DataValueNode::ValType.  The only reason it is shown
    * as an integer in the prototype here is that this class (ASTnode)
    * needs to be defined before the class "DataValueNode" can be defined,
    * and thus at this point the compiler has no clue what
    * "DataValueNode::ValType" means yet.
    * <p>
    * If the search is for some ASTtype other than DATAVALUENODE, then
    * it is irrelevant what the second parameter of this function is, as
    * it will never be used - You can just leave it off and accept the
    * default.
    * @param type - type to search for 
    * @param delim - DataValueNode::ValType to look for.  Default = "dont-care".
    */
  virtual List<ASTnode*> * searchForType(
	  ASTtype type, int delim = -1);

    //-------------------------------------------
   //  searchForTypeByTag()  //
  //-------------------------------------------
  //@{
      /** <u>Overloaded Version</u> */
      virtual List<ASTnode*> * searchForTypeByTag(
	      ASTtype  type, string   &tag);

      /** <u>Overloaded Version</u> */
      virtual List<ASTnode*> * searchForTypeByTag(
	      ASTtype  type, char *tag);

      /** This method is exactly the same as 'searchByTag()', except that
      * it tries to return an object of the type given.  What it does
      * is this: it calls searchByTag(), then it 'walks' up the parent
      * list until it finds an ASTnode-derived object that is of the
      * type given.  Thus if you search for a ASTnode::SAVEFRAMENODE
      * type of object with this function, it will return the save frame
      * in which the match was found, rather than the match itself.
      * Also note that this method will consider derived subtypes of
      * a class as valid 'hits' as well.  For example, you could
      * search for an ASTnode::DATANODE and if a DATAITEMNODE or
      * DATALOOPNODE or SAVEFRAMENODE were found, they would be
      * returned as valid hits.
      * If an empty list is returned, that could be either because
      * no matching tags were found OR because there were matches
      * found, but they were not inside any objects of the requested
      * type.
      * <p>
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
      * <em>
      * YOU NEVER NEED TO OVERRIDE THESE FUNCTIONS IN ANY OF THE
      * SUBCLASSES OF ASTNODE!  THEY OPERATE THE SAME WAY IN ALL
      * THE SUBCLASSES.  THEY MAKE ALL THEIR CALLS THROUGH THE
      * EXISTING searchByTag() calls.
      * </em>
      * @param type - the type to search for.
      * @param tab - the tag (name) to search for.
      */
  //@}
    //-------------------------------------------
   //  searchForTypeByTagValue()   //
  //-------------------------------------------
  //@{
      /** <u>Overloaded Version</u> */
      virtual List<ASTnode*> * searchForTypeByTagValue(
	      ASTtype  type, string   &tag, string   &value);

      /** <u>Overloaded Version</u>
	* Just calls the above version of this function.  There is no
	* need to override this function in subclasses of ASTnode.
	* <p>
	* WARNING: The list returned is allocated in heap space.  It is
	* the caller's responsibility to delete the list after it is no
	* longer needed.
	*/
      virtual List<ASTnode*> * searchForTypeByTagValue(
	      ASTtype  type, string    &tag, char     *value );

      /** <u>Overloaded Version</u>
	* Just calls the above version of this function.  There is no
	* need to override this function in subclasses of ASTnode.
	*/
      virtual List<ASTnode*> * searchForTypeByTagValue(
	      ASTtype  type, char     *tag, string   &value);

      /** <u>Overloaded Version</u>
	* Just calls the above version of this function.  There is no
	* need to override this function in subclasses of ASTnode.
	*/
      virtual List<ASTnode*> * searchForTypeByTagValue(
	      ASTtype  type, char    *tag, char    *value );

      /** This method is exactly like 'searchByTag()', except that
      * it tries to return an object of the type given.  Other
      * than the fact that it calls searchByTagValue() instead
      * of searchByTag(), it behaves exactly as searchForTypeByTag()
      * (see above).
      * <p>
      * <em>
      * YOU NEVER NEED TO OVERRIDE THESE FUNCTIONS IN ANY OF THE
      * SUBCLASSES OF ASTNODE!  THEY OPERATE THE SAME WAY IN ALL
      * THE SUBCLASSES.  THEY MAKE ALL THEIR CALLS THROUGH THE
      * EXISTING searchByTag() calls.
      * </em>
      * @param type - type to search for
      * @param tag - tag name to search for
      * @param value - where it has this value
      */
  //@}

    //-------------------------------------------
   //  removeChild()  //
  //-------------------------------------------
  /**  Removes the given ASTnode from this ASTnode, assuming that the
    *  given ASTnode is an immediate child of this ASTnode.  (Does not
    *  recurse down to grandchildren).  If the given ASTnode is not a
    *  child of this ASTnode, then nothing happens and no error is
    *  reported.  Also calls the destructor of the child node.
    *  This function does NOT need to be overridden for subclasses
    *  of ASTnode.  The default ASTnode removeChild() will call
    *  unlinkChild(), and if that is successful, it will delete
    *  the child pointer.  This should be correct behaviour most
    *  of the time for any subclass of ASTnode, assuming that the
    *  subclass has defined its unlinkChild() properly.
    *  @return true if the child was removed.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    */
  virtual bool removeChild( ASTnode *child );

    //-------------------------------------------
   // unlinkChild() //
  //-------------------------------------------
  /**  unlinks the given ASTnode from this ASTnode, assuming that
    *  the given ASTnode is a child of this ASTnode.  Does NOT
    *  call the destructor of the child node!! Use this function to
    *  remove the child from this ASTnode but not free its space.
    *  <p>
    *  This function MUST be overridden for each subclass of ASTnode.
    *  <p>
    *  @return true if the child was unlinked.  false if the given
    *           ASTnode was not even in this class anywhere, and
    *           therefore nothing was done.
    */
  virtual bool unlinkChild( ASTnode *child );

    //-------------------------------------------
   // removeMe() //
  //-------------------------------------------
  /**  Remove me from my parent ASTnode by calling my parent's
    *  removeChild() on me.  Deletes me as well, so I can't be
    *  used after this call anymore.  I am gone.
    *  This function does NOT need to be overridden for each
    *  ASTnode derivative.  It is very generic.
    *  @return true if this ASTnode was successfully removed.
    *           false if this ASTnode was not removed, usually
    *           this is not fatal - it is just that this node's
    *           parent has disowned this node and has no idea
    *           what it is.  This usually happens when this ASTnode
    *           is a copy of another ASTnode such that it's parent
    *           pointer points at the same place as the original
    *           ASTnode's parent pointer.
    */
  virtual bool removeMe( void );

    //-------------------------------------------
   // unlinkMe()  //
  //-------------------------------------------
  /**  Remove me from my parent ASTnode, but do not delete me
    *  from the universe.  I still exist after this call.
    *  This function does NOT need to be overridden for each
    *  ASTnode derivative.  It is very generic.
    *  #@return true if this ASTnode was successfully removed.
    *           false if this ASTnode was not removed, usually
    *           this is not fatal - it is just that this node's
    *           parent has disowned this node and has no idea
    *           what it is.  This usually happens when this ASTnode
    *           is a copy of another ASTnode such that it's parent
    *           pointer points at the same place as the original
    *           ASTnode's parent pointer.
    */
  virtual bool unlinkMe( void );

    //-------------------------------------------
   //   myParallelCopy()  //
  //-------------------------------------------
  //@{
    ///
    virtual ASTnode *myParallelCopy( void );
    ///
    void setPeer( ASTnode *);
  /**  This method returns a pointer to an ASTnode. The ASTnode
    *  returned is the parallel 'other' version of this node
    *  in the parallel tree. Incedentally, since the two parallel
    *  trees are aware of each other, the parallel copy of the
    *  parallel copy is the orginial node, so this function is
    *  self-inverting, like this:
    *      foo->myParallelCopy()->myParallelCopy() == foo
    *  If there is no parallel node corresponding to this node,
    *  then this method will return a NULL pointer. This can happen
    *  if a node in the parallel tree was deleted after the
    *  parallel copy was made.
    */
  //@}

  //-------------------------------------------
  //  NotVirtualIsOfType()
  //-------------------------------------------
  /** This function should always be identical to isOfType except that
    * it is not a virtual function.  This means that you can call it
    * by casting the ASTnode class to the specific type you want, and
    * you will get a call that does not nessacerliy go down to the deepest
    * level of descendency, like a virtual function would.
    */
  bool NotVirtualIsOfType( ASTtype T );

  //-------------------------------------------
  //  getLineNum() / setLineNum()
  //-------------------------------------------
  //@{
	///
        virtual int   getLineNum( void ) const
	{
#ifdef DEBUG
	    printf( "DEBUG: inside getLineNum for type: %d\n", myType() );
	    printf( "DEBUG:      lineNum is %d\n", lineNum );
#endif
	    // Default behaviour is to go up to the parent and get
	    // its line number if this astnode is one that actually
	    // implements the line number, then it should override
	    // this.
	    if( parent == NULL )
		return -1;
	    else
		return parent->getLineNum();
        };
	///
        virtual void  setLineNum( const int num )
	{
	    // Default behaviour is to do nothing.
	};

      /** These functions return the line number and set the line
	* number on which this node was found in the STAR file.
	* Note that a negative number is an indicator that the
	* node was not in the original file when it was parsed,
	* and hence has no line number.
	* <p>
	* These functions start counting at 1, not zero.
	* <p>
	* These functions actually give the location where
	* the particular part of speech ENDED, not where it
	* started.  For example, with a DataLoopNode, the location
	* given is the location of the closing "stop_" keyword,
	* not the opening "loop_" keyword.  This is an unfortunate
	* neccessity due to the way the parser reads the data.  It
	* doesn't know whether or not it has encountered a complete
	* grammatical construct until it reaches the end of the
	* construct, at which point it has lost track of what the
	* location was at the start of the grammatical construct.
	*/
  //@}

  //-------------------------------------------
  //  getColNum() / setColNum()
  //-------------------------------------------
  //@{
	///
        virtual int   getColNum( void ) const
	{ 
	    // Default behaviour is to go up to the parent and get
	    // its line number if this astnode is one that actually
	    // implements the line number, then it should override
	    // this.
	    if( parent == NULL )
		return -1;
	    else
		return parent->getColNum();
	};
        ///
        virtual void  setColNum( const int num )
	{
	    // Default behaviour is to do nothing.
	};
        /** These functions return the column number and set the
  	* column number on which this node was found in the STAR
  	* file.  Note that a negative number is an indicator that
  	* the node was not in the original file when it was parsed,
  	* and hence has no column number.
  	* <p>
  	* These functions start counting at 1, not zero.
  	* <p>
  	* These functions actually give the location where
  	* the particular part of speech ENDED, not where it
  	* started.  For example, with a DataLoopNode, the location
  	* given is the location of the closing "stop_" keyword,
  	* not the opening "loop_" keyword.  This is an unfortunate
  	* neccessity due to the way the parser reads the data.  It
  	* doesn't know whether or not it has encountered a complete
  	* grammatical construct until it reaches the end of the
  	* construct, at which point it has lost track of what the
  	* location was at the start of the grammatical construct.
  	*/
  //@}


  //-------------------------------------------
  // getPreComment() / setPreComment()
  //-------------------------------------------
  //@{
      ///
      const string  &getPreComment( void )
      {
	  if( preCommentPtr == NULL )
	      return ASTemptyString;
	  else
	      return *preCommentPtr;
      };
      ///
      void          setPreComment( const string &cmt )
      {
	  if( preCommentPtr != NULL )
	      delete preCommentPtr; // get rid of old space
	  if( cmt.length() == 0 )
	      preCommentPtr = NULL;
	  else
	      preCommentPtr = new string( cmt );
      };
      /** These functions are used to give each node in the
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


};

/* Much like the StarBits structure, this one holds information
 * tightly that would otherwise be in seperate ints.
 * The line and column numbers only exist in some of the
 * data value nodes, so they don't get used everywhere.
 * We need to be as tight on space as possible since these
 * nodes get repeated for each node of a STAR file's syntax.
 */
typedef struct
{
    signed   int lNum       : 21; // can't handle more than 2^20 lines
                                  // in a file without false line numbers
				  // being reported (wraparound overflows).
    signed   int cNum       : 11; // can't handle more than 1024 columns
                                  // in a line without false column numbers
				  // being reported (wraparound overflows).
} StarPosBits;

/** This is used internally by those AST nodes which have line number
  * and column information.  It was once a part of ASTnode but it had
  * to be moved out in order to save space in ASTnode for repetative
  * data values in big loops.
  * <p>
  * This does not need to be documented for callers of starlib, only an
  * implementer of starlib need to understand this clas.
  */
class ASTnodeWithPosBits
{
  protected:
    StarPosBits   posBits;

  public:
    ASTnodeWithPosBits( void )
    {
	posBits.lNum  = -1;
	posBits.cNum  = -1;
    };

    int posBitsGetLineNum( void ) const
    {
	return posBits.lNum;
    };
    void posBitsSetLineNum( const int num )
    {
	posBits.lNum = num;
    };
    int posBitsGetColNum( void ) const
    {
	return posBits.cNum;
    };
    void posBitsSetColNum( const int num )
    {
	posBits.cNum = num;
    };
};

#endif
