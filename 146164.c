    DataBuf LoaderExifJpeg::getData() const
    {
        if (!valid()) return DataBuf();
        BasicIo &io = image_.io();

        if (io.open() != 0) {
            throw Error(kerDataSourceOpenFailed, io.path(), strError());
        }
        IoCloser closer(io);

        const Exiv2::byte* base = io.mmap();

        return DataBuf(base + offset_, size_);
    }