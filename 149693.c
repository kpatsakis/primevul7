parse_char_property(Node** np, OnigToken* tok, UChar** src, UChar* end,
		    ScanEnv* env)
{
  int r, ctype;
  CClassNode* cc;

  ctype = fetch_char_property_to_ctype(src, end, env);
  if (ctype < 0) return ctype;

  *np = node_new_cclass();
  CHECK_NULL_RETURN_MEMERR(*np);
  cc = NCCLASS(*np);
  r = add_ctype_to_cc(cc, ctype, 0, 0, env);
  if (r != 0) return r;
  if (tok->u.prop.not != 0) NCCLASS_SET_NOT(cc);

  if (IS_IGNORECASE(env->option)) {
    if (ctype != ONIGENC_CTYPE_ASCII)
      r = cclass_case_fold(np, cc, cc, env);
  }
  return r;
}