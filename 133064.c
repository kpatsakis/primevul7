ospf_ls_upd_send_lsa (struct ospf_neighbor *nbr, struct ospf_lsa *lsa,
		      int flag)
{
  struct list *update;

  update = list_new ();

  listnode_add (update, lsa);
  ospf_ls_upd_send (nbr, update, flag);

  list_delete (update);
}