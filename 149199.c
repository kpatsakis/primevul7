make_extended_grapheme_cluster(Node** node, ScanEnv* env)
{
  int r;
  int i;
  Node* x;
  Node* ns[2];

  /* \X == (?>\O(?:\Y\O)*) */

  ns[1] = NULL_NODE;

  r = ONIGERR_MEMORY;
  ns[0] = onig_node_new_anchor(ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY, 0);
  if (IS_NULL(ns[0])) goto err;

  r = node_new_true_anychar(&ns[1], env);
  if (r != 0) goto err1;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err;
  ns[0] = x;
  ns[1] = NULL_NODE;

  x = node_new_quantifier(0, REPEAT_INFINITE, 1);
  if (IS_NULL(x)) goto err;

  NODE_BODY(x) = ns[0];
  ns[0] = NULL_NODE;
  ns[1] = x;

  r = node_new_true_anychar(&ns[0], env);
  if (r != 0) goto err1;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err;

  ns[0] = x;
  ns[1] = NULL_NODE;

  x = node_new_enclosure(ENCLOSURE_STOP_BACKTRACK);
  if (IS_NULL(x)) goto err;

  NODE_BODY(x) = ns[0];

  *node = x;
  return ONIG_NORMAL;

 err:
  r = ONIGERR_MEMORY;
 err1:
  for (i = 0; i < 2; i++) onig_node_free(ns[i]);
  return r;
}