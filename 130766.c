static int fts3PoslistPhraseMerge(
  char **pp,                      /* IN/OUT: Preallocated output buffer */
  int nToken,                     /* Maximum difference in token positions */
  int isSaveLeft,                 /* Save the left position */
  int isExact,                    /* If *pp1 is exactly nTokens before *pp2 */
  char **pp1,                     /* IN/OUT: Left input list */
  char **pp2                      /* IN/OUT: Right input list */
){
  char *p = *pp;
  char *p1 = *pp1;
  char *p2 = *pp2;
  int iCol1 = 0;
  int iCol2 = 0;

  /* Never set both isSaveLeft and isExact for the same invocation. */
  assert( isSaveLeft==0 || isExact==0 );

  assert( p!=0 && *p1!=0 && *p2!=0 );
  if( *p1==POS_COLUMN ){ 
    p1++;
    p1 += fts3GetVarint32(p1, &iCol1);
  }
  if( *p2==POS_COLUMN ){ 
    p2++;
    p2 += fts3GetVarint32(p2, &iCol2);
  }

  while( 1 ){
    if( iCol1==iCol2 ){
      char *pSave = p;
      sqlite3_int64 iPrev = 0;
      sqlite3_int64 iPos1 = 0;
      sqlite3_int64 iPos2 = 0;

      if( iCol1 ){
        *p++ = POS_COLUMN;
        p += sqlite3Fts3PutVarint(p, iCol1);
      }

      assert( *p1!=POS_END && *p1!=POS_COLUMN );
      assert( *p2!=POS_END && *p2!=POS_COLUMN );
      fts3GetDeltaVarint(&p1, &iPos1); iPos1 -= 2;
      fts3GetDeltaVarint(&p2, &iPos2); iPos2 -= 2;

      while( 1 ){
        if( iPos2==iPos1+nToken 
         || (isExact==0 && iPos2>iPos1 && iPos2<=iPos1+nToken) 
        ){
          sqlite3_int64 iSave;
          iSave = isSaveLeft ? iPos1 : iPos2;
          fts3PutDeltaVarint(&p, &iPrev, iSave+2); iPrev -= 2;
          pSave = 0;
          assert( p );
        }
        if( (!isSaveLeft && iPos2<=(iPos1+nToken)) || iPos2<=iPos1 ){
          if( (*p2&0xFE)==0 ) break;
          fts3GetDeltaVarint(&p2, &iPos2); iPos2 -= 2;
        }else{
          if( (*p1&0xFE)==0 ) break;
          fts3GetDeltaVarint(&p1, &iPos1); iPos1 -= 2;
        }
      }

      if( pSave ){
        assert( pp && p );
        p = pSave;
      }

      fts3ColumnlistCopy(0, &p1);
      fts3ColumnlistCopy(0, &p2);
      assert( (*p1&0xFE)==0 && (*p2&0xFE)==0 );
      if( 0==*p1 || 0==*p2 ) break;

      p1++;
      p1 += fts3GetVarint32(p1, &iCol1);
      p2++;
      p2 += fts3GetVarint32(p2, &iCol2);
    }

    /* Advance pointer p1 or p2 (whichever corresponds to the smaller of
    ** iCol1 and iCol2) so that it points to either the 0x00 that marks the
    ** end of the position list, or the 0x01 that precedes the next 
    ** column-number in the position list. 
    */
    else if( iCol1<iCol2 ){
      fts3ColumnlistCopy(0, &p1);
      if( 0==*p1 ) break;
      p1++;
      p1 += fts3GetVarint32(p1, &iCol1);
    }else{
      fts3ColumnlistCopy(0, &p2);
      if( 0==*p2 ) break;
      p2++;
      p2 += fts3GetVarint32(p2, &iCol2);
    }
  }

  fts3PoslistCopy(0, &p2);
  fts3PoslistCopy(0, &p1);
  *pp1 = p1;
  *pp2 = p2;
  if( *pp==p ){
    return 0;
  }
  *p++ = 0x00;
  *pp = p;
  return 1;
}