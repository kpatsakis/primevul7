string ConstantFolding::OptimizedNodeName(const NodeDef& node,
                                          StringPiece suffix) const {
  return AddPrefixToNodeName(strings::StrCat(node.name(), suffix),
                             kConstantFoldingConst);
}