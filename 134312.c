TORRENT_TEST(corrupt)
{
	std::vector<char> zipped;
	error_code ec;
	load_file(combine_path("..", "corrupt.gz"), zipped, ec, 1000000);
	if (ec) fprintf(stderr, "failed to open file: (%d) %s\n", ec.value()
		, ec.message().c_str());
	TEST_CHECK(!ec);

	std::vector<char> inflated;
	inflate_gzip(&zipped[0], zipped.size(), inflated, 1000000, ec);

	// we expect this to fail
	TEST_CHECK(ec);
}