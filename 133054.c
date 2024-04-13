ospf_upd_list_clean (struct list *lsas)
{
  struct listnode *node, *nnode;
  struct ospf_lsa *lsa;

  for (ALL_LIST_ELEMENTS (lsas, node, nnode, lsa))
    ospf_lsa_discard (lsa);

  list_delete (lsas);
}