static i8 wherePathSatisfiesOrderBy(
  WhereInfo *pWInfo,    /* The WHERE clause */
  ExprList *pOrderBy,   /* ORDER BY or GROUP BY or DISTINCT clause to check */
  WherePath *pPath,     /* The WherePath to check */
  u16 wctrlFlags,       /* WHERE_GROUPBY or _DISTINCTBY or _ORDERBY_LIMIT */
  u16 nLoop,            /* Number of entries in pPath->aLoop[] */
  WhereLoop *pLast,     /* Add this WhereLoop to the end of pPath->aLoop[] */
  Bitmask *pRevMask     /* OUT: Mask of WhereLoops to run in reverse order */
){
  u8 revSet;            /* True if rev is known */
  u8 rev;               /* Composite sort order */
  u8 revIdx;            /* Index sort order */
  u8 isOrderDistinct;   /* All prior WhereLoops are order-distinct */
  u8 distinctColumns;   /* True if the loop has UNIQUE NOT NULL columns */
  u8 isMatch;           /* iColumn matches a term of the ORDER BY clause */
  u16 eqOpMask;         /* Allowed equality operators */
  u16 nKeyCol;          /* Number of key columns in pIndex */
  u16 nColumn;          /* Total number of ordered columns in the index */
  u16 nOrderBy;         /* Number terms in the ORDER BY clause */
  int iLoop;            /* Index of WhereLoop in pPath being processed */
  int i, j;             /* Loop counters */
  int iCur;             /* Cursor number for current WhereLoop */
  int iColumn;          /* A column number within table iCur */
  WhereLoop *pLoop = 0; /* Current WhereLoop being processed. */
  WhereTerm *pTerm;     /* A single term of the WHERE clause */
  Expr *pOBExpr;        /* An expression from the ORDER BY clause */
  CollSeq *pColl;       /* COLLATE function from an ORDER BY clause term */
  Index *pIndex;        /* The index associated with pLoop */
  sqlite3 *db = pWInfo->pParse->db;  /* Database connection */
  Bitmask obSat = 0;    /* Mask of ORDER BY terms satisfied so far */
  Bitmask obDone;       /* Mask of all ORDER BY terms */
  Bitmask orderDistinctMask;  /* Mask of all well-ordered loops */
  Bitmask ready;              /* Mask of inner loops */

  /*
  ** We say the WhereLoop is "one-row" if it generates no more than one
  ** row of output.  A WhereLoop is one-row if all of the following are true:
  **  (a) All index columns match with WHERE_COLUMN_EQ.
  **  (b) The index is unique
  ** Any WhereLoop with an WHERE_COLUMN_EQ constraint on the rowid is one-row.
  ** Every one-row WhereLoop will have the WHERE_ONEROW bit set in wsFlags.
  **
  ** We say the WhereLoop is "order-distinct" if the set of columns from
  ** that WhereLoop that are in the ORDER BY clause are different for every
  ** row of the WhereLoop.  Every one-row WhereLoop is automatically
  ** order-distinct.   A WhereLoop that has no columns in the ORDER BY clause
  ** is not order-distinct. To be order-distinct is not quite the same as being
  ** UNIQUE since a UNIQUE column or index can have multiple rows that 
  ** are NULL and NULL values are equivalent for the purpose of order-distinct.
  ** To be order-distinct, the columns must be UNIQUE and NOT NULL.
  **
  ** The rowid for a table is always UNIQUE and NOT NULL so whenever the
  ** rowid appears in the ORDER BY clause, the corresponding WhereLoop is
  ** automatically order-distinct.
  */

  assert( pOrderBy!=0 );
  if( nLoop && OptimizationDisabled(db, SQLITE_OrderByIdxJoin) ) return 0;

  nOrderBy = pOrderBy->nExpr;
  testcase( nOrderBy==BMS-1 );
  if( nOrderBy>BMS-1 ) return 0;  /* Cannot optimize overly large ORDER BYs */
  isOrderDistinct = 1;
  obDone = MASKBIT(nOrderBy)-1;
  orderDistinctMask = 0;
  ready = 0;
  eqOpMask = WO_EQ | WO_IS | WO_ISNULL;
  if( wctrlFlags & (WHERE_ORDERBY_LIMIT|WHERE_ORDERBY_MAX|WHERE_ORDERBY_MIN) ){
    eqOpMask |= WO_IN;
  }
  for(iLoop=0; isOrderDistinct && obSat<obDone && iLoop<=nLoop; iLoop++){
    if( iLoop>0 ) ready |= pLoop->maskSelf;
    if( iLoop<nLoop ){
      pLoop = pPath->aLoop[iLoop];
      if( wctrlFlags & WHERE_ORDERBY_LIMIT ) continue;
    }else{
      pLoop = pLast;
    }
    if( pLoop->wsFlags & WHERE_VIRTUALTABLE ){
      if( pLoop->u.vtab.isOrdered 
       && ((wctrlFlags&(WHERE_DISTINCTBY|WHERE_SORTBYGROUP))!=WHERE_DISTINCTBY)
      ){
        obSat = obDone;
      }
      break;
    }else if( wctrlFlags & WHERE_DISTINCTBY ){
      pLoop->u.btree.nDistinctCol = 0;
    }
    iCur = pWInfo->pTabList->a[pLoop->iTab].iCursor;

    /* Mark off any ORDER BY term X that is a column in the table of
    ** the current loop for which there is term in the WHERE
    ** clause of the form X IS NULL or X=? that reference only outer
    ** loops.
    */
    for(i=0; i<nOrderBy; i++){
      if( MASKBIT(i) & obSat ) continue;
      pOBExpr = sqlite3ExprSkipCollateAndLikely(pOrderBy->a[i].pExpr);
      if( NEVER(pOBExpr==0) ) continue;
      if( pOBExpr->op!=TK_COLUMN && pOBExpr->op!=TK_AGG_COLUMN ) continue;
      if( pOBExpr->iTable!=iCur ) continue;
      pTerm = sqlite3WhereFindTerm(&pWInfo->sWC, iCur, pOBExpr->iColumn,
                       ~ready, eqOpMask, 0);
      if( pTerm==0 ) continue;
      if( pTerm->eOperator==WO_IN ){
        /* IN terms are only valid for sorting in the ORDER BY LIMIT 
        ** optimization, and then only if they are actually used
        ** by the query plan */
        assert( wctrlFlags & 
               (WHERE_ORDERBY_LIMIT|WHERE_ORDERBY_MIN|WHERE_ORDERBY_MAX) );
        for(j=0; j<pLoop->nLTerm && pTerm!=pLoop->aLTerm[j]; j++){}
        if( j>=pLoop->nLTerm ) continue;
      }
      if( (pTerm->eOperator&(WO_EQ|WO_IS))!=0 && pOBExpr->iColumn>=0 ){
        Parse *pParse = pWInfo->pParse;
        CollSeq *pColl1 = sqlite3ExprNNCollSeq(pParse, pOrderBy->a[i].pExpr);
        CollSeq *pColl2 = sqlite3ExprCompareCollSeq(pParse, pTerm->pExpr);
        assert( pColl1 );
        if( pColl2==0 || sqlite3StrICmp(pColl1->zName, pColl2->zName) ){
          continue;
        }
        testcase( pTerm->pExpr->op==TK_IS );
      }
      obSat |= MASKBIT(i);
    }

    if( (pLoop->wsFlags & WHERE_ONEROW)==0 ){
      if( pLoop->wsFlags & WHERE_IPK ){
        pIndex = 0;
        nKeyCol = 0;
        nColumn = 1;
      }else if( (pIndex = pLoop->u.btree.pIndex)==0 || pIndex->bUnordered ){
        return 0;
      }else{
        nKeyCol = pIndex->nKeyCol;
        nColumn = pIndex->nColumn;
        assert( nColumn==nKeyCol+1 || !HasRowid(pIndex->pTable) );
        assert( pIndex->aiColumn[nColumn-1]==XN_ROWID
                          || !HasRowid(pIndex->pTable));
        /* All relevant terms of the index must also be non-NULL in order
        ** for isOrderDistinct to be true.  So the isOrderDistint value
        ** computed here might be a false positive.  Corrections will be
        ** made at tag-20210426-1 below */
        isOrderDistinct = IsUniqueIndex(pIndex)
                          && (pLoop->wsFlags & WHERE_SKIPSCAN)==0;
      }

      /* Loop through all columns of the index and deal with the ones
      ** that are not constrained by == or IN.
      */
      rev = revSet = 0;
      distinctColumns = 0;
      for(j=0; j<nColumn; j++){
        u8 bOnce = 1; /* True to run the ORDER BY search loop */

        assert( j>=pLoop->u.btree.nEq 
            || (pLoop->aLTerm[j]==0)==(j<pLoop->nSkip)
        );
        if( j<pLoop->u.btree.nEq && j>=pLoop->nSkip ){
          u16 eOp = pLoop->aLTerm[j]->eOperator;

          /* Skip over == and IS and ISNULL terms.  (Also skip IN terms when
          ** doing WHERE_ORDERBY_LIMIT processing).  Except, IS and ISNULL
          ** terms imply that the index is not UNIQUE NOT NULL in which case
          ** the loop need to be marked as not order-distinct because it can
          ** have repeated NULL rows.
          **
          ** If the current term is a column of an ((?,?) IN (SELECT...)) 
          ** expression for which the SELECT returns more than one column,
          ** check that it is the only column used by this loop. Otherwise,
          ** if it is one of two or more, none of the columns can be
          ** considered to match an ORDER BY term.
          */
          if( (eOp & eqOpMask)!=0 ){
            if( eOp & (WO_ISNULL|WO_IS) ){
              testcase( eOp & WO_ISNULL );
              testcase( eOp & WO_IS );
              testcase( isOrderDistinct );
              isOrderDistinct = 0;
            }
            continue;  
          }else if( ALWAYS(eOp & WO_IN) ){
            /* ALWAYS() justification: eOp is an equality operator due to the
            ** j<pLoop->u.btree.nEq constraint above.  Any equality other
            ** than WO_IN is captured by the previous "if".  So this one
            ** always has to be WO_IN. */
            Expr *pX = pLoop->aLTerm[j]->pExpr;
            for(i=j+1; i<pLoop->u.btree.nEq; i++){
              if( pLoop->aLTerm[i]->pExpr==pX ){
                assert( (pLoop->aLTerm[i]->eOperator & WO_IN) );
                bOnce = 0;
                break;
              }
            }
          }
        }

        /* Get the column number in the table (iColumn) and sort order
        ** (revIdx) for the j-th column of the index.
        */
        if( pIndex ){
          iColumn = pIndex->aiColumn[j];
          revIdx = pIndex->aSortOrder[j] & KEYINFO_ORDER_DESC;
          if( iColumn==pIndex->pTable->iPKey ) iColumn = XN_ROWID;
        }else{
          iColumn = XN_ROWID;
          revIdx = 0;
        }

        /* An unconstrained column that might be NULL means that this
        ** WhereLoop is not well-ordered.  tag-20210426-1
        */
        if( isOrderDistinct ){
          if( iColumn>=0
           && j>=pLoop->u.btree.nEq
           && pIndex->pTable->aCol[iColumn].notNull==0
          ){
            isOrderDistinct = 0;
          }
          if( iColumn==XN_EXPR ){
            isOrderDistinct = 0;
          }
        } 

        /* Find the ORDER BY term that corresponds to the j-th column
        ** of the index and mark that ORDER BY term off 
        */
        isMatch = 0;
        for(i=0; bOnce && i<nOrderBy; i++){
          if( MASKBIT(i) & obSat ) continue;
          pOBExpr = sqlite3ExprSkipCollateAndLikely(pOrderBy->a[i].pExpr);
          testcase( wctrlFlags & WHERE_GROUPBY );
          testcase( wctrlFlags & WHERE_DISTINCTBY );
          if( NEVER(pOBExpr==0) ) continue;
          if( (wctrlFlags & (WHERE_GROUPBY|WHERE_DISTINCTBY))==0 ) bOnce = 0;
          if( iColumn>=XN_ROWID ){
            if( pOBExpr->op!=TK_COLUMN && pOBExpr->op!=TK_AGG_COLUMN ) continue;
            if( pOBExpr->iTable!=iCur ) continue;
            if( pOBExpr->iColumn!=iColumn ) continue;
          }else{
            Expr *pIdxExpr = pIndex->aColExpr->a[j].pExpr;
            if( sqlite3ExprCompareSkip(pOBExpr, pIdxExpr, iCur) ){
              continue;
            }
          }
          if( iColumn!=XN_ROWID ){
            pColl = sqlite3ExprNNCollSeq(pWInfo->pParse, pOrderBy->a[i].pExpr);
            if( sqlite3StrICmp(pColl->zName, pIndex->azColl[j])!=0 ) continue;
          }
          if( wctrlFlags & WHERE_DISTINCTBY ){
            pLoop->u.btree.nDistinctCol = j+1;
          }
          isMatch = 1;
          break;
        }
        if( isMatch && (wctrlFlags & WHERE_GROUPBY)==0 ){
          /* Make sure the sort order is compatible in an ORDER BY clause.
          ** Sort order is irrelevant for a GROUP BY clause. */
          if( revSet ){
            if( (rev ^ revIdx) 
                           != (pOrderBy->a[i].fg.sortFlags&KEYINFO_ORDER_DESC)
            ){
              isMatch = 0;
            }
          }else{
            rev = revIdx ^ (pOrderBy->a[i].fg.sortFlags & KEYINFO_ORDER_DESC);
            if( rev ) *pRevMask |= MASKBIT(iLoop);
            revSet = 1;
          }
        }
        if( isMatch && (pOrderBy->a[i].fg.sortFlags & KEYINFO_ORDER_BIGNULL) ){
          if( j==pLoop->u.btree.nEq ){
            pLoop->wsFlags |= WHERE_BIGNULL_SORT;
          }else{
            isMatch = 0;
          }
        }
        if( isMatch ){
          if( iColumn==XN_ROWID ){
            testcase( distinctColumns==0 );
            distinctColumns = 1;
          }
          obSat |= MASKBIT(i);
        }else{
          /* No match found */
          if( j==0 || j<nKeyCol ){
            testcase( isOrderDistinct!=0 );
            isOrderDistinct = 0;
          }
          break;
        }
      } /* end Loop over all index columns */
      if( distinctColumns ){
        testcase( isOrderDistinct==0 );
        isOrderDistinct = 1;
      }
    } /* end-if not one-row */

    /* Mark off any other ORDER BY terms that reference pLoop */
    if( isOrderDistinct ){
      orderDistinctMask |= pLoop->maskSelf;
      for(i=0; i<nOrderBy; i++){
        Expr *p;
        Bitmask mTerm;
        if( MASKBIT(i) & obSat ) continue;
        p = pOrderBy->a[i].pExpr;
        mTerm = sqlite3WhereExprUsage(&pWInfo->sMaskSet,p);
        if( mTerm==0 && !sqlite3ExprIsConstant(p) ) continue;
        if( (mTerm&~orderDistinctMask)==0 ){
          obSat |= MASKBIT(i);
        }
      }
    }
  } /* End the loop over all WhereLoops from outer-most down to inner-most */
  if( obSat==obDone ) return (i8)nOrderBy;
  if( !isOrderDistinct ){
    for(i=nOrderBy-1; i>0; i--){
      Bitmask m = ALWAYS(i<BMS) ? MASKBIT(i) - 1 : 0;
      if( (obSat&m)==m ) return i;
    }
    return 0;
  }
  return -1;
}