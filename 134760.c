test_next_power_of_two (void)
{
   size_t s;

   s = 3;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 4);

   s = 4;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 4);

   s = 33;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 64);

   s = 91;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 128);

   s = 939524096UL;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 1073741824);

   s = 1073741824UL;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 1073741824UL);

#if BSON_WORD_SIZE == 64
   s = 4294967296LL;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 4294967296LL);

   s = 4294967297LL;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 8589934592LL);

   s = 17179901952LL;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 34359738368LL);

   s = 9223372036854775807ULL;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 9223372036854775808ULL);

   s = 36028795806651656ULL;
   s = bson_next_power_of_two (s);
   BSON_ASSERT (s == 36028797018963968ULL);
#endif
}