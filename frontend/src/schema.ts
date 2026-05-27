import {MarkSpec, Schema} from 'prosemirror-model';
import {schema as basicSchema} from 'prosemirror-schema-basic';
import {addListNodes} from 'prosemirror-schema-list';
import {tableNodes} from 'prosemirror-tables';

const customMarks: Record<string, MarkSpec> = {
  ...basicSchema.spec.marks.toObject(),
  underline: {
    parseDOM: [
      {tag: 'u'},
      {style: 'text-decoration=underline'},
    ],
    toDOM() {
      return ['u', 0];
    },
  },
  strikethrough: {
    parseDOM: [
      {tag: 's'},
      {tag: 'del'},
      {tag: 'strike'},
      {style: 'text-decoration=line-through'},
    ],
    toDOM() {
      return ['s', 0];
    },
  },
  superscript: {
    parseDOM: [{tag: 'sup'}],
    toDOM() {
      return ['sup', 0];
    },
  },
  subscript: {
    parseDOM: [{tag: 'sub'}],
    toDOM() {
      return ['sub', 0];
    },
  },
};

const nodesWithLists =
    addListNodes(basicSchema.spec.nodes, 'paragraph block*', 'block');

const nodes: Record<string, any> = nodesWithLists.toObject();
Object.assign(
    nodes,
    tableNodes(
        {tableGroup: 'block', cellContent: 'block+', cellAttributes: {}}));

export const schema = new Schema({
  nodes,
  marks: customMarks,
});
