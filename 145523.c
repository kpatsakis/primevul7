    long DataValue::copy(byte* buf, ByteOrder /*byteOrder*/) const
    {
        // byteOrder not needed
        return static_cast<long>(
            std::copy(value_.begin(), value_.end(), buf) - buf
            );
    }