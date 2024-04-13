bson_iter_init_find_case (bson_iter_t *iter,  /* INOUT */
                          const bson_t *bson, /* IN */
                          const char *key)    /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (bson);
   BSON_ASSERT (key);

   return bson_iter_init (iter, bson) && bson_iter_find_case (iter, key);
}