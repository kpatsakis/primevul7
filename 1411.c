bool ConstantFolding::SimplifyReduction(GraphDef* optimized_graph,
                                        const GraphProperties& properties,
                                        NodeDef* node) {
  bool indices_is_empty = false;
  if (!IsReductionWithConstantIndices(*node, &indices_is_empty)) {
    return false;
  }
  if (indices_is_empty) {
    return ReplaceReductionWithIdentity(node);
  }
  bool is_single_element_op = false;
  TensorShapeProto input_tensor_shape, output_tensor_shape;
  if (!IsReductionCandidateForSimplification(
          *node, properties, &input_tensor_shape, &output_tensor_shape,
          &is_single_element_op)) {
    return false;
  }

  // Get the reduction indices.
  string reduction_indices_input = node->input(1);
  NodeDef* reduction_indices = node_map_->GetNode(reduction_indices_input);
  TensorVector reduction_indices_vector;
  auto outputs_cleanup = gtl::MakeCleanup([&reduction_indices_vector] {
    for (const auto& out : reduction_indices_vector) {
      delete out.tensor;
    }
  });
  if (!EvaluateNode(*reduction_indices, TensorVector(),
                    &reduction_indices_vector)
           .ok() ||
      reduction_indices_vector.size() != 1) {
    return false;
  }

  bool keep_dims =
      node->attr().count("keep_dims") > 0 && node->attr().at("keep_dims").b();
  bool simplifiable_to_reshape =
      is_single_element_op && !keep_dims && (node->attr().count("T") > 0);
  bool simplifiable_to_identity = IsReductionSimplifiableToIdentity(
      *node, input_tensor_shape, keep_dims, reduction_indices_vector);

  if (simplifiable_to_reshape) {
    // Const node to output shape.
    const int new_num_dimensions = output_tensor_shape.dim_size();
    Tensor tensor(DT_INT32, TensorShape({new_num_dimensions}));
    for (int i = 0; i < new_num_dimensions; i++) {
      tensor.flat<int>()(i) = 1;
    }
    TensorValue shape_value(&tensor);
    NodeDef* shape_node = optimized_graph->add_node();
    if (!CreateNodeDef(OptimizedNodeName(*node, "_shape_const"), shape_value,
                       shape_node)
             .ok()) {
      return false;
    }
    shape_node->set_device(node->device());
    node_map_->AddNode(shape_node->name(), shape_node);
    // Control dependency to ensure shape_node is in the correct frame.
    shape_node->add_input(AsControlDependency(reduction_indices_input));
    node_map_->AddOutput(NodeName(reduction_indices_input), shape_node->name());
    // Optimize node to Reshape.
    node->set_op("Reshape");
    node_map_->UpdateInput(node->name(), node->input(1), shape_node->name());
    node->set_input(1, shape_node->name());
    node->mutable_attr()->erase("keep_dims");
    node->mutable_attr()->erase("Tidx");
    AttrValue attr_type_indices;
    attr_type_indices.set_type(DT_INT32);
    (*node->mutable_attr())["Tshape"] = attr_type_indices;
    return true;
  } else if (simplifiable_to_identity) {
    return ReplaceReductionWithIdentity(node);
  }
  return false;
}