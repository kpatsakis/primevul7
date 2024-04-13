bson_iter_overwrite_bool (bson_iter_t *iter, /* IN */
                          bool value)        /* IN */
{
   BSON_ASSERT (iter);
   value = !!value;

   if (ITER_TYPE (iter) == BSON_TYPE_BOOL) {
      memcpy ((void *) (iter->raw + iter->d1), &value, 1);
   }
}