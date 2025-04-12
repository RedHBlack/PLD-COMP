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

#### Fonctionnalités non implémentées

- Opérateur NOT bit à bit (`~`) : Inverse tous les bits de l'opérande
- Opérateurs de décalage de bits (`<<` et `>>`) : Déplacent les bits vers la gauche ou la droite

#### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- L'opérateur NOT bit à bit (`~`) n'est pas supporté
- Les opérateurs de décalage de bits (`<<` et `>>`) ne sont pas supportés
- Les opérations bit à bit peuvent être utilisées de manière similaire aux opérations arithmétiques standard, mais avec des limitations concernant la précédence et la combinaison avec d'autres opérateurs

### À implémenter pour une version future

- Support de l'opérateur NOT bit à bit (`~`)
- Support des opérateurs de décalage de bits (`<<` et `>>`)
- Amélioration de la gestion de la précédence des opérateurs bit à bit dans les expressions complexes

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

#### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- Le modulo par zéro n'est pas correctement géré et peut causer un blocage du programme
- Nous ne générons pas d'avertissement pour les opérations modulo qui pourraient causer un problème à l'exécution

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

#### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

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

#### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- Notre compilateur ne génère pas d'optimisations sur les expressions constantes (contrairement à GCC qui effectue des calculs à la compilation)

#### À implémenter pour une version future

- Support des opérateurs logiques (`&&` et `||`)
- Optimisation des expressions constantes à la compilation
- Support des opérandes de types différents dans une même expression (une fois les types float/double implémentés)

### 1.7 Opérateur unaire d'opposé (-)

#### Fonctionnalités implémentées

- Application de l'opérateur opposé à une constante `(-3)`
- Application de l'opérateur opposé à une variable `(-a)`
- Application de l'opérateur opposé à une expression constante `(-(1 + 2))`
- Application de l'opérateur opposé à une expression avec variables `(-(a + b))`
- Application de l'opérateur opposé à zéro `(-0)`
- Chaînage d'opérateurs opposés comme `-(-a)` et `-(-(-a))` qui fonctionnent correctement

#### Comportement différent de GCC

- Notre compilateur se comporte de façon similaire à GCC pour l'opérateur d'opposé. Nous respectons la priorité standard des opérateurs et le traitement des expressions unaires.
- Le double tiret `(--)` est interprété comme une tentative d'opérateur de décrémentation, qui n'est pas encore implémenté, et non comme un double opposé

#### À implémenter pour une version future

- Optimisations pour les cas particuliers (comme la double négation)
- Meilleure intégration avec les types float/double quand ils seront supportés

### 1.8 Opérations d'incrémentation et de décrémentation

#### Fonctionnalités non implémentées

Notre compilateur ne prend actuellement pas en charge les opérations d'incrémentation et de décrémentation:

- Opérateur d'incrémentation postfixé (`a++`)
- Opérateur de décrémentation postfixé (`a--`)
- Opérateur d'incrémentation préfixé (`++a`)
- Opérateur de décrémentation préfixé (`--a`)

#### Différences avec GCC

L'implémentation actuelle ne traite pas ces opérateurs comme GCC:

#### À implémenter pour une version future

- Support complet des opérateurs d'incrémentation et décrémentation (préfixés et postfixés)
- Gestion correcte de la sémantique (valeur retournée avant ou après modification)
- Support de ces opérateurs dans des expressions complexes
- Vérification que l'opérande est bien une l-value modifiable
- Gestion des cas d'erreurs comme `++(a++)` (l'opérande n'est pas une l-value)

### 2. Affectation de variables

#### Fonctionnalités implémentées

- Affectation simple d'une constante à une variable (`a = 2`)
- Affectation d'une variable à une autre variable (`a = b`)
- Affectation d'expressions complexes à une variable (`a = b * c + 5`)
- Réaffectation d'une valeur à une variable déjà initialisée (`a = 5; a = 10`)

