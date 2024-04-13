bson_iter_key (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   return bson_iter_key_unsafe (iter);
}