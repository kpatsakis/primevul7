check_exponent (void *arg, gcry_mpi_t a)
{
  gcry_mpi_t e = arg;
  gcry_mpi_t tmp;
  int result;

  mpi_sub_ui (a, a, 1);
  tmp = _gcry_mpi_alloc_like (a);
  result = !mpi_gcd(tmp, e, a); /* GCD is not 1. */
  _gcry_mpi_release (tmp);
  mpi_add_ui (a, a, 1);
  return result;
}