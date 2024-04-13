router_get_dir_hash(const char *s, char *digest)
{
  return router_get_hash_impl(s, strlen(s), digest,
                              "signed-directory","\ndirectory-signature",'\n',
                              DIGEST_SHA1);
}