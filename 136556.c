generate_hash_secret_salt(void)
{
  unsigned int seed = time(NULL) % UINT_MAX;
  srand(seed);
  return rand();
}