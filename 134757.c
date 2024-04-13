test_bson_append_binary (void)
{
   const static uint8_t binary[] = {'1', '2', '3', '4'};
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   BSON_ASSERT (
      bson_append_binary (b, "binary", -1, BSON_SUBTYPE_USER, binary, 4));
   b2 = get_bson ("test24.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}