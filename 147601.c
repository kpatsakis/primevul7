bson_iter_utf8 (const bson_iter_t *iter, /* IN */
                uint32_t *length)        /* OUT */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_UTF8) {
      if (length) {
         *length = bson_iter_utf8_len_unsafe (iter);
      }

      return (const char *) (iter->raw + iter->d2);
   }

   if (length) {
      *length = 0;
   }

   return NULL;
}