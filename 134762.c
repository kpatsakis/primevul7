test_bson_install (TestSuite *suite)
{
   TestSuite_Add (suite, "/bson/new", test_bson_new);
   TestSuite_Add (suite, "/bson/new_from_buffer", test_bson_new_from_buffer);
   TestSuite_Add (suite, "/bson/init", test_bson_init);
   TestSuite_Add (suite, "/bson/init_static", test_bson_init_static);
   TestSuite_Add (suite, "/bson/basic", test_bson_alloc);
   TestSuite_Add (suite, "/bson/append_overflow", test_bson_append_overflow);
   TestSuite_Add (suite, "/bson/append_array", test_bson_append_array);
   TestSuite_Add (suite, "/bson/append_binary", test_bson_append_binary);
   TestSuite_Add (suite,
                  "/bson/append_binary_deprecated",
                  test_bson_append_binary_deprecated);
   TestSuite_Add (suite, "/bson/append_bool", test_bson_append_bool);
   TestSuite_Add (suite, "/bson/append_code", test_bson_append_code);
   TestSuite_Add (
      suite, "/bson/append_code_with_scope", test_bson_append_code_with_scope);
   TestSuite_Add (suite, "/bson/append_dbpointer", test_bson_append_dbpointer);
   TestSuite_Add (suite, "/bson/append_document", test_bson_append_document);
   TestSuite_Add (suite, "/bson/append_double", test_bson_append_double);
   TestSuite_Add (suite, "/bson/append_int32", test_bson_append_int32);
   TestSuite_Add (suite, "/bson/append_int64", test_bson_append_int64);
   TestSuite_Add (
      suite, "/bson/append_decimal128", test_bson_append_decimal128);
   TestSuite_Add (suite, "/bson/append_iter", test_bson_append_iter);
   TestSuite_Add (suite, "/bson/append_maxkey", test_bson_append_maxkey);
   TestSuite_Add (suite, "/bson/append_minkey", test_bson_append_minkey);
   TestSuite_Add (suite, "/bson/append_null", test_bson_append_null);
   TestSuite_Add (suite, "/bson/append_oid", test_bson_append_oid);
   TestSuite_Add (suite, "/bson/append_regex", test_bson_append_regex);
   TestSuite_Add (suite, "/bson/append_utf8", test_bson_append_utf8);
   TestSuite_Add (suite, "/bson/append_symbol", test_bson_append_symbol);
   TestSuite_Add (suite, "/bson/append_time_t", test_bson_append_time_t);
   TestSuite_Add (suite, "/bson/append_timestamp", test_bson_append_timestamp);
   TestSuite_Add (suite, "/bson/append_timeval", test_bson_append_timeval);
   TestSuite_Add (suite, "/bson/append_undefined", test_bson_append_undefined);
   TestSuite_Add (suite, "/bson/append_general", test_bson_append_general);
   TestSuite_Add (suite, "/bson/append_deep", test_bson_append_deep);
   TestSuite_Add (suite, "/bson/utf8_key", test_bson_utf8_key);
   TestSuite_Add (suite, "/bson/validate", test_bson_validate);
   TestSuite_Add (suite, "/bson/validate/dbref", test_bson_validate_dbref);
   TestSuite_Add (suite, "/bson/validate/bool", test_bson_validate_bool);
   TestSuite_Add (
      suite, "/bson/validate/dbpointer", test_bson_validate_dbpointer);
   TestSuite_Add (suite, "/bson/new_1mm", test_bson_new_1mm);
   TestSuite_Add (suite, "/bson/init_1mm", test_bson_init_1mm);
   TestSuite_Add (suite, "/bson/build_child", test_bson_build_child);
   TestSuite_Add (suite, "/bson/build_child_deep", test_bson_build_child_deep);
   TestSuite_Add (suite,
                  "/bson/build_child_deep_no_begin_end",
                  test_bson_build_child_deep_no_begin_end);
   TestSuite_Add (
      suite, "/bson/build_child_array", test_bson_build_child_array);
   TestSuite_Add (suite, "/bson/count", test_bson_count_keys);
   TestSuite_Add (suite, "/bson/copy", test_bson_copy);
   TestSuite_Add (suite, "/bson/copy_to", test_bson_copy_to);
   TestSuite_Add (suite,
                  "/bson/copy_to_excluding_noinit",
                  test_bson_copy_to_excluding_noinit);
   TestSuite_Add (suite, "/bson/initializer", test_bson_initializer);
   TestSuite_Add (suite, "/bson/concat", test_bson_concat);
   TestSuite_Add (suite, "/bson/reinit", test_bson_reinit);
   TestSuite_Add (suite, "/bson/macros", test_bson_macros);
   TestSuite_Add (suite, "/bson/clear", test_bson_clear);
   TestSuite_Add (suite, "/bson/steal", test_bson_steal);
   TestSuite_Add (suite, "/bson/reserve_buffer", test_bson_reserve_buffer);
   TestSuite_Add (
      suite, "/bson/reserve_buffer/errors", test_bson_reserve_buffer_errors);
   TestSuite_Add (
      suite, "/bson/destroy_with_steal", test_bson_destroy_with_steal);
   TestSuite_Add (suite, "/bson/has_field", test_bson_has_field);
   TestSuite_Add (
      suite, "/bson/visit_invalid_field", test_bson_visit_invalid_field);
   TestSuite_Add (
      suite, "/bson/unsupported_type", test_bson_visit_unsupported_type);
   TestSuite_Add (suite,
                  "/bson/unsupported_type/bad_key",
                  test_bson_visit_unsupported_type_bad_key);
   TestSuite_Add (suite,
                  "/bson/unsupported_type/empty_key",
                  test_bson_visit_unsupported_type_empty_key);
   TestSuite_Add (suite, "/bson/binary_subtype_2", test_bson_subtype_2);
   TestSuite_Add (suite, "/util/next_power_of_two", test_next_power_of_two);
}