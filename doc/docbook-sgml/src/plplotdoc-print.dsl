<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY dbstyle 
  PUBLIC "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN"
  CDATA DSSSL>
]>

<style-sheet>
<style-specification use="docbook">
<style-specification-body>

(define %two-side% #t)
(define %default-quadding% 'justify)

(element lineannotation
  (make sequence
    font-posture: 'italic
    (literal "&#12296;")
    (process-children)
    (literal "&#12297;")))

(define %title-font-family% "cmss")

(define %body-font-family% "cmr")

(define %mono-font-family% "cmtt")

(define %admon-font-family% "cmss")

(define %guilabel-font-family% "cmss")

(define %footnote-ulinks% #t)
(define bop-footnotes #t)

(define %bf-size%
  ;; Defines the body font size
  (case %visual-acuity%
    (("tiny") 9pt)
    (("normal") 11pt)
    (("presbyopic") 14pt)
    (("large-type") 26`pt)))

(element funcsynopsis 
  (make paragraph
    first-line-start-indent: %para-indent-firstpara%
    space-before: %para-sep%
    font-family-name: %mono-font-family%
    font-weight: 'medium
    font-posture: 'upright
    start-indent: 0pt
    (make box
      display?: #f
      box-type: 'border
      line-thickness: 0.7pt
      start-indent: %body-start-indent%
      end-indent: 0pt
      (process-children))))

(define %block-sep% %para-sep%)

(define ($section-title$)
  (let* ((sect (current-node))
         (info (info-element))
         (exp-children (if (node-list-empty? info)
                           (empty-node-list)
                           (expand-children (children info) 
                                            (list (normalize "bookbiblio") 
                                                  (normalize "bibliomisc")
                                                  (normalize "biblioset")))))
         (parent-titles (select-elements (children sect) (normalize "title")))
         (info-titles   (select-elements exp-children (normalize "title")))
         (titles        (if (node-list-empty? parent-titles)
                            info-titles
                            parent-titles))
         (subtitles     (select-elements exp-children (normalize "subtitle")))
         (renderas (inherited-attribute-string (normalize "renderas") sect))
         (hlevel                          ;; the apparent section level;
          (if renderas                    ;; if not real section level,
              (string->number             ;;   then get the apparent level
               (substring renderas 4 5))  ;;   from "renderas",
              (SECTLEVEL)))               ;; else use the real level
         (hs (HSIZE (- 4 hlevel))))
    (make sequence
      (make paragraph
        font-family-name: %title-font-family%
        font-weight:  (if (< hlevel 5) 'bold 'medium)
        font-posture: (if (< hlevel 5) 'upright 'italic)
        font-size: hs
        line-spacing: (* hs %line-spacing-factor%)
        space-before: (* hs %head-before-factor%)
        space-after: (if (node-list-empty? subtitles)
                         (* hs %head-after-factor%)
                         0pt)
        start-indent: 0pt
        first-line-start-indent: 0pt
        quadding: %section-title-quadding%
        keep-with-next?: #t
        heading-level: (if %generate-heading-level% (+ hlevel 1) 0)
        ;; SimpleSects are never AUTO numbered...they aren't hierarchical
        (if (string=? (element-label (current-node)) "")
            (empty-sosofo)
            (literal (element-label (current-node)) 
                     (gentext-label-title-sep (gi sect))))
        (element-title-sosofo (current-node)))
      (with-mode section-title-mode
        (process-node-list subtitles))
      ($section-info$ info))))


</style-specification-body>
</style-specification>
<external-specification id="docbook" document="dbstyle">
</style-sheet>
