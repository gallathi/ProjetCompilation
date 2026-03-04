#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "VariableVisitor.h"
#include "CodeGenVisitor.h"

using namespace antlr4;
using namespace std;

int main(int argn, const char **argv)
{
  stringstream in;
  if (argn==2)
  {
     ifstream lecture(argv[1]);
     if( !lecture.good() )
     {
         cerr<<"error: cannot read file: " << argv[1] << endl ;
         exit(1);
     }
     in << lecture.rdbuf();
  }
  else
  {
      cerr << "usage: ifcc path/to/file.c" << endl ;
      exit(1);
  }
  
  ANTLRInputStream input(in.str());

  ifccLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();

  ifccParser parser(&tokens);
  tree::ParseTree* tree = parser.axiom();

  if(parser.getNumberOfSyntaxErrors() != 0)
  {
      cerr << "error: syntax error during parsing" << endl;
      exit(1);
  }

  bool debug = false;
  VariableVisitor vv(debug);
  vv.visit(tree);
  
  if (debug)
    cout << endl << "Visite des variables : " << vv.getErrorCount() << " erreur(s) détectée(s)." << endl << endl;
  if (vv.getErrorCount() > 0) {
    if (debug)
      cout << "Génération de code annulée." << endl;
    exit(1);
  }
  CodeGenVisitor v(vv.getVarTable(), vv.getNextOffset());
  v.visit(tree);

  return 0;
}
