node_str_cat_codepoint(Node* node, OnigEncoding enc, OnigCodePoint c)
{
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
  int num = ONIGENC_CODE_TO_MBC(enc, c, buf);
  if (num < 0) return num;
  return onig_node_str_cat(node, buf, buf + num);
}