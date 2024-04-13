test_bson_copy_to (void)
{
   bson_t b;
   bson_t c;
   int i;

   /*
    * Test inline structure copy.
    */
   bson_init (&b);
   BSON_ASSERT (bson_append_int32 (&b, "foobar", -1, 1234));
   bson_copy_to (&b, &c);
   BSON_ASSERT_BSON_EQUAL (&b, &c);
   bson_destroy (&c);
   bson_destroy (&b);

   /*
    * Test malloced copy.
    */
   bson_init (&b);
   for (i = 0; i < 1000; i++) {
      BSON_ASSERT (bson_append_int32 (&b, "foobar", -1, 1234));
   }
   bson_copy_to (&b, &c);
   BSON_ASSERT_BSON_EQUAL (&b, &c);
   bson_destroy (&c);
   bson_destroy (&b);
}