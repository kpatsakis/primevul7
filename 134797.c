test_bson_reserve_buffer (void)
{
   bson_t src = BSON_INITIALIZER;
   bson_t stack_alloced;
   bson_t *heap_alloced;
   uint8_t *buf;

   /* inline, stack-allocated */
   bson_init (&stack_alloced);
   BSON_APPEND_UTF8 (&src, "key", "value");
   ASSERT ((buf = bson_reserve_buffer (&stack_alloced, src.len)));
   ASSERT_CMPUINT32 (src.len, ==, stack_alloced.len);
   ASSERT (stack_alloced.flags & BSON_FLAG_INLINE);
   memcpy (buf, ((bson_impl_inline_t *) &src)->data, src.len);
   /* data was transferred */
   BSON_ASSERT_KEY_AND_VALUE (&stack_alloced);
   bson_destroy (&stack_alloced);

   /* spilled over, stack-allocated */
   bloat (&src);
   bson_init (&stack_alloced);
   ASSERT ((buf = bson_reserve_buffer (&stack_alloced, src.len)));
   ASSERT_CMPUINT32 (src.len, ==, stack_alloced.len);
   ASSERT (!(stack_alloced.flags & BSON_FLAG_INLINE));
   memcpy (buf, ((bson_impl_alloc_t *) &src)->alloc, src.len);
   BSON_ASSERT_KEY_AND_VALUE (&stack_alloced);
   ASSERT (bson_has_field (&stack_alloced, "99"));
   bson_destroy (&src);
   bson_destroy (&stack_alloced);

   /* inline, heap-allocated */
   heap_alloced = bson_new ();
   bson_init (&src);
   BSON_APPEND_UTF8 (&src, "key", "value");
   ASSERT ((buf = bson_reserve_buffer (heap_alloced, src.len)));
   ASSERT_CMPUINT32 (src.len, ==, heap_alloced->len);
   ASSERT (heap_alloced->flags & BSON_FLAG_INLINE);
   memcpy (buf, ((bson_impl_inline_t *) &src)->data, src.len);
   BSON_ASSERT_KEY_AND_VALUE (heap_alloced);
   bson_destroy (heap_alloced);

   /* spilled over, heap-allocated */
   heap_alloced = bson_new ();
   bloat (&src);
   ASSERT ((buf = bson_reserve_buffer (heap_alloced, src.len)));
   ASSERT_CMPUINT32 (src.len, ==, heap_alloced->len);
   ASSERT (!(heap_alloced->flags & BSON_FLAG_INLINE));
   memcpy (buf, ((bson_impl_alloc_t *) &src)->alloc, src.len);
   BSON_ASSERT_KEY_AND_VALUE (heap_alloced);
   ASSERT (bson_has_field (heap_alloced, "99"));

   bson_destroy (&src);
   bson_destroy (heap_alloced);
}