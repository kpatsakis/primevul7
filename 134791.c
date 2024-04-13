test_bson_visit_unsupported_type_bad_key (void)
{
   /* key is invalid utf-8 char, '\x80' */
   const char data[] = "\x0c\x00\x00\x00\x33\x80\x00\x01\x00\x00\x00\x00";
   bson_t b;
   bson_iter_t iter;
   unsupported_type_test_data_t context = {0};
   bson_visitor_t visitor = {0};

   visitor.visit_unsupported_type = visit_unsupported_type;

   BSON_ASSERT (bson_init_static (&b, (const uint8_t *) data, sizeof data - 1));
   BSON_ASSERT (bson_iter_init (&iter, &b));
   bson_iter_visit_all (&iter, &visitor, (void *) &context);
   BSON_ASSERT (!bson_iter_next (&iter));

   /* unsupported type error wasn't reported, because the bson is corrupt */
   BSON_ASSERT (!context.visited);
}