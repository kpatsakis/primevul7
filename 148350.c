elg_check_secret_key (int algo, gcry_mpi_t *skey)
{
  gcry_err_code_t err = GPG_ERR_NO_ERROR;
  ELG_secret_key sk;

  (void)algo;

  if ((! skey[0]) || (! skey[1]) || (! skey[2]) || (! skey[3]))
    err = GPG_ERR_BAD_MPI;
  else
    {
      sk.p = skey[0];
      sk.g = skey[1];
      sk.y = skey[2];
      sk.x = skey[3];

      if (! check_secret_key (&sk))
	err = GPG_ERR_BAD_SECKEY;
    }

  return err;
}