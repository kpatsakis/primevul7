Message buildMessage() {
    const auto data = std::string{"Hello, world!"};
    const auto bufferSize = MsgData::MsgDataHeaderSize + data.size();
    auto buf = SharedBuffer::allocate(bufferSize);
    MsgData::View testView(buf.get());
    testView.setId(123456);
    testView.setResponseToMsgId(654321);
    testView.setOperation(dbQuery);
    testView.setLen(bufferSize);
    memcpy(testView.data(), data.data(), data.size());
    return Message{buf};
}