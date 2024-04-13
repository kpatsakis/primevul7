Status ConstantFolding::SimplifySlice(const GraphProperties& properties,
                                      bool use_shape_info,
                                      GraphDef* optimized_graph,
                                      NodeDef* node) {
  if (!use_shape_info || !IsSlice(*node)) return Status::OK();
  Tensor begin;
  Tensor size;
  if (properties.HasInputProperties(node->name()) &&
      GetTensorFromConstNode(node->input(1), &begin) &&
      GetTensorFromConstNode(node->input(2), &size)) {
    const auto& input = properties.GetInputProperties(node->name())[0];
    // The node is replaceable iff unknown_rank == false &&
    // begin == 0 && (size == -1 || size == input_shape) for all dimensions
    bool replaceable = !input.shape().unknown_rank();
    for (int j = 0; replaceable && j < input.shape().dim_size(); ++j) {
      if (begin.dtype() == DT_INT32) {
        replaceable &= begin.vec<int>()(j) == 0;
      } else {
        replaceable &= begin.vec<int64_t>()(j) == 0;
      }
      if (size.dtype() == DT_INT32) {
        replaceable &= (size.vec<int>()(j) == -1 ||
                        size.vec<int>()(j) == input.shape().dim(j).size());
      } else {
        replaceable &= (size.vec<int64_t>()(j) == -1 ||
                        size.vec<int64_t>()(j) == input.shape().dim(j).size());
      }
    }
    if (replaceable) {
      ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
    }
  }
  return Status::OK();
}