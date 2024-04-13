bson_iter_overwrite_int64 (bson_iter_t *iter, /* IN */
                           int64_t value)     /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_INT64) {
#if BSON_BYTE_ORDER != BSON_LITTLE_ENDIAN
      value = BSON_UINT64_TO_LE (value);
#endif
      memcpy ((void *) (iter->raw + iter->d1), &value, sizeof (value));
   }
}