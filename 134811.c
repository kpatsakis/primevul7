test_bson_copy (void)
{
   bson_t b;
   bson_t *c;

   bson_init (&b);
   BSON_ASSERT (bson_append_int32 (&b, "foobar", -1, 1234));
   c = bson_copy (&b);
   BSON_ASSERT_BSON_EQUAL (&b, c);
   bson_destroy (c);
   bson_destroy (&b);
}