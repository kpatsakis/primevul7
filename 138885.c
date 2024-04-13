router_get_dirobj_signature(const char *digest,
                            size_t digest_len,
                            crypto_pk_t *private_key)
{
  char *signature;
  size_t i, keysize;
  int siglen;
  char *buf = NULL;
  size_t buf_len;
  /* overestimate of BEGIN/END lines total len. */
#define BEGIN_END_OVERHEAD_LEN 64

  keysize = crypto_pk_keysize(private_key);
  signature = tor_malloc(keysize);
  siglen = crypto_pk_private_sign(private_key, signature, keysize,
                                  digest, digest_len);
  if (siglen < 0) {
    log_warn(LD_BUG,"Couldn't sign digest.");
    goto err;
  }

  /* The *2 here is a ridiculous overestimate of base-64 overhead. */
  buf_len = (siglen * 2) + BEGIN_END_OVERHEAD_LEN;
  buf = tor_malloc(buf_len);

  if (strlcpy(buf, "-----BEGIN SIGNATURE-----\n", buf_len) >= buf_len)
    goto truncated;

  i = strlen(buf);
  if (base64_encode(buf+i, buf_len-i, signature, siglen) < 0) {
    log_warn(LD_BUG,"couldn't base64-encode signature");
    goto err;
  }

  if (strlcat(buf, "-----END SIGNATURE-----\n", buf_len) >= buf_len)
    goto truncated;

  tor_free(signature);
  return buf;

 truncated:
  log_warn(LD_BUG,"tried to exceed string length.");
 err:
  tor_free(signature);
  tor_free(buf);
  return NULL;
}