    uint64_t Image::byteSwap8(const DataBuf& buf,size_t offset,bool bSwap) const
    {
        uint64_t v;
        byte*    p = reinterpret_cast<byte *>(&v);

        for(int i = 0; i < 8; i++)
            p[i] = buf.pData_[offset + i];

        return Image::byteSwap(v,bSwap);
    }