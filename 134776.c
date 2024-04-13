test_bson_subtype_2 (void)
{
   bson_t b;
   /* taken from BSON Corpus Tests */
   const char ok[] = "\x13\x00\x00\x00\x05\x78\x00\x06\x00\x00\x00\x02\x02\x00"
                     "\x00\x00\xff\xff\x00";

   /* Deprecated subtype 0x02 includes a redundant length inside the binary
    * payload. Check that we validate this length.
    */
   const char len_too_long[] = "\x13\x00\x00\x00\x05\x78\x00\x06\x00\x00\x00"
                               "\x02\x03\x00\x00\x00\xFF\xFF\x00";
   const char len_too_short[] = "\x13\x00\x00\x00\x05\x78\x00\x06\x00\x00\x00"
                                "\x02\x01\x00\x00\x00\xFF\xFF\x00";
   const char len_negative[] = "\x13\x00\x00\x00\x05\x78\x00\x06\x00\x00\x00"
                               "\x02\xFF\xFF\xFF\xFF\xFF\xFF\x00";

   bson_t *bson_ok = BCON_NEW ("x", BCON_BIN (2, (uint8_t *) "\xff\xff", 2));

   BSON_ASSERT (bson_init_static (&b, (uint8_t *) ok, sizeof (ok) - 1));
   BSON_ASSERT (bson_validate (&b, BSON_VALIDATE_NONE, 0));
   BSON_ASSERT (0 == bson_compare (&b, bson_ok));

   BSON_ASSERT (bson_init_static (
      &b, (uint8_t *) len_too_long, sizeof (len_too_long) - 1));
   BSON_ASSERT (!bson_validate (&b, BSON_VALIDATE_NONE, 0));

   BSON_ASSERT (bson_init_static (
      &b, (uint8_t *) len_too_short, sizeof (len_too_short) - 1));
   BSON_ASSERT (!bson_validate (&b, BSON_VALIDATE_NONE, 0));

   BSON_ASSERT (bson_init_static (
      &b, (uint8_t *) len_negative, sizeof (len_negative) - 1));
   BSON_ASSERT (!bson_validate (&b, BSON_VALIDATE_NONE, 0));

   bson_destroy (bson_ok);
}