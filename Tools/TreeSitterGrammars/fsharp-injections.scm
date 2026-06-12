([
 (line_comment)
 (block_comment_content)
] @injection.content
 (#set! injection.language "comment"))

((xml_doc) @injection.content
 (#offset! @injection.content 0 3 0 0)
 (#set! injection.language "xml")
 (#set! injection.combined))
