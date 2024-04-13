test_bson_build_child_deep_1 (bson_t *b, int *count)
{
   bson_t child;

   (*count)++;

   BSON_ASSERT (bson_append_document_begin (b, "b", -1, &child));
   BSON_ASSERT (!(b->flags & BSON_FLAG_INLINE));
   BSON_ASSERT ((b->flags & BSON_FLAG_IN_CHILD));
   BSON_ASSERT (!(child.flags & BSON_FLAG_INLINE));
   BSON_ASSERT ((child.flags & BSON_FLAG_STATIC));
   BSON_ASSERT ((child.flags & BSON_FLAG_CHILD));

   if (*count < 100) {
      test_bson_build_child_deep_1 (&child, count);
   } else {
      BSON_ASSERT (bson_append_int32 (&child, "b", -1, 1234));
   }

   BSON_ASSERT (bson_append_document_end (b, &child));
   BSON_ASSERT (!(b->flags & BSON_FLAG_IN_CHILD));
}