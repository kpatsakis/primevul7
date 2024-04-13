ospf_ls_upd_send_queue_event (struct thread *thread)
{
  struct ospf_interface *oi = THREAD_ARG(thread);
  struct route_node *rn;
  struct route_node *rnext;
  struct list *update;
  char again = 0;
  
  oi->t_ls_upd_event = NULL;

  if (IS_DEBUG_OSPF_EVENT)
    zlog_debug ("ospf_ls_upd_send_queue start");

  for (rn = route_top (oi->ls_upd_queue); rn; rn = rnext)
    {
      rnext = route_next (rn);
      
      if (rn->info == NULL)
        continue;
      
      update = (struct list *)rn->info;

      ospf_ls_upd_queue_send (oi, update, rn->p.u.prefix4);
      
      /* list might not be empty. */
      if (listcount(update) == 0)
        {
          list_delete (rn->info);
          rn->info = NULL;
          route_unlock_node (rn);
        }
      else
        again = 1;
    }

  if (again != 0)
    {
      if (IS_DEBUG_OSPF_EVENT)
        zlog_debug ("ospf_ls_upd_send_queue: update lists not cleared,"
                   " %d nodes to try again, raising new event", again);
      oi->t_ls_upd_event = 
        thread_add_event (master, ospf_ls_upd_send_queue_event, oi, 0);
    }

  if (IS_DEBUG_OSPF_EVENT)
    zlog_debug ("ospf_ls_upd_send_queue stop");
  
  return 0;
}