bool HasTPUAttributes(const NodeDef& node) {
  AttrSlice attrs(node);
  for (const auto& attr : attrs) {
    if (attr.first.find("_tpu_") != attr.first.npos) {
      return true;
    }
  }
  return false;
}