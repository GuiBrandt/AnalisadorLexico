#include "include/Parser.hpp"
#include "include/Lexer.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <map>
#include <vector>
#include <stack>

/**
 * Construtor
 *
 * \param filename Nome do arquivo para se analisar
 */
Parser::Parser(const char* filename) throw (const char*) : _lexer(filename)
{
    _var = (Variable*)malloc(sizeof(Variable)*_varSize);
    _procedure = (Procedure*)malloc(sizeof(Procedure)*_procedureSize);
    _function = (Function*)malloc(sizeof(Function)* _functionSize);
}

/**
 * Obt�m uma vari�vel
 */
Variable Parser::getVariable(const char* name) const
{
    Variable* r = NULL;
    int level = -1;

    int i;
    for (i = 0; i < _varCount; i++)
    {
        if (_level - _var[i].getLevel() < _level - level && (_var[i].getName(), name) == 0)
        {
            *r = _var[i];
            level = _var[i].getLevel();
        }
    }

    if (r == NULL)
        throw "Vari�vel n�o encontrada";
    else
        return *r;
}

/**
 * Obt�m um procedimento
 */
Procedure Parser::getProcedure(const char* name) const
{
    Procedure* r = NULL;
    int level = -1;

    int i;
    for (i = 0; i < _procedureCount; i++)
    {
        if (_level - _procedure[i].getLevel() < _level - level && strcmp(_procedure[i].getName(), name) == 0)
        {
            *r = _procedure[i];
            level = _procedure[i].getLevel();
        }
    }

    if (r == NULL)
        throw "Procedimento n�o encontrado";
    else
        return *r;
}

/**
 * Obt�m uma fun��o
 */
Function Parser::getFunction(const char* name) const
{
    Function* r = NULL;
    int level = -1;

    int i;
    for (i = 0; i < _functionCount; i++)
    {
        if (_level - _function[i].getLevel() < _level - level && strcmp(_function[i].getName(), name) == 0)
        {
            *r = _function[i];
            level = _function[i].getLevel();
        }
    }

    if (r == NULL)
        throw "Fun��o n�o encontrada";
    else
        return *r;
}

/**
 * Verifica se existe um s�mbolo com certo nome em um n�vel
 *
 * \param name Nome do s�mbolo
 * \param level N�vel a ser verificado
 *
 * \return 0 se n�o houver e 1 se houver o s�mbolo procurado nas listas de
 *         s�mbolos
 */
char Parser::hasSymbolAtLevel(const char* name, unsigned char level) const
{
    int i;

    for (i = 0; i < _varCount; i++)
        if (_var[i].getLevel() == level && strcmp(_var[i].getName(), name) == 0)
            return 1;

    for (i = 0; i < _procedureCount; i++)
        if (_procedure[i].getLevel() == level && strcmp(_procedure[i].getName(), name) == 0)
            return 1;

    for (i = 0; i < _functionCount; i++)
        if (_function[i].getLevel() == level && strcmp(_function[i].getName(), name) == 0)
            return 1;

    return 0;
}

/**
 * Adiciona uma vari�vel
 */
void Parser::addVariable(const Variable& v)
{
    if (hasSymbolAtLevel(v.getName(), _level))
        throw "J� existe um s�mbolo com este nome no escopo atual";

    if (_varCount >= _varSize)
    {
        Variable* aux = (Variable*)malloc(sizeof(Variable) * _varSize * 2);
        memcpy(aux, _var, sizeof(Variable) * _varSize);
        free(_var);
        _var = aux;
        _varSize *= 2;
    }

    _var[_varCount++] = v;
}

/**
 * Adiciona um procedimento
 */
void Parser::addProcedure(const Procedure& p)
{
    if (hasSymbolAtLevel(p.getName(), _level))
        throw "J� existe um s�mbolo com este nome no escopo atual";

    if (_procedureCount >= _procedureSize)
    {
        Procedure* aux = (Procedure*)malloc(sizeof(Procedure) * _procedureSize * 2);
        memcpy(aux, _procedure, sizeof(Procedure) * _procedureSize);
        free(_procedure);
        _procedure = aux;
        _procedureSize *= 2;
    }

    _procedure[_procedureCount++] = p;
}

/**
 * Adiciona uma fun��o
 */
void Parser::addFunction(const Function& f)
{
    if (hasSymbolAtLevel(f.getName(), _level))
        throw "J� existe um s�mbolo com este nome no escopo atual";

    if (_functionCount >= _functionSize)
    {
        Function* aux = (Function*)malloc(sizeof(Function) * _functionSize * 2);
        memcpy(aux, _function, sizeof(Function) * _functionSize);
        free(_function);
        _function = aux;
        _functionSize *= 2;
    }

    _function[_functionCount++] = f;
}

