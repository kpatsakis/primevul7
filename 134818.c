bloat (bson_t *b)
{
   uint32_t i;
   char buf[16];
   const char *key;

   for (i = 0; i < 100; i++) {
      bson_uint32_to_string (i, &key, buf, sizeof buf);
      BSON_APPEND_UTF8 (b, key, "long useless value foo bar baz quux quizzle");
   }

   /* spilled over */
   ASSERT (!(b->flags & BSON_FLAG_INLINE));
}