bson_iter_type (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (iter->raw);
   BSON_ASSERT (iter->len);

   return bson_iter_type_unsafe (iter);
}