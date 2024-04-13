test_keys (RSA_secret_key *sk, unsigned int nbits)
{
  int result = -1; /* Default to failure.  */
  RSA_public_key pk;
  gcry_mpi_t plaintext = mpi_new (nbits);
  gcry_mpi_t ciphertext = mpi_new (nbits);
  gcry_mpi_t decr_plaintext = mpi_new (nbits);
  gcry_mpi_t signature = mpi_new (nbits);

  /* Put the relevant parameters into a public key structure.  */
  pk.n = sk->n;
  pk.e = sk->e;

  /* Create a random plaintext.  */
  _gcry_mpi_randomize (plaintext, nbits, GCRY_WEAK_RANDOM);

  /* Encrypt using the public key.  */
  public (ciphertext, plaintext, &pk);

  /* Check that the cipher text does not match the plaintext.  */
  if (!mpi_cmp (ciphertext, plaintext))
    goto leave; /* Ciphertext is identical to the plaintext.  */

  /* Decrypt using the secret key.  */
  secret (decr_plaintext, ciphertext, sk);

  /* Check that the decrypted plaintext matches the original plaintext.  */
  if (mpi_cmp (decr_plaintext, plaintext))
    goto leave; /* Plaintext does not match.  */

  /* Create another random plaintext as data for signature checking.  */
  _gcry_mpi_randomize (plaintext, nbits, GCRY_WEAK_RANDOM);

  /* Use the RSA secret function to create a signature of the plaintext.  */
  secret (signature, plaintext, sk);

  /* Use the RSA public function to verify this signature.  */
  public (decr_plaintext, signature, &pk);
  if (mpi_cmp (decr_plaintext, plaintext))
    goto leave; /* Signature does not match.  */

  /* Modify the signature and check that the signing fails.  */
  mpi_add_ui (signature, signature, 1);
  public (decr_plaintext, signature, &pk);
  if (!mpi_cmp (decr_plaintext, plaintext))
    goto leave; /* Signature matches but should not.  */

  result = 0; /* All tests succeeded.  */

 leave:
  _gcry_mpi_release (signature);
  _gcry_mpi_release (decr_plaintext);
  _gcry_mpi_release (ciphertext);
  _gcry_mpi_release (plaintext);
  return result;
}