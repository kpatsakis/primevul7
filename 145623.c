    static void checkDataBufBounds(const DataBuf& buf, size_t end) {
        enforce<std::invalid_argument>(end <= static_cast<size_t>(std::numeric_limits<long>::max()),
                                       "end of slice too large to be compared with DataBuf bounds.");
        enforce<std::out_of_range>(end <= buf.size_, "Invalid slice bounds specified");
    }