cclass_case_fold(Node** np, CClassNode* cc, CClassNode* asc_cc, ScanEnv* env)
{
  int r;
  IApplyCaseFoldArg iarg;

  iarg.env         = env;
  iarg.cc          = cc;
  iarg.asc_cc      = asc_cc;
  iarg.alt_root    = NULL_NODE;
  iarg.ptail       = &(iarg.alt_root);

  r = ONIGENC_APPLY_ALL_CASE_FOLD(env->enc, env->case_fold_flag,
				  i_apply_case_fold, &iarg);
  if (r != 0) {
    onig_node_free(iarg.alt_root);
    return r;
  }
  if (IS_NOT_NULL(iarg.alt_root)) {
    Node* work = onig_node_new_alt(*np, iarg.alt_root);
    if (IS_NULL(work)) {
      onig_node_free(iarg.alt_root);
      return ONIGERR_MEMORY;
    }
    *np = work;
  }
  return r;
}