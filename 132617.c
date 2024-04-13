secret_blinded (gcry_mpi_t output, gcry_mpi_t input,
                RSA_secret_key *sk, unsigned int nbits)
{
  gcry_mpi_t r;	           /* Random number needed for blinding.  */
  gcry_mpi_t ri;	   /* Modular multiplicative inverse of r.  */
  gcry_mpi_t bldata;       /* Blinded data to decrypt.  */

  /* First, we need a random number r between 0 and n - 1, which is
   * relatively prime to n (i.e. it is neither p nor q).  The random
   * number needs to be only unpredictable, thus we employ the
   * gcry_create_nonce function by using GCRY_WEAK_RANDOM with
   * gcry_mpi_randomize.  */
  r  = mpi_snew (nbits);
  ri = mpi_snew (nbits);
  bldata = mpi_snew (nbits);

  do
    {
      _gcry_mpi_randomize (r, nbits, GCRY_WEAK_RANDOM);
      mpi_mod (r, r, sk->n);
    }
  while (!mpi_invm (ri, r, sk->n));

  /* Do blinding.  We calculate: y = (x * r^e) mod n, where r is the
   * random number, e is the public exponent, x is the non-blinded
   * input data and n is the RSA modulus.  */
  mpi_powm (bldata, r, sk->e, sk->n);
  mpi_mulm (bldata, bldata, input, sk->n);

  /* Perform decryption.  */
  secret (output, bldata, sk);
  _gcry_mpi_release (bldata);

  /* Undo blinding.  Here we calculate: y = (x * r^-1) mod n, where x
   * is the blinded decrypted data, ri is the modular multiplicative
   * inverse of r and n is the RSA modulus.  */
  mpi_mulm (output, output, ri, sk->n);

  _gcry_mpi_release (r);
  _gcry_mpi_release (ri);
}