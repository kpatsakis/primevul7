void ConstantFolding::SimplifySqueeze(const GraphProperties& properties,
                                      bool use_shape_info,
                                      GraphDef* optimized_graph,
                                      NodeDef* node) {
  if (use_shape_info && IsSqueeze(*node) &&
      !properties.GetInputProperties(node->name()).empty()) {
    // https://www.tensorflow.org/api_docs/python/tf/squeeze mentions it's
    // error to squeeze a dimension that is not 1, so we only need to check
    // whether the input has > 1 size for each dimension.
    const auto& shape = properties.GetInputProperties(node->name())[0].shape();
    // The node is replaceable iff
    // unknown_rank == false && (dim_size == 0 || all dims have size > 1)
    bool replaceable = !shape.unknown_rank();
    for (int j = 0; replaceable && j < shape.dim_size(); ++j) {
      replaceable &= shape.dim(j).size() > 1;
    }
    if (replaceable) {
      ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
    }
  }
}