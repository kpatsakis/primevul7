test_bson_append_regex (void)
{
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   BSON_ASSERT (bson_append_regex (b, "regex", -1, "^abcd", "ilx"));
   b2 = get_bson ("test27.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}