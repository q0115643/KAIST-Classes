#lang plai-typed

;; EXPR abstract syntax trees
(define-type EXPR
  [num (n : number)]
  [bool (b : boolean)]
  [add (lhs : EXPR) (rhs : EXPR)]
  [sub (lhs : EXPR) (rhs : EXPR)]
  [equ (lhs : EXPR) (rhs : EXPR)]
  [id (name : symbol)]
  [fun (param : symbol) (paramty : TE) (body : EXPR)]
  [app (fun-expr : EXPR) (arg-expr : EXPR)]
  [ifthenelse (test-expr : EXPR) (then-expr : EXPR) (else-expr : EXPR)]
  [rec (name : symbol) (ty : TE) (named-expr : EXPR) (body : EXPR)]
  [with-type (name : symbol)
             (var1-name : symbol) (var1-ty : TE)
             (var2-name : symbol) (var2-ty : TE)
             (body-expr : EXPR)]
  [cases (name : symbol)
         (dispatch-expr : EXPR)
         (var1-name : symbol) (bind1-name : symbol) (rhs1-expr : EXPR)
         (var2-name : symbol) (bind2-name : symbol) (rhs2-expr : EXPR)]
  [tfun (name : symbol) (expr : EXPR)]
  [tapp (body : EXPR) (type : TE)])

;; TE abstract syntax tree
(define-type TE
  [numTE]
  [boolTE]
  [arrowTE (param : TE) (result : TE)]
  [polyTE (forall : symbol) (body : TE)]
  [idTE (name : symbol)]
  [tvTE (name : symbol)])

;; Type abstract syntax tree
(define-type Type
  [numT]
  [boolT]
  [arrowT (param : Type) (result : Type)]
  [polyT (forall : symbol) (body : Type)]
  [idT (name : symbol)]
  [tvT (name : symbol)])

;; CType abstract syntax tree
(define-type CType
  [cnumT]
  [cboolT]
  [carrowT (param : CType) (result : CType)]
  [cpolyT (body : CType)]
  [cidT (name : symbol)]
  [ctvT (n : number)])


;; EXPR-Value abstract syntax tree
(define-type EXPR-Value
  [numV (n : number)]
  [boolV (b : boolean)]
  [closureV (param : symbol) (body : EXPR) (ds : DefrdSub)]
  [variantV (right? : boolean) (val : EXPR-Value)]
  [constructorV (right? : boolean)])

;; TypeEnv abstract syntax tree
(define-type TypeEnv
  [mtEnv]
  [aBind (name : symbol)
         (type : Type)
         (rest : TypeEnv)]
  [tBind (name : symbol)
         (var1-name : symbol) (var1-type : Type)
         (var2-name : symbol) (var2-type : Type)
         (rest : TypeEnv)])

;; DefrdSub abstract syntax tree
(define-type DefrdSub
  [mtSub]
  [aSub (name : symbol)
        (value : EXPR-Value)
        (rest : DefrdSub)]
  [aRecSub (sub-name : symbol)
           (val-box : (boxof EXPR-Value))
           (rest : DefrdSub)])

;; PTypeEnv abstract syntax tree
(define-type PTypeEnv
  [mtPSub]
  [aPSub (name : symbol)
         (rest : PTypeEnv)])

;; -----------------------------typecheck-------------------------------------

