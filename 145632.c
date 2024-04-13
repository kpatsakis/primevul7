void mini1(const char* path)
{
    ExifData exifData;
    Blob blob;
    WriteMethod wm;

    // Write nothing to a new structure, without a previous binary image
    wm = ExifParser::encode(blob, 0, 0, bigEndian, exifData);
    enforce(wm == wmIntrusive, Exiv2::kerErrorMessage, "encode returned an unexpected value");
    assert(blob.size() == 0);
    std::cout << "Test 1: Writing empty Exif data without original binary data: ok.\n";

    // Write nothing, this time with a previous binary image
    DataBuf buf = readFile(path);
    wm = ExifParser::encode(blob, buf.pData_, (uint32_t)buf.size_, bigEndian, exifData);
    enforce(wm == wmIntrusive, Exiv2::kerErrorMessage, "encode returned an unexpected value");
    assert(blob.size() == 0);
    std::cout << "Test 2: Writing empty Exif data with original binary data: ok.\n";

    // Write something to a new structure, without a previous binary image
    exifData["Exif.Photo.DateTimeOriginal"] = "Yesterday at noon";
    wm = ExifParser::encode(blob, 0, 0, bigEndian, exifData);
    enforce(wm == wmIntrusive, Exiv2::kerErrorMessage, "encode returned an unexpected value");
    std::cout << "Test 3: Wrote non-empty Exif data without original binary data:\n";
    exifData.clear();
    ByteOrder bo = ExifParser::decode(exifData, &blob[0], (uint32_t) blob.size());
    enforce(bo == bigEndian, Exiv2::kerErrorMessage, "decode returned an unexpected value");
    print(exifData);
}