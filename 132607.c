rsa_get_nbits (gcry_sexp_t parms)
{
  gcry_sexp_t l1;
  gcry_mpi_t n;
  unsigned int nbits;

  l1 = sexp_find_token (parms, "n", 1);
  if (!l1)
    return 0; /* Parameter N not found.  */

  n = sexp_nth_mpi (l1, 1, GCRYMPI_FMT_USG);
  sexp_release (l1);
  nbits = n? mpi_get_nbits (n) : 0;
  _gcry_mpi_release (n);
  return nbits;
}