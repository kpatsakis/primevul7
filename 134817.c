test_bson_build_child_deep (void)
{
   union {
      bson_t b;
      bson_impl_alloc_t a;
   } u;
   int count = 0;

   bson_init (&u.b);
   BSON_ASSERT ((u.b.flags & BSON_FLAG_INLINE));
   test_bson_build_child_deep_1 (&u.b, &count);
   BSON_ASSERT (!(u.b.flags & BSON_FLAG_INLINE));
   BSON_ASSERT ((u.b.flags & BSON_FLAG_STATIC));
   BSON_ASSERT (!(u.b.flags & BSON_FLAG_NO_FREE));
   BSON_ASSERT (!(u.b.flags & BSON_FLAG_RDONLY));
   BSON_ASSERT (bson_validate (&u.b, BSON_VALIDATE_NONE, NULL));
   BSON_ASSERT (((bson_impl_alloc_t *) &u.b)->alloclen == 1024);
   BSON_ASSERT_BSON_EQUAL_FILE (&u.b, "test39.bson");
   bson_destroy (&u.b);
}