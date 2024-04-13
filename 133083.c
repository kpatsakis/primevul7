ospf_fifo_new ()
{
  struct ospf_fifo *new;

  new = XCALLOC (MTYPE_OSPF_FIFO, sizeof (struct ospf_fifo));
  return new;
}