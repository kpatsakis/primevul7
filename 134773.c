test_bson_validate_bool (void)
{
   /* {"b": true}, with implicit NULL at end */
   uint8_t data[] = "\x09\x00\x00\x00\x08\x62\x00\x01";
   bson_t bson;
   bson_iter_t iter;
   size_t err_offset = 0;

   ASSERT (bson_init_static (&bson, data, sizeof data));
   ASSERT (bson_validate (&bson, BSON_VALIDATE_NONE, &err_offset));
   ASSERT (bson_iter_init (&iter, &bson));
   ASSERT (bson_iter_next (&iter));
   ASSERT (BSON_ITER_HOLDS_BOOL (&iter));
   ASSERT (bson_iter_bool (&iter));

   /* replace boolean value 1 with 255 */
   ASSERT (data[7] == '\x01');
   data[7] = (uint8_t) '\xff';

   ASSERT (bson_init_static (&bson, data, 9));
   ASSERT (!bson_validate (&bson, BSON_VALIDATE_NONE, &err_offset));
   BSON_ASSERT_CMPINT (err_offset, ==, 7);

   ASSERT (bson_iter_init (&iter, &bson));
   ASSERT (!bson_iter_next (&iter));
}