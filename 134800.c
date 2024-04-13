test_bson_visit_unsupported_type (void)
{
   /* {k: 1}, but instead of BSON type 0x10 (int32), use unknown type 0x33 */
   const char data[] = "\x0c\x00\x00\x00\x33k\x00\x01\x00\x00\x00\x00";
   bson_t b;
   bson_iter_t iter;
   unsupported_type_test_data_t context = {0};
   bson_visitor_t visitor = {0};

   visitor.visit_unsupported_type = visit_unsupported_type;

   BSON_ASSERT (bson_init_static (&b, (const uint8_t *) data, sizeof data - 1));
   BSON_ASSERT (bson_iter_init (&iter, &b));
   bson_iter_visit_all (&iter, &visitor, (void *) &context);
   BSON_ASSERT (!bson_iter_next (&iter));
   BSON_ASSERT (context.visited);
   BSON_ASSERT (!strcmp (context.key, "k"));
   BSON_ASSERT (context.type_code == '\x33');
}