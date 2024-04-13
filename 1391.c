void ConstantFolding::RemoveRandomShuffle(const GraphProperties& properties,
                                          bool use_shape_info,
                                          GraphDef* optimized_graph,
                                          NodeDef* node) {
  if (use_shape_info && IsRandomShuffle(*node) &&
      !properties.GetInputProperties(node->name()).empty()) {
    const auto& shape = properties.GetInputProperties(node->name())[0].shape();
    // The node is replaceable iff
    // unknown_rank == false && (dim_size == 0 || first dim is of size 1)
    if (!shape.unknown_rank() &&
        (shape.dim_size() == 0 || shape.dim(0).size() == 1)) {
      ReplaceOperationWithIdentity(0, properties, node, optimized_graph);
    }
  }
}