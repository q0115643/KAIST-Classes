#lang plai

;(WAE? {add {num 2} {num 4}}) should say true
(define-type WAE
  [num (n number?)]
  [add (lhs WAE?) (rhs WAE?)]
  [sub (lhs WAE?) (rhs WAE?)]
  [with (name symbol?)
        (named-expr WAE?)
        (body WAE?)]
  [id (name symbol?)])
(test (WAE? {add {num 2} {num 4}}) #t)

;deletev: any value list -> list
;with input of a value and a list, to return a list omitting every element of the list that is equal to the value that's given
;(deletev 2 '(1 2 3 2 1)) should produce '(1 3 1)
(define (deletev v lst)
    (cond
    [(null? lst) '()]
    [(equal? v (first lst)) (deletev v (cdr lst))]
    [else (cons (first lst) (deletev v (cdr lst)))]
    ))
(test (deletev 2 '(1 2 3 2 1)) '(1 3 1))

;symbol<?: symbol symbol -> boolean
;to check if the two symbols are lexicographically sorted increasing
;(symbol<? 'apple 'juice) should say true
(define (symbol<? a b)
  (string<? (symbol->string a) (symbol->string b)))
(test (symbol<? 'apple 'juice) true)

;list-clean: list-of-sym -> list-of-sym
;to sort the list of symbols in lexicorgraphically increasing way, and to remove duplicates
;(list-clean '(b a a c b a)) should produce '(a b c)
(define (list-clean lst)
  (sort (remove-duplicates lst) symbol<?))
(test(list-clean '(b a a c b a)) '(a b c))

;free-ids: WAE -> list-of-sym
;with a WAE, to produce a list of free identifiers in the given WAE, the list will be ordered lexicorgraphically increasing way, without duplicates
(define (free-ids wae)
  (list-clean
   (type-case WAE wae
    [num (n) '()]
    [add (lhs rhs) (append (free-ids lhs) (free-ids rhs))]
    [sub (lhs rhs) (append (free-ids lhs) (free-ids rhs))]
    [with (name named-expr body) (append (free-ids named-expr) (deletev name (free-ids body)))]
    [id (name) (list name)])))

;binding-ids: WAE -> list-of-sym
;with a WAE, to produce a list of binding identifiers in the given WAE, the list will be ordered lexicorgraphically increasing way, without duplicates
(define (binding-ids wae)
  (list-clean
   (type-case WAE wae
     [num (n) '()]
     [add (lhs rhs) (append (binding-ids lhs) (binding-ids rhs))]
     [sub (lhs rhs) (append (binding-ids lhs) (binding-ids rhs))]
     [with (name named-expr body) (append (list name) (binding-ids named-expr) (binding-ids body))]
     [id (name) '()])))

;bound-ids: WAE -> list-of-sym
;with a WAE, to produce a list of bound identifiers in the given WAE, the list will be ordered lexicorgraphically increasing way, without duplicates
(define (bound-ids wae)
  (list-clean
   (type-case WAE wae
     [num (n) '()]
     [add (lhs rhs) (append (bound-ids lhs) (bound-ids rhs))]
     [sub (lhs rhs) (append (bound-ids lhs) (bound-ids rhs))]
     [with (name named-expr body) (append (bound-ids named-expr) (bound-ids body)
                                          (cond
                                            [(member name (free-ids body)) (list name)]
                                            [else '()]))]
     [id (name) '()])))

;test cases
;;free-ids
(test (free-ids (with 'x (num 3) (add (id 'x) (sub (num 3) (id 'x))))) '())
(test (free-ids (with 'x (num 3) (sub (id 'a) (add (num 4) (id 'x))))) '(a))
(test (free-ids (with 'x (num 3) (sub (id 'b) (sub (id 'a) (id 'x))))) '(a b))
(test (free-ids (with 'x (num 3) (sub (id 'a) (sub (id 'b) (add (id 'x) (id 'b)))))) '(a b))
(test (free-ids (with 'x (num 3) (sub (id 'y) (with 'y (num 7) (add (id 'x) (sub (id 'b) (id 'a))))))) '(a b y))
(test (free-ids (with 'x (id 't) (sub (id 'x) (with 'y (id 'y) (add (id 'x) (sub (id 'b) (id 'a))))))) '(a b t y))
(test (free-ids (with 'x (with 'y (num 3) (sub (id 'x) (id 'y))) (add (id 'x) (id 'y)))) '(x y))
(test (free-ids (add (with 'x (num 10) (with 'x (num 3) (sub (id 'y) (with 'y (num 7) (add (id 'x) (sub (id 'c) (id 'b))))))) (with 'a (id 'a) (id 'a)))) '(a b c y))
(test (free-ids (add (with 'x (num 10) (with 'x (num 3) (sub (id 'y) (with 'y (num 7) (add (id 'x) (sub (id 'c) (id 'b))))))) (with 'a (id 'd) (id 'a)))) '(b c d y))
(test (free-ids (add (with 'x (num 10) (with 'x (num 3) (sub (id 'y) (with 'y (num 7) (add (id 'x) (sub (id 'c) (id 'b))))))) (with 'a (id 'd) (id 'z)))) '(b c d y z))
;;binding-ids
(test (binding-ids (add (num 3) (sub (id 'x) (id 'y)))) '())
(test (binding-ids (with 'y (num 3) (with 'x (id 'x) (id 'y)))) '(x y))
(test (binding-ids (with 'y (num 3) (with 'y (id 'x) (add (id 'x) (id 'y))))) '(y))
(test (binding-ids (with 'y (num 3) (with 'y (with 'x (add (num 3) (id 'y)) (sub (id 'x) (id 'y))) (add (id 'x) (id 'y))))) '(x y))
(test (binding-ids (with 'z (num 3) (with 'w (with 'z (add (num 3) (id 'y)) (sub (id 'x) (id 'y))) (with 'w (id 'y) (add (num 7) (id 'w)))))) '(w z))
;;bound-ids
(test (bound-ids (with 'x (num 3) (add (id 'y) (num 3)))) '())
(test (bound-ids (with 'x (num 3) (add (id 'x) (sub (id 'x) (id 'y))))) '(x))
(test (bound-ids (with 'x (num 3) (add (id 'x) (with 'y (num 7) (sub (id 'x) (id 'y)))))) '(x y))
(test (bound-ids (with 'x (num 3) (with 'y (id 'x) (sub (num 3) (id 'y))))) '(x y))
(test (bound-ids (with 'x (num 3) (add (id 'y) (with 'y (id 'x) (sub (num 3) (num 7)))))) '(x))
(test (bound-ids (with 'x (id 'x) (add (id 'y) (with 'y (id 'y) (sub (num 3) (with 'z (num 7) (sub (id 'z) (id 'x)))))))) '(x z))
(test (bound-ids (with 'x (with 'y (num 3) (add (id 'x) (id 'y))) (add (id 'y) (with 'y (id 'y) (sub (num 3) (num 7)))))) '(y))
(test (bound-ids (with 'x (id 'a) (with 'y (id 'b) (with 'z (id 'c) (add (id 'd) (sub (id 'x) (add (id 'y) (id 'z)))))))) '(x y z))
(test (bound-ids (add (with 'x (num 10) (with 'x (num 3) (sub (id 'y) (with 'y (num 7) (add (id 'x) (sub (id 'c) (id 'b))))))) (with 'a (id 'd) (id 'a)))) '(a x))
(test (bound-ids (add (with 'x (num 10) (with 'x (num 3) (sub (id 'y) (with 'y (num 7) (add (id 'x) (sub (id 'c) (id 'b))))))) (with 'a (id 'd) (id 'z)))) '(x))