ospf_ls_upd_queue_send (struct ospf_interface *oi, struct list *update,
			struct in_addr addr)
{
  struct ospf_packet *op;
  u_int16_t length = OSPF_HEADER_SIZE;

  if (IS_DEBUG_OSPF_EVENT)
    zlog_debug ("listcount = %d, dst %s", listcount (update), inet_ntoa(addr));
  
  op = ospf_ls_upd_packet_new (update, oi);

  /* Prepare OSPF common header. */
  ospf_make_header (OSPF_MSG_LS_UPD, oi, op->s);

  /* Prepare OSPF Link State Update body.
   * Includes Type-7 translation. 
   */
  length += ospf_make_ls_upd (oi, update, op->s);

  /* Fill OSPF header. */
  ospf_fill_header (oi, op->s, length);

  /* Set packet length. */
  op->length = length;

  /* Decide destination address. */
  if (oi->type == OSPF_IFTYPE_POINTOPOINT) 
    op->dst.s_addr = htonl (OSPF_ALLSPFROUTERS);
  else
    op->dst.s_addr = addr.s_addr;

  /* Add packet to the interface output queue. */
  ospf_packet_add (oi, op);

  /* Hook thread to write packet. */
  OSPF_ISM_WRITE_ON (oi->ospf);
}