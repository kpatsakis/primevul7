Status ConstantFolding::FoldNode(NodeDef* node, GraphDef* output_graph,
                                 bool* result_too_large) {
  *result_too_large = false;
  if (IsMerge(*node)) {
    return FoldMergeNode(node, output_graph);
  }

  std::vector<NodeDef> const_nodes;
  TF_RETURN_IF_ERROR(
      EvaluateOneFoldable(*node, &const_nodes, result_too_large));
  VLOG(2) << "Folded node: " << SummarizeNodeDef(*node);

  NodeDef* constant_output = nullptr;
  for (int i = 0, end = const_nodes.size(); i < end; i++) {
    NodeDef* const_node = &const_nodes[i];
    VLOG(3) << "Generated constant node: " << SummarizeNodeDef(*const_node);
    if (const_node->name().empty()) {
      // Dead output: we can't create a constant to encode its value, so we'll
      // just skip it. We'll preserve the edges that originate from that
      // output below to preserve the overall behavior of the graph wrt dead
      // edges.
      continue;
    }

    // Returns `true` iff `const_node` already has control input named `input`.
    const auto is_duplicate_control_input = [&](const string& input) -> bool {
      auto it = absl::c_find(const_node->input(), input);
      return it != const_node->input().end();
    };

    // Forward control dependencies.
    for (const string& input : node->input()) {
      // Forward control dependencies from folded node.
      if (IsControlInput(input)) {
        if (!is_duplicate_control_input(input)) {
          *const_node->add_input() = input;
        }
      }

      // Forward control dependencies from constant inputs to folded node.
      if (!IsControlInput(input)) {
        NodeDef* input_node = node_map_->GetNode(input);
        for (const string& fanin_of_input : input_node->input()) {
          if (!is_duplicate_control_input(fanin_of_input)) {
            *const_node->add_input() = fanin_of_input;
          }
        }
      }
    }

    // We rewrite the existing node if it only has a single output, and
    // create new nodes otherwise.
    if (const_nodes.size() == 1) {
      node->set_op("Const");
      // Note we need to clear the inputs in NodeMap before we clear the inputs
      // in the node, otherwise NodeMap would see empty inputs and effectively
      // does nothing.
      node_map_->RemoveInputs(node->name());
      node->clear_input();
      *node->mutable_input() = const_node->input();
      for (const auto& input : node->input()) {
        node_map_->AddOutput(NodeName(input), node->name());
      }
      *node->mutable_attr() = const_node->attr();
      break;
    } else {
      if (node_map_->GetNode(const_node->name())) {
        // Intended name already exists.
        return errors::AlreadyExists(strings::StrCat(
            const_node->name(), " already present in the graph"));
      }
      NodeDef* added_node = output_graph->add_node();
      *added_node = *const_node;
      added_node->set_device(node->device());
      node_map_->AddNode(added_node->name(), added_node);
      for (const auto& input : added_node->input()) {
        node_map_->AddOutput(NodeName(input), added_node->name());
      }
      // All the constant nodes encoding output values have the same control
      // dependencies (since these are the control dependencies of the node
      // we're trying to fold). Record one such constant node.
      constant_output = added_node;
    }
  }

  if (const_nodes.size() > 1) {
    // We make a copy because we mutate the nodes.
    auto outputs = node_map_->GetOutputs(node->name());
    for (NodeDef* output : outputs) {
      for (int i = 0; i < output->input_size(); i++) {
        int port;
        string node_name = ParseNodeName(output->input(i), &port);
        if (node_name == node->name()) {
          if (port < 0) {
            // Propagate control dependencies if possible. If not, we'll just
            // preserve the existing control dependencies.
            if (constant_output != nullptr) {
              node_map_->UpdateInput(node_name, NodeName(output->input(i)),
                                     constant_output->name());
              *output->mutable_input(i) = AsControlDependency(*constant_output);
            }
          } else if (port < static_cast<int>(const_nodes.size()) &&
                     !const_nodes[port].name().empty()) {
            // Replace alive outputs with the corresponding constant.
            node_map_->UpdateInput(output->name(), NodeName(output->input(i)),
                                   const_nodes[port].name());
            *output->mutable_input(i) = const_nodes[port].name();
          } else {
            // Leave this edge alone.
            VLOG(3) << "Preserving edge from " << node->name() << ":" << port
                    << "[" << node->op() << "] to " << output->name() << ":"
                    << i << "[" << output->op() << "]";
          }
        }
      }
    }
    outputs = node_map_->GetOutputs(node->name());
    if (outputs.empty() && has_fetch_ &&
        nodes_to_preserve_.find(node->name()) == nodes_to_preserve_.end()) {
      node_map_->RemoveInputs(node->name());
      node->clear_input();
    }
  }
  return Status::OK();
}