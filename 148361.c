elg_generate (int algo, unsigned int nbits, unsigned long evalue,
              gcry_mpi_t *skey, gcry_mpi_t **retfactors)
{
  ELG_secret_key sk;

  (void)algo;
  (void)evalue;

  generate (&sk, nbits, retfactors);
  skey[0] = sk.p;
  skey[1] = sk.g;
  skey[2] = sk.y;
  skey[3] = sk.x;

  return GPG_ERR_NO_ERROR;
}