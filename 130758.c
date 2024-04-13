static int fts3GetMatchinfo(
  Fts3Cursor *pCsr,               /* FTS3 Cursor object */
  const char *zArg                /* Second argument to matchinfo() function */
){
  MatchInfo sInfo;
  Fts3Table *pTab = (Fts3Table *)pCsr->base.pVtab;
  int rc = SQLITE_OK;
  int bGlobal = 0;                /* Collect 'global' stats as well as local */

  memset(&sInfo, 0, sizeof(MatchInfo));
  sInfo.pCursor = pCsr;
  sInfo.nCol = pTab->nColumn;

  /* If there is cached matchinfo() data, but the format string for the 
  ** cache does not match the format string for this request, discard 
  ** the cached data. */
  if( pCsr->zMatchinfo && strcmp(pCsr->zMatchinfo, zArg) ){
    assert( pCsr->aMatchinfo );
    sqlite3_free(pCsr->aMatchinfo);
    pCsr->zMatchinfo = 0;
    pCsr->aMatchinfo = 0;
  }

  /* If Fts3Cursor.aMatchinfo[] is NULL, then this is the first time the
  ** matchinfo function has been called for this query. In this case 
  ** allocate the array used to accumulate the matchinfo data and
  ** initialize those elements that are constant for every row.
  */
  if( pCsr->aMatchinfo==0 ){
    int nMatchinfo = 0;           /* Number of u32 elements in match-info */
    int nArg;                     /* Bytes in zArg */
    int i;                        /* Used to iterate through zArg */

    /* Determine the number of phrases in the query */
    pCsr->nPhrase = fts3ExprPhraseCount(pCsr->pExpr);
    sInfo.nPhrase = pCsr->nPhrase;

    /* Determine the number of integers in the buffer returned by this call. */
    for(i=0; zArg[i]; i++){
      nMatchinfo += fts3MatchinfoSize(&sInfo, zArg[i]);
    }

    /* Allocate space for Fts3Cursor.aMatchinfo[] and Fts3Cursor.zMatchinfo. */
    nArg = (int)strlen(zArg);
    pCsr->aMatchinfo = (u32 *)sqlite3_malloc(sizeof(u32)*nMatchinfo + nArg + 1);
    if( !pCsr->aMatchinfo ) return SQLITE_NOMEM;

    pCsr->zMatchinfo = (char *)&pCsr->aMatchinfo[nMatchinfo];
    pCsr->nMatchinfo = nMatchinfo;
    memcpy(pCsr->zMatchinfo, zArg, nArg+1);
    memset(pCsr->aMatchinfo, 0, sizeof(u32)*nMatchinfo);
    pCsr->isMatchinfoNeeded = 1;
    bGlobal = 1;
  }

  sInfo.aMatchinfo = pCsr->aMatchinfo;
  sInfo.nPhrase = pCsr->nPhrase;
  if( pCsr->isMatchinfoNeeded ){
    rc = fts3MatchinfoValues(pCsr, bGlobal, &sInfo, zArg);
    pCsr->isMatchinfoNeeded = 0;
  }

  return rc;
}