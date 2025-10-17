; Tag names
(start_tag
  (tag_name) @tag)
(end_tag
  (tag_name) @tag)

; Special Rue section tags
((tag_name) @keyword
  (#match? @keyword "^(schema|data|template|logic)$"))

; Attributes
(attribute_name) @property
(attribute_value) @string

; Comments
(comment) @comment

; Interpolation delimiters
"{{" @punctuation.bracket
"}}" @punctuation.bracket

; Angle brackets
"<" @punctuation.bracket
">" @punctuation.bracket
"</" @punctuation.bracket
"/>" @punctuation.bracket
