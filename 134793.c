test_bson_append_binary_deprecated (void)
{
   const static uint8_t binary[] = {'1', '2', '3', '4'};
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   BSON_ASSERT (bson_append_binary (
      b, "binary", -1, BSON_SUBTYPE_BINARY_DEPRECATED, binary, 4));
   b2 = get_bson ("binary_deprecated.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}