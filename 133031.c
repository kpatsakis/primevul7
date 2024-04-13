ospf_ls_upd_packet_new (struct list *update, struct ospf_interface *oi)
{
  struct ospf_lsa *lsa;
  struct listnode *ln;
  size_t size;
  static char warned = 0;

  lsa = listgetdata((ln = listhead (update)));
  assert (lsa->data);

  if ((OSPF_LS_UPD_MIN_SIZE + ntohs (lsa->data->length))
      > ospf_packet_max (oi))
    {
      if (!warned)
        {
          zlog_warn ("ospf_ls_upd_packet_new: oversized LSA encountered!"
                     "will need to fragment. Not optimal. Try divide up"
                     " your network with areas. Use 'debug ospf packet send'"
                     " to see details, or look at 'show ip ospf database ..'");
          warned = 1;
        }

      if (IS_DEBUG_OSPF_PACKET (0, SEND))
        zlog_debug ("ospf_ls_upd_packet_new: oversized LSA id:%s,"
                   " %d bytes originated by %s, will be fragmented!",
                   inet_ntoa (lsa->data->id),
                   ntohs (lsa->data->length),
                   inet_ntoa (lsa->data->adv_router));

      /* 
       * Allocate just enough to fit this LSA only, to avoid including other
       * LSAs in fragmented LSA Updates.
       */
      size = ntohs (lsa->data->length) + (oi->ifp->mtu - ospf_packet_max (oi))
             + OSPF_LS_UPD_MIN_SIZE;
    }
  else
    size = oi->ifp->mtu;

  if (size > OSPF_MAX_PACKET_SIZE)
    {
      zlog_warn ("ospf_ls_upd_packet_new: oversized LSA id:%s too big,"
                 " %d bytes, packet size %ld, dropping it completely."
                 " OSPF routing is broken!",
                 inet_ntoa (lsa->data->id), ntohs (lsa->data->length),
                 (long int) size);
      list_delete_node (update, ln);
      return NULL;
    }

  /* IP header is built up separately by ospf_write(). This means, that we must
   * reduce the "affordable" size just calculated by length of an IP header.
   * This makes sure, that even if we manage to fill the payload with LSA data
   * completely, the final packet (our data plus IP header) still fits into
   * outgoing interface MTU. This correction isn't really meaningful for an
   * oversized LSA, but for consistency the correction is done for both cases.
   *
   * P.S. OSPF_MAX_PACKET_SIZE above already includes IP header size
   */
  return ospf_packet_new (size - sizeof (struct ip));
}