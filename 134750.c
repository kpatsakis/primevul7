test_bson_validate_dbpointer (void)
{
   /* { "a": DBPointer(ObjectId(...), Collection="b") }, implicit NULL at end */
   uint8_t data[] = "\x1A\x00\x00\x00\x0C\x61\x00\x02\x00\x00\x00\x62\x00"
                    "\x56\xE1\xFC\x72\xE0\xC9\x17\xE9\xC4\x71\x41\x61";

   bson_t bson;
   bson_iter_t iter;
   size_t err_offset = 0;
   uint32_t collection_len;
   const char *collection;
   const bson_oid_t *oid;

   ASSERT (bson_init_static (&bson, data, sizeof data));
   ASSERT (bson_validate (&bson, BSON_VALIDATE_NONE, &err_offset));
   ASSERT (bson_iter_init (&iter, &bson));
   ASSERT (bson_iter_next (&iter));
   ASSERT (BSON_ITER_HOLDS_DBPOINTER (&iter));
   bson_iter_dbpointer (&iter, &collection_len, &collection, &oid);
   BSON_ASSERT_CMPSTR (collection, "b");
   BSON_ASSERT_CMPINT (collection_len, ==, 1);

   /* replace the NULL terminator of "b" with 255 */
   ASSERT (data[12] == '\0');
   data[12] = (uint8_t) '\xff';

   ASSERT (bson_init_static (&bson, data, sizeof data));
   ASSERT (!bson_validate (&bson, BSON_VALIDATE_NONE, &err_offset));
   BSON_ASSERT_CMPINT (err_offset, ==, 12);

   ASSERT (bson_iter_init (&iter, &bson));
   ASSERT (!bson_iter_next (&iter));
}