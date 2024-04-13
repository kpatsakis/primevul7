router_get_networkstatus_v2_hash(const char *s, char *digest)
{
  return router_get_hash_impl(s, strlen(s), digest,
                              "network-status-version","\ndirectory-signature",
                              '\n',
                              DIGEST_SHA1);
}