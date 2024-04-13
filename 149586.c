bitset_invert_to(BitSetRef from, BitSetRef to)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) { to[i] = ~(from[i]); }
}