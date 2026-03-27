(identifier) @local.reference

[
  (namespace)
  (named_module)
  (function_or_value_defn)
] @local.scope

(value_declaration_left
  .
  [
   (_ (identifier) @local.definition)
   (_ (_ (identifier) @local.definition))
   (_ (_ (_ (identifier) @local.definition)))
   (_ (_ (_ (_ (identifier) @local.definition))))
   (_ (_ (_ (_ (_ (identifier) @local.definition)))))
   (_ (_ (_ (_ (_ (_ (identifier) @local.definition))))))
  ])

(function_declaration_left
  .
  ((_) @local.definition
   (#set! "definition.function.scope" "parent"))
  ((argument_patterns
     [
      (_ (identifier) @local.definition)
      (_ (_ (identifier) @local.definition))
      (_ (_ (_ (identifier) @local.definition)))
      (_ (_ (_ (_ (identifier) @local.definition))))
      (_ (_ (_ (_ (_ (identifier) @local.definition)))))
      (_ (_ (_ (_ (_ (_ (identifier) @local.definition))))))
     ])
  ))
