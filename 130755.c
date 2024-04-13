static int fts3EvalNearTrim(
  int nNear,                      /* NEAR distance. As in "NEAR/nNear". */
  char *aTmp,                     /* Temporary space to use */
  char **paPoslist,               /* IN/OUT: Position list */
  int *pnToken,                   /* IN/OUT: Tokens in phrase of *paPoslist */
  Fts3Phrase *pPhrase             /* The phrase object to trim the doclist of */
){
  int nParam1 = nNear + pPhrase->nToken;
  int nParam2 = nNear + *pnToken;
  int nNew;
  char *p2; 
  char *pOut; 
  int res;

  assert( pPhrase->doclist.pList );

  p2 = pOut = pPhrase->doclist.pList;
  res = fts3PoslistNearMerge(
    &pOut, aTmp, nParam1, nParam2, paPoslist, &p2
  );
  if( res ){
    nNew = (int)(pOut - pPhrase->doclist.pList) - 1;
    assert( pPhrase->doclist.pList[nNew]=='\0' );
    assert( nNew<=pPhrase->doclist.nList && nNew>0 );
    memset(&pPhrase->doclist.pList[nNew], 0, pPhrase->doclist.nList - nNew);
    pPhrase->doclist.nList = nNew;
    *paPoslist = pPhrase->doclist.pList;
    *pnToken = pPhrase->nToken;
  }

  return res;
}