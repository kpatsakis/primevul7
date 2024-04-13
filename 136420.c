aspath_empty (void)
{
  return aspath_parse (NULL, 0, 1); /* 32Bit ;-) */
}