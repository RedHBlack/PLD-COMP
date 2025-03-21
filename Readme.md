# Présentation mi-parcours

## Notre architecture de projet

Nous avons décider d'organiser notre projet comme suit :

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
- Le repertoire `generated` et `build` qui contiennent les fichiers générés par ANTLR4 et les fichiers objets générés par le Makefile. Ces dossiers sont générés lors de la compilation de notre application.

C'est dans le repertoire `./testfiles` que tous nos tests sont implémentés.
Nous avons, pour l'instant, organisé nos tests en quatres sous répertoires :

- `arithmetic` : les tests concernant les opérations arithmétiques
- `assignment` : les tests concernant les affectations -`comparaison` : les tests concernant les comparaisons -`declaration` : les tests concernant les déclarations de variables

Les noms de nos tests sont normés de la manière suivante : `<n°test>_<nom du test>.c`.
Si ce test est voué à échouer, il est nommé de la manière suivante : `<n°test>_<nom du test>_FAILTEST.c`.
Nous avons donc un large panel de test afin de vérifier que les fonctionnalitées de notre compilateur sont bien implémentées et reussissent quand elles doivent réussir et échouent quand elles doivent échouer.

Enfin vous trouverez, à la racine de notre projet, le fichier `ifcc-test.py` qui permet de lancer tous nos tests en une seule commande :
`python3 ifcc-test.py ./testfiles` pour exécuter tous les tests ou `python3 ifcc-test.py ./testfiles/<repertoire>` pour exécuter les tests d'un répertoire en particulier.

## Les fonctionnalités implémentées

Actuellement nous avons implémenté les fonctionnalités suivantes :

- Les opérations arithmétiques : addition, soustraction, multiplication, division, modulo, bit à bit, opérations unaires opposé (avec le signe -) et négation (avec le signe !), postfixe et préfixe.
- Les affectations
- Les comparaisons : égalité et inégalité qui renvoient l'équivalent d'un booléen (1 si vrai, 0 si faux)
- Les déclarations de variables

## Cas où notre grammaire se comporte différent de gcc

### Bit

- Notre grammaire ne prend pas en compte le not bit à bit `~` qui est une opération unaire. Nous avons décidé de ne pas l'implémenter pour le moment.

- Notre grammaire ne prend pas en compte les décalages de bits `<<` et `>>`. Nous avons décidé de ne pas les implémenter pour le moment.

### Div

- Nous avons remarqué, en effectuant nos tests, que la division par 0 provoquait une boucle infini lors du lancement du script python. Nous remarquons donc qu'il y a un probleme mais nous n'allons pas le corriger pour le moment.
  Aucun test sur la division par 0 n'est donc effectué dans le repertoire `./testfiles/arithmetic/div`.

Noys avons placé le test de la division par 0 dans le dossier `./tesfiles_draft` car il ne fonctionne pas.

### Mod

- De même que pour la division, le modulo par 0 boucle à l'infini. Nous n'avons donc pas effectué de test sur le modulo par 0 dans le repertoire `./testfiles/arithmetic/mod`.

Nous avons placé le test du modulo par 0 dans le dossier `./tesfiles_draft` car il ne fonctionne pas.

### Multiple operations

- Le test n°6 nommée `6_multiple_operations.c` est un test qui effectue plusieurs opérations arithmétiques complexes. Ce test est censé fonctionné mais ne fonctionne pas. En effet il renvoie un exit status différent de celui de gcc mais nous ne comprenons pas vraiment pourquoi. Nous avons effectué des tests pas à pas et nous avons donc remarqué que l'erreur était générée lors de l'ajout des parenthèses ce qui implique un problème dans la gestion des priorités des opérations arithmétiques.

### Not

- A posteriori, ce test

```c
return !(!(1 == 0) || (1 > 0));
```

devrait fonctionner. Cependant actuellement nous n'avons pas encore implémenté les ou exclusif `||` et et exclusif `&&`. Nous avons donc décidé de ne pas implémenter ce test pour le moment mais sommes conscient qu'il serait utile de les implémenter dans un second temps.

#### Declaration

- Durant nos tests, nous avons remarqué que la déclaration d'une variable nommée uniquement par un numéro passe avec notre compilateur, mais pas avec GCC.

- Le test où l'on déclare `a` et le retourne sans l'initialiser fonctionne, mais il retournera une valeur indéterminée, pouvant entraîner un comportement imprévisible.

#### Comparaison

- Ce test vérifie le comportement de l'incrémentation postfixée (a++) lorsqu'elle est utilisée dans une comparaison. L'opérateur ++ en mode postfixe retourne d'abord la valeur actuelle de a (5), puis l'incrémente après l'évaluation de l'expression. La comparaison 5 == 5 est donc vraie.

- Ce test évalue l'effet de l'incrémentation préfixée (++a) lorsqu'elle est utilisée dans une comparaison. Contrairement à la version postfixée, l'opérateur préfixé incrémente d'abord a avant de retourner sa nouvelle valeur. Ainsi, a passe à 6, et la comparaison 6 == 6 est vraie.

- Ce test examine le comportement du compilateur lorsqu'une variable non déclarée (b) est utilisée dans une expression. La comparaison b == 2 ne peut pas être évaluée car b n'a pas été définie auparavant, ce qui devrait entraîner une erreur de compilation.
