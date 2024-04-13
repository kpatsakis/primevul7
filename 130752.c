static int fts3ReleaseMethod(sqlite3_vtab *pVtab, int iSavepoint){
  TESTONLY( Fts3Table *p = (Fts3Table*)pVtab );
  UNUSED_PARAMETER(iSavepoint);
  UNUSED_PARAMETER(pVtab);
  assert( p->inTransaction );
  assert( p->mxSavepoint >= iSavepoint );
  TESTONLY( p->mxSavepoint = iSavepoint-1 );
  return SQLITE_OK;
}