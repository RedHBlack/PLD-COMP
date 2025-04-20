# Description des fonctionnalitées

## 1. Opérations arithmétiques

### 1.1 Addition et soustraction

#### Fonctionnalités implémentées

- Addition simple entre deux constantes (1 + 2)
- Addition entre deux variables (a + b)
- Addition entre variables et constantes (a + 1)
- Additions complexes avec plusieurs opérandes (a + b + c + d)
- Utilisation de parenthèses pour spécifier l'ordre des opérations ((a + b) + c)
- Gestion des expressions d'addition composées (((1 + 1) + (1 + 1)))
- Même chose pour la soustraction

#### Fonctionnalités non implémentées

- Opérateur unaire plus (+a) n'est pas supporté comme expression valide

### 1.2 Opérations bit à bit

#### Fonctionnalités implémentées

- Opérateur AND bit à bit (`&`) : Effectue un ET logique bit à bit entre deux opérandes
- Opérateur OR bit à bit (`|`) : Effectue un OU logique bit à bit entre deux opérandes
- Opérateur XOR bit à bit (`^`) : Effectue un OU exclusif bit à bit entre deux opérandes
- Opérateur NOT bit à bit (`~`) : Inverse les bits d'un opérande

#### Fonctionnalités non implémentées

- Opérateurs de décalage de bits (`<<` et `>>`) : Déplacent les bits vers la gauche ou la droite

### 1.3 Division Entière

#### Fonctionnalités implémentées

- Division simple entre deux constantes (`2 / 3`)
- Division entre deux variables (`a / b`)
- Division entre variables et constantes (`a / 2`, `10 / b`)
- Division avec valeurs négatives (`a / -1`)
- Division de zéro par une constante (`0 / 2`)

#### Fonctionnalités problématiques

- Division par zéro - provoque une boucle infinie dans le script de test
- Nous avons placé le test de la division par 0 dans le dossier `./tesfiles_draft` car il ne fonctionne pas.

#### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- La division par zéro n'est pas correctement gérée et peut causer un blocage du programme
- Nous ne générons pas d'avertissement pour les divisions qui pourraient causer un problème à l'exécution

#### À implémenter pour une version future

- Division float
- Division double

### 1.4 Opérateur Modulo

#### Fonctionnalités implémentées

- Modulo simple entre deux constantes (`2 % 3`)
- Modulo entre variables et constantes (`a % 2`, `2 % b`)
- Modulo entre deux variables (`a % b`)
- Modulo avec valeurs négatives (`a % -1`)
- Modulo avec zéro comme dividende (`0 % a`)

#### Fonctionnalités problématiques

- Modulo par zéro - provoque une boucle infinie dans le script de test, similaire à la division par zéro
- Nous avons placé le test du modulo par zéro dans le dossier `./tesfiles_draft` car il ne fonctionne pas correctement

#### À implémenter pour une version future

- Support du modulo avec des types float et double (une fois ces types supportés)

### 1.5 Multiplication entière

#### Fonctionnalités implémentées

- Multiplication simple entre deux constantes (`2 * 3`)
- Multiplication entre deux variables (`a * b`)
- Multiplication entre variables et constantes (`a * 2`, `5 * b`)
- Multiplications complexes avec plusieurs opérandes (`a * b * c`)
- Multiplication avec nombres négatifs (`a * -1`)
- Gestion correcte de la priorité des opérations (`a + b * c`)

#### À implémenter pour une version future

- Support de la multiplication avec des types float et double

### 1.6 Opérations multiples

#### Fonctionnalités implémentées

- Combinaison de différentes opérations arithmétiques (`a + b * c`)
- Utilisation de parenthèses pour modifier la priorité des opérations (`(a + b) * c`)
- Chaînages d'opérations de même type (`a + b + c + d`)
- Expressions complexes avec variables et constantes (`a * 2 + b - 3`)
- Combinaison d'opérations avec assignations (`a = b + c * d`)
- Gestion basique des priorités d'opérateurs (multiplication/division avant addition/soustraction)
- Support des opérations imbriquées dans des parenthèses (`a * (b + c)`)

