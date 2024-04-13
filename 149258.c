onig_parse_tree(Node** root, const UChar* pattern, const UChar* end,
                regex_t* reg, ScanEnv* env)
{
  int r;
  UChar* p;

  names_clear(reg);

  scan_env_clear(env);
  env->options        = reg->options;
  env->case_fold_flag = reg->case_fold_flag;
  env->enc            = reg->enc;
  env->syntax         = reg->syntax;
  env->pattern        = (UChar* )pattern;
  env->pattern_end    = (UChar* )end;
  env->reg            = reg;

  *root = NULL;

  if (! ONIGENC_IS_VALID_MBC_STRING(env->enc, pattern, end))
    return ONIGERR_INVALID_WIDE_CHAR_VALUE;

  p = (UChar* )pattern;
  r = parse_regexp(root, &p, (UChar* )end, env);

#ifdef USE_CALL
  if (r != 0) return r;

  if (env->has_call_zero != 0) {
    Node* zero_node;
    r = make_call_zero_body(*root, env, &zero_node);
    if (r != 0) return r;

    *root = zero_node;
  }
#endif

  reg->num_mem = env->num_mem;
  return r;
}