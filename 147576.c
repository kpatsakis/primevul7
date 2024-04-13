bson_iter_dup_utf8 (const bson_iter_t *iter, /* IN */
                    uint32_t *length)        /* OUT */
{
   uint32_t local_length = 0;
   const char *str;
   char *ret = NULL;

   BSON_ASSERT (iter);

   if ((str = bson_iter_utf8 (iter, &local_length))) {
      ret = bson_malloc0 (local_length + 1);
      memcpy (ret, str, local_length);
      ret[local_length] = '\0';
   }

   if (length) {
      *length = local_length;
   }

   return ret;
}