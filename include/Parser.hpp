#include "Lexer.hpp"
#include "Symbols.hpp"
#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

/**
 * Classe para o analisador sint�tico
 */
class Parser
{
    private:
        /// Analisador l�xico
        Lexer _lexer;


        Variable* _var;

        Procedure* _procedure;

        Function* _function;

        Variable getVariable(const char*) const;

        Procedure getProcedure(const char*) const;

        Function getFunction(const char*) const;

        void addVariable(Variable);

        void addProcedure(Procedure);

        void addFunction(Function);

        int _varSize = 100;

        int _procedureSize = 100;

        int _functionSize = 100;

        int _varIndex = 0;

        int _procedureIndex = 0;

        int _functionIndex = 0;


    public:
        /// Construtor
        Parser(const char*);

        /// Compila o cabe�alho do programa
        void compileProgramBeginning();

        /// Compila uma lista de vari�veis
        void compileVariable();

        /// Compila um procedimento
        void compileProcedure();

        /// Compila uma fun��o
        void compileFunction();

        /// Compila um condicional
        void compileIf();

        /// Compila um repetidor `while'
        void compileWhile();

        /// Compila a declara��o de par�metros de um m�todo
        void compileParameterDeclaration();

        /// Compila os par�metros de uma chamada de fun��o
        void compileParameters();

        /// Compila um comando composto (come�a em `begin', termina em `end')
        void compileCompositeCommand();

        /// Compila um comando simples
        void compileCommand();

        /// Compila uma express�o e retorna seu tipo
        ValueType compileExpression();

        void deleteProcedure(const char*);

        void deleteFunction(const char*);

        void deleteVariable(const char*);
};

#endif // PARSER_HPP_INCLUDED
