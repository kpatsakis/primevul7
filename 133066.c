ospf_poll_timer (struct thread *thread)
{
  struct ospf_nbr_nbma *nbr_nbma;

  nbr_nbma = THREAD_ARG (thread);
  nbr_nbma->t_poll = NULL;

  if (IS_DEBUG_OSPF (nsm, NSM_TIMERS))
    zlog (NULL, LOG_DEBUG, "NSM[%s:%s]: Timer (Poll timer expire)",
    IF_NAME (nbr_nbma->oi), inet_ntoa (nbr_nbma->addr));

  ospf_poll_send (nbr_nbma);

  if (nbr_nbma->v_poll > 0)
    OSPF_POLL_TIMER_ON (nbr_nbma->t_poll, ospf_poll_timer,
			nbr_nbma->v_poll);

  return 0;
}