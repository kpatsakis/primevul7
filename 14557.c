void luaV_concat (lua_State *L, int total) {
  if (total == 1)
    return;  /* "all" values already concatenated */
  do {
    StkId top = L->top;
    int n = 2;  /* number of elements handled in this pass (at least 2) */
    if (!(ttisstring(s2v(top - 2)) || cvt2str(s2v(top - 2))) ||
        !tostring(L, s2v(top - 1)))
      luaT_tryconcatTM(L);
    else if (isemptystr(s2v(top - 1)))  /* second operand is empty? */
      cast_void(tostring(L, s2v(top - 2)));  /* result is first operand */
    else if (isemptystr(s2v(top - 2))) {  /* first operand is empty string? */
      setobjs2s(L, top - 2, top - 1);  /* result is second op. */
    }
    else {
      /* at least two non-empty string values; get as many as possible */
      size_t tl = vslen(s2v(top - 1));
      TString *ts;
      /* collect total length and number of strings */
      for (n = 1; n < total && tostring(L, s2v(top - n - 1)); n++) {
        size_t l = vslen(s2v(top - n - 1));
        if (l_unlikely(l >= (MAX_SIZE/sizeof(char)) - tl)) {
          L->top = top - total;  /* pop strings to avoid wasting stack */
          luaG_runerror(L, "string length overflow");
        }
        tl += l;
      }
      if (tl <= LUAI_MAXSHORTLEN) {  /* is result a short string? */
        char buff[LUAI_MAXSHORTLEN];
        copy2buff(top, n, buff);  /* copy strings to buffer */
        ts = luaS_newlstr(L, buff, tl);
      }
      else {  /* long string; copy strings directly to final result */
        ts = luaS_createlngstrobj(L, tl);
        copy2buff(top, n, getstr(ts));
      }
      setsvalue2s(L, top - n, ts);  /* create result */
    }
    total -= n-1;  /* got 'n' strings to create 1 new */
    L->top = top - (n - 1);  /* popped 'n' strings and pushed one */
  } while (total > 1);  /* repeat until only 1 result left */
}