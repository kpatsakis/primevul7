fetch_named_backref_token(OnigCodePoint c, OnigToken* tok, UChar** src,
			  UChar* end, ScanEnv* env)
{
  int r, num;
  const OnigSyntaxType* syn = env->syntax;
  UChar* prev;
  UChar* p = *src;
  UChar* name_end;
  int* backs;
  int back_num;

  prev = p;

# ifdef USE_BACKREF_WITH_LEVEL
  name_end = NULL_UCHARP; /* no need. escape gcc warning. */
  r = fetch_name_with_level(c, &p, end, &name_end,
			    env, &back_num, &tok->u.backref.level);
  if (r == 1) tok->u.backref.exist_level = 1;
  else        tok->u.backref.exist_level = 0;
# else
  r = fetch_name(&p, end, &name_end, env, &back_num, 1);
# endif
  if (r < 0) return r;

  if (back_num != 0) {
    if (back_num < 0) {
      back_num = BACKREF_REL_TO_ABS(back_num, env);
      if (back_num <= 0)
	return ONIGERR_INVALID_BACKREF;
    }

    if (IS_SYNTAX_BV(syn, ONIG_SYN_STRICT_CHECK_BACKREF)) {
      if (back_num > env->num_mem ||
	  IS_NULL(SCANENV_MEM_NODES(env)[back_num]))
	return ONIGERR_INVALID_BACKREF;
    }
    tok->type = TK_BACKREF;
    tok->u.backref.by_name = 0;
    tok->u.backref.num  = 1;
    tok->u.backref.ref1 = back_num;
  }
  else {
    num = onig_name_to_group_numbers(env->reg, prev, name_end, &backs);
    if (num <= 0) {
      onig_scan_env_set_error_string(env,
		     ONIGERR_UNDEFINED_NAME_REFERENCE, prev, name_end);
      return ONIGERR_UNDEFINED_NAME_REFERENCE;
    }
    if (IS_SYNTAX_BV(syn, ONIG_SYN_STRICT_CHECK_BACKREF)) {
      int i;
      for (i = 0; i < num; i++) {
	if (backs[i] > env->num_mem ||
	    IS_NULL(SCANENV_MEM_NODES(env)[backs[i]]))
	  return ONIGERR_INVALID_BACKREF;
      }
    }

    tok->type = TK_BACKREF;
    tok->u.backref.by_name = 1;
    if (num == 1 || IS_SYNTAX_BV(syn, ONIG_SYN_USE_LEFT_MOST_NAMED_GROUP)) {
      tok->u.backref.num  = 1;
      tok->u.backref.ref1 = backs[0];
    }
    else {
      tok->u.backref.num  = num;
      tok->u.backref.refs = backs;
    }
  }
  *src = p;
  return 0;
}