extract_a_from_sexp (gcry_sexp_t encr_data)
{
  gcry_sexp_t l1, l2, l3;
  gcry_mpi_t a_value;

  l1 = sexp_find_token (encr_data, "enc-val", 0);
  if (!l1)
    return NULL;
  l2 = sexp_find_token (l1, "rsa", 0);
  sexp_release (l1);
  if (!l2)
    return NULL;
  l3 = sexp_find_token (l2, "a", 0);
  sexp_release (l2);
  if (!l3)
    return NULL;
  a_value = sexp_nth_mpi (l3, 1, 0);
  sexp_release (l3);

  return a_value;
}