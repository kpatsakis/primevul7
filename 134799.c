visit_unsupported_type (const bson_iter_t *iter,
                        const char *key,
                        uint32_t type_code,
                        void *data)
{
   unsupported_type_test_data_t *context;

   context = (unsupported_type_test_data_t *) data;
   context->visited = true;
   context->key = key;
   context->type_code = type_code;
}