node_new_call(UChar* name, UChar* name_end, int gnum, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_CALL);
  CALL_(node)->by_number   = by_number;
  CALL_(node)->name        = name;
  CALL_(node)->name_end    = name_end;
  CALL_(node)->group_num   = gnum;
  CALL_(node)->entry_count = 1;
  return node;
}