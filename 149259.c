node_new_str_raw(UChar* s, UChar* end)
{
  Node* node = node_new_str(s, end);
  NODE_STRING_SET_RAW(node);
  return node;
}