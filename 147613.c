bson_iter_overwrite_double (bson_iter_t *iter, /* IN */
                            double value)      /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_DOUBLE) {
      value = BSON_DOUBLE_TO_LE (value);
      memcpy ((void *) (iter->raw + iter->d1), &value, sizeof (value));
   }
}