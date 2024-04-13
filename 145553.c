    Slice<byte*> makeSlice(DataBuf& buf, size_t begin, size_t end)
    {
        checkDataBufBounds(buf, end);
        return Slice<byte*>(buf.pData_, begin, end);
    }