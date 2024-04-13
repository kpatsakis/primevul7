BSON_ASSERT_BSON_EQUAL_FILE (const bson_t *b, const char *filename)
{
   bson_t *b2 = get_bson (filename);
   BSON_ASSERT_BSON_EQUAL (b, b2);
   bson_destroy (b2);
}