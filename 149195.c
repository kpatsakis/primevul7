node_new_keep(Node** node, ScanEnv* env)
{
  int r;

  r = node_new_save_gimmick(node, SAVE_KEEP, env);
  if (r != 0) return r;

  env->keep_num++;
  return ONIG_NORMAL;
}