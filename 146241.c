    DataBuf PgfImage::readPgfHeaderStructure(BasicIo& iIo, int& width, int& height)
    {
        DataBuf header(16);
        size_t bufRead = iIo.read(header.pData_, header.size_);
        if (iIo.error()) throw Error(kerFailedToReadImageData);
        if (bufRead != header.size_) throw Error(kerInputDataReadFailed);

        DataBuf work(8);  // don't disturb the binary data - doWriteMetadata reuses it
        memcpy (work.pData_,header.pData_,8);
        width   = byteSwap_(work,0,bSwap_);
        height  = byteSwap_(work,4,bSwap_);

        /* NOTE: properties not yet used
        byte nLevels  = buffer.pData_[8];
        byte quality  = buffer.pData_[9];
        byte bpp      = buffer.pData_[10];
        byte channels = buffer.pData_[11];
        */
        byte mode     = header.pData_[12];

        if (mode == 2)  // Indexed color image. We pass color table (256 * 3 bytes).
        {
            header.alloc(16 + 256*3);

            bufRead = iIo.read(&header.pData_[16], 256*3);
            if (iIo.error()) throw Error(kerFailedToReadImageData);
            if (bufRead != 256*3) throw Error(kerInputDataReadFailed);
        }

        return header;
    } // PgfImage::readPgfHeaderStructure