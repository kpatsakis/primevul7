    DataBuf IptcParser::encode(const IptcData& iptcData)
    {
        DataBuf buf(iptcData.size());
        byte *pWrite = buf.pData_;

        // Copy the iptc data sets and sort them by record but preserve the order of datasets
        IptcMetadata sortedIptcData;
        std::copy(iptcData.begin(), iptcData.end(), std::back_inserter(sortedIptcData));
        std::stable_sort(sortedIptcData.begin(), sortedIptcData.end(), cmpIptcdataByRecord);

        IptcData::const_iterator iter = sortedIptcData.begin();
        IptcData::const_iterator end = sortedIptcData.end();
        for ( ; iter != end; ++iter) {
            // marker, record Id, dataset num
            *pWrite++ = marker_;
            *pWrite++ = static_cast<byte>(iter->record());
            *pWrite++ = static_cast<byte>(iter->tag());

            // extended or standard dataset?
            size_t dataSize = iter->size();
            if (dataSize > 32767) {
                // always use 4 bytes for extended length
                uint16_t sizeOfSize = 4 | 0x8000;
                us2Data(pWrite, sizeOfSize, bigEndian);
                pWrite += 2;
                ul2Data(pWrite, static_cast<uint32_t>(dataSize), bigEndian);
                pWrite += 4;
            }
            else {
                us2Data(pWrite, static_cast<uint16_t>(dataSize), bigEndian);
                pWrite += 2;
            }
            pWrite += iter->value().copy(pWrite, bigEndian);
        }

        return buf;
    } // IptcParser::encode