router_get_networkstatus_v3_hashes(const char *s, digests_t *digests)
{
  return router_get_hashes_impl(s,strlen(s),digests,
                                "network-status-version",
                                "\ndirectory-signature",
                                ' ');
}