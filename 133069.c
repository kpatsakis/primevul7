ospf_make_ls_ack (struct ospf_interface *oi, struct list *ack, struct stream *s)
{
  struct listnode *node, *nnode;
  u_int16_t length = OSPF_LS_ACK_MIN_SIZE;
  unsigned long delta = stream_get_endp(s) + 24;
  struct ospf_lsa *lsa;

  for (ALL_LIST_ELEMENTS (ack, node, nnode, lsa))
    {
      assert (lsa);
      
      if (length + delta > ospf_packet_max (oi))
	break;
      
      stream_put (s, lsa->data, OSPF_LSA_HEADER_SIZE);
      length += OSPF_LSA_HEADER_SIZE;
      
      listnode_delete (ack, lsa);
      ospf_lsa_unlock (&lsa); /* oi->ls_ack_direct.ls_ack */
    }
  
  return length;
}