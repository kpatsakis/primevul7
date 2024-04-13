ospf_packet_max (struct ospf_interface *oi)
{
  int max;

  max = oi->ifp->mtu - ospf_packet_authspace(oi);

  max -= (OSPF_HEADER_SIZE + sizeof (struct ip));

  return max;
}