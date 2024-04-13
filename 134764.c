test_bson_append_time_t (void)
{
   bson_t *b;
   bson_t *b2;
   time_t t;

   t = 1234567890;

   b = bson_new ();
   BSON_ASSERT (bson_append_time_t (b, "time_t", -1, t));
   b2 = get_bson ("test26.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}