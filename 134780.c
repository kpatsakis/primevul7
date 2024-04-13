test_bson_new (void)
{
   bson_t *b;

   b = bson_new ();
   BSON_ASSERT_CMPINT (b->len, ==, 5);
   bson_destroy (b);

   b = bson_sized_new (32);
   BSON_ASSERT_CMPINT (b->len, ==, 5);
   bson_destroy (b);
}