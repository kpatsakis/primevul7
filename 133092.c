ospf_ls_upd_timer (struct thread *thread)
{
  struct ospf_neighbor *nbr;

  nbr = THREAD_ARG (thread);
  nbr->t_ls_upd = NULL;

  /* Send Link State Update. */
  if (ospf_ls_retransmit_count (nbr) > 0)
    {
      struct list *update;
      struct ospf_lsdb *lsdb;
      int i;
      int retransmit_interval;

      retransmit_interval = OSPF_IF_PARAM (nbr->oi, retransmit_interval);

      lsdb = &nbr->ls_rxmt;
      update = list_new ();

      for (i = OSPF_MIN_LSA; i < OSPF_MAX_LSA; i++)
	{
	  struct route_table *table = lsdb->type[i].db;
	  struct route_node *rn;
	  
	  for (rn = route_top (table); rn; rn = route_next (rn))
	    {
	      struct ospf_lsa *lsa;
	      
	      if ((lsa = rn->info) != NULL)
		/* Don't retransmit an LSA if we received it within
		  the last RxmtInterval seconds - this is to allow the
		  neighbour a chance to acknowledge the LSA as it may
		  have ben just received before the retransmit timer
		  fired.  This is a small tweak to what is in the RFC,
		  but it will cut out out a lot of retransmit traffic
		  - MAG */
		if (tv_cmp (tv_sub (recent_relative_time (), lsa->tv_recv), 
			    int2tv (retransmit_interval)) >= 0)
		  listnode_add (update, rn->info);
	    }
	}

      if (listcount (update) > 0)
	ospf_ls_upd_send (nbr, update, OSPF_SEND_PACKET_DIRECT);
      list_delete (update);
    }

  /* Set LS Update retransmission timer. */
  OSPF_NSM_TIMER_ON (nbr->t_ls_upd, ospf_ls_upd_timer, nbr->v_ls_upd);

  return 0;
}