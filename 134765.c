test_bson_append_iter (void)
{
   bson_iter_t iter;
   bool r;
   bson_t b;
   bson_t c;

   bson_init (&b);
   bson_append_int32 (&b, "a", 1, 1);
   bson_append_int32 (&b, "b", 1, 2);
   bson_append_int32 (&b, "c", 1, 3);
   bson_append_utf8 (&b, "d", 1, "hello", 5);

   bson_init (&c);

   r = bson_iter_init_find (&iter, &b, "a");
   BSON_ASSERT (r);
   r = bson_append_iter (&c, NULL, 0, &iter);
   BSON_ASSERT (r);

   r = bson_iter_init_find (&iter, &b, "c");
   BSON_ASSERT (r);
   r = bson_append_iter (&c, NULL, 0, &iter);
   BSON_ASSERT (r);

   r = bson_iter_init_find (&iter, &b, "d");
   BSON_ASSERT (r);
   r = bson_append_iter (&c, "world", -1, &iter);
   BSON_ASSERT (r);

   bson_iter_init (&iter, &c);
   r = bson_iter_next (&iter);
   BSON_ASSERT (r);
   BSON_ASSERT_CMPSTR ("a", bson_iter_key (&iter));
   BSON_ASSERT_CMPINT (BSON_TYPE_INT32, ==, bson_iter_type (&iter));
   BSON_ASSERT_CMPINT (1, ==, bson_iter_int32 (&iter));
   r = bson_iter_next (&iter);
   BSON_ASSERT (r);
   BSON_ASSERT_CMPSTR ("c", bson_iter_key (&iter));
   BSON_ASSERT_CMPINT (BSON_TYPE_INT32, ==, bson_iter_type (&iter));
   BSON_ASSERT_CMPINT (3, ==, bson_iter_int32 (&iter));
   r = bson_iter_next (&iter);
   BSON_ASSERT (r);
   BSON_ASSERT_CMPINT (BSON_TYPE_UTF8, ==, bson_iter_type (&iter));
   BSON_ASSERT_CMPSTR ("world", bson_iter_key (&iter));
   BSON_ASSERT_CMPSTR ("hello", bson_iter_utf8 (&iter, NULL));

   bson_destroy (&b);
   bson_destroy (&c);
}