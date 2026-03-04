#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "CodeGenVisitor.h"
#include "VariableVisitor.h"

using namespace antlr4;
using namespace std;

// Commande de run : ./ifcc ../testfiles/8_testRegexVar.c > ../testfiles/out.s && g++ ../testfiles/out.s -o ../testfiles/a.out && ../testfiles/a.out

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

  if (vv.getErrorCount() > 0) {
  cout << "Il y a eu ";
  cout << vv.getErrorCount();
  cout << " erreur(s)." << endl;
      cout << "Génération de code annulée." << endl;
      exit(1);
  }

  CodeGenVisitor cgv(&vv);
  cgv.visit(tree);

  return 0;
}
