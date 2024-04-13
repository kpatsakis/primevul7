secret (gcry_mpi_t output, gcry_mpi_t input, RSA_secret_key *skey )
{
  /* Remove superfluous leading zeroes from INPUT.  */
  mpi_normalize (input);

  if (!skey->p || !skey->q || !skey->u)
    {
      secret_core_std (output, input, skey->d, skey->n);
    }
  else
    {
      secret_core_crt (output, input, skey->d, mpi_get_nlimbs (skey->n),
                       skey->p, skey->q, skey->u);
    }
}