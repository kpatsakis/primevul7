test_bson_append_timeval (void)
{
   struct timeval tv = {0};
   bson_t *b;
   bson_t *b2;

   tv.tv_sec = 1234567890;
   tv.tv_usec = 0;

   b = bson_new ();
   BSON_ASSERT (bson_append_timeval (b, "time_t", -1, &tv));
   b2 = get_bson ("test26.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}