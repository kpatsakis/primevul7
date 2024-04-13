assegment_data_new (int num)
{
  return (XMALLOC (MTYPE_AS_SEG_DATA, ASSEGMENT_DATA_SIZE (num, 1)));
}