#### À implémenter pour une version future

- Support pour les opérateurs d'affectation composée (`+=`, `-=`, `*=`, `/=`, etc.)
- Vérification correcte des l-values du côté gauche des affectations
- Vérification de compatibilité des types dans les affectations

### 3. Déclaration de variables

#### Fonctionnalités implémentées

- Déclaration simple de variables (`int a;`)
- Déclarations multiples sur une même ligne (`int a, b, c;`)
- Initialisation lors de la déclaration (`int a = 5;`)
- Initialisation avec des expressions complexes (`int a = 2+9;`)
- Mélange de variables initialisées et non initialisées sur une même ligne (`int a=1, b, c=1;`)
- Déclaration de variables avec noms valides incluant lettres, chiffres (sauf au début) et underscores

#### Fonctionnalités problématiques

- Notre compilateur accepte la déclaration d'une variable nommée uniquement par un numéro (`int 0;`)
- Le compilateur n'empêche pas la redéclaration d'une variable déjà déclarée dans le même bloc
- Le test où l'on déclare `a` et le retourne sans l'initialiser fonctionne, mais retournera une valeur indéterminée

#### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- Notre compilateur n'empêche pas la redéclaration de variables dans le même bloc
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
- Initialisation de tableaux avec des valeurs constantes (`int arr[3] = {1, 2, 3};`)
- Accès aux éléments d'un tableau via leur index (`arr[0] = 5;`)
- Utilisation d'éléments de tableaux dans des expressions (`a = arr[1] + 2;`)
- Assignation d'une valeur à un élément de tableau (`arr[2] = a + b;`)

### Fonctionnalités problématiques

- Pas de vérification des dépassements d'index (accès hors des limites du tableau)
- Les tableaux multidimensionnels ne sont pas supportés
- Les tableaux ne peuvent pas être passés comme arguments de fonctions

### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- Les dépassements d'index ne génèrent pas d'erreur ou d'avertissement
- Les tableaux ne peuvent pas être initialisés sans spécifier explicitement leur taille (`int arr[] = {1, 2, 3};` n'est pas supporté)

### À implémenter pour une version future

- Support des tableaux multidimensionnels
- Vérification des dépassements d'index à la compilation et à l'exécution
- Support pour passer des tableaux en tant qu'arguments de fonctions
- Initialisation implicite de la taille des tableaux basée sur les valeurs fournies

## 6. Fonctions

### Fonctionnalités implémentées

- Déclaration et définition de fonctions (`int add(int a, int b) { return a + b; }`)
- Appels de fonctions avec des paramètres (`result = add(2, 3);`)
- Retour de valeurs depuis une fonction (`return a + b;`)
- Support des appels récursifs (`int fib(int n) { if (n <= 1) return n; return fib(n - 1) + fib(n - 2); }`)

### Fonctionnalités problématiques

- Pas de support pour les fonctions sans retour explicite (`void` non supporté)
- Pas de vérification stricte des types des arguments passés aux fonctions
- Les fonctions ne peuvent pas être surchargées

### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- Les fonctions sans retour explicite (`void`) ne sont pas supportées
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

### Fonctionnalités problématiques

- Les boucles imbriquées complexes peuvent causer des problèmes de performance
- Les instructions `break` et `continue` ne sont pas supportées
- Les expressions conditionnelles complexes avec des opérateurs logiques (`&&`, `||`) ne sont pas entièrement supportées

### Comportement différent de GCC

Notre compilateur diffère de GCC sur les points suivants :

- Les instructions `break` et `continue` ne sont pas reconnues
- Les expressions conditionnelles complexes ne sont pas évaluées correctement dans tous les cas

### À implémenter pour une version future

- Support des instructions `break` et `continue`
- Amélioration de la gestion des expressions conditionnelles complexes
- Optimisation des boucles imbriquées pour de meilleures performances
- Support des boucles `do-while`
- Gestion des labels et des instructions `goto`
