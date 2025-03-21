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

### Mod

- De même que pour la division, le modulo par 0 boucle à l'infini. Nous n'avons donc pas effectué de test sur le modulo par 0 dans le repertoire `./testfiles/arithmetic/mod`.
