test_bson_build_child (void)
{
   bson_t b;
   bson_t child;
   bson_t *b2;
   bson_t *child2;

   bson_init (&b);
   BSON_ASSERT (bson_append_document_begin (&b, "foo", -1, &child));
   BSON_ASSERT (bson_append_utf8 (&child, "bar", -1, "baz", -1));
   BSON_ASSERT (bson_append_document_end (&b, &child));

   b2 = bson_new ();
   child2 = bson_new ();
   BSON_ASSERT (bson_append_utf8 (child2, "bar", -1, "baz", -1));
   BSON_ASSERT (bson_append_document (b2, "foo", -1, child2));
   bson_destroy (child2);

   BSON_ASSERT (b.len == b2->len);
   BSON_ASSERT_BSON_EQUAL (&b, b2);

   bson_destroy (&b);
   bson_destroy (b2);
}