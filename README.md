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

# Comment naviguer dans le code

Nous n'avons pas changé le nom des programmes.

Pour make, il faut écrire "make" dans ce dossier.

Pour lancer les tests, il faut écrire "make test" dans ce dossier (prend un peu de temps).

Pour lancer les tests non-fonctionnels, il faut écrire "make testfail" dans ce dossier.

Pour lancer un test précis, il faut copier-coller (en changeant le nom du test) :
./ifcc squelette-pld-comp/testfiles/10_soustraction.c > squelette-pld-comp/testfiles/out.s && g++ squelette-pld-comp/testfiles/out.s -o squelette-pld-comp/testfiles/a.out && squelette-pld-comp/testfiles/a.out
Puis : echo $?
(si besoin, le fichier out.s sera dans squelette-pld-comp/testfiles)

# Gestion du projet

On a commencé par travailler en binôme, puis on a synchronisé nos codes quelques fois avant de finalement créer un git.
Maintenant, on va probablement travailler sur des fonctionnalités différentes puis merge des branches en s'assurant que tout marche bien.


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
