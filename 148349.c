elg_encrypt (int algo, gcry_mpi_t *resarr,
             gcry_mpi_t data, gcry_mpi_t *pkey, int flags)
{
  gcry_err_code_t err = GPG_ERR_NO_ERROR;
  ELG_public_key pk;

  (void)algo;
  (void)flags;

  if ((! data) || (! pkey[0]) || (! pkey[1]) || (! pkey[2]))
    err = GPG_ERR_BAD_MPI;
  else
    {
      pk.p = pkey[0];
      pk.g = pkey[1];
      pk.y = pkey[2];
      resarr[0] = mpi_alloc (mpi_get_nlimbs (pk.p));
      resarr[1] = mpi_alloc (mpi_get_nlimbs (pk.p));
      do_encrypt (resarr[0], resarr[1], data, &pk);
    }
  return err;
}