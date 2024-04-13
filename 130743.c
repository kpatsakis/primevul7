static int fts3ScanInteriorNode(
  const char *zTerm,              /* Term to select leaves for */
  int nTerm,                      /* Size of term zTerm in bytes */
  const char *zNode,              /* Buffer containing segment interior node */
  int nNode,                      /* Size of buffer at zNode */
  sqlite3_int64 *piFirst,         /* OUT: Selected child node */
  sqlite3_int64 *piLast           /* OUT: Selected child node */
){
  int rc = SQLITE_OK;             /* Return code */
  const char *zCsr = zNode;       /* Cursor to iterate through node */
  const char *zEnd = &zCsr[nNode];/* End of interior node buffer */
  char *zBuffer = 0;              /* Buffer to load terms into */
  int nAlloc = 0;                 /* Size of allocated buffer */
  int isFirstTerm = 1;            /* True when processing first term on page */
  sqlite3_int64 iChild;           /* Block id of child node to descend to */

  /* Skip over the 'height' varint that occurs at the start of every 
  ** interior node. Then load the blockid of the left-child of the b-tree
  ** node into variable iChild.  
  **
  ** Even if the data structure on disk is corrupted, this (reading two
  ** varints from the buffer) does not risk an overread. If zNode is a
  ** root node, then the buffer comes from a SELECT statement. SQLite does
  ** not make this guarantee explicitly, but in practice there are always
  ** either more than 20 bytes of allocated space following the nNode bytes of
  ** contents, or two zero bytes. Or, if the node is read from the %_segments
  ** table, then there are always 20 bytes of zeroed padding following the
  ** nNode bytes of content (see sqlite3Fts3ReadBlock() for details).
  */
  zCsr += sqlite3Fts3GetVarint(zCsr, &iChild);
  zCsr += sqlite3Fts3GetVarint(zCsr, &iChild);
  if( zCsr>zEnd ){
    return FTS_CORRUPT_VTAB;
  }
  
  while( zCsr<zEnd && (piFirst || piLast) ){
    int cmp;                      /* memcmp() result */
    int nSuffix;                  /* Size of term suffix */
    int nPrefix = 0;              /* Size of term prefix */
    int nBuffer;                  /* Total term size */
  
    /* Load the next term on the node into zBuffer. Use realloc() to expand
    ** the size of zBuffer if required.  */
    if( !isFirstTerm ){
      zCsr += fts3GetVarint32(zCsr, &nPrefix);
    }
    isFirstTerm = 0;
    zCsr += fts3GetVarint32(zCsr, &nSuffix);
    
    if( nPrefix<0 || nSuffix<0 || &zCsr[nSuffix]>zEnd ){
      rc = FTS_CORRUPT_VTAB;
      goto finish_scan;
    }
    if( nPrefix+nSuffix>nAlloc ){
      char *zNew;
      nAlloc = (nPrefix+nSuffix) * 2;
      zNew = (char *)sqlite3_realloc(zBuffer, nAlloc);
      if( !zNew ){
        rc = SQLITE_NOMEM;
        goto finish_scan;
      }
      zBuffer = zNew;
    }
    assert( zBuffer );
    memcpy(&zBuffer[nPrefix], zCsr, nSuffix);
    nBuffer = nPrefix + nSuffix;
    zCsr += nSuffix;

    /* Compare the term we are searching for with the term just loaded from
    ** the interior node. If the specified term is greater than or equal
    ** to the term from the interior node, then all terms on the sub-tree 
    ** headed by node iChild are smaller than zTerm. No need to search 
    ** iChild.
    **
    ** If the interior node term is larger than the specified term, then
    ** the tree headed by iChild may contain the specified term.
    */
    cmp = memcmp(zTerm, zBuffer, (nBuffer>nTerm ? nTerm : nBuffer));
    if( piFirst && (cmp<0 || (cmp==0 && nBuffer>nTerm)) ){
      *piFirst = iChild;
      piFirst = 0;
    }

    if( piLast && cmp<0 ){
      *piLast = iChild;
      piLast = 0;
    }

    iChild++;
  };

  if( piFirst ) *piFirst = iChild;
  if( piLast ) *piLast = iChild;

 finish_scan:
  sqlite3_free(zBuffer);
  return rc;
}