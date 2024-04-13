    void IptcData::printStructure(std::ostream& out, const Slice<byte*>& bytes, uint32_t depth)
    {
        uint32_t i = 0;
        while (i < bytes.size() - 3 && bytes.at(i) != 0x1c)
            i++;
        depth++;
        out << Internal::indent(depth) << "Record | DataSet | Name                     | Length | Data" << std::endl;
        while (i < bytes.size() - 3) {
            if (bytes.at(i) != 0x1c) {
                break;
            }
            char buff[100];
            uint16_t record = bytes.at(i + 1);
            uint16_t dataset = bytes.at(i + 2);
            uint16_t len = getUShort(bytes.subSlice(i + 3, bytes.size()), bigEndian);
            sprintf(buff, "  %6d | %7d | %-24s | %6d | ", record, dataset,
                    Exiv2::IptcDataSets::dataSetName(dataset, record).c_str(), len);

            out << buff << Internal::binaryToString(makeSlice(bytes, i + 5, i + 5 + (len > 40 ? 40 : len)))
                << (len > 40 ? "..." : "")
                << std::endl;
            i += 5 + len;
        }
        depth--;
    }