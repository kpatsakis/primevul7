        Header readHeader(BasicIo& io)
        {
            byte header[2] = {0, 0};
            io.read(header, 2);

            ByteOrder byteOrder = invalidByteOrder;
            if (header[0] == 'I' && header[1] == 'I')
                byteOrder = littleEndian;
            else if (header[0] == 'M' && header[1] == 'M')
                byteOrder = bigEndian;

            if (byteOrder == invalidByteOrder)
                return Header();

            byte version[2] = {0, 0};
            io.read(version, 2);

            const uint16_t magic = getUShort(version, byteOrder);

            if (magic != 0x2A && magic != 0x2B)
                return Header();

            Header result;

            if (magic == 0x2A)
            {
                byte buffer[4];
                const size_t read = io.read(buffer, 4);

                if (read < 4)
                    throw Exiv2::Error(kerCorruptedMetadata);

                const uint32_t offset = getULong(buffer, byteOrder);
                result = Header(byteOrder, magic, 4, offset);
            }
            else
            {
                byte buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                size_t read = io.read(buffer, 2);
                if (read < 2)
                    throw Exiv2::Error(kerCorruptedMetadata);

                const int size = getUShort(buffer, byteOrder);

                if (size == 8)
                {
                    read = io.read(buffer, 2); // null
                    if (read < 2)
                        throw Exiv2::Error(kerCorruptedMetadata);

                    read = io.read(buffer, 8);
                    if (read < 8)
                        throw Exiv2::Error(kerCorruptedMetadata);

                    const uint64_t offset = getULongLong(buffer, byteOrder);

                    if (offset >= io.size())
                        throw Exiv2::Error(kerCorruptedMetadata);

                    result = Header(byteOrder, magic, size, offset);
                }
                else
                    throw Exiv2::Error(kerCorruptedMetadata);
            }

            return result;
        }