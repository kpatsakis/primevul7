make_absent_tail(Node** node1, Node** node2, int pre_save_right_id,
                 ScanEnv* env)
{
  int r;
  int id;
  Node* save;
  Node* x;
  Node* ns[2];

  *node1 = *node2 = NULL_NODE;
  save = ns[0] = ns[1] = NULL_NODE;

  r = node_new_save_gimmick(&save, SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id = GIMMICK_(save)->id;
  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[1], env);
  if (r != 0) goto err;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = NULL_NODE; ns[1] = x;

  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  pre_save_right_id, env);
  if (r != 0) goto err;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  *node1 = save;
  *node2 = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  onig_node_free(save);
  onig_node_free(ns[0]);
  onig_node_free(ns[1]);
  return r;
}