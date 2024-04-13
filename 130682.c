static int fts3CommitMethod(sqlite3_vtab *pVtab){
  TESTONLY( Fts3Table *p = (Fts3Table*)pVtab );
  UNUSED_PARAMETER(pVtab);
  assert( p->nPendingData==0 );
  assert( p->inTransaction!=0 );
  assert( p->pSegments==0 );
  TESTONLY( p->inTransaction = 0 );
  TESTONLY( p->mxSavepoint = -1; );
  return SQLITE_OK;
}