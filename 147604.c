bson_iter_recurse (const bson_iter_t *iter, /* IN */
                   bson_iter_t *child)      /* OUT */
{
   const uint8_t *data = NULL;
   uint32_t len = 0;

   BSON_ASSERT (iter);
   BSON_ASSERT (child);

   if (ITER_TYPE (iter) == BSON_TYPE_DOCUMENT) {
      bson_iter_document (iter, &len, &data);
   } else if (ITER_TYPE (iter) == BSON_TYPE_ARRAY) {
      bson_iter_array (iter, &len, &data);
   } else {
      return false;
   }

   child->raw = data;
   child->len = len;
   child->off = 0;
   child->type = 0;
   child->key = 0;
   child->d1 = 0;
   child->d2 = 0;
   child->d3 = 0;
   child->d4 = 0;
   child->next_off = 4;
   child->err_off = 0;

   return true;
}