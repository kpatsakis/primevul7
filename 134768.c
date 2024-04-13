test_bson_append_document (void)
{
   bson_t *b;
   bson_t *b2;

   b = bson_new ();
   b2 = bson_new ();
   BSON_ASSERT (bson_append_document (b, "document", -1, b2));
   bson_destroy (b2);
   b2 = get_bson ("test21.bson");
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b);
   bson_destroy (b2);
}