bson_iter_overwrite_decimal128 (bson_iter_t *iter,        /* IN */
                                bson_decimal128_t *value) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DECIMAL128) {
#if BSON_BYTE_ORDER != BSON_LITTLE_ENDIAN
      uint64_t data[2];
      data[0] = BSON_UINT64_TO_LE (value->low);
      data[1] = BSON_UINT64_TO_LE (value->high);
      memcpy ((void *) (iter->raw + iter->d1), data, sizeof (data));
#else
      memcpy ((void *) (iter->raw + iter->d1), value, sizeof (*value));
#endif
   }
}