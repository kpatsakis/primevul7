gen_x931_parm_xi (void)
{
  gcry_mpi_t xi;

  xi = mpi_snew (101);
  _gcry_mpi_randomize (xi, 101, GCRY_VERY_STRONG_RANDOM);
  mpi_set_highbit (xi, 100);
  gcry_assert ( mpi_get_nbits (xi) == 101 );

  return xi;
}