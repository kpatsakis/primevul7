    uint16_t Image::byteSwap2(const DataBuf& buf,size_t offset,bool bSwap) const
    {
        uint16_t v;
        char*    p = (char*) &v;
        p[0] = buf.pData_[offset];
        p[1] = buf.pData_[offset+1];
        return Image::byteSwap(v,bSwap);
    }