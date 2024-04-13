BSON_ASSERT_KEY_AND_VALUE (const bson_t *bson)
{
   bson_iter_t iter;

   ASSERT (bson_iter_init_find (&iter, bson, "key"));
   ASSERT (BSON_ITER_HOLDS_UTF8 (&iter));
   ASSERT_CMPSTR ("value", bson_iter_utf8 (&iter, NULL));
}