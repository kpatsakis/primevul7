bson_iter_find (bson_iter_t *iter, /* INOUT */
                const char *key)   /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (key);

   return _bson_iter_find_with_len (iter, key, -1);
}