/**
 * Apaga uma fun��o
 */
void Parser::deleteFunction(const char* name)
{
    int i;
    for (i = 0; i < _functionCount; i++)
    {
        if (_function[i].getLevel() == _level && strcmp(_function[i].getName(), name) == 0)
        {
            int j;
            for(j=i; j<_functionCount; j++)
                _function[j] = _function[j+1];
            _functionCount--;
            return;
        }
    }

    throw "Fun��o n�o encontrada";
}

/**
 * Apaga um procedimento
 */
void Parser::deleteProcedure(const char* name)
{
    int i;
    for (i = 0; i < _procedureCount; i++)
    {
        if (_procedure[i].getLevel() == _level && strcmp(_procedure[i].getName(), name) == 0)
        {
            int j;
            for(j=i; j<_procedureCount; j++)
                _procedure[j] = _procedure[j+1];
            _procedureCount--;
            return;
        }
    }

    throw "Procedimento n�o encontrado";
}

/**
 * Apaga uma vari�vel
 */
void Parser::deleteVariable(const char* name)
{
    int i;
    for (i = 0; i < _varCount; i++)
    {
        if (_var[i].getLevel() == _level && strcmp(_var[i].getName(), name) == 0)
        {
            int j;
            for(j=i; j<_varCount; j++)
                _var[j] = _var[j+1];
            _varCount--;
            return;
        }
    }

    throw "Vari�vel n�o encontrada";
}

/**
 * Aumenta o n�vel
 */
void Parser::increaseLevel()
{
    _level++;
}

/**
 * Apaga todos os s�mbolos do n�vel atual e diminui o n�vel
 */
void Parser::decreaseLevel()
{
    int i;

    for (i = 0; i < _varCount; i++)
        if (_var[i].getLevel() >= _level)
            deleteVariable(_var[i].getName());

    for (i = 0; i < _procedureCount; i++)
        if (_procedure[i].getLevel() >= _level)
            deleteProcedure(_procedure[i].getName());

    for (i = 0; i < _functionCount; i++)
        if (_function[i].getLevel() >= _level)
            deleteFunction(_function[i].getName());

    _level--;
}

/**
 * Compila o cabe�alho do programa
 */
void Parser::compileProgramBeginning()
{
    TokenType prox = _lexer.getToken();
    if(prox != PROGRAM)
        throw "Program esperado";

    prox = _lexer.nextToken();
    if(prox != NAME)
        throw "Nome do programa esperado";

    prox = _lexer.nextToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();

    if (prox == VAR)
        compileVariableDeclaration();
    else while (prox == PROCEDURE || prox == FUNCTION)
    {
        if (prox == PROCEDURE)
            compileProcedure();
        else
            compileFunction();
    }

    compileCompositeCommand();
}

/**
 * Compila uma declara��o de vari�vel
 */
void Parser::compileVariableDeclaration()
{
    TokenType prox = _lexer.getToken();
    if (prox != VAR)
        throw "Var esperado";

    do
    {
        prox = _lexer.nextToken();
        if (prox != NAME)
            throw "Nome da vari�vel esperado";

        const char* name = _lexer.getName();

        prox = _lexer.nextToken();
        if (prox != COLON)
            throw "':' esperado";

        prox = _lexer.nextToken();
        if (prox != INTEGER && prox != BOOLEAN)
            throw "Tipo da vari�vel esperado";

        ValueType t = prox == INTEGER ? tINTEGER : tBOOLEAN;
        Variable v(name, _level, t);

        addVariable(v);

        prox = _lexer.nextToken();
    } while (prox == COMMA);

    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();
}

/**
 * Compila um comando composto (come�a em `begin', termina em `end')
 */
void Parser::compileCompositeCommand()
{
    TokenType prox = _lexer.getToken();
    if(prox != BEGIN)
        throw "Begin esperado";

    increaseLevel();

    prox = _lexer.nextToken();

    while ((prox = _lexer.getToken()) != END)
        compileCommand();

    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();

    decreaseLevel();
}

/**
 * Compila um comando simples
 */
void Parser::compileCommand()
{
    TokenType prox = _lexer.getToken();

    if (prox == IF)
        compileIf();
    else if (prox == WHILE)
        compileWhile();
    else if (prox == FUNCTION)
        compileFunction();
    else if (prox == PROCEDURE)
        compileProcedure();
    else if (prox == NAME)
    {
        try {
            compileMethodCall();
        }

        // N�o � chamada de m�todo
        catch (const char* e)
        {
            getVariable(_lexer.getName());
            compileVariableAttribution();
        }
    }
    else
        throw "Token inesperado.";
}

