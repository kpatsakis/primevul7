bson_iter_init_from_data (bson_iter_t *iter,   /* OUT */
                          const uint8_t *data, /* IN */
                          size_t length)       /* IN */
{
   uint32_t len_le;

   BSON_ASSERT (iter);
   BSON_ASSERT (data);

   if (BSON_UNLIKELY ((length < 5) || (length > INT_MAX))) {
      memset (iter, 0, sizeof *iter);
      return false;
   }

   memcpy (&len_le, data, sizeof (len_le));

   if (BSON_UNLIKELY ((size_t) BSON_UINT32_FROM_LE (len_le) != length)) {
      memset (iter, 0, sizeof *iter);
      return false;
   }

   if (BSON_UNLIKELY (data[length - 1])) {
      memset (iter, 0, sizeof *iter);
      return false;
   }

   iter->raw = (uint8_t *) data;
   iter->len = length;
   iter->off = 0;
   iter->type = 0;
   iter->key = 0;
   iter->d1 = 0;
   iter->d2 = 0;
   iter->d3 = 0;
   iter->d4 = 0;
   iter->next_off = 4;
   iter->err_off = 0;

   return true;
}