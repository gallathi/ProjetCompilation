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