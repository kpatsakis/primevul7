onig_node_str_clear(Node* node)
{
  if (STR_(node)->capa != 0 &&
      IS_NOT_NULL(STR_(node)->s) && STR_(node)->s != STR_(node)->buf) {
    xfree(STR_(node)->s);
  }

  STR_(node)->capa = 0;
  STR_(node)->flag = 0;
  STR_(node)->s    = STR_(node)->buf;
  STR_(node)->end  = STR_(node)->buf;
}