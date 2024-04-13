test_bson_alloc (void)
{
   static const uint8_t empty_bson[] = {5, 0, 0, 0, 0};
   bson_t *b;

   b = bson_new ();
   BSON_ASSERT_CMPINT (b->len, ==, 5);
   BSON_ASSERT ((b->flags & BSON_FLAG_INLINE));
   BSON_ASSERT (!(b->flags & BSON_FLAG_CHILD));
   BSON_ASSERT (!(b->flags & BSON_FLAG_STATIC));
   BSON_ASSERT (!(b->flags & BSON_FLAG_NO_FREE));
   bson_destroy (b);

   /*
    * This checks that we fit in the inline buffer size.
    */
   b = bson_sized_new (44);
   BSON_ASSERT_CMPINT (b->len, ==, 5);
   BSON_ASSERT ((b->flags & BSON_FLAG_INLINE));
   bson_destroy (b);

   /*
    * Make sure we grow to next power of 2.
    */
   b = bson_sized_new (121);
   BSON_ASSERT_CMPINT (b->len, ==, 5);
   BSON_ASSERT (!(b->flags & BSON_FLAG_INLINE));
   bson_destroy (b);

   /*
    * Make sure we grow to next power of 2.
    */
   b = bson_sized_new (129);
   BSON_ASSERT_CMPINT (b->len, ==, 5);
   BSON_ASSERT (!(b->flags & BSON_FLAG_INLINE));
   bson_destroy (b);

   b = bson_new_from_data (empty_bson, sizeof empty_bson);
   BSON_ASSERT_CMPINT (b->len, ==, sizeof empty_bson);
   BSON_ASSERT ((b->flags & BSON_FLAG_INLINE));
   BSON_ASSERT (!memcmp (bson_get_data (b), empty_bson, sizeof empty_bson));
   bson_destroy (b);
}