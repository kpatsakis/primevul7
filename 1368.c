Status ConstantFolding::SimplifyPad(const GraphProperties& properties,
                                    bool use_shape_info,
                                    GraphDef* optimized_graph, NodeDef* node) {
  if (!use_shape_info || !IsPad(*node)) return Status::OK();

  Tensor paddings;
  if (GetTensorFromConstNode(node->input(1), &paddings)) {
    // The node is replaceable iff all values in paddings are 0.
    bool replaceable = true;
    if (paddings.dtype() == DT_INT32) {
      const auto flatten = paddings.flat<int32>();
      for (int j = 0; replaceable && j < flatten.size(); ++j) {
        replaceable &= flatten(j) == 0;
      }
    } else {
      const auto flatten = paddings.flat<int64_t>();
      for (int j = 0; replaceable && j < flatten.size(); ++j) {
        replaceable &= flatten(j) == 0;
      }
    }
    if (replaceable) {
      ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
    }
  }
  return Status::OK();
}