stack_double(OnigStackType** arg_stk_base, OnigStackType** arg_stk_end,
	     OnigStackType** arg_stk, OnigStackType* stk_alloc, OnigMatchArg* msa)
{
  size_t n;
  OnigStackType *x, *stk_base, *stk_end, *stk;

  stk_base = *arg_stk_base;
  stk_end  = *arg_stk_end;
  stk      = *arg_stk;

  n = stk_end - stk_base;
  if (stk_base == stk_alloc && IS_NULL(msa->stack_p)) {
    x = (OnigStackType* )xmalloc(sizeof(OnigStackType) * n * 2);
    if (IS_NULL(x)) {
      STACK_SAVE;
      return ONIGERR_MEMORY;
    }
    xmemcpy(x, stk_base, n * sizeof(OnigStackType));
    n *= 2;
  }
  else {
    unsigned int limit_size = MatchStackLimitSize;
    n *= 2;
    if (limit_size != 0 && n > limit_size) {
      if ((unsigned int )(stk_end - stk_base) == limit_size)
	return ONIGERR_MATCH_STACK_LIMIT_OVER;
      else
	n = limit_size;
    }
    x = (OnigStackType* )xrealloc(stk_base, sizeof(OnigStackType) * n);
    if (IS_NULL(x)) {
      STACK_SAVE;
      return ONIGERR_MEMORY;
    }
  }
  *arg_stk      = x + (stk - stk_base);
  *arg_stk_base = x;
  *arg_stk_end  = x + n;
  return 0;
}