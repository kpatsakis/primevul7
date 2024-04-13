static int fts3SyncMethod(sqlite3_vtab *pVtab){

  /* Following an incremental-merge operation, assuming that the input
  ** segments are not completely consumed (the usual case), they are updated
  ** in place to remove the entries that have already been merged. This
  ** involves updating the leaf block that contains the smallest unmerged
  ** entry and each block (if any) between the leaf and the root node. So
  ** if the height of the input segment b-trees is N, and input segments
  ** are merged eight at a time, updating the input segments at the end
  ** of an incremental-merge requires writing (8*(1+N)) blocks. N is usually
  ** small - often between 0 and 2. So the overhead of the incremental
  ** merge is somewhere between 8 and 24 blocks. To avoid this overhead
  ** dwarfing the actual productive work accomplished, the incremental merge
  ** is only attempted if it will write at least 64 leaf blocks. Hence
  ** nMinMerge.
  **
  ** Of course, updating the input segments also involves deleting a bunch
  ** of blocks from the segments table. But this is not considered overhead
  ** as it would also be required by a crisis-merge that used the same input 
  ** segments.
  */
  const u32 nMinMerge = 64;       /* Minimum amount of incr-merge work to do */

  Fts3Table *p = (Fts3Table*)pVtab;
  int rc = sqlite3Fts3PendingTermsFlush(p);

  if( rc==SQLITE_OK 
   && p->nLeafAdd>(nMinMerge/16) 
   && p->nAutoincrmerge && p->nAutoincrmerge!=0xff
  ){
    int mxLevel = 0;              /* Maximum relative level value in db */
    int A;                        /* Incr-merge parameter A */

    rc = sqlite3Fts3MaxLevel(p, &mxLevel);
    assert( rc==SQLITE_OK || mxLevel==0 );
    A = p->nLeafAdd * mxLevel;
    A += (A/2);
    if( A>(int)nMinMerge ) rc = sqlite3Fts3Incrmerge(p, A, p->nAutoincrmerge);
  }
  sqlite3Fts3SegmentsClose(p);
  return rc;
}