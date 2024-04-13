gen_x931_parm_xp (unsigned int nbits)
{
  gcry_mpi_t xp;

  xp = mpi_snew (nbits);
  _gcry_mpi_randomize (xp, nbits, GCRY_VERY_STRONG_RANDOM);

  /* The requirement for Xp is:

       sqrt{2}*2^{nbits-1} <= xp <= 2^{nbits} - 1

     We set the two high order bits to 1 to satisfy the lower bound.
     By using mpi_set_highbit we make sure that the upper bound is
     satisfied as well.  */
  mpi_set_highbit (xp, nbits-1);
  mpi_set_bit (xp, nbits-2);
  gcry_assert ( mpi_get_nbits (xp) == nbits );

  return xp;
}