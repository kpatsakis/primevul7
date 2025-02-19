generate_using_x (ELG_secret_key *sk, unsigned int nbits, gcry_mpi_t x,
                  gcry_mpi_t **ret_factors )
{
  gcry_mpi_t p;      /* The prime.  */
  gcry_mpi_t p_min1; /* The prime minus 1.  */
  gcry_mpi_t g;      /* The generator.  */
  gcry_mpi_t y;      /* g^x mod p.  */
  unsigned int qbits;
  unsigned int xbits;

  sk->p = NULL;
  sk->g = NULL;
  sk->y = NULL;
  sk->x = NULL;

  /* Do a quick check to see whether X is suitable.  */
  xbits = mpi_get_nbits (x);
  if ( xbits < 64 || xbits >= nbits )
    return GPG_ERR_INV_VALUE;

  p_min1 = gcry_mpi_new ( nbits );
  qbits  = wiener_map ( nbits );
  if ( (qbits & 1) ) /* Better have an even one.  */
    qbits++;
  g = mpi_alloc (1);
  p = _gcry_generate_elg_prime ( 0, nbits, qbits, g, ret_factors );
  mpi_sub_ui (p_min1, p, 1);

  if (DBG_CIPHER)
    log_debug ("using a supplied x of size %u", xbits );
  if ( !(mpi_cmp_ui ( x, 0 ) > 0 && mpi_cmp ( x, p_min1 ) <0 ) )
    {
      gcry_mpi_release ( p_min1 );
      gcry_mpi_release ( p );
      gcry_mpi_release ( g );
      return GPG_ERR_INV_VALUE;
    }

  y = gcry_mpi_new (nbits);
  gcry_mpi_powm ( y, g, x, p );

  if ( DBG_CIPHER )
    {
      progress ('\n');
      log_mpidump ("elg  p= ", p );
      log_mpidump ("elg  g= ", g );
      log_mpidump ("elg  y= ", y );
      log_mpidump ("elg  x= ", x );
    }

  /* Copy the stuff to the key structures */
  sk->p = p;
  sk->g = g;
  sk->y = y;
  sk->x = gcry_mpi_copy (x);

  gcry_mpi_release ( p_min1 );

  /* Now we can test our keys. */
  if ( test_keys ( sk, nbits - 64, 1 ) )
    {
      gcry_mpi_release ( sk->p ); sk->p = NULL;
      gcry_mpi_release ( sk->g ); sk->g = NULL;
      gcry_mpi_release ( sk->y ); sk->y = NULL;
      gcry_mpi_release ( sk->x ); sk->x = NULL;
      return GPG_ERR_BAD_SECKEY;
    }

  return 0;
}