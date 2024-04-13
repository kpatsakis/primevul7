test_bson_init (void)
{
   bson_t b;
   char key[12];
   int i;

   bson_init (&b);
   BSON_ASSERT ((b.flags & BSON_FLAG_INLINE));
   BSON_ASSERT ((b.flags & BSON_FLAG_STATIC));
   BSON_ASSERT (!(b.flags & BSON_FLAG_RDONLY));
   for (i = 0; i < 100; i++) {
      bson_snprintf (key, sizeof key, "%d", i);
      BSON_ASSERT (bson_append_utf8 (&b, key, -1, "bar", -1));
   }
   BSON_ASSERT (!(b.flags & BSON_FLAG_INLINE));
   bson_destroy (&b);
}