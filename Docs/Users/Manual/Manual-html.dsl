<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA dsssl>
]>

<style-sheet>
<style-specification use="docbook">
<style-specification-body>

;; Based on stylesheet of Newbiedoc project at
;; http://sourceforge.net/projects/newbiedoc 

(define %generate-article-toc%
  ;; Should a Table of Contents be produced for Articles?
  #t)

(define (toc-depth nd)
  (if (string=? (gi nd) (normalize "book"))
      ;;
      ;; Docbook default is 1 level deep
      ;; I don't understand "normalize book" but
      ;; it doesn't seem to affect if we use
      ;; articles.  I changed it to 2 deep.
      ;;
      3
      2))

(define %admon-graphics%
  ;; Use graphics in admonitions?
  #t)

(define %admon-graphics-path%
  ;; Path to admonition graphics
  ;; Sets the path, probably relative to the directory
  ;; where the HTML files are created, to the admonition
  ;; graphics.
  "./images/")

(define %callout-graphics%
  ;; If true, callouts are presented with graphics (e.g., reverse-video
  ;; circled numbers instead of "(1)", "(2)", etc.).
  ;; Default graphics are provided in the distribution.
  #t)

(define %callout-graphics-path%
  ;; Sets the path, probably relative to the directory where the HTML
  ;; files are created, to the callout graphics.
  "./images/callouts/")  

(define %callout-graphics-number-limit%
  ;; If '%callout-graphics%' is true, graphics are used to represent
  ;; callout numbers. The value of '%callout-graphics-number-limit%' is
  ;; the largest number for which a graphic exists. If the callout number
  ;; exceeds this limit, the default presentation "(nnn)" will always
  ;; be used.
  10)

(define ($admon-graphic$ #!optional (nd (current-node)))
  ;; Admonition graphic file
  ;; Given an admonition node, returns the name of the
  ;; graphic that should be used for that admonition.
  (cond ((equal? (gi nd) (normalize "tip"))
         (string-append %admon-graphics-path% "tip.gif"))
        ((equal? (gi nd) (normalize "note"))
         (string-append %admon-graphics-path% "note.gif"))
        ((equal? (gi nd) (normalize "important"))
         (string-append %admon-graphics-path% "important.gif"))
        ((equal? (gi nd) (normalize "caution"))
         (string-append %admon-graphics-path% "caution.gif"))
        ((equal? (gi nd) (normalize "warning"))
         (string-append %admon-graphics-path% "warning.gif"))
        (else (error (string-append (gi nd) " is not an admonition.")))))


(define ($admon-graphic-width$ #!optional (nd (current-node)))
  "25")

(define %header-navigation%
  ;; Should navigation links be added to the top of each page?
  #t)

(define %footer-navigation%
  ;; Should navigation links be added to the bottom of each page?
  #t)

(define %gentext-nav-tblwidth%
  ;; If using tables for navigation, how wide should the tables be?
  "100%")

(define %gentext-nav-use-tables%
  ;; Use tables to build the navigation headers and footers?
  #t)

;;Add arrows to navigation (comment these
;;out if you want admon graphics here)
;;(define (gentext-en-nav-prev prev)
;;  (make sequence (literal "<< Previous")))

;;Add arrows to navigation (comment these
;;out if you want admon graphics here)
;;(define (gentext-en-nav-next next)
;;  (make sequence (literal "Next >>")))

;; For the life of me I still haven't figured out how to
;; configure these two monsters.
;;
;;(define ($user-header-navigation$ #!optional
;;                                  (prev (empty-node-list))
;;                                  (next (empty-node-list))
;;                                  (prevm (empty-node-list))
;;                                  (nextm (empty-node-list)))
;;  (empty-sosofo))

;;(define ($user-footer-navigation$ #!optional
;;                                  (prev (empty-node-list))
;;                                  (next (empty-node-list))
;;                                  (prevm (empty-node-list))
;;                                  (nextm (empty-node-list)))
;;  (empty-sosofo))

(define %number-programlisting-lines%
  ;; Enumerate lines in a 'ProgramListing'?
  #t)

(define %linenumber-length%
  ;; Width of line numbers in enumerated environments
  ;; Line numbers will be padded to %linenumber-length% characters.
  0)

(define %linenumber-mod%
  ;; Controls line-number frequency in enumerated environments.
  ;; Every %linenumber-mod% line will be enumerated.
  1)

(define %linenumber-padchar%
  ;; Pad character in line numbers
  ;; Line numbers will be padded (on the left) with %linenumber-padchar%
  " ")


(define %shade-verbatim%
  ;; Should verbatim environments be shaded?
  #t)

(define ($shade-verbatim-attr$)
  ;; Attributes used to create a shaded verbatim environment.
  (list
   (list "BORDER" "0")
   (list "BGCOLOR" "#BBDDFF")
   (list "WIDTH" ($table-width$))
   (list "CELLPADDING" "0")
   (list "CELLSPACING" "0")
   ))

(define %body-attr%
  ;; What attributes should be hung off of BODY?
  (list
   (list "BGCOLOR" "#FFFFFF")
   (list "TEXT" "#000000")
   (list "LINK" "#0000FF")
   (list "VLINK" "#800080")
   (list "ALINK" "#FF0000")))


(define %stylesheet%
  ;; Name of the stylesheet to use
  #f) ;; uncomment this to make false and comment out line below
  ;; "../../nd-style.css")

(define %stylesheet-type%
  ;; The type of the stylesheet to use
  "text/css")

(define %html40%
  ;; Generate HTML 4.0
  #t)

(define %use-id-as-filename%
  ;; Use ID attributes as name for component HTML files?
  #t)

;;Default extension for filenames?
(define %html-ext%
  ".html")

;; I like that:
(define (chunk-skip-first-element-list)
  ;; forces the Table of Contents on separate page
  '())

;; I didn't see any bug ?!
(define (list-element-list)
  ;; fixes bug in Table of Contents generation
  '())
 
;; What section levels get put into separate HTML files
;; (chunks) in the chunked version
(define (chunk-section-depth)
  1)

;; Indent lines in a programlisting? by how many spaces?
(define %indent-programlisting-lines%
  "")

  ;; Enumerate lines in a 'Screen'?
(define %number-screen-lines%
  #f)

;; Indent lines in a 'Screen'?
;; This is a string of characters used to indent every line of
;; a screen. 
(define %indent-screen-lines%
  "    ")

;; This puts figure titles below the figure.
(define ($object-titles-after$)
  (list (normalize "figure")))

;; Are sections enumerated?
;; The number appears in their title, both in the table
;; of contents, and in the text.
(define %section-autolabel%
  #t)

;; Name for the root HTML document
;; If we want to change "book1" to "index" as the
;; root filename, we can use this:
;;        (define %root-filename%
;;          "index")

;; Make GUI-related names appear in bold font.
;; (element guibutton ($bold-seq$))
;; (element guiicon ($bold-seq$))
;; (element guilabel ($bold-seq$))
;; (element guimenu ($bold-seq$))
;; (element guimenuitem ($bold-seq$))
;; (element guisubmenu ($bold-seq$))


</style-specification-body>
</style-specification>
<external-specification id="docbook" document="docbook.dsl">
</style-sheet>