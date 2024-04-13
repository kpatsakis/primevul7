rsa_verify (gcry_sexp_t s_sig, gcry_sexp_t s_data, gcry_sexp_t keyparms)
{
  gcry_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_sexp_t l1 = NULL;
  gcry_mpi_t sig = NULL;
  gcry_mpi_t data = NULL;
  RSA_public_key pk = { NULL, NULL };
  gcry_mpi_t result = NULL;

  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_VERIFY,
                                   rsa_get_nbits (keyparms));

  /* Extract the data.  */
  rc = _gcry_pk_util_data_to_mpi (s_data, &data, &ctx);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_printmpi ("rsa_verify data", data);
  if (mpi_is_opaque (data))
    {
      rc = GPG_ERR_INV_DATA;
      goto leave;
    }

  /* Extract the signature value.  */
  rc = _gcry_pk_util_preparse_sigval (s_sig, rsa_names, &l1, NULL);
  if (rc)
    goto leave;
  rc = sexp_extract_param (l1, NULL, "s", &sig, NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_printmpi ("rsa_verify  sig", sig);

  /* Extract the key.  */
  rc = sexp_extract_param (keyparms, NULL, "ne", &pk.n, &pk.e, NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_printmpi ("rsa_verify    n", pk.n);
      log_printmpi ("rsa_verify    e", pk.e);
    }

  /* Do RSA computation and compare.  */
  result = mpi_new (0);
  public (result, sig, &pk);
  if (DBG_CIPHER)
    log_printmpi ("rsa_verify  cmp", result);
  if (ctx.verify_cmp)
    rc = ctx.verify_cmp (&ctx, result);
  else
    rc = mpi_cmp (result, data) ? GPG_ERR_BAD_SIGNATURE : 0;

 leave:
  _gcry_mpi_release (result);
  _gcry_mpi_release (pk.n);
  _gcry_mpi_release (pk.e);
  _gcry_mpi_release (data);
  _gcry_mpi_release (sig);
  sexp_release (l1);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("rsa_verify    => %s\n", rc?gpg_strerror (rc):"Good");
  return rc;
}