elg_decrypt (int algo, gcry_mpi_t *result,
             gcry_mpi_t *data, gcry_mpi_t *skey, int flags)
{
  gcry_err_code_t err = GPG_ERR_NO_ERROR;
  ELG_secret_key sk;

  (void)algo;
  (void)flags;

  if ((! data[0]) || (! data[1])
      || (! skey[0]) || (! skey[1]) || (! skey[2]) || (! skey[3]))
    err = GPG_ERR_BAD_MPI;
  else
    {
      sk.p = skey[0];
      sk.g = skey[1];
      sk.y = skey[2];
      sk.x = skey[3];
      *result = mpi_alloc_secure (mpi_get_nlimbs (sk.p));
      decrypt (*result, data[0], data[1], &sk);
    }
  return err;
}