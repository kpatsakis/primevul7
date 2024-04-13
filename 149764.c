compile_string_node(Node* node, regex_t* reg)
{
  int r, len, prev_len, blen, ambig;
  OnigEncoding enc = reg->enc;
  UChar *p, *prev, *end;
  StrNode* sn;

  sn = NSTR(node);
  if (sn->end <= sn->s)
    return 0;

  end = sn->end;
  ambig = NSTRING_IS_AMBIG(node);

  p = prev = sn->s;
  prev_len = enclen(enc, p, end);
  p += prev_len;
  blen = prev_len;

  for (; p < end; ) {
    len = enclen(enc, p, end);
    if (len == prev_len || ambig) {
      blen += len;
    }
    else {
      r = add_compile_string(prev, prev_len, blen, reg, ambig);
      if (r) return r;

      prev  = p;
      blen  = len;
      prev_len = len;
    }

    p += len;
  }
  return add_compile_string(prev, prev_len, blen, reg, ambig);
}