test_bson_append_deep (void)
{
   bson_t *a;
   bson_t *tmp;
   int i;

   a = bson_new ();

   for (i = 0; i < 100; i++) {
      tmp = a;
      a = bson_new ();
      BSON_ASSERT (bson_append_document (a, "a", -1, tmp));
      bson_destroy (tmp);
   }

   BSON_ASSERT_BSON_EQUAL_FILE (a, "test38.bson");

   bson_destroy (a);
}