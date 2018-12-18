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

;; FWAE abstract syntax trees                                               
(define-type FWAE
  [num (n number?)]
  [add (left FWAE?) (right FWAE?)]
  [sub (left FWAE?) (right FWAE?)]
  [with (name symbol?) (named-expr FWAE?) (body FWAE?)]
  [id (name symbol?)]
  [app (ftn FWAE?) (args (listof FWAE?))]
  [fun (param (listof symbol?)) (body FWAE?)]
  [rec (idl (listof symbol?)) (vdl (listof FWAE?))]
  [get (record FWAE?) (name symbol?)]
  )

;; FWAE-Value abstract syntax trees
(define-type FWAE-Value
  [numV        (n number?)]
  [closureV    (param (listof symbol?)) (body FWAE?) (ds DefrdSub?)]
  [recordV     (idl (listof symbol?)) (vdl (listof FWAE-Value?)) (ds DefrdSub?)])

;; DefrdSub abstract syntax trees
(define-type DefrdSub
  [mtSub]
  [aSub (name symbol?) (value FWAE-Value?) (rest DefrdSub?)])
    
;; lookup : symbol DefrdSub -> FWAE-Value
;; find the symbol from DefrdSub and get the paired FWAE-Value
;; (lookup 'b (aSub 'b (numV 100) (mtSub))) should produce (numV 100)
(define (lookup name ds)
  (type-case DefrdSub ds
    [mtSub () (error 'lookup "no such field")]
    [aSub (x val rest)
          (if (symbol=? x name) val (lookup name rest))]))
;test to check lookup
(test (lookup 'b (aSub 'b (numV 100) (mtSub))) (numV 100))


;; dup-bad : list -> list/error
;; error "bad syntax" when there are duplicates in the list, otherwise, return the same list
;;(dup-bad (list 1 3 4 1)) should print "bad syntax"
(define (dup-bad lst)
  (cond
    [(empty? lst) empty]
    [(member (first lst) (rest lst)) 
     (error 'parse "bad syntax: ~lst" lst)]
    [else (cons (first lst) (dup-bad (rest lst)))]))
;test to check dup-bad
(test/exn (dup-bad (list 1 3 4 1)) "bad syntax")

;; dup-fil : list -> list/error
;; error "duplicate fields" when there are duplicates in the list, otherwise, return the same list
;;(dup-bad (list 1 3 4 1)) should print "duplicate fields"
(define (dup-fil lst)
  (cond
    [(empty? lst) empty]
    [(member (first lst) (rest lst)) 
     (error 'parse "duplicate fields" lst)]
    [else (cons (first lst) (dup-fil (rest lst)))]))
;test to check dup-fil
(test/exn (dup-fil (list 1 3 4 1)) "duplicate fields")

;helprec : listof tuple -> tupleof list
;change array's dimention so that I can handle it easier
(define (helprec r)
  (cond
    [(empty? r) (rec empty empty)]
    [else       (rec (dup-fil (cons (first (car r)) (rec-idl (helprec (cdr r)))))
                  (cons (parse-sexpr (second (car r))) (rec-vdl (helprec (cdr r)))))]))

; parse-sexpr : sexpr -> FWAE
;; to convert s-expressions into FWAE
; (parse-sexpr '(1 2 3) should produce (num '(1 2 3))
(define (parse-sexpr sexp)
  (match sexp
    [(? number?)                 (num sexp)]
    [(list '+ l r)               (add (parse-sexpr l) (parse-sexpr r))]
    [(list '- l r)               (sub (parse-sexpr l) (parse-sexpr r))]
    [(list 'with (list x i) b)   (with x (parse-sexpr i) (parse-sexpr b))]
    [(? symbol?)                 (id sexp)]
    [(list 'fun x b)             (fun (dup-bad x) (parse-sexpr b))]
    [(cons f a)          (cond
                           [(eq? f 'get) (get (parse-sexpr (first a)) (second a))]
                           [(eq? f 'rec) (helprec a)]
                           [else (app (parse-sexpr f)(map parse-sexpr (cdr sexp)))])]
    [else (error 'parse "bad syntax: ~a" sexp)]))
;test to check parse-sexpr
(test (parse-sexpr 1) (num 1))
(test (parse-sexpr '(+ 1 3)) (add (num 1) (num 3)))

;; parses a string containing an FWAE expression to an FWAE
;; (parse "{get {rec {r {rec {z 0}}}} r}") should produce (get (rec '(r) (list (rec '(z) (list (num 0))))) 'r)
(define (parse str)
  (parse-sexpr (string->sexpr str)))
;test for parse
(test (parse "{get {rec {r {rec {z 0}}}} r}") (get (rec '(r) (list (rec '(z) (list (num 0))))) 'r))

;; num-op : (number number -> number) -> (FWAE FWAE -> FWAE)
;; to have opration on numV value
;; (num+ (numV 3) (numV 102)) should produce (numV 105)
(define (num-op op)
  (lambda (x y)
    (numV (op (numV-n x) (numV-n y)))))
(define num+ (num-op +))
(define num- (num-op -))
;test for num-op
(test (num+ (numV 3) (numV 102)) (numV 105))

;; helpapp : DefrdSub (listof FWAE) (listof symbol) -> DefrdSub
;; to stack list of paired FWAE and symbol data to DefrdSub at once
(define (helpapp ds p a)
  (if (= (length p) (length a))
      (cond
        [(empty? p) ds]
        [else (aSub (car p) (interp (car a) ds) (helpapp ds (cdr p) (cdr a)))])
      (error 'interp "wrong arity" p a)))

;; inds : DefrdSub (listof FWAE-Value) (listof symbol) -> DefrdSub
;; to stack list of paired FWAE-Value and symbol data to DefrdSub at once
(define (inds ds p v)
  (if (eq? (length p) (length v))
      (cond
        [(empty? p) ds]
        [else (aSub (car p) (car v) (inds ds (cdr p) (cdr v)))])
      (error 'interp "wrong arity" p v)))

;; interp : FWAE DefrdSub -> FWAE-Value
;; to compute the FWAE-Value from FWAE and DefrdSub
;; (interp (add (num 3) (num 19)) (mtSub)) should produce (numV 22)
(define (interp fae ds)
  (type-case FWAE fae
    [num (n) (numV n)]    
    [add (l r) (num+ (interp l ds) (interp r ds))]
    [sub (l r) (num- (interp l ds) (interp r ds))]
    [with (x i b)   (interp b (aSub x (interp i ds) ds))]
    [id (s) (lookup s ds)]
    [fun (x b) (closureV x b ds)]
    [app (f a) (local [(define f-val (interp f ds))]
                 (interp (closureV-body f-val)
                         (helpapp ds (closureV-param f-val) a)))]
    [rec (l r) (recordV l (interplst r ds) (inds (mtSub) l (interplst r ds)))]
    [get (rc x) (lookup x (recordV-ds (interp rc ds)))]))
;test for interp
(test (interp (add (num 3) (num 19)) (mtSub)) (numV 22))

;; interplst : listof FWAE -> listof FWAE-Value
;; to interp with listof FWAE at once
;; (interplst  (list (add (num 1) (num 4)) (sub (num 1) (num 4))) (mtSub)) should produce (list (numV 5) (numV -3))
(define (interplst r ds)
  (cond
    [(empty? r) empty]
    [else (cons (interp (car r) ds) (interplst (rest r) ds))]))
;test for interplst
(test (interplst  (list (add (num 1) (num 4)) (sub (num 1) (num 4))) (mtSub)) (list (numV 5) (numV -3)))

;; run : string -> FWAE-Value/'function/'record
;; parses and interprets a sexpr type string to get FWAE-Value or 'funtion or 'record
;; (run "{with {f {fun {a b c} {rec {a a} {b b} {c c}}}} {get {f 1 2 3} b}}") should produce 2
(define (run str)
  (type-case FWAE-Value (interp (parse str) (mtSub))
    [numV (n) n]
    [closureV (s n d) 'function]
    [recordV (i v d) 'record]))
;test for run
(test (run "{with {f {fun {a b c} {rec {a a} {b b} {c c}}}}
                  {get {f 1 2 3} b}}") 2)


;; test cases
(test (parse "{fun {x y} {+ x y}}") (fun '(x y) (add (id 'x) (id 'y))))
(test (run "{fun {x} x}") 'function)
(test (run "{with {f {fun {a b} {+ a b}}}
                  {with {g {fun {x} {- x 5}}}
                        {with {x {f 2 5}} {g x}}}}") 2)
(test (run "{with {f {fun {x y} {+ x y}}} {f 1 2}}") 3)
(test (run "{with {f {fun {} 5}}
                  {+ {f} {f}}}") 10)
(test (run "{with {h {fun {x y z w} {+ x w}}}
                  {h 1 4 5 6}}") 7) 
(test (run "{with {f {fun {} 4}}
                  {with {g {fun {x} {+ x x}}}
                        {with {x 10} {- {+ x {f}} {g 4}}}}}") 6)
(test (run "{rec {a 10} {b {+ 1 2}}}")
      'record)
(test (run "{get {rec {a 10} {b {+ 1 2}}} b}")
      3)
(test/exn (run "{get {rec {b 10} {b {+ 1 2}}} b}")
          "duplicate fields")
(test/exn (run "{get {rec {a 10}} b}")
          "no such field")
(test (run "{with {g {fun {r} {get r c}}}
                  {g {rec {a 0} {c 12} {b 7}}}}")
      12)
(test (run "{get {rec {r {rec {z 0}}}} r}")
      'record)
(test (run "{get {get {rec {r {rec {z 0}}}} r} z}")
      0)
(test/exn (run "{rec {z {get {rec {z 0}} y}}}")
          "no such field")
(test (run "{with {f {fun {a b} {+ a b}}}
                  {with {g {fun {x} {- x 5}}}
                        {with {x {f 2 5}} {g x}}}}") 2)
(test (run "{with {f {fun {x y} {+ x y}}} {f 1 2}}") 3)
(test (run "{with {f {fun {} 5}}
                  {+ {f} {f}}}") 10)
(test (run "{with {h {fun {x y z w} {+ x w}}}
                  {h 1 4 5 6}}") 7) 
(test (run "{with {f {fun {} 4}}
                  {with {g {fun {x} {+ x x}}}
                        {with {x 10} {- {+ x {f}} {g 4}}}}}") 6)
(test (run "{rec {a 10} {b {+ 1 2}}}") 'record)
(test (run "{get {rec {r {rec {z 0}}}} r}") 'record)
(test (run "{get {get {rec {r {rec {z 0}}}} r} z}") 0)
(test (run "{with {x 3} {with {y 5} {get {rec {a x} {b y}} a}}}") 3)
(test (run "{with {f {fun {a b} {+ {get a a} b}}}
                  {with {g {fun {x} {+ 5 x}}}
                        {with {x {f {rec {a 10} {b 5}} 2}} {g x}}}}") 17)
(test (run "{with {f {fun {a b c d e} {rec {a a} {b b} {c c} {d d} {e e}}}}
                  {get {f 1 2 3 4 5} c}}") 3)
(test (run "{with {f {fun {a b c} {rec {a a} {b b} {c c}}}}
                  {get {f 1 2 3} b}}") 2)
(test (run "{with {f {fun {a b c} {rec {x a} {y b} {z c} {d 2} {e 3}}}}
                  {get {f 1 2 3} y}}") 2)
(test (run "{with {f {fun {a b c} {rec {x a} {y b} {z c} {d 2} {e 3}}}}
                  {get {f 1 2 3} d}}") 2)
(test (run "{with {f {fun {x} {+ 5 x}}}
                  {f {get {get {rec {a {rec {a 10} {b {- 5 2}}}} {b {get {rec {x 50}} x}}} a} b}}}") 8)
(test (run "{get {rec {a 10} {b {+ 1 2}}} b}") 3)
(test (run "{get {rec {r {rec {z 0}}}} r}") 'record)
(test (run "{get {get {rec {r {rec {z 0}}}} r} z}") 0)
(test (run "{rec {a 10}}") `record)
(test (run "{get {rec {a 10}} a}") 10)
(test (run "{get {rec {a {+ 1 2}}} a}") 3)
(test (run "{fun {x} x}") 'function)
(test (run "{get {rec {a {rec {b 10}}}} a}") `record)
(test (run "{get {get {rec {a {rec {a 10}}}} a} a}") 10)
(test (run "{get {get {rec {a {rec {a 10} {b 20}}}} a} a}") 10)
(test (run "{get {get {rec {a {rec {a 10} {b 20}}}} a} b}") 20)
(test (run "{+ {get {rec {a 10}} a} {get {rec {a 20}} a}}") 30)
(test (run "{+ {get {rec {a 10}} a} {get {rec {a 20}} a}}") 30)
(test (run "{rec {a 10}}") `record)
(test (run "{rec {a {- 2 1}}}") `record)
(test (run "{get {rec {a 10}} a}") 10)
(test (run "{get {rec {a {- 2 1}}} a}") 1)
(test (run "{get {rec {a {rec {b 10}}}} a}") `record)
(test (run "{get {get {rec {a {rec {a 10}}}} a} a}") 10)
(test (run "{get {get {rec {a {rec {a 10} {b 20}}}} a} a}") 10)
(test (run "{get {get {rec {a {rec {a 10} {b 20}}}} a} b}") 20)
(test (run "{get {rec {r {rec {z 0}}}} r}") 'record)
(test (run "{get {get {rec {r {rec {z 0}}}} r} z}") 0)
(test (run "{with {y {rec {x 1} {y 2} {z 3}}} {get y y}}") 2)
(test (run "{with {y {rec {x 1} {y 2} {z 3}}} {get y z}}") 3)
(test (run "{rec {a 10} {b {+ 1 2}}}") 'record)
(test (run "{get {rec {a 10} {b {+ 1 2}}} b}") 3)
(test (run "{with {g {fun {r} {get r c}}}
                  {g {rec {a 0} {c 12} {b 7}}}}") 12)
(test (run "{get {rec {r {rec {z 0}}}} r}") 'record)
(test (run "{get {get {rec {r {rec {z 0}}}} r} z}") 0)