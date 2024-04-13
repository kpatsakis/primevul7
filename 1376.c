Status ConstantFolding::RemoveReverse(const GraphProperties& properties,
                                      bool use_shape_info,
                                      GraphDef* optimized_graph,
                                      NodeDef* node) {
  if (!use_shape_info || node->op() != "ReverseV2") return Status::OK();
  Tensor axis;
  if (properties.HasInputProperties(node->name()) &&
      GetTensorFromConstNode(node->input(1), &axis)) {
    const auto& shape = properties.GetInputProperties(node->name())[0].shape();
    if (shape.unknown_rank()) return Status::OK();
    std::set<int> target_axes;
    for (int j = 0; j < axis.NumElements(); ++j) {
      // value of axis can be negative.
      if (axis.dtype() == DT_INT64) {
        target_axes.insert((axis.vec<int64_t>()(j) + shape.dim_size()) %
                           shape.dim_size());
      } else {
        target_axes.insert((axis.vec<int>()(j) + shape.dim_size()) %
                           shape.dim_size());
      }
    }

    // The node is replaceable iff
    // unknown_rank == false &&
    // (dim_size == 0 || all dims have size 1 ||
    //  all dims with > 1 size are not in target_axes)
    bool replaceable = true;
    for (int j = 0; replaceable && j < shape.dim_size(); ++j) {
      replaceable &=
          shape.dim(j).size() == 1 || target_axes.find(j) == target_axes.end();
    }
    if (replaceable) {
      ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
    }
  }
  return Status::OK();
}