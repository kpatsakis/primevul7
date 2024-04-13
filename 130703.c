static int fts3BeginMethod(sqlite3_vtab *pVtab){
  Fts3Table *p = (Fts3Table*)pVtab;
  UNUSED_PARAMETER(pVtab);
  assert( p->pSegments==0 );
  assert( p->nPendingData==0 );
  assert( p->inTransaction!=1 );
  TESTONLY( p->inTransaction = 1 );
  TESTONLY( p->mxSavepoint = -1; );
  p->nLeafAdd = 0;
  return fts3SetHasStat(p);
}