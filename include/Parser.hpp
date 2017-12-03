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

        char hasSymbolAtLevel(const char*, unsigned char) const;

        void addVariable(const Variable&);

        void addProcedure(const Procedure&);

        void addFunction(const Function&);

        int _varSize = 100;

        int _procedureSize = 100;

        int _functionSize = 100;

        int _varCount = 0;

        int _procedureCount = 0;

        int _functionCount = 0;

        int _level = 0;

        void deleteProcedure(const char*);

        void deleteFunction(const char*);

        void deleteVariable(const char*);

        char hasProcedure(const char*);

        char hasFunction(const char*);

        char hasVariable(const char*);

        void compileParameters(const Procedure*);

        void increaseLevel();

        void decreaseLevel();

    public:
        /// Construtor
        Parser(const char*) throw (const char*);

        /// Obt�m a linha atual sendo compilada
        int currentLine() const throw ();

        /// Obt�m a coluna atual do arquivo sendo lida pelo analisador l�xico
        int currentColumn() const throw ();

        /// Compila o cabe�alho do programa
        void compileProgram();

        /// Compila uma lista de vari�veis
        void compileVariableDeclaration();

        /// Compila uma atribui��o de vari�vel
        void compileVariableAttribution();

        /// Compila um procedimento
        void compileProcedure();

        /// Compila uma fun��o
        void compileFunction();

        /// Compila uma chamada de escrita
        void compileWrite();

        /// Compila uma chamada de leitura
        void compileRead();

        /// Compila um condicional
        void compileIf();

        /// Compila um repetidor `while'
        void compileWhile();

        /// Compila a declara��o de par�metros de um m�todo
        int compileParameterDeclaration(Parameter**);

        /// Compila uma chamada de fun��o/procedimento
        void compileMethodCall();
        ValueType compileFunctionCall();
        void compileProcedureCall();

        /// Compila um comando composto (come�a em `begin', termina em `end')
        void compileCompositeCommand();

        /// Compila um comando simples
        void compileCommand();

        /// Compila uma express�o e retorna seu tipo
        ValueType compileExpression();
};

#endif // PARSER_HPP_INCLUDED
