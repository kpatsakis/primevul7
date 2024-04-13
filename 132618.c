selftest_encr_2048 (gcry_sexp_t pkey, gcry_sexp_t skey)
{
  const char *errtxt = NULL;
  gcry_error_t err;
  static const char plaintext[] =
    "Jim quickly realized that the beautiful gowns are expensive.";
  gcry_sexp_t plain = NULL;
  gcry_sexp_t encr  = NULL;
  gcry_mpi_t  ciphertext = NULL;
  gcry_sexp_t decr  = NULL;
  char *decr_plaintext = NULL;
  gcry_sexp_t tmplist = NULL;
  /* expected result of encrypting the plaintext with sample_secret_key */
  static const char ref_data[] =
    "18022e2593a402a737caaa93b4c7e750e20ca265452980e1d6b7710fbd3e"
    "7dce72be5c2110fb47691cb38f42170ee3b4a37f2498d4a51567d762585e"
    "4cb81d04fbc7df4144f8e5eac2d4b8688521b64011f11d7ad53f4c874004"
    "819856f2e2a6f83d1c9c4e73ac26089789c14482b0b8d44139133c88c4a5"
    "2dba9dd6d6ffc622666b7d129168333d999706af30a2d7d272db7734e5ed"
    "fb8c64ea3018af3ad20f4a013a5060cb0f5e72753967bebe294280a6ed0d"
    "dbd3c4f11d0a8696e9d32a0dc03deb0b5e49b2cbd1503392642d4e1211f3"
    "e8e2ee38abaa3671ccd57fcde8ca76e85fd2cb77c35706a970a213a27352"
    "cec92a9604d543ddb5fc478ff50e0622";
  gcry_mpi_t ref_mpi = NULL;

  /* Put the plaintext into an S-expression.  */
  err = sexp_build (&plain, NULL, "(data (flags raw) (value %s))", plaintext);
  if (err)
    {
      errtxt = "converting data failed";
      goto leave;
    }

  /* Encrypt.  */
  err = _gcry_pk_encrypt (&encr, plain, pkey);
  if (err)
    {
      errtxt = "encrypt failed";
      goto leave;
    }

  err = _gcry_mpi_scan(&ref_mpi, GCRYMPI_FMT_HEX, ref_data, 0, NULL);
  if (err)
    {
      errtxt = "converting encrydata to mpi failed";
      goto leave;
    }

  /* Extraxt the ciphertext from the returned S-expression.  */
  /*sexp_dump (encr);*/
  ciphertext = extract_a_from_sexp (encr);
  if (!ciphertext)
    {
      errtxt = "gcry_pk_decrypt returned garbage";
      goto leave;
    }

  /* Check that the ciphertext does no match the plaintext.  */
  /* _gcry_log_printmpi ("plaintext", plaintext); */
  /* _gcry_log_printmpi ("ciphertxt", ciphertext); */
  if (mpi_cmp (ref_mpi, ciphertext))
    {
      errtxt = "ciphertext doesn't match reference data";
      goto leave;
    }

  /* Decrypt.  */
  err = _gcry_pk_decrypt (&decr, encr, skey);
  if (err)
    {
      errtxt = "decrypt failed";
      goto leave;
    }

  /* Extract the decrypted data from the S-expression.  Note that the
     output of gcry_pk_decrypt depends on whether a flags lists occurs
     in its input data.  Because we passed the output of
     gcry_pk_encrypt directly to gcry_pk_decrypt, such a flag value
     won't be there as of today.  To be prepared for future changes we
     take care of it anyway.  */
  tmplist = sexp_find_token (decr, "value", 0);
  if (tmplist)
    decr_plaintext = sexp_nth_string (tmplist, 1);
  else
    decr_plaintext = sexp_nth_string (decr, 0);
  if (!decr_plaintext)
    {
      errtxt = "decrypt returned no plaintext";
      goto leave;
    }

  /* Check that the decrypted plaintext matches the original  plaintext.  */
  if (strcmp (plaintext, decr_plaintext))
    {
      errtxt = "mismatch";
      goto leave;
    }

 leave:
  sexp_release (tmplist);
  xfree (decr_plaintext);
  sexp_release (decr);
  _gcry_mpi_release (ciphertext);
  _gcry_mpi_release (ref_mpi);
  sexp_release (encr);
  sexp_release (plain);
  return errtxt;
}