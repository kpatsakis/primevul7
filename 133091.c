ospf_packet_new (size_t size)
{
  struct ospf_packet *new;

  new = XCALLOC (MTYPE_OSPF_PACKET, sizeof (struct ospf_packet));
  new->s = stream_new (size);

  return new;
}