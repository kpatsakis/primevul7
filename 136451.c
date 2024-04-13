aspath_left_confed_check (struct aspath *aspath)
{

  if ( !(aspath && aspath->segments) )
    return 0;

  if ( (aspath->segments->type == AS_CONFED_SEQUENCE)
      || (aspath->segments->type == AS_CONFED_SET) )
    return 1;

  return 0;
}