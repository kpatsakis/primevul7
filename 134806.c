test_bson_append_dbpointer (void)
{
   bson_oid_t oid;
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   bson_oid_init_from_string (&oid, "0123abcd0123abcd0123abcd");
   BSON_ASSERT (bson_append_dbpointer (b, "dbpointer", -1, "foo", &oid));
   b2 = get_bson ("test28.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}