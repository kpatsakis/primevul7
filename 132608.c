selftest_sign_2048 (gcry_sexp_t pkey, gcry_sexp_t skey)
{
  static const char sample_data[] =
    "(data (flags pkcs1)"
    " (hash sha256 #11223344556677889900aabbccddeeff"
    /**/           "102030405060708090a0b0c0d0f01121#))";
  static const char sample_data_bad[] =
    "(data (flags pkcs1)"
    " (hash sha256 #11223344556677889900aabbccddeeff"
    /**/           "802030405060708090a0b0c0d0f01121#))";

  const char *errtxt = NULL;
  gcry_error_t err;
  gcry_sexp_t data = NULL;
  gcry_sexp_t data_bad = NULL;
  gcry_sexp_t sig = NULL;
  /* raw signature data reference */
  const char ref_data[] =
    "6252a19a11e1d5155ed9376036277193d644fa239397fff03e9b92d6f86415d6"
    "d30da9273775f290e580d038295ff8ff89522becccfa6ae870bf76b76df402a8"
    "54f69347e3db3de8e1e7d4dada281ec556810c7a8ecd0b5f51f9b1c0e7aa7557"
    "61aa2b8ba5f811304acc6af0eca41fe49baf33bf34eddaf44e21e036ac7f0b68"
    "03cdef1c60021fb7b5b97ebacdd88ab755ce29af568dbc5728cc6e6eff42618d"
    "62a0386ca8beed46402bdeeef29b6a3feded906bace411a06a39192bf516ae10"
    "67e4320fa8ea113968525f4574d022a3ceeaafdc41079efe1f22cc94bf59d8d3"
    "328085da9674857db56de5978a62394aab48aa3b72e23a1b16260cfd9daafe65";
  gcry_mpi_t ref_mpi = NULL;
  gcry_mpi_t sig_mpi = NULL;

  err = sexp_sscan (&data, NULL, sample_data, strlen (sample_data));
  if (!err)
    err = sexp_sscan (&data_bad, NULL,
                      sample_data_bad, strlen (sample_data_bad));
  if (err)
    {
      errtxt = "converting data failed";
      goto leave;
    }

  err = _gcry_pk_sign (&sig, data, skey);
  if (err)
    {
      errtxt = "signing failed";
      goto leave;
    }

  err = _gcry_mpi_scan(&ref_mpi, GCRYMPI_FMT_HEX, ref_data, 0, NULL);
  if (err)
    {
      errtxt = "converting ref_data to mpi failed";
      goto leave;
    }

  err = _gcry_sexp_extract_param(sig, "sig-val!rsa", "s", &sig_mpi, NULL);
  if (err)
    {
      errtxt = "extracting signature data failed";
      goto leave;
    }

  if (mpi_cmp (sig_mpi, ref_mpi))
    {
      errtxt = "signature does not match reference data";
      goto leave;
    }

  err = _gcry_pk_verify (sig, data, pkey);
  if (err)
    {
      errtxt = "verify failed";
      goto leave;
    }
  err = _gcry_pk_verify (sig, data_bad, pkey);
  if (gcry_err_code (err) != GPG_ERR_BAD_SIGNATURE)
    {
      errtxt = "bad signature not detected";
      goto leave;
    }


 leave:
  sexp_release (sig);
  sexp_release (data_bad);
  sexp_release (data);
  _gcry_mpi_release (ref_mpi);
  _gcry_mpi_release (sig_mpi);
  return errtxt;
}