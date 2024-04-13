bson_iter_decimal128 (const bson_iter_t *iter, /* IN */
                      bson_decimal128_t *dec)  /* OUT */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DECIMAL128) {
      bson_iter_decimal128_unsafe (iter, dec);
      return true;
   }

   return false;
}