Status ConstantFolding::SimplifyTile(const GraphProperties& properties,
                                     bool use_shape_info,
                                     GraphDef* optimized_graph, NodeDef* node) {
  Tensor multiplies;
  if (use_shape_info && IsTile(*node) &&
      GetTensorFromConstNode(node->input(1), &multiplies)) {
    // The node is replaceable iff all values in multiplies are 1.
    bool replaceable = true;
    if (multiplies.dtype() == DT_INT32) {
      for (int j = 0; replaceable && j < multiplies.vec<int>().size(); ++j) {
        replaceable &= multiplies.vec<int>()(j) == 1;
      }
    } else {
      for (int j = 0; replaceable && j < multiplies.vec<int64_t>().size();
           ++j) {
        replaceable &= multiplies.vec<int64_t>()(j) == 1;
      }
    }
    if (replaceable) {
      ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
    }
  }
  return Status::OK();
}