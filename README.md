# Fonctionnalités implémentées

Notre code implémente toutes les fonctionnalités du TP jusqu'au 4.7 inclus, c'est-à-dire :
- Gestion des variables int et constantes
- Opérateurs arithmétiques +, -, *, / et %
- Opérateur unaire -
- Parenthèses
- Affectations (sans retour de valeur pour l'instant)
- 1 déclaration au début du code

# Comment naviguer dans le code

Nous n'avons pas changé le nom des programmes.
Pour make, il faut faire make dans squelette-pld-comp/compiler.

Depuis le dossier compiler :

Pour compiler et lancer un test, il faut faire :
./ifcc ../testfiles/11_soustraction.c > ../testfiles/out.s && g++ ../testfiles/out.s -o ../testfiles/a.out && ../testfiles/a.out
Puis : echo $?
Pour lancer le programme de test python :
python3 ../ifcc-test.py --verbose ../testfiles

# Gestion du projet

On a commencé par travailler en binôme, puis on a synchronisé nos codes quelques fois avant de finalement créer un git.
Maintenant, on va probablement travailler sur des fonctionnalités différentes puis merge des branches en s'assurant que tout marche bien.


# TODO 

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
* [X] Opérations unaires (`!`, `-`).
* [x] Affectation (qui retourne aussi une valeur).
* [x] Utilisation des fonctions standard `putchar` et `getchar` pour les entrées-sorties.
* [ ] Définition de fonctions avec paramètres, et type de retour `int` ou `void`.
* [ ] Vérification de la cohérence des appels de fonctions et leurs paramètres.
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
* [ ] `switch...case`.
* [ ] Les opérateurs logiques paresseux `||`, `&&`.
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
