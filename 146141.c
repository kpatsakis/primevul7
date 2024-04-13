int main(int argc, char* const argv[])
{
try {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " filein fileout1 fileout2\n";
        std::cout << "fileouts are overwritten and should match filein exactly\n";
        return 1;
    }

    FileIo fileIn(argv[1]);
    if (fileIn.open() != 0) {
        throw Error(Exiv2::kerDataSourceOpenFailed, fileIn.path(), strError());
    }

    FileIo fileOut1(argv[2]);
    if (fileOut1.open("w+b") != 0) {
        throw Error(Exiv2::kerFileOpenFailed, argv[2], "w+b", strError());
    }

    MemIo memIo1;

    // Copy to output file through memIo
    memIo1.write(fileIn);
    memIo1.seek(0, BasicIo::beg);
    fileOut1.write(memIo1);

    // Make sure they are all the same size
    if(fileIn.size() != memIo1.size() || memIo1.size() != fileOut1.size()) {
        std::cerr << argv[0] <<
            ": Sizes do not match\n";
        return 1;
    }

    // Read writereadseek test on MemIo
    MemIo memIo2;
    int rc = WriteReadSeek(memIo2);
    if (rc != 0) return rc;

    // Read writereadseek test on FileIo
    // Create or overwrite the file, then close it
    FileIo fileTest("iotest.txt");
    if (fileTest.open("w+b") != 0) {
        throw Error(Exiv2::kerFileOpenFailed, "iotest.txt", "w+b", strError());
    }

    fileTest.close();
    rc = WriteReadSeek(fileTest);
    if (rc != 0) return rc;

    // Another test of reading and writing
    fileOut1.seek(0, BasicIo::beg);
    memIo2.seek(0, BasicIo::beg);
    FileIo fileOut2(argv[3]);
    if (fileOut2.open("w+b") != 0) {
        throw Error(Exiv2::kerFileOpenFailed, argv[3], "w+b", strError());
    }

    size_t readCount = 0;
    byte buf[32];
    while ((readCount=fileOut1.read(buf, sizeof(buf)))) {
        if (memIo2.write(buf, readCount) != readCount) {
            std::cerr << argv[0] <<
                ": MemIo bad write 2\n";
            return 13;
        }
        if (fileOut2.write(buf, readCount) != readCount) {
            std::cerr << argv[0] <<
                ": FileIo bad write 2\n";
            return 14;
        }
    }

    return 0;
}
catch (Exiv2::AnyError& e) {
    std::cerr << "Caught Exiv2 exception '" << e << "'\n";
    return 20;
}
}