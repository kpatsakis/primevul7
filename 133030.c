ospf_ls_ack_send (struct ospf_neighbor *nbr, struct ospf_lsa *lsa)
{
  struct ospf_interface *oi = nbr->oi;

  if (listcount (oi->ls_ack_direct.ls_ack) == 0)
    oi->ls_ack_direct.dst = nbr->address.u.prefix4;
  
  listnode_add (oi->ls_ack_direct.ls_ack, ospf_lsa_lock (lsa));
  
  if (oi->t_ls_ack_direct == NULL)
    oi->t_ls_ack_direct =
      thread_add_event (master, ospf_ls_ack_send_event, oi, 0);
}