bson_iter_regex (const bson_iter_t *iter, /* IN */
                 const char **options)    /* IN */
{
   const char *ret = NULL;
   const char *ret_options = NULL;

   BSON_ASSERT (iter);

   if (ITER_TYPE (iter) == BSON_TYPE_REGEX) {
      ret = (const char *) (iter->raw + iter->d1);
      ret_options = (const char *) (iter->raw + iter->d2);
   }

   if (options) {
      *options = ret_options;
   }

   return ret;
}