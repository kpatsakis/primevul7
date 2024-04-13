static void collectvalidlines (lua_State *L, Closure *f) {
  if (noLuaClosure(f)) {
    setnilvalue(s2v(L->top));
    api_incr_top(L);
  }
  else {
    int i;
    TValue v;
    const Proto *p = f->l.p;
    int currentline = p->linedefined;
    Table *t = luaH_new(L);  /* new table to store active lines */
    sethvalue2s(L, L->top, t);  /* push it on stack */
    api_incr_top(L);
    setbtvalue(&v);  /* boolean 'true' to be the value of all indices */
    if (!p->is_vararg)  /* regular function? */
      i = 0;  /* consider all instructions */
    else {  /* vararg function */
      lua_assert(GET_OPCODE(p->code[0]) == OP_VARARGPREP);
      currentline = nextline(p, currentline, 0);
      i = 1;  /* skip first instruction (OP_VARARGPREP) */
    }
    for (; i < p->sizelineinfo; i++) {  /* for each instruction */
      currentline = nextline(p, currentline, i);  /* get its line */
      luaH_setint(L, t, currentline, &v);  /* table[line] = true */
    }
  }
}