test_bson_reserve_buffer_errors (void)
{
   bson_t bson = BSON_INITIALIZER;
   bson_t child;
   uint8_t data[5] = {0};
   uint32_t len_le;

   /* too big */
   ASSERT (!bson_reserve_buffer (&bson, (uint32_t) (INT32_MAX - bson.len - 1)));

   /* make a static bson, it refuses bson_reserve_buffer since it's read-only */
   len_le = BSON_UINT32_TO_LE (5);
   memcpy (data, &len_le, sizeof (len_le));
   ASSERT (bson_init_static (&bson, data, sizeof data));
   ASSERT (!bson_reserve_buffer (&bson, 10));

   /* parent's and child's buffers are locked */
   bson_init (&bson);
   BSON_APPEND_DOCUMENT_BEGIN (&bson, "child", &child);
   ASSERT (!bson_reserve_buffer (&bson, 10));
   ASSERT (!bson_reserve_buffer (&child, 10));
   /* unlock parent's buffer */
   bson_append_document_end (&bson, &child);
   ASSERT (bson_reserve_buffer (&bson, 10));

   bson_destroy (&bson);
}