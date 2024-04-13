bson_iter_as_int64 (const bson_iter_t *iter) /* IN */
{
   BSON_ASSERT (iter);

   switch ((int) ITER_TYPE (iter)) {
   case BSON_TYPE_BOOL:
      return (int64_t) bson_iter_bool (iter);
   case BSON_TYPE_DOUBLE:
      return (int64_t) bson_iter_double (iter);
   case BSON_TYPE_INT64:
      return bson_iter_int64 (iter);
   case BSON_TYPE_INT32:
      return (int64_t) bson_iter_int32 (iter);
   default:
      return 0;
   }
}