test_bson_destroy_with_steal (void)
{
   bson_t *b1;
   bson_t b2;
   uint32_t len = 0;
   uint8_t *data;
   int i;

   b1 = bson_new ();
   for (i = 0; i < 100; i++) {
      BSON_APPEND_INT32 (b1, "some-key", i);
   }

   data = bson_destroy_with_steal (b1, true, &len);
   BSON_ASSERT (data);
   BSON_ASSERT (len == 1405);
   bson_free (data);
   data = NULL;

   bson_init (&b2);
   len = 0;
   for (i = 0; i < 100; i++) {
      BSON_APPEND_INT32 (&b2, "some-key", i);
   }
   BSON_ASSERT (!bson_destroy_with_steal (&b2, false, &len));
   BSON_ASSERT (len == 1405);

   bson_init (&b2);
   BSON_ASSERT (!bson_destroy_with_steal (&b2, false, NULL));

   bson_init (&b2);
   data = bson_destroy_with_steal (&b2, true, &len);
   BSON_ASSERT (data);
   BSON_ASSERT (len == 5);
   bson_free (data);
   data = NULL;
}