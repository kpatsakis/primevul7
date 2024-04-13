generate_hash_secret_salt(XML_Parser parser)
{
  /* Process ID is 0 bits entropy if attacker has local access
   * XML_Parser address is few bits of entropy if attacker has local access */
  const unsigned long entropy =
      gather_time_entropy() ^ getpid() ^ (unsigned long)parser;

  /* Factors are 2^31-1 and 2^61-1 (Mersenne primes M31 and M61) */
  if (sizeof(unsigned long) == 4) {
    return entropy * 2147483647;
  } else {
    return entropy * 2305843009213693951;
  }
}