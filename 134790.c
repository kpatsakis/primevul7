test_bson_count_keys (void)
{
   bson_t b;

   bson_init (&b);
   BSON_ASSERT (bson_append_int32 (&b, "0", -1, 0));
   BSON_ASSERT (bson_append_int32 (&b, "1", -1, 1));
   BSON_ASSERT (bson_append_int32 (&b, "2", -1, 2));
   BSON_ASSERT_CMPINT (bson_count_keys (&b), ==, 3);
   bson_destroy (&b);
}