;; get-type : symbol TypeEnv -> Type
;; return the type of input identifier
;; (get-type 'abc (aBind 'abc (arrowT (numT) (numT)) (mtEnv))) should return (arrowT (numT) (numT))
(define (get-type [name-to-find : symbol] [env : TypeEnv])
  (type-case TypeEnv env
    [mtEnv () (error 'get-type "free")]
    [aBind (name ty rest)
           (if (symbol=? name-to-find name)
               ty
               (get-type name-to-find rest))]
    [tBind (name var1-name var1-ty var2-name var2-ty rest)
           (get-type name-to-find rest)]))
; test for get-type
(test (get-type 'abc (aBind 'abc (arrowT (numT) (numT)) (mtEnv))) (arrowT (numT) (numT)))

;; find-type-id : symbol TypeEnv -> TypeEnv
;; return TypeEnv where input identifier is found as Type
;; (find-type-id 'abcd (aBind 'aaa (numT) (tBind 'abcd 'a (numT) 'b (boolT) (mtEnv)))) should return (tBind 'abcd 'a (numT) 'b (boolT) (mtEnv))
(define (find-type-id [name-to-find : symbol] [env : TypeEnv])
  (type-case TypeEnv env
    [mtEnv () (error 'find-type-id "free")]
    [aBind (name ty rest) (find-type-id name-to-find rest)]
    [tBind (name var1-name var1-ty var2-name var2-ty rest)
           (if (symbol=? name-to-find name)
               env
               (find-type-id name-to-find rest))]))
; test for find-type-id
(test (find-type-id 'abcd (aBind 'aaa (numT) (tBind 'abcd 'a (numT) 'b (boolT) (mtEnv)))) (tBind 'abcd 'a (numT) 'b (boolT) (mtEnv)))

;; find-poly-type-id : symbol TypeEnv -> TypeEnv
;; return TypeEnv where input identifier(from a Polymorphic Type) is found
;; (find-poly-type-id 'abc (aBind 'aaa (numT) (aBind 'abc (boolT) (mtEnv)))) should return (aBind 'abc (boolT) (mtEnv))
(define (find-poly-type-id [name-to-find : symbol] [env : TypeEnv])
  (type-case TypeEnv env
    [mtEnv () (error 'find-poly-type-id "free")]
    [aBind (name ty rest)
           (if (symbol=? name-to-find name)
               env
               (find-poly-type-id name-to-find rest))]
    [tBind (name var1-name var1-ty var2-name var2-ty rest) (find-poly-type-id name-to-find rest)]))
; test for find-poly-type-id
(test (find-poly-type-id 'abc (aBind 'aaa (numT) (aBind 'abc (boolT) (mtEnv)))) (aBind 'abc (boolT) (mtEnv)))

;; validtype : Type TypeEnv -> TypeEnv
;; return TypeEnv where the input type exist in TypeEnv, if numT or boolT, return mtEnv
;; (validtype (numT) (aBind 'a (boolT) (mtEnv))) should return (mtEnv)
(define (validtype [ty : Type] [env : TypeEnv])
  (type-case Type ty
    [numT () (mtEnv)]
    [boolT () (mtEnv)]
    [arrowT (a b) (begin (validtype a env)
                         (validtype b env))]
    [idT (id) (find-type-id id env)]
    [polyT (fa body) (validtype body (aBind fa body env))]
    [tvT (id) (find-poly-type-id id env)]))
; test for validtype
(test (validtype (numT) (aBind 'a (boolT) (mtEnv))) (mtEnv))

;; typecheck : EXPR TypeEnv -> Type
;; return the Type of input EXPR expression, use help function ttypecheck
;; (typecheck (tfun 'alpha (num 3)) (mtEnv)) should return (polyT 'alpha (numT))
(define typecheck
  (lambda (expr env)
    (ttypecheck expr env (mtPSub))))
;; ttypecheck : EXPR TypeEnv PTypeEnv -> Type
;; return the Type of input EXPR expression
;; (ttypecheck (tfun 'alpha (num 3)) (mtEnv) (mtPSub)) should return (polyT 'alpha (numT))
(define (ttypecheck [expr : EXPR] [env : TypeEnv] [pds : PTypeEnv])
    (type-case EXPR expr
      [num (n) (numT)]
      [bool (b) (boolT)]
      [add (l r) (type-case Type (ttypecheck l env pds)
                   [numT ()
                         (type-case Type (ttypecheck r env pds)
                           [numT () (numT)]
                           [else (error 'd "no")])]
                   [else (error 'l "no")])]
      [sub (l r) (type-case Type (ttypecheck l env pds)
                   [numT ()
                         (type-case Type (ttypecheck r env pds)
                           [numT () (numT)]
                           [else (error 'r "no")])]
                   [else (error 'l "no")])]
      [equ (l r) (type-case Type (ttypecheck l env pds)
                   [numT ()
                         (type-case Type (ttypecheck r env pds)
                           [numT () (boolT)]
                           [else (error 'r "no")])]
                   [else (error 'l "no")])]
      [id (name) (get-type name env)]
      [fun (name te body)
           (local [(define param-type (parse-type te))]
             (begin
               (validtype param-type env)
               (arrowT param-type
                       (ttypecheck body
                                  (aBind name param-type env) pds))))]
      [app (fn arg)
           (type-case Type (ttypecheck fn env pds)
             [arrowT (param-type result-type)
                     (if (help-equal param-type (ttypecheck arg env pds) pds)
                         result-type
                         (error 'arg "no"))]
             [else (error 'fn "no")])]
      [ifthenelse (test-expr then-expr else-expr)
                  (type-case Type (ttypecheck test-expr env pds)
                    [boolT () (local [(define then-ty (ttypecheck then-expr env pds))]
                                (if (help-equal then-ty (ttypecheck else-expr env pds) pds)
                                    then-ty
                                    (error 'else-expr "no")))]
                    [else (error 'test-expr "no")])]
      [rec (name ty rhs-expr body-expr)
           (local [(define rhs-ty (parse-type ty))
                   (define new-env (aBind name rhs-ty env))]
             (begin
               (validtype rhs-ty env)
               (if (help-equal rhs-ty (ttypecheck rhs-expr new-env pds) pds)
                   (ttypecheck body-expr new-env pds)
                   (error 'rhs-expr "no"))))]
      [with-type (type-name var1-name var1-te
                            var2-name var2-te
                            body-expr)
                 (local [(define var1-ty (parse-type var1-te))
                         (define var2-ty (parse-type var2-te))
                         (define new-env (tBind type-name
                                                var1-name var1-ty
                                                var2-name var2-ty env))]
                   (begin
                     (validtype var1-ty new-env)
                     (validtype var2-ty new-env)
                     (ttypecheck body-expr
                                (aBind var1-name
                                       (arrowT var1-ty
                                               (idT type-name))
                                       (aBind var2-name
                                              (arrowT var2-ty
                                                      (idT type-name))
                                              new-env))
                                       pds)))]
      [cases (type-name dispatch-expr var1-name var1-id var1-rhs
                                      var2-name var2-id var2-rhs)
        (local [(define bind (find-type-id type-name env))]
          (if (and (equal? var1-name (tBind-var1-name bind))
                   (equal? var2-name (tBind-var2-name bind)))
              (type-case Type (ttypecheck dispatch-expr env pds)
                [idT (name)
                     (if (equal? name type-name)
                         (local [(define rhs1-ty
                                   (ttypecheck var1-rhs
                                              (aBind var1-id (tBind-var1-type bind) env)
                                              pds))
                                 (define rhs2-ty
                                   (ttypecheck var2-rhs
                                              (aBind var2-id (tBind-var2-type bind) env)
                                              pds))]
                           (if (help-equal rhs1-ty rhs2-ty pds)
                               rhs1-ty
                               (error 'var2-rhs "no")))
                         (error 'dispatch-expr "no"))]
                [else (error 'dispatch-expr "no")])
              (error 'expr "no")))]
      [tfun (name body) (polyT name (ttypecheck body (aBind name (tvT name) env) (aPSub name pds)))]
      [tapp (l r) (local [(define r-ty (parse-type r))]
                          (begin
                            (validtype r-ty env)
                            (type-case Type (ttypecheck l env pds)
                              [polyT (forall body) (change-poly-type body forall r-ty)]
                              [else (error 'd "no")])))]))
; test for typecheck
(test (typecheck (tfun 'alpha (num 3)) (mtEnv)) (polyT 'alpha (numT)))
; test for ttypecheck
(test (ttypecheck (tfun 'alpha (num 3)) (mtEnv) (mtPSub)) (polyT 'alpha (numT)))

;; parse-type : TE -> Type
;; parses TE to Type
;; (parse-type (idTE 'a)) should return (idT 'a)
(define (parse-type [te : TE])
  (type-case TE te
    [numTE () (numT)]
    [boolTE () (boolT)]
    [arrowTE (p r) (arrowT (parse-type p) (parse-type r))]
    [polyTE (fa body) (polyT fa (parse-type body))]
    [idTE (id) (idT id)]
    [tvTE (id) (tvT id)]))
; test for parse-type
(test (parse-type (idTE 'a)) (idT 'a))

;; change-poly-type : Type symbol Type -> Type
;; to help tapp typecheck, check input Type and changes all polymorphic input symbol Type to 3rd input Type.
;; (change-poly-type (arrowT (numT) (tvT 'alpha)) 'alpha (boolT)) should return (arrowT (numT) (boolT))
(define (change-poly-type [t : Type] [forall : symbol] [result : Type])
  (type-case Type t
    [arrowT (p r) (arrowT (change-poly-type p forall result)
                          (change-poly-type r forall result))]
    [tvT (id) (if (equal? id forall)
                   result
                   t)]
    [polyT (fa ty) (if (equal? fa forall)
                       t
                       (polyT fa (change-poly-type ty forall result)))]
    [else t]))
; test for change-poly-type
(test (change-poly-type (arrowT (numT) (tvT 'alpha)) 'alpha (boolT)) (arrowT (numT) (boolT)))

;; locate : symbol PTypeEnv -> number
;; find the location of input symbol in PTypeEnv and return it as a number
;; (locate 'abc (aPSub 'a (aPSub 'b (aPSub 'abc (mtPSub))))) should return 2
(define (locate [name : symbol] [ds : PTypeEnv])
  (type-case PTypeEnv ds
    [mtPSub () (error 'type-eror "no")]
    [aPSub (sub-name rest-ds)
           (if (symbol=? sub-name name)
               0
               (+ 1 (locate name rest-ds)))]))
; test for locate
(test (locate 'abc (aPSub 'a (aPSub 'b (aPSub 'abc (mtPSub))))) 2)

;; convert-tvT : Type PTypeEnv -> CType
;; Changes input Type into CType form, to change Polymorphic Types to numbers
;; (convert-tvT (arrowT (polyT 'alpha (tvT 'alpha)) (numT)) (mtPSub)) should return (carrowT (cpolyT (ctvT 0)) (cnumT))
(define (convert-tvT [ty : Type] [pds : PTypeEnv])
  (type-case Type ty
    [numT () (cnumT)]
    [boolT () (cboolT)]
    [idT (id) (cidT id)]
    [arrowT (a1 a2) (carrowT (convert-tvT a1 pds) (convert-tvT a2 pds))]
    [tvT (a) (ctvT (locate a pds))]
    [polyT (afa abody) (cpolyT (convert-tvT abody (aPSub afa pds)))]))
; test for convert-tvT
(test (convert-tvT (arrowT (polyT 'alpha (tvT 'alpha)) (numT)) (mtPSub)) (carrowT (cpolyT (ctvT 0)) (cnumT)))

;; help-equal : Type Type PTypeEnv -> boolean
;; return boolean value if input 2 types are same or not
;; (help-equal (arrowT (numT) (boolT)) (arrowT (numT) (boolT))) should return #t
(define (help-equal [a-ty : Type] [b-ty : Type] [pds : PTypeEnv])
  (local [(define aa-ty (convert-tvT a-ty pds))
          (define bb-ty (convert-tvT b-ty pds))]
    (equal? aa-ty bb-ty)))
; test for help-equal
(test (help-equal (arrowT (numT) (boolT)) (arrowT (numT) (boolT)) (mtPSub)) #t)
(test (help-equal (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'beta)))) (polyT 'beta (polyT 'alpha (arrowT (tvT 'beta) (tvT 'alpha)))) (mtPSub)) #t)
(test (help-equal (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'beta)))) (polyT 'alpha (polyT 'beta (arrowT (tvT 'beta) (tvT 'alpha)))) (mtPSub)) #f)
(test (help-equal (tvT 'alpha) (tvT 'beta) (aPSub 'alpha (aPSub 'beta (mtPSub)))) #f)

;; ------------------------------interp---------------------------------------

;; interp : EXPR DefrdSub -> EXPR-Value
;; compute the input EXPR expression with input environment DefrdSub and return EXPR-Value of it.
;; (interp (tfun 'alpha (tfun 'beta (num 3))) (mtSub)) should return (numV 3)
(define (interp [expr : EXPR] [ds : DefrdSub])
  (type-case EXPR expr
    [num (n) (numV n)]
    [bool (b) (boolV b)]
    [add (l r) (num+ (interp l ds) (interp r ds))]
    [sub (l r) (num- (interp l ds) (interp r ds))]
    [equ (l r) (boolV (equal? (numV-n (interp l ds)) (numV-n (interp r ds))))]
    [id (s) (lookup s ds)]
    [fun (x te b) (closureV x b ds)]
    [app (f a) (local [(define f-val (interp f ds))
                       (define a-val (interp a ds))]
                 (type-case EXPR-Value f-val
                   [closureV (param body ds)
                             (interp body (aSub param a-val ds))]
                   [constructorV (right?)
                                 (variantV right? a-val)]
                   [else (error 'interp "not applicable")]))]
    [ifthenelse (test-expr then-expr else-expr)
                (if (boolV-b (interp test-expr ds))
                    (interp then-expr ds)
                    (interp else-expr ds))]
    [rec (bound-id type named-expr body-expr)
         (local [(define value-holder (box (numV 42)))
                 (define new-ds
                   (aRecSub bound-id value-holder ds))]
           (begin
             (set-box! value-holder (interp named-expr new-ds))
             (interp body-expr new-ds)))]
    [with-type (type-name var1-name var1-te
                          var2-name var2-te
                          body-expr)
               (interp body-expr
                       (aSub var1-name
                             (constructorV false)
                             (aSub var2-name
                                   (constructorV true)
                                   ds)))]
    [cases (ty dispatch-expr
               var1-name var1-id var1-rhs
               var2-name var2-id var2-rhs)
      (type-case EXPR-Value (interp dispatch-expr ds)
        [variantV (right? val)
                  (if (not right?)
                      (interp var1-rhs (aSub var1-id val ds))
                      (interp var2-rhs (aSub var2-id val ds)))]
        [else (error 'interp "not a variant result")])]
    [tfun (name body) (interp body ds)]
    [tapp (l r) (interp l ds)]))
; test for interp
(test (interp (tfun 'alpha (tfun 'beta (num 3))) (mtSub)) (numV 3))

;; lookup : symbol DefrdSub -> FWAE-Value
;; find the symbol from DefrdSub and get the paired FWAE-Value
;; (lookup 'b (aSub 'b (numV 100) (mtSub))) should produce (numV 100)
(define (lookup [name : symbol] [ds : DefrdSub])
  (type-case DefrdSub ds
    [mtSub () (error 'lookup "no such field")]
    [aSub (x val rest)
          (if (symbol=? x name) val (lookup name rest))]
    [aRecSub (sub-name val-box rest)
             (if (symbol=? sub-name name)
                 (unbox val-box)
                 (lookup name rest))]))
;test to check lookup
(test (lookup 'b (aSub 'b (numV 100) (mtSub))) (numV 100))

;; num-op : (number number -> number) -> (EXPR EXPR -> EXPR)
;; to have opration on numV value
;; (num+ (numV 3) (numV 102)) should produce (numV 105)
(define (num-op [op : (number number -> number)])
  (lambda (x y)
    (numV (op (numV-n x) (numV-n y)))))
(define num+ (num-op +))
(define num- (num-op -))
;test for num-op
(test (num+ (numV 3) (numV 102)) (numV 105))

;; ------------------------------test-cases-------------------------------------

; test for type check
(test (typecheck (tfun 'alpha (num 3)) (mtEnv))
      (polyT 'alpha (numT)))
 
(test (typecheck (tfun 'alpha (tfun 'beta (num 3))) (mtEnv))
      (polyT 'alpha (polyT 'beta (numT))))

(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))
 
(test (typecheck (tapp (id 'f) (numTE)) (aBind 'f (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))) (mtEnv)))
      (arrowT (numT) (numT)))

(test (typecheck (tfun 'alpha (tfun 'beta (fun 'x (polyTE 'alpha (polyTE 'beta (tvTE 'alpha))) (id 'x)))) (mtEnv))
      (polyT 'alpha (polyT 'beta (arrowT (polyT 'alpha (polyT 'beta (tvT 'alpha)))
                                         (polyT 'alpha (polyT 'beta (tvT 'alpha)))))))

(test (typecheck (tapp (tfun 'alpha (tfun 'beta (fun 'x (polyTE 'alpha (polyTE 'beta (tvTE 'alpha))) (id 'x)))) (numTE)) (mtEnv)) (polyT 'beta (arrowT (polyT 'alpha (polyT 'beta (tvT 'alpha))) (polyT 'alpha (polyT 'beta (tvT 'alpha))))))

(test (typecheck (fun 'x (polyTE 'alpha (tvTE 'alpha)) (id 'x)) (mtEnv)) (arrowT (polyT 'alpha (tvT 'alpha)) (polyT 'alpha (tvT 'alpha))))

(test/exn (typecheck (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'beta))) (id 'x)) (mtEnv)) "free")

(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtEnv)) "free")

(test/exn (typecheck (tapp (id 'f) (numTE)) (aBind 'f (arrowT (numT) (numT)) (mtEnv))) "no")

(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtEnv)) "free")

(test/exn (typecheck (tapp (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (numTE)) (mtEnv)) "free")

(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (tfun 'beta (fun 'y (tvTE 'beta) (add (id 'x) (id 'y)))))) (mtEnv)) "no")

(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtEnv)) "free")

(test (typecheck (tfun 'alpha (num 3)) (mtEnv))
      (polyT 'alpha (numT)))

(test (typecheck (tfun 'alpha (tfun 'beta (num 3))) (mtEnv))
      (polyT 'alpha (polyT 'beta (numT))))

(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))

(test (typecheck (tapp (id 'f) (numTE)) (aBind 'f (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))) (mtEnv)))
      (arrowT (numT) (numT)))

(test (typecheck (tapp (id 'f) (numTE)) (aBind 'f (polyT 'alpha (polyT 'alpha (tvT 'alpha))) (mtEnv)))
      (polyT 'alpha (tvT 'alpha)))

(test (typecheck
       (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x)))
             (polyTE 'beta (arrowTE (tvTE 'beta) (tvTE 'beta))))
       (mtEnv))
      (arrowT (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta)))
              (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta)))))
              
(test (typecheck (tfun 'alpha (tfun 'beta (num 3))) (mtEnv))
      (polyT 'alpha (polyT 'beta (numT))))

(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))

(test (typecheck (tapp (tfun 'alpha (num 3)) (tvTE 'beta)) (aBind 'beta (numT) (mtEnv))) (numT))

(test (typecheck (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x))))  (mtEnv)) (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'alpha)))))

(test (typecheck (ifthenelse (bool true)
                             (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x)))
                             (tfun 'beta (fun 'y (tvTE 'beta) (id 'y))))
                 (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))

(test (typecheck (ifthenelse (bool true)
                             (tfun 'beta (fun 'y (tvTE 'beta) (id 'y)))
                             (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))))
                 (mtEnv))
      (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta))))


(test (typecheck (ifthenelse (equ (num 8) (num 10))
                             (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x))))
                             (tfun 'beta (tfun 'gamma (fun 'x (tvTE 'beta) (id 'x)))))
                 (mtEnv))
      (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'alpha)))))

(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha)
                                   (tfun 'beta (fun 'y (tvTE 'alpha)
                                                    (ifthenelse (bool true)
                                                                (id 'x)
                                                                (id 'y))))))
                 (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha)
                            (polyT 'beta (arrowT (tvT 'alpha)
                                                 (tvT 'alpha))))))
        
(test (typecheck
       (tfun 'alpha
             (fun 'alpha (arrowTE (numTE) (numTE))
                  (fun 'x (tvTE 'alpha)
                       (id 'x)))) (mtEnv))
      (polyT 'alpha (arrowT (arrowT (numT) (numT)) (arrowT (tvT 'alpha) (tvT 'alpha)))))
      
(test (typecheck
       (fun 'alpha (arrowTE (numTE) (numTE))
            (tfun 'alpha
                  (fun 'x (tvTE 'alpha)
                       (id 'x)))) (mtEnv))
      (arrowT (arrowT (numT) (numT)) (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha)))))

(test (typecheck (tapp (tfun 'alpha (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (id 'x))) (numTE)) (mtEnv)) (arrowT (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))) (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha)))))

(test (typecheck (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (num 5))) (numTE)) (mtEnv)) (arrowT (numT) (numT)))

(test (typecheck (ifthenelse (bool false)
                             (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (fun 'y (tvTE 'beta) (id 'y)))))
                             (tfun 'beta (tfun 'alpha (fun 'x (tvTE 'beta) (fun 'y (tvTE 'alpha) (id 'y))))))
                 (mtEnv))
      (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (arrowT (tvT 'beta) (tvT 'beta))))))

(test (typecheck (tapp (tfun 'alpha
                                 (fun 'alpha (tvTE 'alpha)
                                      (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha)))
                                           (app (tapp (id 'x) (numTE)) (num 10))) (tfun 'beta
                                                                                        (fun 'beta (tvTE 'beta)
                                                                                             (id 'beta)))))) (arrowTE (numTE) (numTE)))
          (mtEnv)) (arrowT (arrowT (numT) (numT)) (numT)))
(test (typecheck (tapp (tfun 'alpha
                                 (fun 'alpha (tvTE 'alpha)
                                      (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha)))
                                           (app (tapp (id 'x) (numTE)) (num 10))) (tfun 'beta
                                                                                        (fun 'beta (tvTE 'beta)
                                                                                             (id 'beta)))))) (numTE))
          (mtEnv)) (arrowT (numT) (numT)))
(test (typecheck (tapp (tfun 'alpha
                                 (fun 'alpha (tvTE 'alpha)
                                      (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha)))
                                           (app (tapp (id 'x) (numTE)) (num 10))) (tfun 'alpha
                                                                                        (fun 'alpha (tvTE 'alpha)
                                                                                             (id 'alpha)))))) (numTE))
          (mtEnv)) (arrowT (numT) (numT)))


(test (typecheck (tfun 'alpha (num 3)) (mtEnv))
      (polyT 'alpha (numT)))

(test (typecheck (tfun 'alpha (tfun 'beta (num 3))) (mtEnv))
      (polyT 'alpha (polyT 'beta (numT))))

(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))

(test (typecheck (tapp (id 'f) (numTE)) (aBind 'f (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))) (mtEnv)))
      (arrowT (numT) (numT)))

(test (typecheck (ifthenelse (bool true)
                             (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x)))
                             (tfun 'beta (fun 'y (tvTE 'beta) (id 'y))))
                 (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))

(test (typecheck (ifthenelse (bool true)
                             (tfun 'beta (fun 'y (tvTE 'beta) (id 'y)))
                             (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))))
                 (mtEnv))
      (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta))))


(test (typecheck (ifthenelse (bool true)
                             (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x))))
                             (tfun 'beta (tfun 'gamma (fun 'x (tvTE 'beta) (id 'x)))))
                 (mtEnv))
      (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'alpha)))))

(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha)
                                   (tfun 'beta (fun 'y (tvTE 'alpha)
                                                    (ifthenelse (bool true)
                                                                (id 'x)
                                                                (id 'y))))))
                 (mtEnv))
      (polyT 'alpha (arrowT (tvT 'alpha)
                            (polyT 'beta (arrowT (tvT 'alpha)
                                                 (tvT 'alpha))))))

(test (typecheck (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (num 42)) (id 'f)) (aBind 'f (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta))) (mtEnv))) (numT))

(test (typecheck (fun 'x (polyTE 'alpha (tvTE 'alpha)) (id 'x)) (mtEnv))
      (arrowT (polyT 'alpha (tvT 'alpha)) (polyT 'alpha (tvT 'alpha))))

(test (typecheck (tapp (tfun 'alpha (tfun 'beta (fun 'x (polyTE 'alpha (polyTE 'beta (tvTE 'alpha))) (id 'x)))) (numTE)) (mtEnv))
      (polyT 'beta (arrowT (polyT 'alpha (polyT 'beta (tvT 'alpha))) (polyT 'alpha (polyT 'beta (tvT 'alpha))))))

(test (typecheck (app (tapp (tapp (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x)))) (numTE)) (numTE)) (num 10)) (mtEnv)) (numT))

; test for interp

(test (interp (app (app (tapp (tfun 'alpha (fun 'f (tvTE 'alpha) (id 'f))) (arrowTE (numTE) (numTE))) (fun 'x (numTE) (id 'x))) (num 10)) (mtSub)) (numV 10))

(test (interp (tapp (tfun 'alpha (fun 'f (tvTE 'alpha) (id 'f))) (arrowTE (numTE) (numTE))) (mtSub)) (closureV 'f (id 'f) (mtSub)))

(test (interp (tapp (tapp (tfun 'alpha (tfun 'beta (num 3))) (numTE)) (numTE)) (mtSub)) (numV 3))

(test (interp (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtSub)) (closureV 'x (id 'x) (mtSub)))

(test (interp (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtSub)) (closureV 'x (id 'x) (mtSub)))

(test (interp (id 'x)
              (aSub 'x (numV 10) (mtSub)))
      (numV 10))

(test (interp (app (fun 'x (numTE)
                        (app (fun 'f (arrowTE (numTE) (numTE))
                                  (add (app (id 'f) (num 1))
                                       (app (fun 'x (numTE)
                                                 (app (id 'f)
                                                      (num 2)))
                                            (num 3))))
                             (fun 'y (numTE)
                                  (add (id 'x) (id 'y)))))
                   (num 0))
              (mtSub))
      (numV 3))

(test (interp (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (numTE))
              (mtSub))
      (closureV 'x (id 'x) (mtSub)))

(test (interp (tfun 'alpha (tfun 'beta (num 3))) (mtSub))
      (numV 3))
      
(test (interp (app (app (tapp (tfun 'alpha (fun 'f (tvTE 'alpha) (id 'f))) (arrowTE (numTE) (numTE))) (fun 'x (numTE) (id 'x))) (num 10)) (mtSub)) (numV 10))

(test (interp (tapp (tfun 'alpha (fun 'f (tvTE 'alpha) (id 'f))) (arrowTE (numTE) (numTE))) (mtSub)) (closureV 'f (id 'f) (mtSub)))

(test (interp (tapp (tapp (tfun 'alpha (tfun 'beta (num 3))) (numTE)) (numTE)) (mtSub)) (numV 3))

(test (interp (tapp (tfun 'alpha (fun 'f (tvTE 'alpha) (id 'f))) (arrowTE (numTE) (numTE))) (mtSub)) (closureV 'f (id 'f) (mtSub)))

(test (interp (app
               (tapp (ifthenelse (equ (num 3) (num 3))
                                 (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x)))
                                 (tfun 'beta (fun 'x (tvTE 'beta) (id 'x))))
                     (numTE)) (num 30)) (mtSub))
      (numV 30))
      
(test (interp
       (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha)))
                 (app (tapp (id 'x) (numTE)) (num 10)))
        (tfun 'beta (fun 'y (tvTE 'beta) (id 'y)))) (mtSub)) (numV 10))

(test (interp (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (num 5))) (numTE)) (mtSub)) (closureV 'x (num 5) (mtSub)))

(test (interp (tapp (tfun 'alpha (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (id 'x))) (numTE)) (mtSub)) (closureV 'x (id 'x) (mtSub)))

(test (interp (app (app (tapp (tapp (tfun 'alpha (tfun 'beta (fun 'x (arrowTE (tvTE 'alpha) (tvTE 'beta)) (id 'x))))
                                    (numTE))
                              (numTE))
                        (fun 'x (numTE) (add (num 5) (id 'x))))
                   (num 3))
              (mtSub))
      (numV 8))

(test (interp (app (app (tapp (tapp (tfun 'alpha (tfun 'alpha (fun 'x (arrowTE (tvTE 'alpha) (tvTE 'alpha)) (id 'x))))
                                    (numTE))
                              (numTE))
                        (fun 'x (numTE) (add (num 5) (id 'x))))
                   (num 3))
              (mtSub))
      (numV 8))

(test (interp (tapp (tapp (tfun 'alpha (tfun 'beta (num 3))) (numTE)) (numTE)) (mtSub))
      (numV 3))

(test (interp (app (tapp (tapp (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x)))) (numTE)) (numTE)) (num 10)) (mtSub)) (numV 10))

;; given tests (46)
(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (mtEnv)) (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))
(test (typecheck (tapp (id 'f) (numTE)) (aBind 'f (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))) (mtEnv))) (arrowT (numT) (numT)))
(test (typecheck (tfun 'alpha (tfun 'beta (fun 'x (polyTE 'alpha (polyTE 'beta (tvTE 'alpha))) (id 'x)))) (mtEnv)) (polyT 'alpha (polyT 'beta (arrowT (polyT 'alpha (polyT 'beta (tvT 'alpha))) (polyT 'alpha (polyT 'beta (tvT 'alpha)))))))
(test (typecheck (tapp (tfun 'alpha (tfun 'beta (fun 'x (polyTE 'alpha (polyTE 'beta (tvTE 'alpha))) (id 'x)))) (numTE)) (mtEnv)) (polyT 'beta (arrowT (polyT 'alpha (polyT 'beta (tvT 'alpha))) (polyT 'alpha (polyT 'beta (tvT 'alpha))))))
(test (typecheck (fun 'x (polyTE 'alpha (tvTE 'alpha)) (id 'x)) (mtEnv)) (arrowT (polyT 'alpha (tvT 'alpha)) (polyT 'alpha (tvT 'alpha))))
(test (typecheck (tapp (id 'f) (numTE)) (aBind 'f (polyT 'alpha (polyT 'alpha (tvT 'alpha))) (mtEnv))) (polyT 'alpha (tvT 'alpha)))
(test (typecheck (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (polyTE 'beta (arrowTE (tvTE 'beta) (tvTE 'beta)))) (mtEnv)) (arrowT (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta))) (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta)))))
(test (typecheck (tfun 'alpha (tfun 'beta (num 3))) (mtEnv)) (polyT 'alpha (polyT 'beta (numT))))
(test (typecheck (tapp (tfun 'alpha (num 3)) (tvTE 'beta)) (aBind 'beta (numT) (mtEnv))) (numT))
(test (typecheck (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x))))  (mtEnv)) (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'alpha)))))
(test (typecheck (ifthenelse (bool true) (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (tfun 'beta (fun 'y (tvTE 'beta) (id 'y)))) (mtEnv)) (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))))
(test (typecheck (ifthenelse (bool true) (tfun 'beta (fun 'y (tvTE 'beta) (id 'y))) (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x)))) (mtEnv)) (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta))))
(test (typecheck (ifthenelse (equ (num 8) (num 10)) (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x)))) (tfun 'beta (tfun 'gamma (fun 'x (tvTE 'beta) (id 'x))))) (mtEnv)) (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'alpha)))))
(test (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (tfun 'beta (fun 'y (tvTE 'alpha) (ifthenelse (bool true) (id 'x) (id 'y)))))) (mtEnv)) (polyT 'alpha (arrowT (tvT 'alpha) (polyT 'beta (arrowT (tvT 'alpha) (tvT 'alpha))))))
(test (typecheck (fun 'a (arrowTE (numTE) (numTE)) (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x)))) (mtEnv)) (arrowT (arrowT (numT) (numT)) (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha)))))
(test (typecheck (tapp (tfun 'alpha (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (id 'x))) (numTE)) (mtEnv)) (arrowT (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha))) (polyT 'alpha (arrowT (tvT 'alpha) (tvT 'alpha)))))
(test (typecheck (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (num 5))) (numTE)) (mtEnv)) (arrowT (numT) (numT)))
(test (typecheck (ifthenelse (bool false) (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (fun 'y (tvTE 'beta) (id 'y))))) (tfun 'beta (tfun 'alpha (fun 'x (tvTE 'beta) (fun 'y (tvTE 'alpha) (id 'y)))))) (mtEnv)) (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (arrowT (tvT 'beta) (tvT 'beta))))))
(test (typecheck (tapp (tfun 'alpha (fun 'a (tvTE 'alpha) (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (app (tapp (id 'x) (numTE)) (num 10))) (tfun 'beta (fun 'b (tvTE 'beta) (id 'b)))))) (arrowTE (numTE) (numTE))) (mtEnv)) (arrowT (arrowT (numT) (numT)) (numT)))
(test (typecheck (tapp (tfun 'alpha (fun 'a (tvTE 'alpha) (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (app (tapp (id 'x) (numTE)) (num 10))) (tfun 'beta (fun 'b (tvTE 'beta) (id 'b)))))) (numTE)) (mtEnv)) (arrowT (numT) (numT)))
(test (typecheck (ifthenelse (bool true) (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x)))) (tfun 'beta (tfun 'gamma (fun 'x (tvTE 'beta) (id 'x))))) (mtEnv)) (polyT 'alpha (polyT 'beta (arrowT (tvT 'alpha) (tvT 'alpha)))))
(test (typecheck (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (num 42)) (id 'f)) (aBind 'f (polyT 'beta (arrowT (tvT 'beta) (tvT 'beta))) (mtEnv))) (numT))
(test (typecheck (tapp (tfun 'alpha (tfun 'beta (fun 'x (polyTE 'alpha (polyTE 'beta (tvTE 'alpha))) (id 'x)))) (numTE)) (mtEnv)) (polyT 'beta (arrowT (polyT 'alpha (polyT 'beta (tvT 'alpha))) (polyT 'alpha (polyT 'beta (tvT 'alpha))))))
(test (typecheck (app (tapp (tapp (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x)))) (numTE)) (numTE)) (num 10)) (mtEnv)) (numT))
(test (interp (app (app (tapp (tfun 'alpha (fun 'f (tvTE 'alpha) (id 'f))) (arrowTE (numTE) (numTE))) (fun 'x (numTE) (id 'x))) (num 10)) (mtSub)) (numV 10))
(test (interp (tapp (tfun 'alpha (fun 'f (tvTE 'alpha) (id 'f))) (arrowTE (numTE) (numTE))) (mtSub)) (closureV 'f (id 'f) (mtSub)))
(test (interp (tapp (tapp (tfun 'alpha (tfun 'beta (num 3))) (numTE)) (numTE)) (mtSub)) (numV 3))
(test (interp (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtSub)) (closureV 'x (id 'x) (mtSub)))
(test (interp (id 'x) (aSub 'x (numV 10) (mtSub))) (numV 10))
(test (interp (app (fun 'x (numTE) (app (fun 'f (arrowTE (numTE) (numTE)) (add (app (id 'f) (num 1)) (app (fun 'x (numTE) (app (id 'f) (num 2))) (num 3)))) (fun 'y (numTE) (add (id 'x) (id 'y))))) (num 0)) (mtSub)) (numV 3))
(test (interp (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (numTE)) (mtSub)) (closureV 'x (id 'x) (mtSub)))
(test (interp (tfun 'alpha (tfun 'beta (num 3))) (mtSub)) (numV 3))
(test (interp (app (tapp (ifthenelse (equ (num 3) (num 3)) (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (tfun 'beta (fun 'x (tvTE 'beta) (id 'x)))) (numTE)) (num 30)) (mtSub)) (numV 30))
(test (interp (app (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (app (tapp (id 'x) (numTE)) (num 10))) (tfun 'beta (fun 'y (tvTE 'beta) (id 'y)))) (mtSub)) (numV 10))
(test (interp (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (num 5))) (numTE)) (mtSub)) (closureV 'x (num 5) (mtSub)))
(test (interp (tapp (tfun 'alpha (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) (id 'x))) (numTE)) (mtSub)) (closureV 'x (id 'x) (mtSub)))
(test (interp (app (app (tapp (tapp (tfun 'alpha (tfun 'beta (fun 'x (arrowTE (tvTE 'alpha) (tvTE 'beta)) (id 'x)))) (numTE)) (numTE)) (fun 'x (numTE) (add (num 5) (id 'x)))) (num 3)) (mtSub)) (numV 8))
(test (interp (app (app (tapp (tapp (tfun 'alpha (tfun 'alpha (fun 'x (arrowTE (tvTE 'alpha) (tvTE 'alpha)) (id 'x)))) (numTE)) (numTE)) (fun 'x (numTE) (add (num 5) (id 'x)))) (num 3)) (mtSub)) (numV 8))
(test (interp (app (tapp (tapp (tfun 'alpha (tfun 'beta (fun 'x (tvTE 'alpha) (id 'x)))) (numTE)) (numTE)) (num 10)) (mtSub)) (numV 10))
;;exception
(test/exn (typecheck (fun 'x (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'beta))) (id 'x)) (mtEnv)) "free")
(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtEnv)) "free")
(test/exn (typecheck (tapp (id 'f) (numTE)) (aBind 'f (arrowT (numT) (numT)) (mtEnv))) "no")
(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtEnv)) "free")
(test/exn (typecheck (tapp (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (numTE)) (mtEnv)) "free")
(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'alpha) (tfun 'beta (fun 'y (tvTE 'beta) (add (id 'x) (id 'y)))))) (mtEnv)) "no")
(test/exn (typecheck (tfun 'alpha (fun 'x (tvTE 'beta) (id 'x))) (mtEnv)) "free")

;; additional tests (4)
(test (typecheck (with-type 'fruit 'apple (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) 'banana (arrowTE (boolTE) (boolTE)) (cases 'fruit (app (id 'apple) (tfun 'beta (fun 'x (tvTE 'beta) (id 'x)))) 'apple 'a (app (tapp (id 'a) (boolTE)) (bool true)) 'banana 'b (app (id 'b) (bool false)))) (mtEnv)) (boolT))
(test (typecheck (rec 'f (arrowTE (numTE) (numTE)) (fun 'x (numTE) (ifthenelse (equ (id 'x) (num 0)) (id 'x) (app (id 'f) (sub (id 'x) (num 1))))) (app (id 'f) (app (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (numTE)) (num 1)))) (mtEnv)) (numT)) 
(test (interp (with-type 'fruit 'apple (polyTE 'alpha (arrowTE (tvTE 'alpha) (tvTE 'alpha))) 'banana (arrowTE (boolTE) (boolTE)) (cases 'fruit (app (id 'apple) (tfun 'beta (fun 'x (tvTE 'beta) (id 'x)))) 'apple 'a (app (tapp (id 'a) (boolTE)) (bool true)) 'banana 'b (app (id 'b) (bool false)))) (mtSub)) (boolV true))
(test (interp (rec 'f (arrowTE (numTE) (numTE)) (fun 'x (numTE) (ifthenelse (equ (id 'x) (num 0)) (id 'x) (app (id 'f) (sub (id 'x) (num 1))))) (app (id 'f) (app (tapp (tfun 'alpha (fun 'x (tvTE 'alpha) (id 'x))) (numTE)) (num 1)))) (mtSub)) (numV 0)) 
