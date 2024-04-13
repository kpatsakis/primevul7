static int fts3EvalSelectDeferred(
  Fts3Cursor *pCsr,               /* FTS Cursor handle */
  Fts3Expr *pRoot,                /* Consider tokens with this root node */
  Fts3TokenAndCost *aTC,          /* Array of expression tokens and costs */
  int nTC                         /* Number of entries in aTC[] */
){
  Fts3Table *pTab = (Fts3Table *)pCsr->base.pVtab;
  int nDocSize = 0;               /* Number of pages per doc loaded */
  int rc = SQLITE_OK;             /* Return code */
  int ii;                         /* Iterator variable for various purposes */
  int nOvfl = 0;                  /* Total overflow pages used by doclists */
  int nToken = 0;                 /* Total number of tokens in cluster */

  int nMinEst = 0;                /* The minimum count for any phrase so far. */
  int nLoad4 = 1;                 /* (Phrases that will be loaded)^4. */

  /* Tokens are never deferred for FTS tables created using the content=xxx
  ** option. The reason being that it is not guaranteed that the content
  ** table actually contains the same data as the index. To prevent this from
  ** causing any problems, the deferred token optimization is completely
  ** disabled for content=xxx tables. */
  if( pTab->zContentTbl ){
    return SQLITE_OK;
  }

  /* Count the tokens in this AND/NEAR cluster. If none of the doclists
  ** associated with the tokens spill onto overflow pages, or if there is
  ** only 1 token, exit early. No tokens to defer in this case. */
  for(ii=0; ii<nTC; ii++){
    if( aTC[ii].pRoot==pRoot ){
      nOvfl += aTC[ii].nOvfl;
      nToken++;
    }
  }
  if( nOvfl==0 || nToken<2 ) return SQLITE_OK;

  /* Obtain the average docsize (in pages). */
  rc = fts3EvalAverageDocsize(pCsr, &nDocSize);
  assert( rc!=SQLITE_OK || nDocSize>0 );


  /* Iterate through all tokens in this AND/NEAR cluster, in ascending order 
  ** of the number of overflow pages that will be loaded by the pager layer 
  ** to retrieve the entire doclist for the token from the full-text index.
  ** Load the doclists for tokens that are either:
  **
  **   a. The cheapest token in the entire query (i.e. the one visited by the
  **      first iteration of this loop), or
  **
  **   b. Part of a multi-token phrase.
  **
  ** After each token doclist is loaded, merge it with the others from the
  ** same phrase and count the number of documents that the merged doclist
  ** contains. Set variable "nMinEst" to the smallest number of documents in 
  ** any phrase doclist for which 1 or more token doclists have been loaded.
  ** Let nOther be the number of other phrases for which it is certain that
  ** one or more tokens will not be deferred.
  **
  ** Then, for each token, defer it if loading the doclist would result in
  ** loading N or more overflow pages into memory, where N is computed as:
  **
  **    (nMinEst + 4^nOther - 1) / (4^nOther)
  */
  for(ii=0; ii<nToken && rc==SQLITE_OK; ii++){
    int iTC;                      /* Used to iterate through aTC[] array. */
    Fts3TokenAndCost *pTC = 0;    /* Set to cheapest remaining token. */

    /* Set pTC to point to the cheapest remaining token. */
    for(iTC=0; iTC<nTC; iTC++){
      if( aTC[iTC].pToken && aTC[iTC].pRoot==pRoot 
       && (!pTC || aTC[iTC].nOvfl<pTC->nOvfl) 
      ){
        pTC = &aTC[iTC];
      }
    }
    assert( pTC );

    if( ii && pTC->nOvfl>=((nMinEst+(nLoad4/4)-1)/(nLoad4/4))*nDocSize ){
      /* The number of overflow pages to load for this (and therefore all
      ** subsequent) tokens is greater than the estimated number of pages 
      ** that will be loaded if all subsequent tokens are deferred.
      */
      Fts3PhraseToken *pToken = pTC->pToken;
      rc = sqlite3Fts3DeferToken(pCsr, pToken, pTC->iCol);
      fts3SegReaderCursorFree(pToken->pSegcsr);
      pToken->pSegcsr = 0;
    }else{
      /* Set nLoad4 to the value of (4^nOther) for the next iteration of the
      ** for-loop. Except, limit the value to 2^24 to prevent it from 
      ** overflowing the 32-bit integer it is stored in. */
      if( ii<12 ) nLoad4 = nLoad4*4;

      if( ii==0 || (pTC->pPhrase->nToken>1 && ii!=nToken-1) ){
        /* Either this is the cheapest token in the entire query, or it is
        ** part of a multi-token phrase. Either way, the entire doclist will
        ** (eventually) be loaded into memory. It may as well be now. */
        Fts3PhraseToken *pToken = pTC->pToken;
        int nList = 0;
        char *pList = 0;
        rc = fts3TermSelect(pTab, pToken, pTC->iCol, &nList, &pList);
        assert( rc==SQLITE_OK || pList==0 );
        if( rc==SQLITE_OK ){
          int nCount;
          fts3EvalPhraseMergeToken(pTab, pTC->pPhrase, pTC->iToken,pList,nList);
          nCount = fts3DoclistCountDocids(
              pTC->pPhrase->doclist.aAll, pTC->pPhrase->doclist.nAll
          );
          if( ii==0 || nCount<nMinEst ) nMinEst = nCount;
        }
      }
    }
    pTC->pToken = 0;
  }

  return rc;
}