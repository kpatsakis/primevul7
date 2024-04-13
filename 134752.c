test_bson_concat (void)
{
   bson_t a = BSON_INITIALIZER;
   bson_t b = BSON_INITIALIZER;
   bson_t c = BSON_INITIALIZER;

   bson_append_int32 (&a, "abc", 3, 1);
   bson_append_int32 (&b, "def", 3, 1);
   bson_concat (&a, &b);

   bson_append_int32 (&c, "abc", 3, 1);
   bson_append_int32 (&c, "def", 3, 1);

   BSON_ASSERT (0 == bson_compare (&c, &a));

   bson_destroy (&a);
   bson_destroy (&b);
   bson_destroy (&c);
}