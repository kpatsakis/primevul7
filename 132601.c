rsa_check_secret_key (gcry_sexp_t keyparms)
{
  gcry_err_code_t rc;
  RSA_secret_key sk = {NULL, NULL, NULL, NULL, NULL, NULL};

  /* To check the key we need the optional parameters. */
  rc = sexp_extract_param (keyparms, NULL, "nedpqu",
                           &sk.n, &sk.e, &sk.d, &sk.p, &sk.q, &sk.u,
                           NULL);
  if (rc)
    goto leave;

  if (!check_secret_key (&sk))
    rc = GPG_ERR_BAD_SECKEY;

 leave:
  _gcry_mpi_release (sk.n);
  _gcry_mpi_release (sk.e);
  _gcry_mpi_release (sk.d);
  _gcry_mpi_release (sk.p);
  _gcry_mpi_release (sk.q);
  _gcry_mpi_release (sk.u);
  if (DBG_CIPHER)
    log_debug ("rsa_testkey    => %s\n", gpg_strerror (rc));
  return rc;
}