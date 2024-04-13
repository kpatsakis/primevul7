microdescs_parse_from_string(const char *s, const char *eos,
                             int allow_annotations,
                             saved_location_t where)
{
  smartlist_t *tokens;
  smartlist_t *result;
  microdesc_t *md = NULL;
  memarea_t *area;
  const char *start = s;
  const char *start_of_next_microdesc;
  int flags = allow_annotations ? TS_ANNOTATIONS_OK : 0;
  const int copy_body = (where != SAVED_IN_CACHE);

  directory_token_t *tok;

  if (!eos)
    eos = s + strlen(s);

  s = eat_whitespace_eos(s, eos);
  area = memarea_new();
  result = smartlist_new();
  tokens = smartlist_new();

  while (s < eos) {
    start_of_next_microdesc = find_start_of_next_microdesc(s, eos);
    if (!start_of_next_microdesc)
      start_of_next_microdesc = eos;

    if (tokenize_string(area, s, start_of_next_microdesc, tokens,
                        microdesc_token_table, flags)) {
      log_warn(LD_DIR, "Unparseable microdescriptor");
      goto next;
    }

    md = tor_malloc_zero(sizeof(microdesc_t));
    {
      const char *cp = tor_memstr(s, start_of_next_microdesc-s,
                                  "onion-key");
      tor_assert(cp);

      md->bodylen = start_of_next_microdesc - cp;
      md->saved_location = where;
      if (copy_body)
        md->body = tor_memdup_nulterm(cp, md->bodylen);
      else
        md->body = (char*)cp;
      md->off = cp - start;
    }

    if ((tok = find_opt_by_keyword(tokens, A_LAST_LISTED))) {
      if (parse_iso_time(tok->args[0], &md->last_listed)) {
        log_warn(LD_DIR, "Bad last-listed time in microdescriptor");
        goto next;
      }
    }

    tok = find_by_keyword(tokens, K_ONION_KEY);
    if (!crypto_pk_public_exponent_ok(tok->key)) {
      log_warn(LD_DIR,
               "Relay's onion key had invalid exponent.");
      goto next;
    }
    md->onion_pkey = tok->key;
    tok->key = NULL;

    if ((tok = find_opt_by_keyword(tokens, K_ONION_KEY_NTOR))) {
      curve25519_public_key_t k;
      tor_assert(tok->n_args >= 1);
      if (curve25519_public_from_base64(&k, tok->args[0]) < 0) {
        log_warn(LD_DIR, "Bogus ntor-onion-key in microdesc");
        goto next;
      }
      md->onion_curve25519_pkey =
        tor_memdup(&k, sizeof(curve25519_public_key_t));
    }

    {
      smartlist_t *a_lines = find_all_by_keyword(tokens, K_A);
      if (a_lines) {
        find_single_ipv6_orport(a_lines, &md->ipv6_addr, &md->ipv6_orport);
        smartlist_free(a_lines);
      }
    }

    if ((tok = find_opt_by_keyword(tokens, K_FAMILY))) {
      int i;
      md->family = smartlist_new();
      for (i=0;i<tok->n_args;++i) {
        if (!is_legal_nickname_or_hexdigest(tok->args[i])) {
          log_warn(LD_DIR, "Illegal nickname %s in family line",
                   escaped(tok->args[i]));
          goto next;
        }
        smartlist_add(md->family, tor_strdup(tok->args[i]));
      }
    }

    if ((tok = find_opt_by_keyword(tokens, K_P))) {
      md->exit_policy = parse_short_policy(tok->args[0]);
    }
    if ((tok = find_opt_by_keyword(tokens, K_P6))) {
      md->ipv6_exit_policy = parse_short_policy(tok->args[0]);
    }

    crypto_digest256(md->digest, md->body, md->bodylen, DIGEST_SHA256);

    smartlist_add(result, md);

    md = NULL;
  next:
    microdesc_free(md);
    md = NULL;

    SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
    memarea_clear(area);
    smartlist_clear(tokens);
    s = start_of_next_microdesc;
  }

  SMARTLIST_FOREACH(tokens, directory_token_t *, t, token_clear(t));
  memarea_drop_all(area);
  smartlist_free(tokens);

  return result;
}