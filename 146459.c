    uint32_t Image::byteSwap4(const DataBuf& buf,size_t offset,bool bSwap) const
    {
        uint32_t v;
        char*    p = (char*) &v;
        p[0] = buf.pData_[offset];
        p[1] = buf.pData_[offset+1];
        p[2] = buf.pData_[offset+2];
        p[3] = buf.pData_[offset+3];
        return Image::byteSwap(v,bSwap);
    }