test_bson_clear (void)
{
   bson_t *doc = NULL;

   bson_clear (&doc);
   BSON_ASSERT (doc == NULL);

   doc = bson_new ();
   BSON_ASSERT (doc != NULL);
   bson_clear (&doc);
   BSON_ASSERT (doc == NULL);
}