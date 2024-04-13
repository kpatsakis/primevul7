void mini9(const char* path)
{
    TiffImage tiffImage(BasicIo::UniquePtr(new FileIo(path)), false);
    tiffImage.readMetadata();

    std::cout << "MIME type:  " << tiffImage.mimeType() << "\n";
    std::cout << "Image size: " << tiffImage.pixelWidth() << " x " << tiffImage.pixelHeight() << "\n";

    ExifData& exifData = tiffImage.exifData();
    std::cout << "Before\n";
    print(exifData);
    std::cout << "======\n";

    exifData["Exif.Photo.DateTimeOriginal"] = "Yesterday at noon";

    std::cout << "After\n";
    print(exifData);
    tiffImage.writeMetadata();
}