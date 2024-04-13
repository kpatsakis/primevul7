int sqlite3Fts3EvalPhrasePoslist(
  Fts3Cursor *pCsr,               /* FTS3 cursor object */
  Fts3Expr *pExpr,                /* Phrase to return doclist for */
  int iCol,                       /* Column to return position list for */
  char **ppOut                    /* OUT: Pointer to position list */
){
  Fts3Phrase *pPhrase = pExpr->pPhrase;
  Fts3Table *pTab = (Fts3Table *)pCsr->base.pVtab;
  char *pIter;
  int iThis;
  sqlite3_int64 iDocid;

  /* If this phrase is applies specifically to some column other than 
  ** column iCol, return a NULL pointer.  */
  *ppOut = 0;
  assert( iCol>=0 && iCol<pTab->nColumn );
  if( (pPhrase->iColumn<pTab->nColumn && pPhrase->iColumn!=iCol) ){
    return SQLITE_OK;
  }

  iDocid = pExpr->iDocid;
  pIter = pPhrase->doclist.pList;
  if( iDocid!=pCsr->iPrevId || pExpr->bEof ){
    int rc = SQLITE_OK;
    int bDescDoclist = pTab->bDescIdx;      /* For DOCID_CMP macro */
    int iMul;                     /* +1 if csr dir matches index dir, else -1 */
    int bOr = 0;
    u8 bEof = 0;
    u8 bTreeEof = 0;
    Fts3Expr *p;                  /* Used to iterate from pExpr to root */
    Fts3Expr *pNear;              /* Most senior NEAR ancestor (or pExpr) */

    /* Check if this phrase descends from an OR expression node. If not, 
    ** return NULL. Otherwise, the entry that corresponds to docid 
    ** pCsr->iPrevId may lie earlier in the doclist buffer. Or, if the
    ** tree that the node is part of has been marked as EOF, but the node
    ** itself is not EOF, then it may point to an earlier entry. */
    pNear = pExpr;
    for(p=pExpr->pParent; p; p=p->pParent){
      if( p->eType==FTSQUERY_OR ) bOr = 1;
      if( p->eType==FTSQUERY_NEAR ) pNear = p;
      if( p->bEof ) bTreeEof = 1;
    }
    if( bOr==0 ) return SQLITE_OK;

    /* This is the descendent of an OR node. In this case we cannot use
    ** an incremental phrase. Load the entire doclist for the phrase
    ** into memory in this case.  */
    if( pPhrase->bIncr ){
      int bEofSave = pNear->bEof;
      fts3EvalRestart(pCsr, pNear, &rc);
      while( rc==SQLITE_OK && !pNear->bEof ){
        fts3EvalNextRow(pCsr, pNear, &rc);
        if( bEofSave==0 && pNear->iDocid==iDocid ) break;
      }
      assert( rc!=SQLITE_OK || pPhrase->bIncr==0 );
    }
    if( bTreeEof ){
      while( rc==SQLITE_OK && !pNear->bEof ){
        fts3EvalNextRow(pCsr, pNear, &rc);
      }
    }
    if( rc!=SQLITE_OK ) return rc;

    pIter = pPhrase->pOrPoslist;
    iDocid = pPhrase->iOrDocid;
    if( pCsr->bDesc==bDescDoclist ){
      bEof = (pIter >= (pPhrase->doclist.aAll + pPhrase->doclist.nAll));
      while( (pIter==0 || DOCID_CMP(iDocid, pCsr->iPrevId)<0 ) && bEof==0 ){
        sqlite3Fts3DoclistNext(
            bDescDoclist, pPhrase->doclist.aAll, pPhrase->doclist.nAll, 
            &pIter, &iDocid, &bEof
        );
      }
    }else{
      bEof = !pPhrase->doclist.nAll || (pIter && pIter<=pPhrase->doclist.aAll);
      while( (pIter==0 || DOCID_CMP(iDocid, pCsr->iPrevId)>0 ) && bEof==0 ){
        int dummy;
        sqlite3Fts3DoclistPrev(
            bDescDoclist, pPhrase->doclist.aAll, pPhrase->doclist.nAll, 
            &pIter, &iDocid, &dummy, &bEof
        );
      }
    }
    pPhrase->pOrPoslist = pIter;
    pPhrase->iOrDocid = iDocid;

    if( bEof || iDocid!=pCsr->iPrevId ) pIter = 0;
  }
  if( pIter==0 ) return SQLITE_OK;

  if( *pIter==0x01 ){
    pIter++;
    pIter += fts3GetVarint32(pIter, &iThis);
  }else{
    iThis = 0;
  }
  while( iThis<iCol ){
    fts3ColumnlistCopy(0, &pIter);
    if( *pIter==0x00 ) return SQLITE_OK;
    pIter++;
    pIter += fts3GetVarint32(pIter, &iThis);
  }
  if( *pIter==0x00 ){
    pIter = 0;
  }

  *ppOut = ((iCol==iThis)?pIter:0);
  return SQLITE_OK;
}