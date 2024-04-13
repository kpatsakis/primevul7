selftests_rsa (selftest_report_func_t report)
{
  const char *what;
  const char *errtxt;
  gcry_error_t err;
  gcry_sexp_t skey = NULL;
  gcry_sexp_t pkey = NULL;

  /* Convert the S-expressions into the internal representation.  */
  what = "convert";
  err = sexp_sscan (&skey, NULL, sample_secret_key, strlen (sample_secret_key));
  if (!err)
    err = sexp_sscan (&pkey, NULL,
                      sample_public_key, strlen (sample_public_key));
  if (err)
    {
      errtxt = _gcry_strerror (err);
      goto failed;
    }

  what = "key consistency";
  err = _gcry_pk_testkey (skey);
  if (err)
    {
      errtxt = _gcry_strerror (err);
      goto failed;
    }

  what = "sign";
  errtxt = selftest_sign_2048 (pkey, skey);
  if (errtxt)
    goto failed;

  what = "encrypt";
  errtxt = selftest_encr_2048 (pkey, skey);
  if (errtxt)
    goto failed;

  sexp_release (pkey);
  sexp_release (skey);
  return 0; /* Succeeded. */

 failed:
  sexp_release (pkey);
  sexp_release (skey);
  if (report)
    report ("pubkey", GCRY_PK_RSA, what, errtxt);
  return GPG_ERR_SELFTEST_FAILED;
}