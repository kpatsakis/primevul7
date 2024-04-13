bson_iter_int32 (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_INT32) {
      return bson_iter_int32_unsafe (iter);
   }

   return 0;
}