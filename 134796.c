test_bson_append_overflow (void)
{
   const char *key = "a";
   uint32_t len;
   bson_t b;

   len = BSON_MAX_SIZE;
   len -= 4; /* len */
   len -= 1; /* type */
   len -= 1; /* value */
   len -= 1; /* end byte */

   bson_init (&b);
   BSON_ASSERT (!bson_append_bool (&b, key, len, true));
   bson_destroy (&b);
}