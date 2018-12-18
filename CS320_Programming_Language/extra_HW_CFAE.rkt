#lang plai

(require (for-syntax racket/base) racket/match racket/list racket/string
         (only-in mzlib/string read-from-string-all))

;; build a regexp that matches restricted character expressions, can use only
;; {}s for lists, and limited strings that use '...' (normal racket escapes
;; like \n, and '' for a single ')
(define good-char "(?:[ \t\r\na-zA-Z0-9_{}!?*/<=>:+-]|[.][.][.])")
;; this would make it awkward for students to use \" for strings
;; (define good-string "\"[^\"\\]*(?:\\\\.[^\"\\]*)*\"")
(define good-string "[^\"\\']*(?:''[^\"\\']*)*")
(define expr-re
  (regexp (string-append "^"
                         good-char"*"
                         "(?:'"good-string"'"good-char"*)*"
                         "$")))
(define string-re
  (regexp (string-append "'("good-string")'")))

(define (string->sexpr str)
  (unless (string? str)
    (error 'string->sexpr "expects argument of type <string>"))
    (unless (regexp-match expr-re str)
      (error 'string->sexpr "syntax error (bad contents)"))
    (let ([sexprs (read-from-string-all
                 (regexp-replace*
                  "''" (regexp-replace* string-re str "\"\\1\"") "'"))])
    (if (= 1 (length sexprs))
      (car sexprs)
      (error 'string->sexpr "bad syntax (multiple expressions)"))))

(test/exn (string->sexpr 1) "expects argument of type <string>")
(test/exn (string->sexpr ".") "syntax error (bad contents)")
(test/exn (string->sexpr "{} {}") "bad syntax (multiple expressions)")

;;-----------------------------------------

;; FAE abstract syntax trees                                               
(define-type FAE
  [num (n number?)]
  [add (left FAE?) (right FAE?)]
  [sub (left FAE?) (right FAE?)]
  [id (name symbol?)]
  [app (ftn FAE?) (arg FAE?)]
  [fun (param symbol?) (body FAE?)])

;; CFAE abstract syntax trees
(define-type CFAE
  [cnum (n number?)]
  [cadd (left CFAE?) (right CFAE?)]
  [csub (left CFAE?) (right CFAE?)]
  [cid (pos number?)]
  [cfun (body CFAE?)]
  [capp (ftn CFAE?) (arg CFAE?)])

;; CFAE-Value abstract syntax tree
(define-type CFAE-Value
  [numV (n number?)]
  [closureV (body CFAE?)
            (ds (listof CFAE-Value?))])

;; CDefrdSub abstract syntax tree
(define-type CDefrdSub
  [mtCSub]
  [aCSub (name symbol?)
         (rest CDefrdSub?)])

;; CFAE-Cont abstract syntax tree
(define-type CFAE-Cont
  [mtK]
  [addSecondK (r CFAE?) (ds (listof CFAE-Value?)) (k CFAE-Cont?)]
  [doAddK     (v CFAE-Value?) (k CFAE-Cont?)]
  [subSecondK (r CFAE?) (ds (listof CFAE-Value?)) (k CFAE-Cont?)]
  [doSubK     (v CFAE-Value?) (k CFAE-Cont?)]
  [appArgK    (a CFAE?) (ds (listof CFAE-Value?)) (k CFAE-Cont?)]
  [doAppK     (v CFAE-Value?) (k CFAE-Cont?)])

;; -----------------parse-------------------

;; parse-sexpr : sexpr -> FAE
;; to convert s-expressions into FAE
(define (parse-sexpr sexp)
  (cond
    [(number? sexp) (num sexp)]
    [(symbol? sexp) (id sexp)]
    [(pair? sexp)
     (case (car sexp)
       [(+) (add (parse-sexpr (second sexp)) (parse-sexpr (third sexp)))]
       [(-) (sub (parse-sexpr (second sexp)) (parse-sexpr (third sexp)))]
       [(fun) (fun (car (second sexp)) (parse-sexpr (third sexp)))]
       [else (app (parse-sexpr (first sexp)) (parse-sexpr (second sexp)))])]))

;; parses a string containing a FAE expression to a FAE AST
(define (parse str)
  (parse-sexpr (string->sexpr str)))

;;-------------------------compile--------------------------------------

;; compile : FAE CDefrdSub -> CFAE
(define (compile fae ds)
  (type-case FAE fae
    [num (n) (cnum n)]
    [add (l r) (cadd (compile l ds) (compile r ds))]
    [sub (l r) (csub (compile l ds) (compile r ds))]
    [id (name) (cid (locate name ds))]
    [fun (param body)
         (cfun (compile body (aCSub param ds)))]
    [app (ftn arg) (capp (compile ftn ds) (compile arg ds))]))

;; --------------------------resisters-----------------------------------

;; resisters
(define cfae-reg (cnum 0))
(define k-reg (mtK))
(define v-reg (numV 0))
(define ds-reg empty)

;; --------------------------interpret-----------------------------------

;; interp : CFAE list-of-CFAE-Value CFAE-Cont -> CFAE-Value
(define (interp)
  (type-case CFAE cfae-reg
    [cnum (n) (begin
                (set! cfae-reg (numV n))
                (continue))]
    [cadd (l r) (begin
                  (set! cfae-reg l)
                  (set! k-reg (addSecondK
                               r ds-reg k-reg))
                  (interp))]
    [csub (l r) (begin
                  (set! cfae-reg l)
                  (set! k-reg (subSecondK
                               r ds-reg k-reg))
                  (interp))]
    [cid  (pos) (begin
                  (set! cfae-reg (list-ref ds-reg pos))
                  (continue))]
    [cfun (body) (begin
                   (set! cfae-reg (closureV body ds-reg))
                   (continue))]
    [capp (fun-expr arg-expr) (begin
                                (set! cfae-reg fun-expr)
                                (set! k-reg (appArgK
                                             arg-expr ds-reg k-reg))
                                (interp))]))

;; continue : CFAE-Cont CFAE-Value -> CFAE-Value
(define (continue)
  (type-case CFAE-Cont k-reg
    [mtK () cfae-reg]
    [addSecondK (r ds k1) (begin
                            (set! k-reg (doAddK cfae-reg k1))
                            (set! cfae-reg r)
                            (set! ds-reg ds)
                            (interp))]
    [doAddK (v1 k1) (begin  
                      (set! k-reg k1)
                      (set! cfae-reg (num+ v1 cfae-reg))
                      (continue))]
    [subSecondK (r ds k1) (begin
                            (set! k-reg (doSubK cfae-reg k1))
                            (set! cfae-reg r)
                            (set! ds-reg ds)
                            (interp))]
    [doSubK (v1 k1) (begin    
                      (set! k-reg k1)
                      (set! cfae-reg (num- v1 cfae-reg))
                      (continue))]
    [appArgK (arg-expr ds k1) (begin
                                (set! k-reg (doAppK cfae-reg k1))
                                (set! cfae-reg arg-expr)
                                (set! ds-reg ds)
                                (interp))]
    [doAppK (fun-val k1) (type-case CFAE-Value fun-val
                          [closureV (body ds) (begin
                                                (set! ds-reg (cons cfae-reg ds))
                                                (set! cfae-reg body)
                                                (set! k-reg k1)
                                                (interp))]
                          [else (error 'interp "not a function")])]))
    

;;-----------------help functions for interp----------------------------------

;; num-op : (number number -> number) -> (CFAE CFAE -> CFAE)
;; to have opration on numV value
;; (num+ (numV 3) (numV 102)) should produce (numV 105)
(define (num-op op)
  (lambda (x y)
    (numV (op (numV-n x) (numV-n y)))))
(define num+ (num-op +))
(define num- (num-op -))
;test for num-op
(test (num+ (numV 3) (numV 102)) (numV 105))

;; locate : symbol CDefrdSub -> number
(define (locate name ds)
  (type-case CDefrdSub ds
    [mtCSub () (error 'locate "free variable")]
    [aCSub (sub-name rest-ds)
           (if (symbol=? sub-name name)
               0
               (+ 1 (locate name rest-ds)))]))

;;-----------------------------------------------------------------------

;; interp-expr : sexp -> CFAE-Value
(define (interp-expr sexp)
  (begin
    (set! cfae-reg sexp)
    (set! ds-reg empty)
    (set! k-reg (mtK))
    (interp)))

;; run : String -> CFAE-Value
(define (run str)
  (interp-expr (compile (parse str) (mtCSub))))

;;-----------------------------------------------------------------------

(parse "{{{{fun {x} {fun {y} {fun {z} {+ z {+ y x}}}}} 10} 7} 13}")
(compile (parse "{{{{fun {x} {fun {y} {fun {z} {+ z {+ y x}}}}} 10} 7} 13}") (mtCSub))
(interp-expr (compile (parse "{{{{fun {x} {fun {y} {fun {z} {+ z {+ y x}}}}} 10} 7} 13}") (mtCSub)))





