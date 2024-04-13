void MessageCompressorManager::clientBegin(BSONObjBuilder* output) {
    LOG(3) << "Starting client-side compression negotiation";

    // We're about to update the compressor list with the negotiation result from the server.
    _negotiated.clear();

    auto& compressorList = _registry->getCompressorNames();
    if (compressorList.size() == 0)
        return;

    BSONArrayBuilder sub(output->subarrayStart("compression"));
    for (const auto e : _registry->getCompressorNames()) {
        LOG(3) << "Offering " << e << " compressor to server";
        sub.append(e);
    }
    sub.doneFast();
}