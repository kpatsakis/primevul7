router_get_extrainfo_hash(const char *s, size_t s_len, char *digest)
{
  return router_get_hash_impl(s, s_len, digest, "extra-info",
                              "\nrouter-signature",'\n', DIGEST_SHA1);
}