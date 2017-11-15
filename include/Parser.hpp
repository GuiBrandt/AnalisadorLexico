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

        /// Compila par�metros de um m�todo
        void compileParameter();

        /// Compila um comando composto (come�a em `begin', termina em `end')
        void compileCompositeCommand();

        /// Compila um comando simples
        void compileCommand();

        /// Compila uma express�o e retorna seu tipo
        ValueType compileExpression();
};

#endif // PARSER_HPP_INCLUDED
