bool ConstantFolding::OptimizedNodeExists(const NodeDef& node,
                                          StringPiece suffix) const {
  return node_map_->NodeExists(OptimizedNodeName(node, suffix));
}