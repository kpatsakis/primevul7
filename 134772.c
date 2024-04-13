test_bson_append_general (void)
{
   uint8_t bytes[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0x23, 0x45};
   bson_oid_t oid;
   bson_t *bson;
   bson_t *array;
   bson_t *subdoc;

   bson = bson_new ();
   BSON_ASSERT (bson_append_int32 (bson, "int", -1, 1));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test1.bson");
   bson_destroy (bson);

   bson = bson_new ();
   BSON_ASSERT (bson_append_int64 (bson, "int64", -1, 1));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test2.bson");
   bson_destroy (bson);

   bson = bson_new ();
   BSON_ASSERT (bson_append_double (bson, "double", -1, 1.123));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test3.bson");
   bson_destroy (bson);

   bson = bson_new ();
   BSON_ASSERT (bson_append_utf8 (bson, "string", -1, "some string", -1));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test5.bson");
   bson_destroy (bson);

   bson = bson_new ();
   array = bson_new ();
   BSON_ASSERT (bson_append_int32 (array, "0", -1, 1));
   BSON_ASSERT (bson_append_int32 (array, "1", -1, 2));
   BSON_ASSERT (bson_append_int32 (array, "2", -1, 3));
   BSON_ASSERT (bson_append_int32 (array, "3", -1, 4));
   BSON_ASSERT (bson_append_int32 (array, "4", -1, 5));
   BSON_ASSERT (bson_append_int32 (array, "5", -1, 6));
   BSON_ASSERT (bson_append_array (bson, "array[int]", -1, array));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test6.bson");
   bson_destroy (array);
   bson_destroy (bson);

   bson = bson_new ();
   array = bson_new ();
   BSON_ASSERT (bson_append_double (array, "0", -1, 1.123));
   BSON_ASSERT (bson_append_double (array, "1", -1, 2.123));
   BSON_ASSERT (bson_append_array (bson, "array[double]", -1, array));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test7.bson");
   bson_destroy (array);
   bson_destroy (bson);

   bson = bson_new ();
   subdoc = bson_new ();
   BSON_ASSERT (bson_append_int32 (subdoc, "int", -1, 1));
   BSON_ASSERT (bson_append_document (bson, "document", -1, subdoc));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test8.bson");
   bson_destroy (subdoc);
   bson_destroy (bson);

   bson = bson_new ();
   BSON_ASSERT (bson_append_null (bson, "null", -1));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test9.bson");
   bson_destroy (bson);

   bson = bson_new ();
   BSON_ASSERT (bson_append_regex (bson, "regex", -1, "1234", "i"));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test10.bson");
   bson_destroy (bson);

   bson = bson_new ();
   BSON_ASSERT (bson_append_utf8 (bson, "hello", -1, "world", -1));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test11.bson");
   bson_destroy (bson);

   bson = bson_new ();
   array = bson_new ();
   BSON_ASSERT (bson_append_utf8 (array, "0", -1, "awesome", -1));
   BSON_ASSERT (bson_append_double (array, "1", -1, 5.05));
   BSON_ASSERT (bson_append_int32 (array, "2", -1, 1986));
   BSON_ASSERT (bson_append_array (bson, "BSON", -1, array));
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test12.bson");
   bson_destroy (bson);
   bson_destroy (array);

   bson = bson_new ();
   memcpy (&oid, bytes, sizeof oid);
   BSON_ASSERT (bson_append_oid (bson, "_id", -1, &oid));
   subdoc = bson_new ();
   BSON_ASSERT (bson_append_oid (subdoc, "_id", -1, &oid));
   array = bson_new ();
   BSON_ASSERT (bson_append_utf8 (array, "0", -1, "1", -1));
   BSON_ASSERT (bson_append_utf8 (array, "1", -1, "2", -1));
   BSON_ASSERT (bson_append_utf8 (array, "2", -1, "3", -1));
   BSON_ASSERT (bson_append_utf8 (array, "3", -1, "4", -1));
   BSON_ASSERT (bson_append_array (subdoc, "tags", -1, array));
   bson_destroy (array);
   BSON_ASSERT (bson_append_utf8 (subdoc, "text", -1, "asdfanother", -1));
   array = bson_new ();
   BSON_ASSERT (bson_append_utf8 (array, "name", -1, "blah", -1));
   BSON_ASSERT (bson_append_document (subdoc, "source", -1, array));
   bson_destroy (array);
   BSON_ASSERT (bson_append_document (bson, "document", -1, subdoc));
   bson_destroy (subdoc);
   array = bson_new ();
   BSON_ASSERT (bson_append_utf8 (array, "0", -1, "source", -1));
   BSON_ASSERT (bson_append_array (bson, "type", -1, array));
   bson_destroy (array);
   array = bson_new ();
   BSON_ASSERT (bson_append_utf8 (array, "0", -1, "server_created_at", -1));
   BSON_ASSERT (bson_append_array (bson, "missing", -1, array));
   bson_destroy (array);
   BSON_ASSERT_BSON_EQUAL_FILE (bson, "test17.bson");
   bson_destroy (bson);
}