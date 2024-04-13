                uint64_t readData(int size) const
                {
                    const DataBuf data = Image::io().read(size);
                    enforce(data.size_ != 0, kerCorruptedMetadata);

                    uint64_t result = 0;

                    if (data.size_ == 1)
                        {}
                    else if (data.size_ == 2)
                        result = byteSwap2(data, 0, doSwap_);
                    else if (data.size_ == 4)
                        result = byteSwap4(data, 0, doSwap_);
                    else if (data.size_ == 8)
                        result = byteSwap8(data, 0, doSwap_);
                    else
                        throw Exiv2::Error(kerCorruptedMetadata);

                    return result;
                }