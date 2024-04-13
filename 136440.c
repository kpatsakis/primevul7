assegment_new (u_char type, u_short length)
{
  struct assegment *new;
  
  new = XCALLOC (MTYPE_AS_SEG, sizeof (struct assegment));
  
  if (length)
    new->as = assegment_data_new (length);
  
  new->length = length;
  new->type = type;
  
  return new;
}