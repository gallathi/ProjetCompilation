# Lien vers le git

https://github.com/gallathi/ProjetCompilation.git

# Présentation de l'hexanôme

Les parties communes comme la grammaire, le visiteur de variables, le générateur de code et l'allocation de la pile ne sont pas mentionnées car tous ceux ayant travaillé sur le projet les ont modifié à un moment ou un autre.
- GALLAPONT Thibault (opérateurs de l'IR, switch, tests)
- WYBOUW Esteban (opérateurs de l'IR, CFG, doubles, tests)
- SOLIMAN Nouraldin (fonctions, gestion des char, tests)
- HASBANI Elie (boucles, gestion des blocs, tests)
- OWEIDAT Mohamad (opérateurs paresseux)
- SAVCHENKO Grigory
- SENAH Yao

# Gestion du projet

On a commencé par travailler en binôme, puis on a synchronisé nos codes quelques fois avant de finalement créer un git.
Maintenant, on va probablement travailler sur des fonctionnalités différentes puis merge des branches en s'assurant que tout marche bien.

# Documentation Utilisateur

Ce projet est un compilateur pour un sous-ensemble du langage C, développé dans le cadre du cours de compilation à l’INSA.
Le compilateur utilise ANTLR4 pour l'analyse syntaxique et génère du code assembleur x86.

Pour l'utiliser :
- Ajouter dans squelette-pld-comp/compiler un fichier config.mk basé sur le fichier example-config.mk renseignant le chemin vers ANTLR4.
- Dans ce dossier (ProjetCompilation), faire un "make"
- Pour lancer les tests, dans ce dossier :
    - "make test" pour lancer les tests
    - "make testfail" pour lancer les tests non-fonctionnels
- Pour exécuter/tester un fichier précis, dans ce dossier :
    - Changer le "10_soustraction.c" de la commande suivante pour exécuter le test voulu
    - "./ifcc squelette-pld-comp/testfiles/10_soustraction.c > squelette-pld-comp/testfiles/out.s && g++ squelette-pld-comp/testfiles/out.s -o squelette-pld-comp/testfiles/a.out && squelette-pld-comp/testfiles/a.out"
    - "echo $?" pour obtenir le résultat
    - Si besoin, consulter le fichier out.s dans squelette-pld-comp/testfiles

# Documentation Développeur

Sources :
Le compilateur est dans squelette-pld-comp/compiler.
Les dossier build et generated sont générés lors du make et peuvent être supprimés.
ifcc.g4 est la grammaire de notre compilateur écrite avec ANTLR4.
VariableVisitor.cpp est un visiteur qui parcourt le programme à compiler pour compter les variables à allouer et vérifier des erreurs que la grammaire de détecte pas (ex : variable déclarée mais pas utilisée).
CodeGenVisitor.cpp est un visiteur qui fait un 2ème parcours pour créer les variables temporaires nécessaires aux opérations de la grammaires et appeler l'IR.
IR.cpp contient le CFG, BasicBlock et les instructions assembleurs. Il est appelé par CodeGenVisitor pour enregistrer le contexte d'un bloc, d'une fonction, ou pour générer l'assembleur associé à une opération spécifique.
main.cpp lance les parcours des 2 visiteurs en transmettant la table des variables de VariableVisitor à CodeGenVisitor. Il permet également de set le debug à true pour avoir des messages de debug.
symbole.h et type.h sont des fichiers minimalistes qui auraient été utiles pour des fonctionnalités plus avancées.

Tests :
Les tests sont situés dans squelette-pld-comp/testfiles et squelette-pld-comp/testfilesfail.
Ceux de testfiles sont tous ceux où notre compilateur produit le même résultat que gcc (ou les 2 échouent)
Ceux de testfilesfail sont tous ceux où notre compilateur ne produit pas le même résultat que gcc (ou les 2 échouent) mais que nous n'avons pas réglés par souci de temps et parce qu'ils sont mineurs.
ifcc-test.py est le script python qui permet d'exécuter l'intégralité des tests d'un dossier et de donner des détails sur les différences d'exécution avec gcc.

# Fonctionnalités implémentées 

### ✅ Déjà Implémenté (Already Implemented) 
* [x] Un seul fichier source sans pré-processing.
* [x] Les commentaires sont ignorés.

### 🎯 Obligatoire (Mandatory) 
* [x] Type de données de base `int` (un type 32 bits).
* [x] Déclaration de variables n'importe où.
* [x] Constantes entières .
* [x] Constantes  caractère (avec simple quote).
* [x] Opérations arithmétiques de base (`+`, `-`, `*`).
* [x] Division et modulo.
* [x] Opérations logiques bit-à-bit (`|`, `&`, `^`).
* [x] Opérations de comparaison (`==`, `!=`, `<`, `>`).
* [x] Opérations unaires (`!`, `-`).
* [x] Affectation (qui retourne aussi une valeur).
* [x] Utilisation des fonctions standard `putchar` et `getchar` pour les entrées-sorties.
* [x] Définition de fonctions avec paramètres, et type de retour `int` ou `void`.
* [x] Vérification de la cohérence des appels de fonctions et leurs paramètres.
* [x] Structure de blocs grâce à `{` et `}`.
* [x] Support des portées de variables.
* [x] shadowing.
* [x] Les structures de contrôle `if`, `else`, `while`.
* [x] Support du `return expression` n'importe où.
* [x] Vérification qu'une variable utilisée a été déclarée.
* [x] Vérification qu'une variable n'est pas déclarée plusieurs fois.
* [x] Vérification qu'une variable déclarée est utilisée.

### 🚀 Facultatif (Optional) 
* [ ] Reciblage vers plusieurs architectures : x86, MSP430, ARM.
* [ ] Support des `double` avec toutes les conversions implicites.
* [ ] Propagation de constantes simple.
* [ ] Propagation de variables constantes (avec analyse du data-flow).
* [ ] Tableaux (à une dimension).
* [ ] Pointeurs.
* [x] `break` et `continue`.
* [ ] Les chaînes de caractères représentées par des tableaux de `char`.
* [x] Possibilité d'initialiser une variable lors de sa déclaration.
* [x] `switch...case`.
* [x] Les opérateurs logiques paresseux `||`, `&&`.
* [x] Opérateurs d'affectation (`+=`, `-=`) et d'incrémentation/décrémentation (`++`, `--`).

### 📉 Non Prioritaire (Low Priority) 
* [ ] Les variables globales.
* [ ] Les autres types de `inttypes.h`, les `float`.
* [ ] Le support dans les moindres détails de tous les autres opérateurs arithmétiques et logiques (`<=`, `>=`, `<<`, `>>`, etc.).
* [ ] Les autres structures de contrôle `for`, `do...while`.

### 🛑 Déconseillé (Not Recommended) 
* [ ] La possibilité de séparer dans des fichiers distincts les déclarations et les définitions.
* [ ] Le support du préprocesseur (`#define`, `#include`, `#if`, etc.).
* [ ] Les structures et unions.
* [ ] Support en largeur du type de données `char` (entier 8 bits).