/**
 * Verifica se uma token � um operador
 *
 * \param tokenType Tipo do token a ser verificado
 * \return 1 se for operador, 0 se n�o
 */
char isoperator(TokenType tokenType)
{
    return tokenType >= SUM && tokenType <= NOT;
}

/**
 * Compila uma express�o
 *
 * \return O tipo da express�o compilada
 */
ValueType Parser::compileExpression()
{
    ValueType type;
    TokenType prox = _lexer.getToken();

    // Usando o mapa, vetor e a pilha do C++ mesmo porque n�o d� tempo de
    // programar classes eficientes par isso na voadora assim e eu quero
    // usar nota��o posfixa pra isso aqui

    // Prioridades
    static const std::map<TokenType, int> priority = {
        { OPEN_PARENTESIS,   4 },
        { AND,               3 },
        { OR,                3 },
        { XOR,               3 },
        { NOT,               3 },
        { MULTIPLY,          2 },
        { DIVIDE,            2 },
        { SUM,               1 },
        { SUBTRACT,          1 },
        { EQUAL,             0 },
        { DIFFERENT,         0 },
        { GREATER_OR_EQUAL,  0 },
        { SMALLER_OR_EQUAL,  0 },
        { SMALLER_THAN,      0 },
        { GREATER_THAN,      0 },
        { CLOSE_PARENTESIS, -1 }
    };

    std::vector<TokenType> v;
    std::stack<TokenType> s;

    while (prox == TRUE || prox == FALSE || prox == NUMBER || prox == NAME ||
           isoperator(prox) || prox == OPEN_PARENTESIS ||
           prox == CLOSE_PARENTESIS)
    {
        if (prox == NUMBER)
            v.push_back(INTEGER);
        else if (prox == TRUE || prox == FALSE)
            v.push_back(BOOLEAN);
        else if (prox == NAME)
        {
            ValueType t;
            try {
                t = compileFunctionCall();
            } catch (const char* e) {
                Variable v = getVariable(_lexer.getName());
                t = v.getType();
            }

            if (t == tINTEGER)
                v.push_back(INTEGER);
            else
                v.push_back(BOOLEAN);
        }
    }

    _lexer.nextToken();

    return type;
}

/**
 * Compila um condicional
 */
void Parser::compileIf()
{
    TokenType prox = _lexer.getToken();
    if (prox != IF)
        throw "If esperado";

    _lexer.nextToken();

    ValueType type = compileExpression();
    if (type != tBOOLEAN)
        throw "Express�o booleana esperada";

    prox = _lexer.getToken();
    if (prox == BEGIN)
        compileCompositeCommand();
    else
        compileCommand();

    prox = _lexer.getToken();
    if (prox == ELSE)
    {
        prox = _lexer.nextToken();

        if (prox == BEGIN)
            compileCompositeCommand();
        else
            compileCommand();
    }
}

/**
 * Compila um repetidor `while'
 */
void Parser::compileWhile()
{
    TokenType prox = _lexer.getToken();
    if(prox != WHILE)
        throw "While esperado";

    ValueType type = compileExpression();
    if(type != tBOOLEAN)
        throw "Express�o booleana esperada";

    prox = _lexer.getToken();

    if (prox == BEGIN)
        compileCompositeCommand();
    else
        compileCommand();
}

/**
 * Compila uma lista de par�metros
 */
int Parser::compileParameterDeclaration(Parameter** params)
{
    TokenType prox = _lexer.nextToken();
    if (prox != OPEN_PARENTESIS)
        throw "Abre par�nteses esperado";

    int n = 0;

    int sz = 8;
    *params = (Parameter*)malloc(sz * sizeof(Parameter));

    while(prox != CLOSE_PARENTESIS)
    {
        prox = _lexer.nextToken();
        if (prox != INTEGER && prox != BOOLEAN)
            throw "Tipo do par�metro esperado";

        if (n >= sz)
        {
            Parameter* aux = (Parameter*)malloc(sizeof(Parameter) * sz * 2);
            memcpy(aux, *params, sz);
            sz *= 2;

            free(*params);
            *params = aux;
        }

        (*params)[n].setType(prox == INTEGER ? tINTEGER : tBOOLEAN);

        prox = _lexer.nextToken();

        if (prox != NAME)
            throw "Nome do par�metro esperado";

        (*params)[n].setName(_lexer.getName());
        (*params)[n].setLevel(_level);

        prox = _lexer.nextToken();

        if(prox != COMMA && prox != CLOSE_PARENTESIS)
            throw "V�rgula esperada";

        n++;
    }

    return n;
}

