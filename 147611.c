bson_iter_find_case (bson_iter_t *iter, /* INOUT */
                     const char *key)   /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (key);

   while (bson_iter_next (iter)) {
      if (!bson_strcasecmp (key, bson_iter_key (iter))) {
         return true;
      }
   }

   return false;
}