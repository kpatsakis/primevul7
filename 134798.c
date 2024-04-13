test_bson_reinit (void)
{
   bson_t b = BSON_INITIALIZER;
   int i;

   for (i = 0; i < 1000; i++) {
      bson_append_int32 (&b, "", 0, i);
   }

   bson_reinit (&b);

   for (i = 0; i < 1000; i++) {
      bson_append_int32 (&b, "", 0, i);
   }

   bson_destroy (&b);
}