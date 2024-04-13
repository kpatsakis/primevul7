rsa_generate (const gcry_sexp_t genparms, gcry_sexp_t *r_skey)
{
  gpg_err_code_t ec;
  unsigned int nbits;
  unsigned long evalue;
  RSA_secret_key sk;
  gcry_sexp_t deriveparms;
  int flags = 0;
  gcry_sexp_t l1;
  gcry_sexp_t swap_info = NULL;

  memset (&sk, 0, sizeof sk);

  ec = _gcry_pk_util_get_nbits (genparms, &nbits);
  if (ec)
    return ec;

  ec = _gcry_pk_util_get_rsa_use_e (genparms, &evalue);
  if (ec)
    return ec;

  /* Parse the optional flags list.  */
  l1 = sexp_find_token (genparms, "flags", 0);
  if (l1)
    {
      ec = _gcry_pk_util_parse_flaglist (l1, &flags, NULL);
      sexp_release (l1);
      if (ec)
        return ec;
    }

  deriveparms = (genparms?
                 sexp_find_token (genparms, "derive-parms", 0) : NULL);
  if (!deriveparms)
    {
      /* Parse the optional "use-x931" flag. */
      l1 = sexp_find_token (genparms, "use-x931", 0);
      if (l1)
        {
          flags |= PUBKEY_FLAG_USE_X931;
          sexp_release (l1);
        }
    }

  if (deriveparms || (flags & PUBKEY_FLAG_USE_X931))
    {
      int swapped;
      ec = generate_x931 (&sk, nbits, evalue, deriveparms, &swapped);
      sexp_release (deriveparms);
      if (!ec && swapped)
        ec = sexp_new (&swap_info, "(misc-key-info(p-q-swapped))", 0, 1);
    }
  else
    {
      /* Parse the optional "transient-key" flag. */
      if (!(flags & PUBKEY_FLAG_TRANSIENT_KEY))
        {
          l1 = sexp_find_token (genparms, "transient-key", 0);
          if (l1)
            {
              flags |= PUBKEY_FLAG_TRANSIENT_KEY;
              sexp_release (l1);
            }
        }
      deriveparms = (genparms? sexp_find_token (genparms, "test-parms", 0)
                     /**/    : NULL);

      /* Generate.  */
      if (deriveparms || fips_mode())
        {
          ec = generate_fips (&sk, nbits, evalue, deriveparms,
                              !!(flags & PUBKEY_FLAG_TRANSIENT_KEY));
        }
      else
        {
          ec = generate_std (&sk, nbits, evalue,
                             !!(flags & PUBKEY_FLAG_TRANSIENT_KEY));
        }
      sexp_release (deriveparms);
    }

  if (!ec)
    {
      ec = sexp_build (r_skey, NULL,
                       "(key-data"
                       " (public-key"
                       "  (rsa(n%m)(e%m)))"
                       " (private-key"
                       "  (rsa(n%m)(e%m)(d%m)(p%m)(q%m)(u%m)))"
                       " %S)",
                       sk.n, sk.e,
                       sk.n, sk.e, sk.d, sk.p, sk.q, sk.u,
                       swap_info);
    }

  mpi_free (sk.n);
  mpi_free (sk.e);
  mpi_free (sk.p);
  mpi_free (sk.q);
  mpi_free (sk.d);
  mpi_free (sk.u);
  sexp_release (swap_info);

  return ec;
}