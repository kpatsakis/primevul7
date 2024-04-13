                BigTiffImage(BasicIo::UniquePtr io):
                    Image(ImageType::bigtiff, mdExif, std::move(io)),
                    header_(readHeader(Image::io())),
                    dataSize_(0),
                    doSwap_(false)
                {
                    assert(header_.isValid());

                    doSwap_ =  (isLittleEndianPlatform() && header_.byteOrder() == bigEndian)
                          ||   (isBigEndianPlatform()    && header_.byteOrder() == littleEndian);

                    dataSize_ = header_.format() == Header::StandardTiff? 4 : 8;
                }