    void PgfImage::doWriteMetadata(BasicIo& outIo)
    {
        if (!io_->isopen()) throw Error(kerInputDataReadFailed);
        if (!outIo.isopen()) throw Error(kerImageWriteFailed);

#ifdef DEBUG
        std::cout << "Exiv2::PgfImage::doWriteMetadata: Writing PGF file " << io_->path() << "\n";
        std::cout << "Exiv2::PgfImage::doWriteMetadata: tmp file created " << outIo.path() << "\n";
#endif

        // Ensure that this is the correct image type
        if (!isPgfType(*io_, true))
        {
            if (io_->error() || io_->eof()) throw Error(kerInputDataReadFailed);
            throw Error(kerNoImageInInputData);
        }

        // Ensure PGF version.
        byte mnb            = readPgfMagicNumber(*io_);

        readPgfHeaderSize(*io_);

        int w, h;
        DataBuf header      = readPgfHeaderStructure(*io_, w, h);

        Image::UniquePtr img  = ImageFactory::create(ImageType::png);

        img->setExifData(exifData_);
        img->setIptcData(iptcData_);
        img->setXmpData(xmpData_);
        img->writeMetadata();
        size_t imgSize = img->io().size();
        DataBuf imgBuf = img->io().read((long)imgSize);

#ifdef DEBUG
        std::cout << "Exiv2::PgfImage::doWriteMetadata: Creating image to host metadata (" << imgSize << " bytes)\n";
#endif

        //---------------------------------------------------------------

        // Write PGF Signature.
        if (outIo.write(pgfSignature, 3) != 3) throw Error(kerImageWriteFailed);

        // Write Magic number.
        if (outIo.putb(mnb) == EOF) throw Error(kerImageWriteFailed);

        // Write new Header size.
        size_t newHeaderSize = header.size_ + imgSize;
        DataBuf buffer(4);
        memcpy (buffer.pData_, &newHeaderSize, 4);
        byteSwap_(buffer,0,bSwap_);
        if (outIo.write(buffer.pData_, 4) != 4) throw Error(kerImageWriteFailed);

#ifdef DEBUG
        std::cout << "Exiv2::PgfImage: new PGF header size : " << newHeaderSize << " bytes\n";

        printf("%x\n", buffer.pData_[0]);
        printf("%x\n", buffer.pData_[1]);
        printf("%x\n", buffer.pData_[2]);
        printf("%x\n", buffer.pData_[3]);
#endif

        // Write Header data.
        if (outIo.write(header.pData_, header.size_) != header.size_) throw Error(kerImageWriteFailed);

        // Write new metadata byte array.
        if (outIo.write(imgBuf.pData_, imgBuf.size_) != imgBuf.size_) throw Error(kerImageWriteFailed);

        // Copy the rest of PGF image data.

        DataBuf buf(4096);
        size_t readSize = 0;
        while ((readSize=io_->read(buf.pData_, buf.size_)))
        {
            if (outIo.write(buf.pData_, readSize) != readSize) throw Error(kerImageWriteFailed);
        }
        if (outIo.error()) throw Error(kerImageWriteFailed);

    } // PgfImage::doWriteMetadata