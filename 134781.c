test_bson_build_child_deep_no_begin_end_1 (bson_t *b, int *count)
{
   bson_t child;

   (*count)++;

   bson_init (&child);
   if (*count < 100) {
      test_bson_build_child_deep_1 (&child, count);
   } else {
      BSON_ASSERT (bson_append_int32 (&child, "b", -1, 1234));
   }
   BSON_ASSERT (bson_append_document (b, "b", -1, &child));
   bson_destroy (&child);
}