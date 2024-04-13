aspath_leftmost (struct aspath *aspath)
{
  struct assegment *seg = aspath->segments;
  as_t leftmost = 0;

  if (seg && seg->length && seg->type == AS_SEQUENCE)
    leftmost = seg->as[0];

  return leftmost;
}