/**
 * Compila uma declara��o de procedimento
 */
void Parser::compileProcedure()
{
    TokenType prox = _lexer.getToken();

    if (prox != PROCEDURE)
        throw "Declara��o de procedimento esperada";

    prox = _lexer.nextToken();
    if (prox != NAME)
        throw "Nome do procedimento esperado";

    const char* name = _lexer.getName();

    Parameter* parameters;
    int n = compileParameterDeclaration(&parameters);

    compileCompositeCommand();

    Procedure p(name, _level, n, parameters);
    addProcedure(p);
}

/**
 * Compila uma declara��o de fun��o
 */
void Parser::compileFunction()
{
    TokenType prox = _lexer.getToken();

    if (prox != FUNCTION)
        throw "Declara��o de fun��o esperada.";

    prox = _lexer.nextToken();
    if (prox != NAME)
        throw "Nome da fun��o esperado.";

    const char* name = _lexer.getName();

    prox = _lexer.nextToken();

    Parameter* parameters;
    int n = compileParameterDeclaration(&parameters);

    prox = _lexer.nextToken();
    if (prox != COLON)
        throw "':' esperado.";

    ValueType type;

    prox = _lexer.nextToken();
    if (prox == INTEGER)
        type = tINTEGER;
    else if (prox == BOOLEAN)
        type = tBOOLEAN;
    else
        throw "Tipo da fun��o esperado.";

    prox = _lexer.nextToken();
    compileCompositeCommand();

    Function f(name, _level, type, n, parameters);
    addFunction(f);
}

/**
 * Compila uma atribui��o de vari�vel
 */
void Parser::compileVariableAttribution()
{
    TokenType prox = _lexer.getToken();
    if (prox != NAME)
        throw "Nome esperado.";

    Variable v = getVariable(_lexer.getName());

    prox = _lexer.nextToken();
    if (prox != SETTER)
        throw "':=' esperado.";

    prox = _lexer.nextToken();
    ValueType t = compileExpression();

    if (t != v.getType())
        throw "Tipo inv�lido para a vari�vel.";

    prox = _lexer.nextToken();
    if (prox != SEMICOLON)
        throw "';' esperado.";
}

/**
 * Compila uma chamada de m�todo
 */
void Parser::compileMethodCall()
{
    TokenType prox = _lexer.getToken();
    if (prox != NAME)
        throw "Nome esperado.";

    const char* name = _lexer.getName();

    try {
        getFunction(name);
        compileFunctionCall();
    } catch (const char* e) {
        getProcedure(name);
        compileProcedureCall();
    }
}

/**
 * Compila a lista de par�metros passada para uma chamada de fun��o ou
 * procedimento
 *
 * \param method Ponteiro para a fun��o ou procedimento sendo chamado
 */
void Parser::compileParameters(const Procedure* method)
{
    TokenType prox = _lexer.getToken();
    if (prox == SEMICOLON)
        return;
    else if (prox != OPEN_PARENTESIS)
        throw "'(' esperado.";

    int n = 0;

    prox = _lexer.nextToken();
    while (prox != CLOSE_PARENTESIS)
    {
        ValueType type = compileExpression();

        prox = _lexer.getToken();
        if (prox == COMMA)
            prox = _lexer.nextToken();
        else if (prox != CLOSE_PARENTESIS)
            throw "')' esperado";

        if (n + 1 > method->getParameterCount())
            throw "N�mero inv�lido de par�metros.";

        if (method->getParameters()[n].getType() != type)
            throw "Tipo inv�lido para este par�metro.";

        n++;
    }

    prox = _lexer.nextToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

}

/**
 * Compila uma chamada de fun��o
 *
 * \return O tipo da fun��o
 */
ValueType Parser::compileFunctionCall()
{
    TokenType prox = _lexer.getToken();
    if (prox != NAME)
        throw "Nome esperado.";

    const char* name = _lexer.getName();
    Function f = getFunction(name);

    _lexer.nextToken();

    compileParameters(&f);

    return f.getReturnType();
}

/**
 * Compila uma chamada de procedimento
 */
void Parser::compileProcedureCall()
{
    TokenType prox = _lexer.getToken();
    if (prox != NAME)
        throw "Nome esperado.";

    const char* name = _lexer.getName();
    Procedure p = getProcedure(name);

    _lexer.nextToken();

    compileParameters(&p);
}
