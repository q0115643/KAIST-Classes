#lang plai

;; BWAE abstract syntax trees                                               
(define-type BFAE
  [num (n number?)]
  [add (left BFAE?) (right BFAE?)]
  [sub (left BFAE?) (right BFAE?)]
  [id (name symbol?)]
  [app (ftn BFAE?) (args BFAE?)]
  [fun (param symbol?) (body BFAE?)]
  [newbox (val BFAE?)]
  [setbox (bx BFAE?) (val BFAE?)]
  [openbox (bx BFAE?)]
  [seqn (left (listof BFAE?))]
  [rec (idl (listof symbol?)) (vdl (listof BFAE?))]
  [get (record BFAE?) (name symbol?)]
  [sett (record BFAE?) (name symbol?) (val BFAE?)]
  )

;; Store abstract syntax trees
(define-type Store
  [mtSto]
  [aSto (address integer?) (value BFAE-Value?) (rest Store?)]
  [rSto (address integer?) (idl (listof symbol?)) (vdl (listof BFAE-Value?)) (rest Store?)])

;; BFAE-Value abstract syntax trees
(define-type BFAE-Value
  [numV        (n number?)]
  [closureV    (param symbol?) (body BFAE?) (ds DefrdSub?)]
  [boxV        (address integer?)]
  [recordV     (address integer?)])

;; Value*Store abstract syntax trees
(define-type Value*Store
  [v*s (value BFAE-Value?) (store Store?)])

;; DefrdSub abstract syntax trees
(define-type DefrdSub
  [mtSub]
  [aSub (name symbol?) (value BFAE-Value?) (rest DefrdSub?)])


;; max-address : Store -> integer
;; return the max-address in Store
; (max-address (mtSto)) should produce 0
(define (max-address st)
  (type-case Store st
    [mtSto () 0]
    [aSto (n v st) (max n (max-address st))]
    [rSto (n l r st) (max n (max-address st))]))
; test for max-address
(test (max-address (mtSto)) 0)

;; malloc : Store -> integer
;; give a free adress to use safely
;; (malloc (mtSto)) should produce 1
(define (malloc st)
  (+ 1 (max-address st)))
; test for malloc
(test (malloc (mtSto)) 1)

