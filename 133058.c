ospf_packet_add (struct ospf_interface *oi, struct ospf_packet *op)
{
  if (!oi->obuf)
    {
      zlog_err("ospf_packet_add(interface %s in state %d [%s], packet type %s, "
	       "destination %s) called with NULL obuf, ignoring "
	       "(please report this bug)!\n",
	       IF_NAME(oi), oi->state, LOOKUP (ospf_ism_state_msg, oi->state),
	       LOOKUP (ospf_packet_type_str, stream_getc_from(op->s, 1)),
	       inet_ntoa (op->dst));
      return;
    }

  /* Add packet to end of queue. */
  ospf_fifo_push (oi->obuf, op);

  /* Debug of packet fifo*/
  /* ospf_fifo_debug (oi->obuf); */
}