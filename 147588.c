_bson_iter_find_with_len (bson_iter_t *iter, /* INOUT */
                          const char *key,   /* IN */
                          int keylen)        /* IN */
{
   const char *ikey;

   if (keylen == 0) {
      return false;
   }

   if (keylen < 0) {
      keylen = (int) strlen (key);
   }

   while (bson_iter_next (iter)) {
      ikey = bson_iter_key (iter);

      if ((0 == strncmp (key, ikey, keylen)) && (ikey[keylen] == '\0')) {
         return true;
      }
   }

   return false;
}