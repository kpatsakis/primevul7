rsa_encrypt (gcry_sexp_t *r_ciph, gcry_sexp_t s_data, gcry_sexp_t keyparms)
{
  gcry_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_mpi_t data = NULL;
  RSA_public_key pk = {NULL, NULL};
  gcry_mpi_t ciph = NULL;

  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_ENCRYPT,
                                   rsa_get_nbits (keyparms));

  /* Extract the data.  */
  rc = _gcry_pk_util_data_to_mpi (s_data, &data, &ctx);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_mpidump ("rsa_encrypt data", data);
  if (!data || mpi_is_opaque (data))
    {
      rc = GPG_ERR_INV_DATA;
      goto leave;
    }

  /* Extract the key.  */
  rc = sexp_extract_param (keyparms, NULL, "ne", &pk.n, &pk.e, NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_mpidump ("rsa_encrypt    n", pk.n);
      log_mpidump ("rsa_encrypt    e", pk.e);
    }

  /* Do RSA computation and build result.  */
  ciph = mpi_new (0);
  public (ciph, data, &pk);
  if (DBG_CIPHER)
    log_mpidump ("rsa_encrypt  res", ciph);
  if ((ctx.flags & PUBKEY_FLAG_FIXEDLEN))
    {
      /* We need to make sure to return the correct length to avoid
         problems with missing leading zeroes.  */
      unsigned char *em;
      size_t emlen = (mpi_get_nbits (pk.n)+7)/8;

      rc = _gcry_mpi_to_octet_string (&em, NULL, ciph, emlen);
      if (!rc)
        {
          rc = sexp_build (r_ciph, NULL, "(enc-val(rsa(a%b)))", (int)emlen, em);
          xfree (em);
        }
    }
  else
    rc = sexp_build (r_ciph, NULL, "(enc-val(rsa(a%m)))", ciph);

 leave:
  _gcry_mpi_release (ciph);
  _gcry_mpi_release (pk.n);
  _gcry_mpi_release (pk.e);
  _gcry_mpi_release (data);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("rsa_encrypt    => %s\n", gpg_strerror (rc));
  return rc;
}