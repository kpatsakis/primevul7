        void PngChunk::decodeIHDRChunk(const DataBuf& data, PngImageHeader& h)
        {
            enforce(data.size_ == 13, kerCorruptedMetadata);

            h.width = getLong((const byte*)data.pData_, bigEndian);
            h.height = getLong((const byte*)data.pData_ + 4, bigEndian);
            h.bitDepth = data.pData_[8];
            h.colorType = data.pData_[9];
            h.compressionMethod = data.pData_[10];
            h.filterMethod = data.pData_[11];
            h.interlaceMethod = data.pData_[12];

            enforce(h.colorType == 0 || h.colorType == 2 || h.colorType == 3 || h.colorType == 4 || h.colorType == 6,
                    kerCorruptedMetadata);
            switch (h.colorType) {
                case 0:
                    enforce(
                        h.bitDepth == 1 || h.bitDepth == 2 || h.bitDepth == 4 || h.bitDepth == 8 || h.bitDepth == 16,
                        kerCorruptedMetadata);
                    break;
                case 2:
                    enforce(h.bitDepth == 8 || h.bitDepth == 16, kerCorruptedMetadata);
                    break;
                case 3:
                    enforce(h.bitDepth == 1 || h.bitDepth == 2 || h.bitDepth == 4 || h.bitDepth == 8,
                            kerCorruptedMetadata);
                    break;
                case 4:
                    enforce(h.bitDepth == 8 || h.bitDepth == 16, kerCorruptedMetadata);
                    break;
                case 6:
                    enforce(h.bitDepth == 8 || h.bitDepth == 16, kerCorruptedMetadata);
                    break;
            }
            enforce(h.compressionMethod == 0, kerCorruptedMetadata);
            enforce(h.filterMethod == 0, kerCorruptedMetadata);
            enforce(h.interlaceMethod == 0 || h.interlaceMethod == 1, kerCorruptedMetadata);
        }