## 1.7 Opérateurs logiques

#### Fonctionnalités implémentées

- Opérateur logique ET (`&&`) : Évalue à vrai si les deux opérandes sont vrais
- Opérateur logique OU (`||`) : Évalue à vrai si au moins un des opérandes est vrai
- Opérateur logique NON (`!`) : Inverse la valeur de vérité de l'opérande

### 1.8 Opérateur unaire d'opposé (-)

#### Fonctionnalités implémentées

- Application de l'opérateur opposé à une constante `(-3)`
- Application de l'opérateur opposé à une variable `(-a)`
- Application de l'opérateur opposé à une expression constante `(-(1 + 2))`
- Application de l'opérateur opposé à une expression avec variables `(-(a + b))`
- Application de l'opérateur opposé à zéro `(-0)`
- Chaînage d'opérateurs opposés comme `-(-a)` et `-(-(-a))` qui fonctionnent correctement

### 1.9 Opérations d'incrémentation et de décrémentation

#### Fonctionnalités implémentées

- Opérateur d'incrémentation postfixé (`a++`)
- Opérateur de décrémentation postfixé (`a--`)
- Opérateur d'incrémentation préfixé (`++a`)
- Opérateur de décrémentation préfixé (`--a`)

### 2. Affectation de variables

#### Fonctionnalités implémentées

- Affectation simple d'une constante à une variable (`a = 2`)
- Affectation d'une variable à une autre variable (`a = b`)
- Affectation d'expressions complexes à une variable (`a = b * c + 5`)
- Affectation en cascade pour des variables déjà déclarées (`a = b = c = b * c + 5`)
- Réaffectation d'une valeur à une variable déjà initialisée (`a = 5; a = 10`)

#### Fonctionnalités problématiques

- Support pour les opérateurs d'affectation composée (`+=`, `-=`, `*=`, `/=`)

### 3. Déclaration de variables

#### Fonctionnalités implémentées

- Déclaration simple de variables (`int a;`)
- Déclarations multiples sur une même ligne (`int a, b, c;`)
- Initialisation lors de la déclaration (`int a, b = 5;`)
- Initialisation avec des expressions complexes (`int a = 2+9;`)
- Mélange de variables initialisées et non initialisées sur une même ligne (`int a=1, b, c=1;`)
- Déclaration de variables avec noms valides incluant lettres, chiffres (sauf au début) et underscores

#### Fonctionnalités problématiques

- Notre compilateur accepte la déclaration d'une variable nommée uniquement par un numéro (`int 0;`)
- Le test où l'on déclare `a` et le retourne sans l'initialiser fonctionne, mais retournera une valeur indéterminée (le test porterait donc le mot UNKOWN dans le nom du fichier car ne nous pouvons pas savoir si le resultat de notre compilateur sera la même que GCC).

- Nous acceptons des noms de variables que GCC pourrait rejeter

#### À implémenter pour une version future

### 4. Opérateurs de comparaison

#### Fonctionnalités implémentées

- Opérateur d'égalité (`==`) entre variables et constantes
- Opérateur d'inégalité (`!=`) entre variables et constantes
- Opérateurs de relation (`>`, `<`, `>=`, `<=`) pour comparer des valeurs
- Comparaisons combinées avec des opérations arithmétiques (`a + b > c`)
- Opérateur de négation (`!`) appliqué aux comparaisons
- Utilisation d'opérateurs bit à bit avec des comparaisons (`a & b == c`)

#### Fonctionnalités problématiques

- Notre compilateur accepte les comparaisons avec des variables non déclarées
- Les comparaisons incomplètes (`10 >`) ne sont pas correctement détectées comme erreurs
- Confusion possible entre l'opérateur d'affectation (`=`) et l'opérateur d'égalité (`==`)

#### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- Notre compilateur n'émet pas d'avertissement pour l'utilisation de `=` au lieu de `==` dans les expressions conditionnelles
- La vérification des comparaisons incomplètes est moins stricte que GCC

#### À implémenter pour une version future

- Support des opérateurs logiques (`&&` et `||`) pour combiner des comparaisons
- Avertissements pour les confusions courantes (`=` vs `==`)

## 5. Support des tableaux

### Fonctionnalités implémentées

- Déclaration de tableaux avec une taille fixe (`int arr[10];`)
- Initialisation à la déclaration de tableaux avec des valeurs constantes (`int arr[3] = {1, 2, 3};`)
- Accès aux éléments d'un tableau via leur index (`arr[0] = 5;` | `arr[b] = 3;` | `arr[5*9] = 1;` | `arr[5>b] = 9;` | `arr[a++] = 5;` | `arr[a = 1] = 9;` | `tab[a++] = 5` |...)
- Utilisation d'éléments de tableaux dans des expressions (`a = arr[1] + 2;`)
- Tableaux "imbriquées" (`tab[tab[tab[a]]] = tab[6]`)
- Assignation d'une valeur à un élément de tableau (`arr[2] = a + b;`)

### Limitations et différences avec GCC

- Pas de vérification des dépassements d'index (accès hors des limites)
- Les tableaux multidimensionnels ne sont pas supportés
- Les tableaux ne peuvent pas être passés comme arguments de fonctions
- Les tableaux ne peuvent pas être initialisés sans spécifier explicitement leur taille (`int arr[] = {1, 2, 3};` n'est pas supporté)
- Les tableaux n'acceptent pas d'indice mélangeant variable et constante dans une expression arithmétique (`tab[a+9] = 5`) 

### À implémenter pour une version future

- Support des tableaux multidimensionnels
- Vérification des dépassements d'index à la compilation et à l'exécution
- Support pour passer des tableaux en tant qu'arguments de fonctions
- Initialisation implicite de la taille des tableaux basée sur les valeurs fournies
- Gestion de tous les indices possibles

## 6. Fonctions

### Fonctionnalités implémentées

- Déclaration et définition de fonctions (`int add(int a, int b) { return a + b; }`)
- Appels de fonctions avec des paramètres (`result = add(2, 3);`)
- Retour de valeurs depuis une fonction (`return a + b;`)
- Support des appels récursifs (`int fib(int n) { if (n <= 1) return n; return fib(n - 1) + fib(n - 2); }`)

### Limitations et différences avec GCC

- Pas de support pour les fonctions sans retour explicite (`void` non supporté)
- Pas de vérification stricte des types des arguments passés aux fonctions
- Les fonctions ne peuvent pas être surchargées
- Les erreurs de type dans les arguments des fonctions ne sont pas détectées

### À implémenter pour une version future

- Support des fonctions `void`
- Vérification stricte des types des arguments
- Support de la surcharge de fonctions
- Gestion des fonctions inline pour des optimisations

## 7. Structures de contrôle

### Fonctionnalités implémentées

- Blocs conditionnels simples (`if (a > b) { ... }`)
- Blocs conditionnels avec `else` (`if (a > b) { ... } else { ... }`)
- Boucles `while` (`while (a < 10) { ... }`)

### Limitations et différences avec GCC

- Les boucles imbriquées complexes peuvent causer des problèmes de performance
- Les instructions `break` et `continue` ne sont pas supportées
- Support limité des expressions conditionnelles avec opérateurs logiques (`&&`, `||`)
- Expressions conditionnelles en ligne non supportées (`a = b > c ? b : c;`)
- Parenthèses obligatoires pour le if
- Comparaisons entre structures complexes impossibles (tableaux, etc.)
- Les expressions conditionnelles complexes ne sont pas évaluées correctement dans tous les cas
