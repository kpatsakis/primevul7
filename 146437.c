    long Xmpdatum::copy(byte* /*buf*/, ByteOrder /*byteOrder*/) const
    {
        throw Error(kerFunctionNotSupported, "Xmpdatum::copy");
        return 0;
    }