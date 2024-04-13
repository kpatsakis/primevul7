void checkServerNegotiation(const BSONObj& input, const std::vector<std::string>& expected) {
    auto registry = buildRegistry();
    MessageCompressorManager manager(&registry);

    BSONObjBuilder serverOutput;
    manager.serverNegotiate(input, &serverOutput);
    checkNegotiationResult(serverOutput.done(), expected);
}