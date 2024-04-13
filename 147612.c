bson_iter_symbol (const bson_iter_t *iter, /* IN */
                  uint32_t *length)        /* OUT */
{
   const char *ret = NULL;
   uint32_t ret_length = 0;

   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_SYMBOL) {
      ret = (const char *) (iter->raw + iter->d2);
      ret_length = bson_iter_utf8_len_unsafe (iter);
   }

   if (length) {
      *length = ret_length;
   }

   return ret;
}