ospf_hello_reply_timer (struct thread *thread)
{
  struct ospf_neighbor *nbr;

  nbr = THREAD_ARG (thread);
  nbr->t_hello_reply = NULL;

  assert (nbr->oi);

  if (IS_DEBUG_OSPF (nsm, NSM_TIMERS))
    zlog (NULL, LOG_DEBUG, "NSM[%s:%s]: Timer (hello-reply timer expire)",
	  IF_NAME (nbr->oi), inet_ntoa (nbr->router_id));

  ospf_hello_send_sub (nbr->oi, nbr->address.u.prefix4.s_addr);

  return 0;
}