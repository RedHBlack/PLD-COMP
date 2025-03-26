# Présentation mi-parcours

## Notre architecture de projet

Nous avons décidé d'organiser notre projet comme suit :

Dans le repertoire `./compiler` à la racine du projet se trouve le code source de notre application.
Vous y trouverez les fichiers suivants :

- `main.cpp` : le fichier principal de notre application
- `CodeGenerator.h/.cpp` : les fichiers contenant la classe CodeGenerator qui permet de générer le code assembleur de toutes les règles de notre grammaire.
- `CodeCheckVisitor.h/.cpp` : les fichiers contenant la classe CodeCheckVisitor qui permet de vérifier la validité des règles de notre grammaire.
  C'est dans ce code que les erreurs de notre grammaire sont détectées, certaines erreurs moins critiques
  sont gérées comme des warnings.
- `Makefile` : le fichier permettant de compiler notre application
- `config.mk` : le fichier permettant l'éxecution de ANTLR4
- `ifcc.g4` : le fichier de grammaire ANTLR4
- Le répertoire `generated` et `build` qui contiennent les fichiers générés par ANTLR4 et les fichiers objets générés par le Makefile. Ces dossiers sont générés lors de la compilation de notre application.

C'est dans le repertoire `./testfiles` que tous nos tests sont implémentés.
Nous avons, pour l'instant, organisé nos tests en quatres sous répertoires :

- `arithmetic` : les tests concernant les opérations arithmétiques
- `assignment` : les tests concernant les affectations
- `comparaison` : les tests concernant les comparaisons
- `declaration` : les tests concernant les déclarations de variables

Enfin vous trouverez, à la racine de notre projet, le fichier `ifcc-test.py` qui permet de lancer tous nos tests en une seule commande :
`python3 ifcc-test.py ./testfiles` pour exécuter tous les tests ou `python3 ifcc-test.py ./testfiles/<repertoire>` pour exécuter les tests d'un répertoire en particulier.

## Stratégie de test

Les noms de nos tests sont normés de la manière suivante : `<n°test>_<nom du test>.c`.
Si ce test est voué à échouer, il est nommé de la manière suivante : `<n°test>_<nom du test>_FAILTEST.c`.

Notre système de test fonctionne selon cette logique :

- Un test standard (sans suffixe \_FAILTEST) doit compiler et s'exécuter correctement
- Un test marqué \_FAILTEST est conçu pour provoquer une erreur intentionnellement
  - Si ce test échoue (comme prévu), il est considéré comme "réussi" car il reproduit le comportement attendu
  - Si ce test réussit contrairement à nos attentes, il est marqué comme échoué car notre compilateur accepte un code qui devrait être rejeté

Cette approche nous permet de tester à la fois les fonctionnalités correctes et la détection d'erreurs de notre compilateur, assurant ainsi qu'il rejette les codes non valides tout en acceptant les codes conformes.

# Description des fonctionnalitées

## 1. Opérations arithmétiques

### 1.1 Addition

#### Fonctionnalités implémentées

- Addition simple entre deux constantes (1 + 2)
- Addition entre deux variables (a + b)
- Addition entre variables et constantes (a + 1)
- Additions complexes avec plusieurs opérandes (a + b + c + d)
- Utilisation de parenthèses pour spécifier l'ordre des opérations ((a + b) + c)
- Gestion des expressions d'addition composées (((1 + 1) + (1 + 1)))

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

### Multiple operations

- Le test n°6 nommée `6_multiple_operations.c` est un test qui effectue plusieurs opérations arithmétiques complexes. Ce test est censé fonctionné mais ne fonctionne pas. En effet il renvoie un exit status différent de celui de gcc mais nous ne comprenons pas vraiment pourquoi. Nous avons effectué des tests pas à pas et nous avons donc remarqué que l'erreur était générée lors de l'ajout des parenthèses ce qui implique un problème dans la gestion des priorités des opérations arithmétiques.

### Not

- A posteriori, ce test

```c
return !(!(1 == 0) || (1 > 0));
```

devrait fonctionner. Cependant actuellement nous n'avons pas encore implémenté les ou exclusif `||` et et exclusif `&&`. Nous avons donc décidé de ne pas implémenter ce test pour le moment mais sommes conscients qu'il serait utile de les implémenter dans un second temps.

### Declaration

- Durant nos tests, nous avons remarqué que la déclaration d'une variable nommée uniquement par un numéro passe avec notre compilateur, mais pas avec GCC.

- Le test où l'on déclare `a` et le retourne sans l'initialiser fonctionne, mais il retournera une valeur indéterminée. C'est pour cette raison que le test ne passe pas en comparant les valeurs de sorties de `a`.

- L'affectation multiple sur la même ligne n'est pas acceptée par notre compilateur.

### Operation postfix & prefix

- Pour l'instant notre grammaire ne considere pas `a++`, `++a`, `a--` et `--a` comme des expressions et donc

```c
int a = 5;
int b = 2;
return a++ == ++b;
```

ne fonctionne pas.
Nous ne pouvons donc pas avoir de comparaison sur l'incrémentation postfixée et préfixée.
A posteriori, nous souhaitons que ce type de tests fonctionne et nous allons donc les implémenter dans un second temps.

#### Comparaison

- Ce test vérifie le comportement de l'incrémentation postfixée (a++) lorsqu'elle est utilisée dans une comparaison. L'opérateur ++ en mode postfixe retourne d'abord la valeur actuelle de a (5), puis l'incrémente après l'évaluation de l'expression. La comparaison 5 == 5 est donc vraie.

- Ce test évalue l'effet de l'incrémentation préfixée (++a) lorsqu'elle est utilisée dans une comparaison. Contrairement à la version postfixée, l'opérateur préfixé incrémente d'abord a avant de retourner sa nouvelle valeur. Ainsi, a passe à 6, et la comparaison 6 == 6 est vraie.

- Ce test examine le comportement du compilateur lorsqu'une variable non déclarée (b) est utilisée dans une expression. La comparaison b == 2 ne peut pas être évaluée car b n'a pas été définie auparavant, ce qui devrait entraîner une erreur de compilation.

### Affectation de variable

- Notre compilateur permet d'initialiser une variable non déclarée.
- Notre compilateur permet d'utiliser une variable non déclarée en l'affectant à une variable déclarée.
- Notre compilateur n'accepte pas l'affectation de la variable au return.

### Comparaison

- Notre compilateur accepte la comparaison avec une variable qui n'est ni déclarée ni initialisée.

## Organisation du travail

Pour la repartition du travail, nous avons décidé de travailler en hexanôme dès la 3ème séance.
Billy a crée le repository et nous avons donc tous pu travailler sur le projet en même temps.
A partir de ce moment là, nous travaillions en binôme sur une fonctionnalité précise.
Quand nous finissons une fonctionnalité, nous faisions un merge request pour que les autres puissent voir notre travail et le valider.

Sur la branche main, la version est stable et chaque branche (partant de main) est une fonctionnalité en cours de développement.
Cette organisation nous permet de travailler en parallèle sur des fonctionnalités différentes sans que cela ne pose de problème.

Billy, notre chef de projet, nous informe des fonctionnalités à developper en tenant le backlog à jour.
Nous nous retrouvons au debut et à la fin de chaque séance afin de discuter des fonctionnalités à développer et des potentielles difficultés rencontrées.

Nous comptons continuer sur cette organisation pour la suite du projet.

### Groupe 432

- Villeroy Billy
- Brossat Pierrick
- Mariat Quentin
- Foissey Isaline
- Southerland José
- Abi Saleh Adrian
- Marchi Mekari Gabriel
