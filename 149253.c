str_node_split_last_char(StrNode* sn, OnigEncoding enc)
{
  const UChar *p;
  Node* n = NULL_NODE;

  if (sn->end > sn->s) {
    p = onigenc_get_prev_char_head(enc, sn->s, sn->end);
    if (p && p > sn->s) { /* can be split. */
      n = node_new_str(p, sn->end);
      if ((sn->flag & STRING_RAW) != 0)
        NODE_STRING_SET_RAW(n);

      sn->end = (UChar* )p;
    }
  }
  return n;
}