/**
 * @file Tree-sitter grammar for Ruby Single File Components (.rue)
 * @author Your Name
 * @license MIT
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: 'rue',

  externals: $ => [
    $._schema_start_tag_name,
    $._data_start_tag_name,
    $._template_start_tag_name,
    $._logic_start_tag_name,
    $._end_tag_name,
    $.raw_text,
    $.comment,
  ],

  extras: $ => [/\s/],

  rules: {
    document: $ => repeat($._element),

    _element: $ => choice(
      $.schema_element,
      $.data_element,
      $.template_element,
      $.logic_element,
      $.comment,
    ),

    schema_element: $ => seq(
      '<',
      alias($._schema_start_tag_name, $.tag_name),
      repeat($.attribute),
      '>',
      optional($.raw_text),
      '</',
      $._end_tag_name,
      '>',
    ),

    data_element: $ => seq(
      '<',
      alias($._data_start_tag_name, $.tag_name),
      repeat($.attribute),
      '>',
      optional($.raw_text),
      '</',
      $._end_tag_name,
      '>',
    ),

    template_element: $ => seq(
      '<',
      alias($._template_start_tag_name, $.tag_name),
      repeat($.attribute),
      '>',
      optional($.raw_text),
      '</',
      $._end_tag_name,
      '>',
    ),

    logic_element: $ => seq(
      '<',
      alias($._logic_start_tag_name, $.tag_name),
      repeat($.attribute),
      '>',
      optional($.raw_text),
      '</',
      $._end_tag_name,
      '>',
    ),

    attribute: $ => seq(
      $.attribute_name,
      optional(seq(
        '=',
        choice(
          $.quoted_attribute_value,
          $.attribute_value,
        ),
      )),
    ),

    attribute_name: $ => /[^<>"'=/\s]+/,

    attribute_value: $ => /[^<>"'=\s]+/,

    quoted_attribute_value: $ => choice(
      seq('"', optional(alias(/[^"]+/, $.attribute_value)), '"'),
      seq("'", optional(alias(/[^']+/, $.attribute_value)), "'"),
    ),
  },
});
