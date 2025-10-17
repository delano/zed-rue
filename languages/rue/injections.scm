; Inject Zod schema language into <schema> sections
((schema_element
  (start_tag
    (attribute
      (attribute_name) @_lang
      (quoted_attribute_value (attribute_value) @_zod)))
  (raw_text) @content)
  (#eq? @_lang "lang")
  (#match? @_zod "(js-zod|ts-zod)")
  (#set! "language" "typescript"))

; Default JavaScript for schema sections without lang attribute
((schema_element
  (start_tag) @_no_lang
  (raw_text) @content)
  (#not-match? @_no_lang "lang=")
  (#set! "language" "javascript"))

; Inject JSON into <data> sections (deprecated but still supported)
((data_element
  (raw_text) @content)
  (#set! "language" "json"))

; Inject HTML/Handlebars into <template> sections
((template_element
  (raw_text) @content)
  (#set! "language" "html"))

; Inject Ruby into <logic> sections
((logic_element
  (raw_text) @content)
  (#set! "language" "ruby"))

; Highlight {{interpolation}} in templates as JavaScript/TypeScript
((interpolation
  (raw_text) @content)
  (#set! "language" "javascript"))
