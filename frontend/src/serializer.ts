import { MarkdownSerializer, defaultMarkdownSerializer } from "prosemirror-markdown";
import { schema } from "./schema";

const extendedSerializer = new MarkdownSerializer(
  {
    ...defaultMarkdownSerializer.nodes,
    heading(state, node) {
      state.write("#".repeat(node.attrs.level) + " ");
      state.renderInline(node);
      state.closeBlock();
    },
  },
  {
    ...defaultMarkdownSerializer.marks,
    strikethrough: {
      open: "~~",
      close: "~~",
      mixable: true,
      expelEnclosingWhitespace: true,
    },
    underline: {
      open: "",
      close: "",
      mixable: true,
      expelEnclosingWhitespace: true,
    },
    superscript: {
      open: "",
      close: "",
      mixable: true,
      expelEnclosingWhitespace: true,
    },
    subscript: {
      open: "",
      close: "",
      mixable: true,
      expelEnclosingWhitespace: true,
    },
  }
);

export { extendedSerializer as markdownSerializer };
