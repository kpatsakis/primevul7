bson_iter_init (bson_iter_t *iter,  /* OUT */
                const bson_t *bson) /* IN */
{
   BSON_ASSERT (iter);
   BSON_ASSERT (bson);

   if (BSON_UNLIKELY (bson->len < 5)) {
      memset (iter, 0, sizeof *iter);
      return false;
   }

   iter->raw = bson_get_data (bson);
   iter->len = bson->len;
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