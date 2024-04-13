node_new_fail(Node** node, ScanEnv* env)
{
  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->type = GIMMICK_FAIL;
  return ONIG_NORMAL;
}