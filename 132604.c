rsa_sign (gcry_sexp_t *r_sig, gcry_sexp_t s_data, gcry_sexp_t keyparms)
{
  gpg_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_mpi_t data = NULL;
  RSA_secret_key sk = {NULL, NULL, NULL, NULL, NULL, NULL};
  RSA_public_key pk;
  gcry_mpi_t sig = NULL;
  gcry_mpi_t result = NULL;

  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_SIGN,
                                   rsa_get_nbits (keyparms));

  /* Extract the data.  */
  rc = _gcry_pk_util_data_to_mpi (s_data, &data, &ctx);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_printmpi ("rsa_sign   data", data);
  if (mpi_is_opaque (data))
    {
      rc = GPG_ERR_INV_DATA;
      goto leave;
    }

  /* Extract the key.  */
  rc = sexp_extract_param (keyparms, NULL, "nedp?q?u?",
                           &sk.n, &sk.e, &sk.d, &sk.p, &sk.q, &sk.u,
                           NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_printmpi ("rsa_sign      n", sk.n);
      log_printmpi ("rsa_sign      e", sk.e);
      if (!fips_mode ())
        {
          log_printmpi ("rsa_sign      d", sk.d);
          log_printmpi ("rsa_sign      p", sk.p);
          log_printmpi ("rsa_sign      q", sk.q);
          log_printmpi ("rsa_sign      u", sk.u);
        }
    }

  /* Do RSA computation.  */
  sig = mpi_new (0);
  if ((ctx.flags & PUBKEY_FLAG_NO_BLINDING))
    secret (sig, data, &sk);
  else
    secret_blinded (sig, data, &sk, ctx.nbits);
  if (DBG_CIPHER)
    log_printmpi ("rsa_sign    res", sig);

  /* Check that the created signature is good.  This detects a failure
     of the CRT algorithm  (Lenstra's attack on RSA's use of the CRT).  */
  result = mpi_new (0);
  pk.n = sk.n;
  pk.e = sk.e;
  public (result, sig, &pk);
  if (mpi_cmp (result, data))
    {
      rc = GPG_ERR_BAD_SIGNATURE;
      goto leave;
    }

  /* Convert the result.  */
  if ((ctx.flags & PUBKEY_FLAG_FIXEDLEN))
    {
      /* We need to make sure to return the correct length to avoid
         problems with missing leading zeroes.  */
      unsigned char *em;
      size_t emlen = (mpi_get_nbits (sk.n)+7)/8;

      rc = _gcry_mpi_to_octet_string (&em, NULL, sig, emlen);
      if (!rc)
        {
          rc = sexp_build (r_sig, NULL, "(sig-val(rsa(s%b)))", (int)emlen, em);
          xfree (em);
        }
    }
  else
    rc = sexp_build (r_sig, NULL, "(sig-val(rsa(s%M)))", sig);


 leave:
  _gcry_mpi_release (result);
  _gcry_mpi_release (sig);
  _gcry_mpi_release (sk.n);
  _gcry_mpi_release (sk.e);
  _gcry_mpi_release (sk.d);
  _gcry_mpi_release (sk.p);
  _gcry_mpi_release (sk.q);
  _gcry_mpi_release (sk.u);
  _gcry_mpi_release (data);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("rsa_sign      => %s\n", gpg_strerror (rc));
  return rc;
}