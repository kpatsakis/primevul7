void WasmBinaryWriter::writeSourceMapEpilog() {
  // write source map entries
  size_t lastOffset = 0;
  Function::DebugLocation lastLoc = {0, /* lineNumber = */ 1, 0};
  for (const auto& [offset, loc] : sourceMapLocations) {
    if (lastOffset > 0) {
      *sourceMap << ",";
    }
    writeBase64VLQ(*sourceMap, int32_t(offset - lastOffset));
    writeBase64VLQ(*sourceMap, int32_t(loc->fileIndex - lastLoc.fileIndex));
    writeBase64VLQ(*sourceMap, int32_t(loc->lineNumber - lastLoc.lineNumber));
    writeBase64VLQ(*sourceMap,
                   int32_t(loc->columnNumber - lastLoc.columnNumber));
    lastLoc = *loc;
    lastOffset = offset;
  }
  *sourceMap << "\"}";
}