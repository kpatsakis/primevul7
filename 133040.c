ospf_poll_send (struct ospf_nbr_nbma *nbr_nbma)
{
  struct ospf_interface *oi;

  oi = nbr_nbma->oi;
  assert(oi);

  /* If this is passive interface, do not send OSPF Hello. */
  if (OSPF_IF_PASSIVE_STATUS (oi) == OSPF_IF_PASSIVE)
    return;

  if (oi->type != OSPF_IFTYPE_NBMA)
    return;

  if (nbr_nbma->nbr != NULL && nbr_nbma->nbr->state != NSM_Down)
    return;

  if (PRIORITY(oi) == 0)
    return;

  if (nbr_nbma->priority == 0
      && oi->state != ISM_DR && oi->state != ISM_Backup)
    return;

  ospf_hello_send_sub (oi, nbr_nbma->addr.s_addr);
}