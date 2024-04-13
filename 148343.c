elg_generate_ext (int algo, unsigned int nbits, unsigned long evalue,
                  const gcry_sexp_t genparms,
                  gcry_mpi_t *skey, gcry_mpi_t **retfactors,
                  gcry_sexp_t *r_extrainfo)
{
  gpg_err_code_t ec;
  ELG_secret_key sk;
  gcry_mpi_t xvalue = NULL;
  gcry_sexp_t l1;

  (void)algo;
  (void)evalue;
  (void)r_extrainfo;

  if (genparms)
    {
      /* Parse the optional xvalue element. */
      l1 = gcry_sexp_find_token (genparms, "xvalue", 0);
      if (l1)
        {
          xvalue = gcry_sexp_nth_mpi (l1, 1, 0);
          gcry_sexp_release (l1);
          if (!xvalue)
            return GPG_ERR_BAD_MPI;
        }
    }

  if (xvalue)
    ec = generate_using_x (&sk, nbits, xvalue, retfactors);
  else
    {
      generate (&sk, nbits, retfactors);
      ec = 0;
    }

  skey[0] = sk.p;
  skey[1] = sk.g;
  skey[2] = sk.y;
  skey[3] = sk.x;

  return ec;
}