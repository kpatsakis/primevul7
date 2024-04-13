ospf_router_lsa_links_examin
(
  struct router_lsa_link * link,
  u_int16_t linkbytes,
  const u_int16_t num_links
)
{
  unsigned counted_links = 0, thislinklen;

  while (linkbytes)
  {
    thislinklen = OSPF_ROUTER_LSA_LINK_SIZE + 4 * link->m[0].tos_count;
    if (thislinklen > linkbytes)
    {
      if (IS_DEBUG_OSPF_PACKET (0, RECV))
        zlog_debug ("%s: length error in link block #%u", __func__, counted_links);
      return MSG_NG;
    }
    link = (struct router_lsa_link *)((caddr_t) link + thislinklen);
    linkbytes -= thislinklen;
    counted_links++;
  }
  if (counted_links != num_links)
  {
    if (IS_DEBUG_OSPF_PACKET (0, RECV))
      zlog_debug ("%s: %u link blocks declared, %u present",
                  __func__, num_links, counted_links);
    return MSG_NG;
  }
  return MSG_OK;
}