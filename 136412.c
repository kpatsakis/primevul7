assegment_data_put (struct stream *s, as_t *as, int num, int use32bit)
{
  int i;
  assert (num <= AS_SEGMENT_MAX);
  
  for (i = 0; i < num; i++)
    if ( use32bit )
      stream_putl (s, as[i]);
    else
      {
        if ( as[i] <= BGP_AS_MAX )
	  stream_putw(s, as[i]);
	else
	  stream_putw(s, BGP_AS_TRANS);
      }
}