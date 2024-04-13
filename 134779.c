test_bson_initializer (void)
{
   bson_t b = BSON_INITIALIZER;

   BSON_ASSERT (bson_empty (&b));
   bson_append_bool (&b, "foo", -1, true);
   BSON_ASSERT (!bson_empty (&b));
   bson_destroy (&b);
}