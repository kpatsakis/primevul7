history_tree_clone(OnigCaptureTreeNode* node)
{
  int i, r;
  OnigCaptureTreeNode *clone, *child;

  clone = history_node_new();
  CHECK_NULL_RETURN(clone);

  clone->beg = node->beg;
  clone->end = node->end;
  for (i = 0; i < node->num_childs; i++) {
    child = history_tree_clone(node->childs[i]);
    if (IS_NULL(child)) {
      history_tree_free(clone);
      return (OnigCaptureTreeNode* )0;
    }
    r = history_tree_add_child(clone, child);
    if (r != 0) {
      history_tree_free(child);
      history_tree_free(clone);
      return (OnigCaptureTreeNode* )0;
    }
  }

  return clone;
}