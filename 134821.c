test_bson_copy_to_excluding_noinit (void)
{
   bson_iter_t iter;
   bool r;
   bson_t b;
   bson_t c;
   int i;

   bson_init (&b);
   bson_append_int32 (&b, "a", 1, 1);
   bson_append_int32 (&b, "b", 1, 2);

   bson_init (&c);
   bson_copy_to_excluding_noinit (&b, &c, "b", NULL);
   r = bson_iter_init_find (&iter, &c, "a");
   BSON_ASSERT (r);
   r = bson_iter_init_find (&iter, &c, "b");
   BSON_ASSERT (!r);

   i = bson_count_keys (&b);
   BSON_ASSERT_CMPINT (i, ==, 2);

   i = bson_count_keys (&c);
   BSON_ASSERT_CMPINT (i, ==, 1);

   bson_destroy (&b);
   bson_destroy (&c);
}