test_bson_append_int64 (void)
{
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   BSON_ASSERT (bson_append_int64 (b, "a", -1, 100000000000000ULL));
   b2 = get_bson ("test34.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}