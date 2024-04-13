bool ConstantFolding::SimplifyCase(GraphDef* optimized_graph, NodeDef* node) {
  if (node->op() != "Case") return false;
  const NodeDef* output_idx_node = node_map_->GetNode(node->input(0));
  if (output_idx_node == nullptr ||
      !CheckAttrExists(*output_idx_node, "value").ok()) {
    return false;
  }
  Tensor output_idx_t;
  if (!output_idx_t.FromProto(output_idx_node->attr().at("value").tensor()))
    return false;
  int output_idx = output_idx_t.scalar<int>()();
  const auto& func_list = node->attr().at("branches").list();
  if (output_idx < 0 || output_idx >= func_list.func_size()) return false;
  NodeDef call_node = *node;
  call_node.set_op("PartitionedCall");
  call_node.clear_input();
  for (int i = 1; i < node->input_size(); ++i) {
    call_node.add_input(node->input(i));
  }
  auto* new_func = (*call_node.mutable_attr())["f"].mutable_func();
  *new_func = func_list.func(output_idx);

  // Move the output shape of the branch to _output_shapes if it is known.
  const auto& output_shape_list =
      (*node->mutable_attr())["output_shapes"].list();
  if (output_shape_list.shape_size() > output_idx) {
    TensorShapeProto* new_output_shape =
        (*call_node.mutable_attr())["_output_shapes"]
            .mutable_list()
            ->add_shape();
    *new_output_shape =
        std::move(node->attr().at("output_shapes").list().shape(output_idx));
  }

  call_node.mutable_attr()->erase("output_shapes");
  call_node.mutable_attr()->erase("branches");

  *node = std::move(call_node);
  return true;
}