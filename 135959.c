TEST(MessageCompressorManager, NormalCompressionRequested) {
    auto input = BSON("isMaster" << 1 << "compression" << BSON_ARRAY("noop"));
    checkServerNegotiation(input, {"noop"});
}