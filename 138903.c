router_get_runningrouters_hash(const char *s, char *digest)
{
  return router_get_hash_impl(s, strlen(s), digest,
                              "network-status","\ndirectory-signature", '\n',
                              DIGEST_SHA1);
}