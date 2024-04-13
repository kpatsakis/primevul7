visit_corrupt (const bson_iter_t *iter, void *data)
{
   *((bool *) data) = true;
}