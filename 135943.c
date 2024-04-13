TEST(MessageCompressorManager, BadCompressionRequested) {
    auto input = BSON("isMaster" << 1 << "compression" << BSON_ARRAY("fakecompressor"));
    checkServerNegotiation(input, {});
}