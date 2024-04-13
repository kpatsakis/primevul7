test_bson_new_from_buffer (void)
{
   bson_t *b;
   uint8_t *buf = bson_malloc0 (5);
   size_t len = 5;
   uint32_t len_le = BSON_UINT32_TO_LE (5);

   memcpy (buf, &len_le, sizeof (len_le));

   b = bson_new_from_buffer (&buf, &len, bson_realloc_ctx, NULL);

   BSON_ASSERT (b->flags & BSON_FLAG_NO_FREE);
   BSON_ASSERT (len == 5);
   BSON_ASSERT (b->len == 5);

   bson_append_utf8 (b, "hello", -1, "world", -1);

   BSON_ASSERT (len == 32);
   BSON_ASSERT (b->len == 22);

   bson_destroy (b);

   bson_free (buf);

   buf = NULL;
   len = 0;

   b = bson_new_from_buffer (&buf, &len, bson_realloc_ctx, NULL);

   BSON_ASSERT (b->flags & BSON_FLAG_NO_FREE);
   BSON_ASSERT (len == 5);
   BSON_ASSERT (b->len == 5);

   bson_destroy (b);
   bson_free (buf);
}