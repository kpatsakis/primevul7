router_append_dirobj_signature(char *buf, size_t buf_len, const char *digest,
                               size_t digest_len, crypto_pk_t *private_key)
{
  size_t sig_len, s_len;
  char *sig = router_get_dirobj_signature(digest, digest_len, private_key);
  if (!sig) {
    log_warn(LD_BUG, "No signature generated");
    return -1;
  }
  sig_len = strlen(sig);
  s_len = strlen(buf);
  if (sig_len + s_len + 1 > buf_len) {
    log_warn(LD_BUG, "Not enough room for signature");
    tor_free(sig);
    return -1;
  }
  memcpy(buf+s_len, sig, sig_len+1);
  return 0;
}