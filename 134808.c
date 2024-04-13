test_bson_append_array (void)
{
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   b2 = bson_new ();
   BSON_ASSERT (bson_append_utf8 (b2, "0", -1, "hello", -1));
   BSON_ASSERT (bson_append_utf8 (b2, "1", -1, "world", -1));
   BSON_ASSERT (bson_append_array (b, "array", -1, b2));
   bson_destroy (b2);
   b2 = get_bson ("test23.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}