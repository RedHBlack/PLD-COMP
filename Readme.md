# Présentation du projet

Ce document fournit une vue d'ensemble technique et organisationnelle du projet. Vous y trouverez des informations essentielles pour :

- Comprendre l'architecture et la structure du projet.
- Configurer et exécuter le projet sur différents systèmes d'exploitation.
- Découvrir la stratégie de test adoptée pour garantir la qualité du code.
- Appréhender la gestion de projet et la répartition des tâches au sein de l'équipe.

Ce guide est conçu pour être une référence claire et concise, facilitant la prise en main du projet et la collaboration entre les membres de l'équipe.

## Notre architecture de projet

Nous avons décidé d'organiser notre projet comme suit :

Dans le repertoire `./compiler` à la racine du projet se trouve le code source de notre application.
Vous y trouverez les fichiers suivants :

- `Makefile` : le fichier permettant de compiler notre application
- `config.mk` : le fichier permettant l'éxecution de ANTLR4
- `ifcc.g4` : le fichier de grammaire ANTLR4
- Le répertoire `generated` et `build` qui contiennent les fichiers générés par ANTLR4 et les fichiers objets générés par le Makefile. Ces dossiers sont générés lors de la compilation de notre application.

C'est dans le répertoire `./test/testfiles` que tous nos tests sont implémentés.  
Nous avons, pour l'instant, organisé nos tests en plusieurs sous-répertoires :

- `arithmetic` : les tests concernant les opérations arithmétiques.
- `assignment` : les tests concernant les affectations.
- `comparison` : les tests concernant les comparaisons.
- `declaration` : les tests concernant les déclarations de variables.
- `block` : les tests concernant les blocs de code, comme les blocs conditionnels (`if`, `else`) ou les boucles (`while`, `for`).
- `function` : les tests concernant les définitions et appels de fonctions.
  - pour certains cas, on utilise getChar donc il faut taper Ctrl+D pour indiquer end of file.
- `incrementation` : les tests concernant les opérations d'incrémentation et de décrémentation.
- `propagation_constant` : les tests concernant l'optimisation du compilateur. Ces tests peuvent être similaires à d'autres tests placés ailleurs, mais ce répertoire permettent de les regrouper afin de vérifier manuellement le code assembleur généré pour valider les optimisations.

Enfin vous trouverez, dans ce même dossier `./test`, le fichier `ifcc-test.py` qui permet de lancer tous nos tests en une seule commande :
`python3 ifcc-test.py ./testfiles` pour exécuter tous les tests ou `python3 ifcc-test.py ./testfiles/<repertoire>` pour exécuter les tests d'un répertoire en particulier.

## Stratégie de test

### Conventions de nommage

Nos tests suivent une convention de nommage claire :

- Tests standards : `<n°test>_<nom du test>.c`
- Tests d'erreurs : `<n°test>_<nom du test>_FAILTEST.c`
- Tests de fonctionnalités non implémentées : `<n°test>_<nom du test>_NOT_IMPLEMENTED.c`
- Tests donc le comportement est aléatoire pour gcc donc qui sont soit FAIL soient OK : `<n°test>_<nom du test>_UNKNOWN.c`

### Logique d'évaluation

Notre système de test s'appuie sur la logique suivante :

- **Tests standards** : Doivent compiler et s'exécuter correctement
- **Tests FAILTEST** : Conçus pour échouer intentionnellement
  - Un échec de ces tests est considéré comme un succès (comportement attendu)
  - Un succès inattendu est considéré comme un échec (car notre compilateur accepte du code non valide)

### Prévention des régressions

Pour garantir la stabilité du code :

- Tous les tests doivent passer avant toute fusion dans la branche principale
- L'exécution complète de la suite de tests est requise pour chaque nouvelle fonctionnalité

### Tests de fonctionnalités futures

Les tests pour des fonctionnalités non encore implémentées sont :

- Clairement identifiés par le suffixe `_NOT_IMPLEMENTED`
- Inclus dans les statistiques de test final
- Conservés pour documenter les fonctionnalités à développer

## Lancement du code

### Instructions pour macOS

Si vous êtes sous macOS et que votre machine utilise une architecture ARM (comme les Mac équipés de puces Apple Silicon), il est nécessaire d'utiliser un émulateur Rosetta pour exécuter le code en mode x86_64. Cela est dû au fait que notre projet n'est pas compatible avec l'architecture ARM par défaut.

Pour activer Rosetta, suivez les étapes ci-dessous :

1. Ouvrez un terminal.
2. Lancez une session bash en mode x86_64 avec la commande suivante :

```bash
arch -x86_64 bash
```

Pour information, si vous avez lancé make sous arm64, veuillez make clean avant de remake avec i386.

3. Une fois dans cette session, vous pouvez compiler et exécuter le projet normalement en utilisant le `Makefile`.

### Vérification de l'architecture

Pour vérifier l'architecture utilisée par votre terminal, vous pouvez exécuter la commande suivante :

```bash
uname -m
```

- Si la sortie est `arm64`, vous êtes en mode ARM.
- Si la sortie est `x86_64`, vous êtes en mode x86_64.

### Résolution des problèmes courants

- **Erreur de compilation liée à l'architecture** : Assurez-vous d'avoir bien lancé le terminal en mode x86_64 avant de compiler.
- **Rosetta non installé** : Si vous n'avez pas encore installé Rosetta, macOS vous proposera de le faire automatiquement lors de la première tentative d'exécution d'une application x86_64. Suivez les instructions à l'écran pour l'installer.

### Compatibilité future

Nous arions prévu (si nous avions eu plus de temps) d'ajouter la compatibilité avec l'architecture ARM dans une version future du projet. Cela permettra d'exécuter le code nativement sur les machines Apple Silicon sans nécessiter Rosetta.

### Pour les autres systèmes d'exploitation

- **Linux** : Aucune configuration spécifique n'est requise. Assurez-vous simplement d'avoir les dépendances nécessaires installées (comme Python3 et ANTLR4).
- **Windows** : Utilisez un environnement WSL (Windows Subsystem for Linux) ou un terminal compatible avec les outils GNU pour exécuter le projet.

### Commandes utiles

- Pour compiler le projet :
  ```bash
  make
  ```
- Pour exécuter les tests :
  ```bash
  python3 ifcc-test.py ./testfiles
  ```
- Pour nettoyer les fichiers générés :
  ```bash
  make clean
  ```

## Organisation du travail

Pour la repartition du travail, nous avons décidé de travailler en hexanôme dès la 3ème séance.
Billy a crée le repository et nous avons donc tous pu travailler sur le projet en même temps.
A partir de ce moment là, nous travaillions en binôme sur une fonctionnalité précise.
Quand nous finissons une fonctionnalité, nous faisions un merge request pour que les autres puissent voir notre travail et le valider.

Sur la branche main, la version est stable et chaque branche (partant de main) est une fonctionnalité en cours de développement.
Cette organisation nous permet de travailler en parallèle sur des fonctionnalités différentes sans que cela ne pose de problème.

Billy, notre chef de projet, nous informe des fonctionnalités à developper en tenant le backlog à jour.
Nous nous retrouvons au debut et à la fin de chaque séance afin de discuter des fonctionnalités à développer et des potentielles difficultés rencontrées.

### Groupe 432

- Villeroy Billy
- Brossat Pierrick
- Mariat Quentin
- Foissey Isaline
- Southerland José
- Abi Saleh Adrian
- Marchi Mekari Gabriel
