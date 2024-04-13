test_bson_append_timestamp (void)
{
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   BSON_ASSERT (bson_append_timestamp (b, "timestamp", -1, 1234, 9876));
   b2 = get_bson ("test35.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}