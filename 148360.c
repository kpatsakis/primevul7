elg_sign (int algo, gcry_mpi_t *resarr, gcry_mpi_t data, gcry_mpi_t *skey)
{
  gcry_err_code_t err = GPG_ERR_NO_ERROR;
  ELG_secret_key sk;

  (void)algo;

  if ((! data)
      || (! skey[0]) || (! skey[1]) || (! skey[2]) || (! skey[3]))
    err = GPG_ERR_BAD_MPI;
  else
    {
      sk.p = skey[0];
      sk.g = skey[1];
      sk.y = skey[2];
      sk.x = skey[3];
      resarr[0] = mpi_alloc (mpi_get_nlimbs (sk.p));
      resarr[1] = mpi_alloc (mpi_get_nlimbs (sk.p));
      sign (resarr[0], resarr[1], data, &sk);
    }

  return err;
}