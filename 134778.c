test_bson_has_field (void)
{
   bson_t *b;
   bool r;

   b = BCON_NEW ("foo", "[", "{", "bar", BCON_INT32 (1), "}", "]");

   r = bson_has_field (b, "foo");
   BSON_ASSERT (r);

   r = bson_has_field (b, "foo.0");
   BSON_ASSERT (r);

   r = bson_has_field (b, "foo.0.bar");
   BSON_ASSERT (r);

   r = bson_has_field (b, "0");
   BSON_ASSERT (!r);

   r = bson_has_field (b, "bar");
   BSON_ASSERT (!r);

   r = bson_has_field (b, "0.bar");
   BSON_ASSERT (!r);

   bson_destroy (b);
}