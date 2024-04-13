verify(gcry_mpi_t a, gcry_mpi_t b, gcry_mpi_t input, ELG_public_key *pkey )
{
  int rc;
  gcry_mpi_t t1;
  gcry_mpi_t t2;
  gcry_mpi_t base[4];
  gcry_mpi_t ex[4];

  if( !(mpi_cmp_ui( a, 0 ) > 0 && mpi_cmp( a, pkey->p ) < 0) )
    return 0; /* assertion	0 < a < p  failed */

  t1 = mpi_alloc( mpi_get_nlimbs(a) );
  t2 = mpi_alloc( mpi_get_nlimbs(a) );

#if 0
  /* t1 = (y^a mod p) * (a^b mod p) mod p */
  gcry_mpi_powm( t1, pkey->y, a, pkey->p );
  gcry_mpi_powm( t2, a, b, pkey->p );
  mpi_mulm( t1, t1, t2, pkey->p );

  /* t2 = g ^ input mod p */
  gcry_mpi_powm( t2, pkey->g, input, pkey->p );

  rc = !mpi_cmp( t1, t2 );
#elif 0
  /* t1 = (y^a mod p) * (a^b mod p) mod p */
  base[0] = pkey->y; ex[0] = a;
  base[1] = a;       ex[1] = b;
  base[2] = NULL;    ex[2] = NULL;
  mpi_mulpowm( t1, base, ex, pkey->p );

  /* t2 = g ^ input mod p */
  gcry_mpi_powm( t2, pkey->g, input, pkey->p );

  rc = !mpi_cmp( t1, t2 );
#else
  /* t1 = g ^ - input * y ^ a * a ^ b  mod p */
  mpi_invm(t2, pkey->g, pkey->p );
  base[0] = t2     ; ex[0] = input;
  base[1] = pkey->y; ex[1] = a;
  base[2] = a;       ex[2] = b;
  base[3] = NULL;    ex[3] = NULL;
  mpi_mulpowm( t1, base, ex, pkey->p );
  rc = !mpi_cmp_ui( t1, 1 );

#endif

  mpi_free(t1);
  mpi_free(t2);
  return rc;
}