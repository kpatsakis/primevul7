node_new_save_gimmick(Node** node, enum SaveType save_type, ScanEnv* env)
{
  int id;
  int r;

  r = save_entry(env, save_type, &id);
  if (r != ONIG_NORMAL) return r;

  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->id   = id;
  GIMMICK_(*node)->type = GIMMICK_SAVE;
  GIMMICK_(*node)->detail_type = (int )save_type;

  return ONIG_NORMAL;
}