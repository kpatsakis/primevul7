    uint32_t PgfImage::readPgfHeaderSize(BasicIo& iIo)
    {
        DataBuf buffer(4);
        size_t bufRead = iIo.read(buffer.pData_, buffer.size_);
        if (iIo.error()) throw Error(kerFailedToReadImageData);
        if (bufRead != buffer.size_) throw Error(kerInputDataReadFailed);

        int headerSize = (int) byteSwap_(buffer,0,bSwap_);
        if (headerSize <= 0 ) throw Error(kerNoImageInInputData);

#ifdef DEBUG
        std::cout << "Exiv2::PgfImage: PGF header size : " << headerSize << " bytes\n";
#endif

        return headerSize;
    } // PgfImage::readPgfHeaderSize