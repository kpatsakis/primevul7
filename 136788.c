static int pharobj_cancompress(HashTable *manifest) /* {{{ */
{
	int test;

	test = 1;
	zend_hash_apply_with_argument(manifest, phar_test_compression, &test);
	return test;
}