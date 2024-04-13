test_bson_append_symbol (void)
{
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   b2 = get_bson ("test32.bson");
   BSON_ASSERT (bson_append_symbol (b, "hello", -1, "world", -1));
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}