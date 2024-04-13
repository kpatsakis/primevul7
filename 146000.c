    Slice<const byte*> makeSlice(const DataBuf& buf, size_t begin, size_t end)
    {
        checkDataBufBounds(buf, end);
        return Slice<const byte*>(buf.pData_, begin, end);
    }