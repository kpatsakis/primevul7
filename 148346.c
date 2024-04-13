decrypt(gcry_mpi_t output, gcry_mpi_t a, gcry_mpi_t b, ELG_secret_key *skey )
{
  gcry_mpi_t t1, t2, r;
  unsigned int nbits = mpi_get_nbits (skey->p);

  mpi_normalize (a);
  mpi_normalize (b);

  t1 = mpi_snew (nbits);

#ifdef USE_BLINDING

  t2 = mpi_snew (nbits);
  r  = mpi_new (nbits);

  /* We need a random number of about the prime size.  The random
     number merely needs to be unpredictable; thus we use level 0.  */
  _gcry_mpi_randomize (r, nbits, GCRY_WEAK_RANDOM);

  /* t1 = r^x mod p */
  mpi_powm (t1, r, skey->x, skey->p);
  /* t2 = (a * r)^-x mod p */
  mpi_mulm (t2, a, r, skey->p);
  mpi_powm (t2, t2, skey->x, skey->p);
  mpi_invm (t2, t2, skey->p);
  /* t1 = (t1 * t2) mod p*/
  mpi_mulm (t1, t1, t2, skey->p);

  mpi_free (r);
  mpi_free (t2);

#else /*!USE_BLINDING*/

  /* output = b/(a^x) mod p */
  mpi_powm (t1, a, skey->x, skey->p);
  mpi_invm (t1, t1, skey->p);

#endif /*!USE_BLINDING*/

  mpi_mulm (output, b, t1, skey->p);

#if 0
  if( DBG_CIPHER )
    {
      log_mpidump("elg decrypted x= ", skey->x);
      log_mpidump("elg decrypted p= ", skey->p);
      log_mpidump("elg decrypted a= ", a);
      log_mpidump("elg decrypted b= ", b);
      log_mpidump("elg decrypted M= ", output);
    }
#endif
  mpi_free (t1);
}