;; lookup : symbol DefrdSub -> BFAE-Value
;; find the symbol from DefrdSub and get the paired BFAE-Value
;; (lookup 'b (aSub 'b (numV 100) (mtSub))) should produce (numV 100)
(define (lookup name ds)
  (type-case DefrdSub ds
    [mtSub () (error 'lookup "no such field")]
    [aSub (x val rest)
          (if (symbol=? x name) val (lookup name rest))]))
; test to check lookup
(test (lookup 'b (aSub 'b (numV 100) (mtSub))) (numV 100))

;; parser : sexpr -> BFAE
;; to convert s-expressions into BFAE
; (parse 1) should produce (num 1)
(define (parse sexp)
  (match sexp
    [(? number?)                 (num sexp)]
    [(list '+ l r)               (add (parse l) (parse r))]
    [(list '- l r)               (sub (parse l) (parse r))]
    [(? symbol?)                 (id sexp)]
    [(list 'fun (list x) b)      (fun x (parse b))]
    [(list 'newbox e)            (newbox (parse e))]
    [(list 'setbox b e)          (setbox (parse b) (parse e))]
    [(list 'openbox e)           (openbox (parse e))]
    [(cons 'seqn f)              (seqn (map parse f))]
    [(list 'get r x)             (get (parse r) x)]
    [(cons 'rec a)               (helprec a)]
    [(list 'set r x v)           (sett (parse r) x (parse v))]
    [(list l r)                  (app (parse l) (parse r))]
    [else              (error 'parse "bad syntax: ~a" sexp)]))
; test for parse
(test (parse 1) (num 1))

;; helprec : listof tuple -> tupleof list
;; change array's dimention so that I can handle it easier
(define (helprec r)
  (cond
    [(empty? r) (rec empty empty)]
    [else       (rec (dup-fil (cons (first (car r)) (rec-idl (helprec (cdr r)))))
                  (cons (parse (second (car r))) (rec-vdl (helprec (cdr r)))))]))
; test for helprec
(test (helprec empty) (rec empty empty))

;; dup-fil : list -> list/error
;; error "duplicate fields" when there are duplicates in the list, otherwise, return the same list
;;(dup-fil (list 1 3 4 1)) should print "duplicate fields"
(define (dup-fil lst)
  (cond
    [(empty? lst) empty]
    [(member (first lst) (rest lst)) 
     (error 'parse "duplicate fields" lst)]
    [else (cons (first lst) (dup-fil (rest lst)))]))
; test for dup-fil
(test/exn (dup-fil (list 1 3 4 1)) "duplicate fields")

;; num-op : (number number -> number) -> (BFAE BFAE -> BFAE)
;; to have opration on numV value
;; (num+ (numV 3) (numV 102)) should produce (numV 105)
(define (num-op op)
  (lambda (x y)
    (numV (op (numV-n x) (numV-n y)))))
(define num+ (num-op +))
(define num- (num-op -))
; test for num-+
(test(num+ (numV 3) (numV 102))(numV 105))

;; interp-two : BFAE BFAE DefrdSub Store
;;              (Value Value Store -> Value*Store)
;;              -> Value*Store
;; for the comfort of using interp funtion with keeping changes of DefrdSub and Store
;; (interp-two (num 1) (num 2) (mtSub) (mtSto) (lambda (v1 v2 st1) (v*s (num+ v1 v2) st1)) should produce (v*s (numV 3) (mtSto))
(define (interp-two expr1 expr2 ds st handle)
  (type-case Value*Store (interp expr1 ds st)
    [v*s (val1 st2)
         [type-case Value*Store (interp expr2 ds st2)
           [v*s (val2 st3)
                (handle val1 val2 st3)]]]))
; test for interp-two is written below the interp function 

;; store-lookup : integer Store -> BFAE-Value
;; find and bring the BFAE-Value in Store of input address
;; (store-lookup 1 (aSto 1 (numV 12) (mtSto)) will produce (numV 12)
(define (store-lookup a st)
  (type-case Store st
    [mtSto () (error 'store-lookup "unallocated")]
    [aSto (n v rest)
          (if (eq? n a) v (store-lookup a rest))]
    [rSto (n sL vL rest) (store-lookup a rest)]
    ))
;test for store-lookup
(test (store-lookup 1 (aSto 1 (numV 12) (mtSto))) (numV 12))

;; rtore-lookup : symbol integer Store -> BFAE-Value
;; find and bring the BFAE-Value in Store of input address and matched with input symbol
;; (rtore-lookup HRK 10 (rSto 10 (list HHH HRR HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto))) should produce (numV 4)
(define (rtore-lookup s a st)
  (type-case Store st
    [mtSto () (error 'rtore-lookup "no such fields")]
    [aSto (n v rest) (rtore-lookup s a rest)]
    [rSto (n sL vL rest)
          (if (eq? n a)
              (if (eq? s (car sL))
                  (car vL)
                  (rtore-lookup s a (rSto n (cdr sL) (cdr vL) rest)))
              (rtore-lookup s a rest))]
    ))
; test for rtore-lookup
(test (rtore-lookup 'HRK 10 (rSto 10 (list 'HHH 'HRR 'HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto))) (numV 4))

;; insXV : integer symbol BFAE-Value Store Store -> Store
;; from the Store, go in to the address and stores the symbol and BFAE-Value
;; (insXV 12 'xxx (numV 4) (rSto 10 (list 'HHH 'HRR 'HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto)) (rSto 10 (list 'HHH 'HRR 'HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto))
;; should produce (rSto 12 '(xxx HHH HRR HRK) (list (numV 4) (numV 2) (numV 3) (numV 4)) (rSto 12 '(HHH HRR HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto)))
(define (insXV a x Val st ST)
  (type-case Store st
    [mtSto () (error 'set "no such fields")]
    [aSto (n v rest) (insXV a x Val rest ST)]
    [rSto (n sL vL rest)
          (if (eq? n a)
              (local [(define newSL (cons x sL)) (define newVL (cons Val vL))]
                (rSto a newSL newVL ST))
              (insXV a x Val rest ST))]))
; test for insXV
(test (insXV 12 'xxx (numV 4) (rSto 12 (list 'HHH 'HRR 'HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto)) (rSto 12 (list 'HHH 'HRR 'HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto)))
      (rSto 12 '(xxx HHH HRR HRK) (list (numV 4) (numV 2) (numV 3) (numV 4)) (rSto 12 '(HHH HRR HRK) (list (numV 2) (numV 3) (numV 4)) (mtSto))))

;; sameA : integer BFAE-Value Store -> Store
;; in the Store, if there's saved value with same address, overwrite the value
;; (sameA 2 (numV 3) (aSto 2 (numV 1234) (mtSto)) should produce (aSto 2 (numV 3) (mtSto))
(define (sameA a val st)
  (type-case Store st
    [mtSto () (mtSto)]
    [aSto (n v rest)
          (if (eq? n a)
              (aSto a val (sameA a val rest))
              (aSto n v (sameA a val rest)))]
    [rSto (n l r rest)
          (rSto n l r (sameA a val rest))])) 
; test for sameA
(test (sameA 2 (numV 3) (aSto 2 (numV 1234) (mtSto))) (aSto 2 (numV 3) (mtSto)))

;; interp : BFAE DefrdSub Store -> Value*Store
;; to compute the Value*Store from BFAE and DefrdSub and Store
;; (interp (parse '(seqn 1 2)) (mtSub) (mtSto)) should produce (v*s (numV 2) (mtSto))
(define (interp expr ds st)
  (type-case BFAE expr
    [num (n) (v*s (numV n) st)]    
    [add (l r) (interp-two l r ds st
                           (lambda (v1 v2 st1) (v*s (num+ v1 v2) st1)))]
    [sub (l r) (interp-two l r ds st
                           (lambda (v1 v2 st1) (v*s (num- v1 v2) st1)))]
    [id (name) (v*s (lookup name ds) st)]
    [fun (x b) (v*s (closureV x b ds) st)]
    [app (fun-expr arg-expr)
         (interp-two fun-expr arg-expr ds st
                     (lambda (fun-val arg-val st1)
                       (interp (closureV-body fun-val)
                               (aSub (closureV-param fun-val)
                                     arg-val
                                     (closureV-ds fun-val))
                               st1)))]
    [newbox (val)
            (type-case Value*Store (interp val ds st)
              [v*s (vl st1)
                   (local [(define a (malloc st1))]
                     (v*s (boxV a)
                          (aSto a vl st1)))])]
    [setbox (bx-expr val-expr)
            (interp-two bx-expr val-expr ds st
                        (lambda (bx-val val st1)
                          (local [(define newa (boxV-address bx-val))]
                            (v*s val
                                 (sameA newa
                                       val
                                       st1)))))]
    [openbox (bx-expr)
             (type-case Value*Store (interp bx-expr ds st)
               [v*s (bx-val st1)
                    (v*s (store-lookup (boxV-address bx-val)
                                       st1)
                         st1)])]
    [seqn (a) (interp-seqnL a ds st
                              (lambda (e ds1 st1)
                                (interp e ds1 st1)))]
    [rec (l r) (local [(define vL (interp-recB r ds st)) (define ST (interp-recBST r ds st))]
                 (local [(define ad (malloc ST))]
                   (v*s (recordV ad) (rSto ad l vL ST))))]
    [get (rc x) (type-case Value*Store (interp rc ds st)
                                               [v*s (val st2)
                                                    (v*s (rtore-lookup x (recordV-address val) st2) st2)])]
    [sett (rc x val) (type-case Value*Store (interp rc ds st)
                       [v*s (rV st2)  (type-case Value*Store (interp val ds st2)
                                        [v*s (Val ST)  (local [(define ad (recordV-address rV))]
                                                         (v*s Val (insXV ad x Val ST ST)))])])]
     ))
; test for interp
(test (interp (add (num 124) (num 1240)) (mtSub) (mtSto)) (v*s (numV 1364) (mtSto)))
; test for interp-two
(test (interp-two (num 1) (num 2) (mtSub) (mtSto) (lambda (v1 v2 st1) (v*s (num+ v1 v2) st1))) (v*s (numV 3) (mtSto)))
                           
;; interp-seqnL : (Listof BFAE) DefrdSub Store function -> Value*Store
;; gets the list of sequence BPAE exprs, DefrdSub, Store, and lambda function to compute the interpreted Value*Store form of sequence
;; (interp (parse '(seqn 1 2)) (mtSub) (mtSto)) should produce (v*s (numV 2) (mtSto))
(define (interp-seqnL exprL ds st handle)
  (if (empty? exprL) (error 'parse "bad syntax: ~a" seqn)
      (if (eq? (length exprL) 1) (handle (car exprL) ds st)
          (type-case Value*Store (interp (car exprL) ds st)
            [v*s (val1 st2)
                 (interp-seqnL (cdr exprL) ds st2 handle)]))))
; test for interp-seqnL
(test (interp (parse '(seqn 1 2)) (mtSub) (mtSto)) (v*s (numV 2) (mtSto)))

;; interp-recB : (Listof BFAE) DefrdSub Store -> (Listof BFAE-Value)
;; withe the listof BFAE exprs and DefrdSub and Store, make the list of all interpreted BFAE-Values
;; (interp-recB (list (num 5) (num 3) (add (num 3) (num 12))) (mtSub) (mtSto)) will produce (list (numV 5) (numV 3) (numV 15))
(define (interp-recB bfaeL ds st)
  (if (empty? bfaeL) empty
      (type-case Value*Store (interp (car bfaeL) ds st)
        [v*s (val st2)
             (cons val (interp-recB (cdr bfaeL) ds st2))])))
; test for interp-recB
(test (interp-recB (list (num 5) (num 3) (add (num 3) (num 12))) (mtSub) (mtSto)) (list (numV 5) (numV 3) (numV 15)))

;; interp-recBST : (Listof BFAE) DefrdSub Store -> Store
;; withe the listof BFAE exprs and DefrdSub and Store, get the result Store when you interpret all BFAE-Values in bfaeL
;; (interp-recBST (list (num 5) (num 3) (add (num 3) (num 12))) (mtSub) (mtSto)) will produce (mtSub)
(define (interp-recBST bfaeL ds st)
  (if (empty? bfaeL) st
      (type-case Value*Store (interp (car bfaeL) ds st)
        [v*s (val st2)
             (interp-recBST (cdr bfaeL) ds st2)])))
; test for interp-recBST
(test (interp-recBST (list (num 5) (num 3) (add (num 3) (num 12))) (mtSub) (mtSto)) (mtSto))

;; interp-expr : BFAE -> integer/symbol
;; withe BFAE expr, use mtSub and mtSto to interpret it to Value*Store and from that, answer as a number or symbols refers to it
;; (interp-expr (parse '(fun (x) x))) should produce 'func
(define (interp-expr expr)
  (type-case Value*Store (interp expr (mtSub) (mtSto))
    [v*s (v st)   (type-case BFAE-Value v
                    [numV (n) n]
                    [closureV (s n d) 'func]
                    [boxV (a) 'box]
                    [recordV (a) 'record])]))
; test for interp-expr
(test (interp-expr (parse '(fun (x) x))) 'func)

; test for interp
(test (interp (parse '(seqn 1 2)) (mtSub) (mtSto)) (v*s (numV 2) (mtSto)))

; test-cases
(test (interp-expr (parse '{{fun {b} {seqn
                                      {setbox b 12}
                                      {openbox b}}}
                            {newbox 10}}))
      12)
(test (interp (parse '{seqn 1 2})
              (mtSub)
              (mtSto))
      (v*s (numV 2) (mtSto)))

(test (interp (parse '{{fun {b}
                          {seqn
                           {setbox b 2}
                           {openbox b}}}
                         {newbox 1}})
                (mtSub)
                (mtSto))
        (v*s (numV 2)
             (aSto 1 (numV 2) (mtSto))))
(test (interp (parse '{{fun {b}
                          {seqn
                           {setbox b {+ 2 {openbox b}}}
                           {setbox b {+ 3 {openbox b}}}
                           {setbox b {+ 4 {openbox b}}}
                           {openbox b}}}
                         {newbox 1}})
                (mtSub)
                (mtSto))
        (v*s (numV 10)
             (aSto 1 (numV 10) (mtSto))))
(test (interp (parse '{seqn 1 2})
              (mtSub)
              (mtSto))
      (v*s (numV 2) (mtSto)))

(test (interp (parse '{{fun {b} {openbox b}}
                       {newbox 10}})
              (mtSub)
              (mtSto))
      (v*s (numV 10)
           (aSto 1 (numV 10) (mtSto))))

(test (interp (parse '{{fun {b} {seqn
                                 {setbox b 12}
                                 {openbox b}}}
                       {newbox 10}})
              (mtSub)
              (mtSto))
      (v*s (numV 12)
           (aSto 1
                 (numV 12)
                 (mtSto))))
(test (interp (parse '{{fun {b} {openbox b}}
                       {seqn
                        {newbox 9}
                        {newbox 10}}})
              (mtSub)
              (mtSto))
      (v*s (numV 10)
           (aSto 2 (numV 10)
                 (aSto 1 (numV 9) (mtSto)))))

(test (interp (parse '{{{fun {b}
                             {fun {a}
                                  {openbox b}}}
                        {newbox 9}}
                       {newbox 10}})
              (mtSub)
              (mtSto))
      (v*s (numV 9)
           (aSto 2 (numV 10)
                 (aSto 1 (numV 9) (mtSto)))))
(test (interp (parse '{{fun {b}
                            {seqn
                             {setbox b 2}
                             {openbox b}}}
                       {newbox 1}})
              (mtSub)
              (mtSto))
      (v*s (numV 2)
           (aSto 1 (numV 2) (mtSto))))

(test (interp (parse '{{fun {b}
                            {seqn
                             {setbox b {+ 2 (openbox b)}}
                             {setbox b {+ 3 (openbox b)}}
                             {setbox b {+ 4 (openbox b)}}
                             {openbox b}}}
                       {newbox 1}})
              (mtSub)
              (mtSto))
        (v*s (numV 10)
             (aSto 1 (numV 10) (mtSto))))


(test/exn (interp (parse '{openbox x})
                  (aSub 'x (boxV 1) (mtSub))
                  (mtSto))
          "unallocated")
;; records

(test (interp-expr (parse '{{fun {r}
                                 {get r x}}
                            {rec {x 1}}}))
      1)

(test (interp-expr (parse '{{fun {r}
                                 {seqn
                                  {set r x 5}
                                  {get r x}}}
                            {rec {x 1}}}))
      5)
(test (interp-expr (parse '{{{{{fun {g}
                                    {fun {s}
                                         {fun {r1}
                                              {fun {r2}
                                                   {+ {get r1 b}
                                                      {seqn
                                                       {{s r1} {g r2}}
                                                       {+ {seqn
                                                           {{s r2} {g r1}}
                                                           {get r1 b}}
                                                          {get r2 b}}}}}}}}
                               {fun {r} {get r a}}}            ; g
                              {fun {r} {fun {v} {set r b v}}}} ; s
                             {rec {a 0} {b 2}}}                ; r1
                            {rec {a 3} {b 4}}}))               ; r2
      5)

(test (interp-expr (parse '{fun {x} x}))
      'func)
(test (interp-expr (parse '{newbox 1}))
      'box)
(test (interp-expr (parse '{rec}))
      'record)
(test (interp-expr (parse '{{fun {r} {seqn {set r x 5} {get r x}}} {rec {x 1}}})) 5)
(test (interp-expr (parse '{{fun {r} {seqn {set r x 10} {rec {y 3} {x 1}} {get r x}}} {rec {y 2} {x 1}}})) 10)
;; given tests (17)
(test (interp (parse '{{fun {b} {seqn {setbox b 2} {openbox b}}} {newbox 1}}) (mtSub) (mtSto)) (v*s (numV 2) (aSto 1 (numV 2) (mtSto))))
(test (interp (parse '{{fun {b} {seqn {setbox b {+ 2 {openbox b}}} {setbox b {+ 3 {openbox b}}} {setbox b {+ 4 {openbox b}}} {openbox b}}} {newbox 1}}) (mtSub) (mtSto)) (v*s (numV 10) (aSto 1 (numV 10) (mtSto))))
(test (interp (parse '{seqn 1 2}) (mtSub) (mtSto)) (v*s (numV 2) (mtSto)))
(test (interp (parse '{{fun {b} {openbox b}} {newbox 10}}) (mtSub) (mtSto)) (v*s (numV 10) (aSto 1 (numV 10) (mtSto))))
(test (interp (parse '{{fun {b} {seqn {setbox b 12} {openbox b}}} {newbox 10}}) (mtSub) (mtSto)) (v*s (numV 12) (aSto 1 (numV 12) (mtSto))))
(test (interp-expr (parse '{{fun {b} {seqn {setbox b 12} {openbox b}}} {newbox 10}})) 12)
(test (interp (parse '{{fun {b} {openbox b}} {seqn {newbox 9} {newbox 10}}}) (mtSub) (mtSto)) (v*s (numV 10) (aSto 2 (numV 10) (aSto 1 (numV 9) (mtSto)))))
(test (interp (parse '{{{fun {b} {fun {a} {openbox b}}} {newbox 9}} {newbox 10}}) (mtSub) (mtSto)) (v*s (numV 9) (aSto 2 (numV 10) (aSto 1 (numV 9) (mtSto)))))
(test (interp (parse '{{fun {b} {seqn {setbox b 2} {openbox b}}} {newbox 1}}) (mtSub) (mtSto)) (v*s (numV 2) (aSto 1 (numV 2) (mtSto))))
(test (interp (parse '{{fun {b} {seqn {setbox b {+ 2 (openbox b)}} {setbox b {+ 3 (openbox b)}} {setbox b {+ 4 (openbox b)}} {openbox b}}} {newbox 1}}) (mtSub) (mtSto)) (v*s (numV 10) (aSto 1 (numV 10) (mtSto))))
(test (interp-expr (parse '{{fun {r} {get r x}} {rec {x 1}}})) 1)
(test (interp-expr (parse '{{fun {r} {seqn {set r x 5} {get r x}}} {rec {x 1}}})) 5)
(test (interp-expr (parse '{{{{{fun {g} {fun {s} {fun {r1} {fun {r2} {+ {get r1 b} {seqn {{s r1} {g r2}} {+ {seqn {{s r2} {g r1}} {get r1 b}} {get r2 b}}}}}}}} {fun {r} {get r a}}} {fun {r} {fun {v} {set r b v}}}} {rec {a 0} {b 2}}} {rec {a 3} {b 4}}})) 5)
(test (interp-expr (parse '{fun {x} x})) 'func)
(test (interp-expr (parse '{newbox 1})) 'box)
(test (interp-expr (parse '{rec})) 'record)
;;exception
(test/exn (interp (parse '{openbox x}) (aSub 'x (boxV 1) (mtSub)) (mtSto)) "unallocated")

;; additional tests (33)
(test (interp (parse '{{fun {b} {setbox b 2}} {seqn {newbox 0} {newbox 1}}}) (mtSub) (mtSto)) (v*s (numV 2) (aSto 2 (numV 2) (aSto 1 (numV 0) (mtSto)))))
(test (interp (parse '{{fun {b} {setbox b 2}} {newbox 0}}) (mtSub) (aSto 1 (numV 0) (mtSto))) (v*s (numV 2) (aSto 2 (numV 2) (aSto 1 (numV 0) (mtSto)))))
(test (interp (parse '{{{fun {a} {fun {b} {setbox a 2}}} {newbox 1}} {newbox 0}}) (mtSub) (mtSto)) (v*s (numV 2) (aSto 2 (numV 0) (aSto 1 (numV 2) (mtSto)))))
(test (interp (parse '{+ {{fun {b} {setbox b 2}} {newbox 0}} {{fun {b} {setbox b 2}} {newbox 1}}}) (mtSub) (mtSto)) (v*s (numV 4) (aSto 2 (numV 2) (aSto 1 (numV 2) (mtSto)))))
(test (interp (parse '{newbox {{fun {b} {setbox b 2}} {newbox 0}}}) (mtSub) (mtSto)) (v*s (boxV 2) (aSto 2 (numV 2) (aSto 1 (numV 2) (mtSto)))))
(test (interp (parse '{openbox {{fun {b} {seqn {setbox b 2} {newbox {fun {a} {setbox a 3}}}}} {newbox 0}}}) (mtSub) (mtSto)) (v*s (closureV 'a (setbox (id 'a) (num 3)) (aSub 'b (boxV 1) (mtSub))) (aSto 2 (closureV 'a (setbox (id 'a) (num 3)) (aSub 'b (boxV 1) (mtSub))) (aSto 1 (numV 2) (mtSto)))))
(test (interp (parse '{{openbox {{fun {b} {seqn {setbox b 2} {newbox {fun {a} {setbox a 3}}}}} {newbox 0}}} {newbox 1}}) (mtSub) (mtSto)) (v*s (numV 3) (aSto 3 (numV 3) (aSto 2 (closureV 'a (setbox (id 'a) (num 3)) (aSub 'b (boxV 1) (mtSub))) (aSto 1 (numV 2) (mtSto))))))
(test (interp (parse '{seqn {newbox 0} {setbox x 1} {openbox x}}) (aSub 'x (boxV 1) (mtSub)) (aSto 1 (numV 0) (mtSto))) (v*s (numV 1) (aSto 2 (numV 0) (aSto 1 (numV 1) (mtSto)))))
(test (interp (parse '{{fun {b} {+ {openbox b} {seqn {setbox b 2} {openbox b}}}} {newbox 1}}) (mtSub) (mtSto)) (v*s (numV 3) (aSto 1 (numV 2) (mtSto))))
(test (interp (parse '{{fun {a} {{fun {b} {seqn {setbox b {- {openbox b} 1}} {setbox a {+ {openbox a} 1}} {newbox 0} {openbox b}}} {newbox 1}}} {newbox 2}}) (aSub 'a (boxV 0) (mtSub)) (mtSto)) (v*s (numV 0) (aSto 3 (numV 0) (aSto 2 (numV 0) (aSto 1 (numV 3) (mtSto))))))
(test (interp (parse '{seqn {newbox 1}}) (mtSub) (mtSto)) (v*s (boxV 1) (aSto 1 (numV 1) (mtSto))))
(test (interp (parse '{setbox {{fun {b} {seqn {newbox b} {newbox b}}} 0} 1}) (mtSub) (mtSto)) (v*s (numV 1) (aSto 2 (numV 1) (aSto 1 (numV 0) (mtSto)))))
(test (interp (parse '{{fun {a} {{fun {b} {seqn {setbox b 2} {setbox a {fun {c} {+ c 1}}} {{openbox a} {openbox b}}}} {openbox a}}} {newbox {newbox 1}}}) (mtSub) (mtSto)) (v*s (numV 3) (aSto 2 (closureV 'c (add (id 'c) (num 1)) (aSub 'b (boxV 1) (aSub 'a (boxV 2) (mtSub)))) (aSto 1 (numV 2) (mtSto)))))
(test (interp (parse '{seqn 1 {fun {x} {+ x 1}} {newbox 2} {{fun {x} {setbox x {newbox 1}}} {newbox 3}}}) (mtSub) (mtSto)) (v*s (boxV 3) (aSto 3 (numV 1) (aSto 2 (boxV 3) (aSto 1 (numV 2) (mtSto))))))
(test (interp (parse '{{fun {b} {seqn {setbox b b} {openbox b}}} {newbox 0}}) (mtSub) (mtSto)) (v*s (boxV 1) (aSto 1 (boxV 1) (mtSto))))
(test (interp (parse '{{fun {b} {openbox {setbox b b}}} {newbox 0}}) (mtSub) (mtSto)) (v*s (boxV 1) (aSto 1 (boxV 1) (mtSto))))
(test (interp (parse '{{fun {b} {- {openbox b} {seqn {setbox b b} {setbox {openbox b} 1} {openbox b}}}} {newbox 0}}) (mtSub) (mtSto)) (v*s (numV -1) (aSto 1 (numV 1) (mtSto))))
(test (interp-expr (parse '{{fun {b} {{fun {a} {seqn {set a x {openbox b}} {setbox b 1} {set a y {openbox b}} {get a x}}} {rec {x 1} {y 2}}}} {newbox 0}})) 0)
(test (interp-expr (parse '{set {rec {x 1}} x 0})) 0)
(test (interp-expr (parse '{{fun {r} {seqn {setbox {get r x} 1} {get r y}}} {{fun {b} {rec {x b} {y {openbox b}}}} {newbox 0}}})) 0)
(test (interp-expr (parse '{{fun {r} {seqn {setbox {get r x} 1} {get r y}}} {{fun {b} {rec {x b} {y {openbox b}}}} {newbox 0}}})) 0)
(test (interp-expr (parse '{{fun {r1} {{fun {r} {seqn {set r x 0} {get r1 x}}} {rec {x 1} {y 2}}}} {rec {x 3} {y 4}}})) 3)
(test (interp-expr (parse '{{fun {r} {+ {get r x} {seqn {set r x 2}}}} {rec {z 3} {y 2} {x 1}}})) 3)
(test (interp-expr (parse '{{fun {b} {seqn {set {openbox b} y {newbox {openbox b}}} {openbox {get {openbox b} y}}}} {newbox {rec {x 1} {y 2}}}})) 'record)
(test (interp-expr (parse '{{fun {b} {seqn {set {openbox b} y {newbox {openbox b}}} {get {openbox {get {openbox b} y}} y}}} {newbox {rec {x 1} {y 2}}}})) 'box)
(test (interp-expr (parse '{{fun {r} {seqn {setbox {get r x} 2} {openbox {get r x}}}} {rec {x {newbox 0}}}})) 2)
(test (interp-expr (parse '{{fun {r} {seqn {setbox {get r x} 2} {openbox {get r x}}}} {rec {x {newbox 0}}}})) 2)
(test (interp-expr (parse '{{fun {r} {+ {setbox {get r x} 2} {openbox {get r x}}}} {rec {x {newbox 0}}}})) 4)
;;exception
(test/exn (interp (parse '{seqn {setbox x 1} {openbox x}}) (aSub 'x (boxV 1) (mtSub)) (mtSto)) "unallocated")
(test/exn (interp (parse '{seqn}) (mtSub) (mtSto)) "bad syntax")
(test/exn (interp-expr (parse '{{fun {a} {set a y {newbox 0}}} {rec {x 1}}})) "no such field")
(test/exn  (interp-expr (parse '{set {rec {x 1} {x {newbox 1}}} x {newbox 1}})) "duplicate fields")
(test/exn (interp-expr (parse '{{fun {r} {get r x}} {rec}})) "no such field")
