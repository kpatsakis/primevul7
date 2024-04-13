bson_iter_next (bson_iter_t *iter) /* INOUT */
{
   uint32_t bson_type;
   const char *key;
   bool unsupported;

   return _bson_iter_next_internal (iter, &key, &bson_type, &unsupported);
}