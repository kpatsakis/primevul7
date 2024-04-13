node_new_update_var_gimmick(Node** node, enum UpdateVarType update_var_type,
                            int id, ScanEnv* env)
{
  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->id   = id;
  GIMMICK_(*node)->type = GIMMICK_UPDATE_VAR;
  GIMMICK_(*node)->detail_type = (int )update_var_type;

  return ONIG_NORMAL;
}