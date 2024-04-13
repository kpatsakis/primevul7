generate_fips (RSA_secret_key *sk, unsigned int nbits, unsigned long use_e,
               gcry_sexp_t testparms, int transient_key)
{
  gcry_mpi_t p, q; /* the two primes */
  gcry_mpi_t d;    /* the private key */
  gcry_mpi_t u;
  gcry_mpi_t p1, q1;
  gcry_mpi_t n;    /* the public key */
  gcry_mpi_t e;    /* the exponent */
  gcry_mpi_t g;
  gcry_mpi_t minp;
  gcry_mpi_t diff, mindiff;
  gcry_random_level_t random_level;
  unsigned int pbits = nbits/2;
  unsigned int i;
  int pqswitch;
  gpg_err_code_t ec = GPG_ERR_NO_PRIME;

  if (nbits < 1024 || (nbits & 0x1FF))
    return GPG_ERR_INV_VALUE;
  if (_gcry_enforced_fips_mode() && nbits != 2048 && nbits != 3072)
      return GPG_ERR_INV_VALUE;

  /* The random quality depends on the transient_key flag.  */
  random_level = transient_key ? GCRY_STRONG_RANDOM : GCRY_VERY_STRONG_RANDOM;

  if (testparms)
    {
      /* Parameters to derive the key are given.  */
      /* Note that we explicitly need to setup the values of tbl
         because some compilers (e.g. OpenWatcom, IRIX) don't allow to
         initialize a structure with automatic variables.  */
      struct { const char *name; gcry_mpi_t *value; } tbl[] = {
        { "e" },
        { "p" },
        { "q" },
        { NULL }
      };
      int idx;
      gcry_sexp_t oneparm;

      tbl[0].value = &e;
      tbl[1].value = &p;
      tbl[2].value = &q;

      for (idx=0; tbl[idx].name; idx++)
        {
          oneparm = sexp_find_token (testparms, tbl[idx].name, 0);
          if (oneparm)
            {
              *tbl[idx].value = sexp_nth_mpi (oneparm, 1, GCRYMPI_FMT_USG);
              sexp_release (oneparm);
            }
        }
      for (idx=0; tbl[idx].name; idx++)
        if (!*tbl[idx].value)
          break;
      if (tbl[idx].name)
        {
          /* At least one parameter is missing.  */
          for (idx=0; tbl[idx].name; idx++)
            _gcry_mpi_release (*tbl[idx].value);
          return GPG_ERR_MISSING_VALUE;
        }
    }
  else
    {
      if (use_e < 65537)
        use_e = 65537;  /* This is the smallest value allowed by FIPS */

      e = mpi_alloc ((32+BITS_PER_MPI_LIMB-1)/BITS_PER_MPI_LIMB);

      use_e |= 1; /* make sure this is odd */
      mpi_set_ui (e, use_e);

      p = mpi_snew (pbits);
      q = mpi_snew (pbits);
    }

  n = mpi_new (nbits);
  d = mpi_snew (nbits);
  u = mpi_snew (nbits);

  /* prepare approximate minimum p and q */
  minp = mpi_new (pbits);
  mpi_set_ui (minp, 0xB504F334);
  mpi_lshift (minp, minp, pbits - 32);

  /* prepare minimum p and q difference */
  diff = mpi_new (pbits);
  mindiff = mpi_new (pbits - 99);
  mpi_set_ui (mindiff, 1);
  mpi_lshift (mindiff, mindiff, pbits - 100);

  p1 = mpi_snew (pbits);
  q1 = mpi_snew (pbits);
  g  = mpi_snew (pbits);

 retry:
  /* generate p and q */
  for (i = 0; i < 5 * pbits; i++)
    {
    ploop:
      if (!testparms)
        {
          _gcry_mpi_randomize (p, pbits, random_level);
        }
      if (mpi_cmp (p, minp) < 0)
        {
          if (testparms)
            goto err;
          goto ploop;
        }

      mpi_sub_ui (p1, p, 1);
      if (mpi_gcd (g, p1, e))
        {
          if (_gcry_fips186_4_prime_check (p, pbits) != GPG_ERR_NO_ERROR)
            {
              /* not a prime */
              if (testparms)
                goto err;
            }
          else
            break;
        }
      else if (testparms)
        goto err;
    }
  if (i >= 5 * pbits)
    goto err;

  for (i = 0; i < 5 * pbits; i++)
    {
    qloop:
      if (!testparms)
        {
          _gcry_mpi_randomize (q, pbits, random_level);
        }
      if (mpi_cmp (q, minp) < 0)
        {
          if (testparms)
            goto err;
          goto qloop;
        }
      if (mpi_cmp (p, q) > 0)
        {
          pqswitch = 1;
          mpi_sub (diff, p, q);
        }
      else
        {
          pqswitch = 0;
          mpi_sub (diff, q, p);
        }
      if (mpi_cmp (diff, mindiff) < 0)
        {
          if (testparms)
            goto err;
          goto qloop;
        }

      mpi_sub_ui (q1, q, 1);
      if (mpi_gcd (g, q1, e))
        {
          if (_gcry_fips186_4_prime_check (q, pbits) != GPG_ERR_NO_ERROR)
            {
              /* not a prime */
              if (testparms)
                goto err;
            }
          else
            break;
        }
      else if (testparms)
        goto err;
    }
  if (i >= 5 * pbits)
    goto err;

  if (testparms)
    {
      mpi_clear (p);
      mpi_clear (q);
    }
  else
    {
      gcry_mpi_t f;

      if (pqswitch)
        {
          gcry_mpi_t tmp;

          tmp = p;
          p = q;
          q = tmp;
        }

      f = mpi_snew (nbits);

      /* calculate the modulus */
      mpi_mul (n, p, q);

      /* calculate the secret key d = e^1 mod phi */
      mpi_gcd (g, p1, q1);
      mpi_fdiv_q (f, p1, g);
      mpi_mul (f, f, q1);

      mpi_invm (d, e, f);

      _gcry_mpi_release (f);

      if (mpi_get_nbits (d) < pbits)
        goto retry;

      /* calculate the inverse of p and q (used for chinese remainder theorem)*/
      mpi_invm (u, p, q );
    }

  ec = 0;

  if (DBG_CIPHER)
    {
      log_mpidump("  p= ", p );
      log_mpidump("  q= ", q );
      log_mpidump("  n= ", n );
      log_mpidump("  e= ", e );
      log_mpidump("  d= ", d );
      log_mpidump("  u= ", u );
    }

 err:

  _gcry_mpi_release (p1);
  _gcry_mpi_release (q1);
  _gcry_mpi_release (g);
  _gcry_mpi_release (minp);
  _gcry_mpi_release (mindiff);
  _gcry_mpi_release (diff);

  sk->n = n;
  sk->e = e;
  sk->p = p;
  sk->q = q;
  sk->d = d;
  sk->u = u;

  /* Now we can test our keys. */
  if (ec || (!testparms && test_keys (sk, nbits - 64)))
    {
      _gcry_mpi_release (sk->n); sk->n = NULL;
      _gcry_mpi_release (sk->e); sk->e = NULL;
      _gcry_mpi_release (sk->p); sk->p = NULL;
      _gcry_mpi_release (sk->q); sk->q = NULL;
      _gcry_mpi_release (sk->d); sk->d = NULL;
      _gcry_mpi_release (sk->u); sk->u = NULL;
      if (!ec)
        {
          fips_signal_error ("self-test after key generation failed");
          return GPG_ERR_SELFTEST_FAILED;
        }
    }

  return ec;
}