<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY dbstyle 
  PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN"
  CDATA DSSSL>
]>

<style-sheet>
<style-specification use="docbook">
<style-specification-body>

(element funcsynopsis
  (make element gi: "TABLE"
	attributes: '(("BORDER"  "0")
                      ("BGCOLOR" "#D6E8FF")
                      ("WIDTH"   "100%"))
    (make element gi: "TR"
      (make element gi: "TD"
        (process-children)))))

(element variablelist
  (make element gi: "TABLE"
	attributes: '(("BORDER"  "0")
                      ("BGCOLOR" "#FFF0D0")
                      ("WIDTH"   "100%"))
    (make element gi: "TR"
      (make element gi: "TD"
	(make element gi: "DL"
          (process-children))))))

(define %shade-verbatim% #t)

(define ($shade-verbatim-attr$)
  (list
   (list "BORDER" "0")
   (list "BGCOLOR" "#E0FFE0")
   (list "WIDTH" "100%")))

(element lineannotation
  (make sequence
    font-posture: 'italic
    (literal "<")
    (process-children)
    (literal ">")))

</style-specification-body>
</style-specification>
<external-specification id="docbook" document="dbstyle">
</style-sheet>
