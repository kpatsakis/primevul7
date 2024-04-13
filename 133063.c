ospf_ls_ack_send_event (struct thread *thread)
{
  struct ospf_interface *oi = THREAD_ARG (thread);

  oi->t_ls_ack_direct = NULL;
  
  while (listcount (oi->ls_ack_direct.ls_ack))
    ospf_ls_ack_send_list (oi, oi->ls_ack_direct.ls_ack,
			   oi->ls_ack_direct.dst);

  return 0;
}