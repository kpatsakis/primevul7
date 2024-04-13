bool ConstantFolding::ForwardInputs(NodeDef* node,
                                    absl::Span<const int> inputs_to_forward) {
  for (int input_idx : inputs_to_forward) {
    if (input_idx < 0 || input_idx >= node->input_size()) {
      return false;
    }
  }

  const auto& tmp = node_map_->GetOutputs(node->name());
  const std::vector<NodeDef*> consumers(tmp.begin(), tmp.end());
  bool updated_graph = false;
  for (int input_idx : inputs_to_forward) {
    const string& input = node->input(input_idx);
    if (IsControlInput(input) && consumers.size() > 1) {
      continue;
    }
    const NodeDef* input_node = node_map_->GetNode(NodeName(input));
    if (input_node == nullptr) {
      LOG(ERROR) << "Bad input: " << input;
      break;
    }
    // Update each consumer.
    for (NodeDef* consumer : consumers) {
      bool add_dep = false;
      for (int consumer_input_idx = 0;
           consumer_input_idx < consumer->input_size(); ++consumer_input_idx) {
        const string& consumer_input = consumer->input(consumer_input_idx);
        if (IsControlInput(consumer_input)) {
          break;
        }
        // It is illegal to add control dependencies to _Retval nodes, so we
        // can't bypass value producing `node` and forward inputs to `consumer`.
        if (IsRetval(*consumer)) {
          break;
        }
        int output_idx;
        const string input_node_name =
            ParseNodeName(consumer_input, &output_idx);
        if (input_node_name == node->name() && output_idx == input_idx) {
          consumer->set_input(consumer_input_idx, input);
          // We will keep the input from the node through a control
          // dependency, so we only need to add the consumer as an output
          // for the input node.
          node_map_->AddOutput(NodeName(input), consumer->name());
          add_dep = true;
        }
      }
      if (add_dep) {
        consumer->add_input(AsControlDependency(node->name()));
        updated_graph = true;
      }
    }
  }

  if (updated_graph) {
    for (NodeDef* consumer : consumers) {
      DedupControlInputs(consumer);
    }
  }
  return updated_graph;
}