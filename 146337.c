int WriteReadSeek(BasicIo &io)
{
    byte buf[4096];
    const char   tester1[] = "this is a little test of MemIo";
    const char   tester2[] = "Appending this on the end";
    const char   expect[]  = "this is a little teAppending this on the end";
    const size_t insert = 19;
    const size_t size1  = std::strlen(tester1) + 1;
    const size_t size2  = std::strlen(tester2) + 1;

    if (io.open() != 0) {
        throw Error(Exiv2::kerDataSourceOpenFailed, io.path(), strError());
    }
    IoCloser closer(io);
    if ((size_t) io.write((byte*)tester1, (long)size1) != size1) {
        std::cerr << ": WRS initial write failed\n";
        return 2;
    }

    if (io.size() != size1) {
        std::cerr << ": WRS size is not " << size1 << "\n";
        return 2;
    }
    long     backup = (long)size1;
    io.seek(-backup, BasicIo::cur);

    int c = EOF;
    std::memset(buf, -1, sizeof(buf));
    for (int i = 0; (c=io.getb()) != EOF; ++i) {
        buf[i] = (byte)c;
    }

    // Make sure we got the null back
    if(buf[size1-1] != 0) {
        std::cerr << ": WRS missing null terminator 1\n";
        return 3;
    }

    if (strcmp(tester1, (char*)buf) != 0 ) {
        std::cerr << ": WRS strings don't match 1\n";
        return 4;
    }

    io.seek(-2, BasicIo::end);
    if (io.getb() != 'o') {
        std::cerr << ": WRS bad getb o\n";
        return 5;
    }

    io.seek(-2, BasicIo::cur);
    if (io.getb() != 'I') {
        std::cerr << ": WRS bad getb I\n";
        return 6;
    }

    if (io.putb('O') != 'O') {
        std::cerr << ": WRS bad putb\n";
        return 7;
    }

    io.seek(-1, BasicIo::cur);
    if (io.getb() != 'O') {
        std::cerr << ": WRS bad getb O\n";
        return 8;
    }

    io.seek(insert, BasicIo::beg);
    if((size_t)io.write((byte*)tester2, (long)size2) != size2) {
        std::cerr << ": WRS bad write 1\n";
        return 9;
    }

    // open should seek to beginning
    if (io.open() != 0)  {
        throw Error(Exiv2::kerDataSourceOpenFailed, io.path(), strError());
    }
    std::memset(buf, -1, sizeof(buf));
    if ((size_t) io.read(buf, sizeof(buf)) != insert + size2) {
        std::cerr << ": WRS something went wrong\n";
        return 10;
    }

    // Make sure we got the null back
    if(buf[insert + size2 - 1] != 0) {
        std::cerr << ": WRS missing null terminator 2\n";
        return 11;
    }

    if (std::strcmp(expect, (char*)buf) != 0 ) {
        std::cerr << ": WRS strings don't match 2\n";
        return 12;
    }

    return 0;
}