node_linebreak(Node** np, ScanEnv* env)
{
  /* same as (?>\x0D\x0A|[\x0A-\x0D\x{85}\x{2028}\x{2029}]) */
  Node* left = NULL;
  Node* right = NULL;
  Node* target1 = NULL;
  Node* target2 = NULL;
  CClassNode* cc;
  int num1, num2, r;
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN * 2];

  /* \x0D\x0A */
  num1 = ONIGENC_CODE_TO_MBC(env->enc, 0x0D, buf);
  if (num1 < 0) return num1;
  num2 = ONIGENC_CODE_TO_MBC(env->enc, 0x0A, buf + num1);
  if (num2 < 0) return num2;
  left = node_new_str_raw(buf, buf + num1 + num2);
  if (IS_NULL(left)) goto err;

  /* [\x0A-\x0D] or [\x0A-\x0D\x{85}\x{2028}\x{2029}] */
  right = node_new_cclass();
  if (IS_NULL(right)) goto err;
  cc = NCCLASS(right);
  if (ONIGENC_MBC_MINLEN(env->enc) > 1) {
    r = add_code_range(&(cc->mbuf), env, 0x0A, 0x0D);
    if (r != 0) goto err;
  }
  else {
    bitset_set_range(env, cc->bs, 0x0A, 0x0D);
  }

  /* TODO: move this block to enc/unicode.c */
  if (ONIGENC_IS_UNICODE(env->enc)) {
    /* UTF-8, UTF-16BE/LE, UTF-32BE/LE */
    r = add_code_range(&(cc->mbuf), env, 0x85, 0x85);
    if (r != 0) goto err;
    r = add_code_range(&(cc->mbuf), env, 0x2028, 0x2029);
    if (r != 0) goto err;
  }

  /* ...|... */
  target1 = onig_node_new_alt(right, NULL_NODE);
  if (IS_NULL(target1)) goto err;
  right = NULL;
  target2 = onig_node_new_alt(left, target1);
  if (IS_NULL(target2)) goto err;
  left = NULL;
  target1 = NULL;

  /* (?>...) */
  *np = node_new_enclose(ENCLOSE_STOP_BACKTRACK);
  if (IS_NULL(*np)) goto err;
  NENCLOSE(*np)->target = target2;
  return ONIG_NORMAL;

 err:
  onig_node_free(left);
  onig_node_free(right);
  onig_node_free(target1);
  onig_node_free(target2);
  return ONIGERR_MEMORY;
}