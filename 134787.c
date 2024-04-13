test_bson_append_decimal128 (void)
{
   bson_t *b;
   bson_t *b2;
   bson_decimal128_t value;
   value.high = 0;
   value.low = 1;

   b = bson_new ();
   BSON_ASSERT (bson_append_decimal128 (b, "a", -1, &value));
   b2 = get_bson ("test58.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}