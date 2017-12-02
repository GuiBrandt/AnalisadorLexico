#include "include/Parser.hpp"
#include "include/Lexer.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>

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
    for (i = 0; i < _varIndex; i++)
    {
        if (strcmp(_var[i].getName(), name) == 0 && _level - _var[i].getLevel() < _level - level)
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
    for (i = 0; i < _procedureIndex; i++)
    {
        if (strcmp(_procedure[i].getName(), name) == 0 && _level - _procedure[i].getLevel() < _level - level)
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
    for (i = 0; i < _functionIndex; i++)
    {
        if (strcmp(_function[i].getName(), name) == 0 && _level - _function[i].getLevel() < _level - level)
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
 * Adiciona uma vari�vel
 */
void Parser::addVariable(const Variable& v)
{
    try
    {
        Variable v2 = getVariable(v.getName());

        if (v2.getLevel() == _level)
            throw "A vari�vel j� existe";
    } catch (const char* e) {}

    if (_varIndex >= _varSize)
    {
        Variable* aux = (Variable*)malloc(sizeof(Variable) * _varSize * 2);
        memcpy(aux, _var, sizeof(Variable) * _varSize);
        free(_var);
        _var = aux;
        _varSize *= 2;
    }

    _var[_varIndex++] = v;
}

/**
 * Adiciona um procedimento
 */
void Parser::addProcedure(const Procedure& p)
{
    try
    {
        Procedure p2 = getProcedure(p.getName());

        if (p2.getLevel() == _level)
            throw "O procedimento j� existe";
    } catch (const char* e) {}

    if (_procedureIndex >= _procedureSize)
    {
        Procedure* aux = (Procedure*)malloc(sizeof(Procedure) * _procedureSize * 2);
        memcpy(aux, _procedure, sizeof(Procedure) * _procedureSize);
        free(_procedure);
        _procedure = aux;
        _procedureSize *= 2;
    }

    _procedure[_procedureIndex++] = p;
}

/**
 * Adiciona uma fun��o
 */
void Parser::addFunction(const Function& f)
{
    try
    {
        Function f2 = getFunction(f.getName());

        if (f2.getLevel() == _level)
            throw "A fun��o j� existe";
    } catch (const char* e) {}

    if (_functionIndex >= _functionSize)
    {
        Function* aux = (Function*)malloc(sizeof(Function) * _functionSize * 2);
        memcpy(aux, _function, sizeof(Function) * _functionSize);
        free(_function);
        _function = aux;
        _functionSize *= 2;
    }

    _function[_functionIndex++] = f;
}

/**
 * Apaga uma fun��o
 */
void Parser::deleteFunction(const char* name)
{
    int i;
    for (i = 0; i < _functionIndex; i++)
    {
        if (strcmp(_function[i].getName(), name) == 0)
        {
            int j;
            for(j=i; j<_functionIndex; j++)
                _function[j] = _function[j+1];
            _functionIndex--;
            break;
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
    for (i = 0; i < _procedureIndex; i++)
    {
        if (strcmp(_procedure[i].getName(), name) == 0)
        {
            int j;
            for(j=i; j<_procedureIndex; j++)
                _procedure[j] = _procedure[j+1];
            _procedureIndex--;
            break;
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
    for (i = 0; i < _varIndex; i++)
    {
        if (strcmp(_var[i].getName(), name) == 0)
        {
            int j;
            for(j=i; j<_varIndex; j++)
                _var[j] = _var[j+1];
            _varIndex--;
            break;
        }
    }
    throw "Vari�vel n�o encontrada";
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

    _level++;

    prox = _lexer.nextToken();

    while ((prox = _lexer.getToken()) != END)
        compileCommand();

    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();

    _level--;
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
        prox = _lexer.nextToken();

        if (prox == SETTER)
            compileVariableAttribution();
        else if (prox == OPEN_PARENTESIS)
            compileMethodCall();
        else
            throw "Atribui��o de vari�vel ou chamada de fun��o esperados";
    }
}

/**
 * Compila uma express�o
 *
 * \return O tipo da express�o compilada
 */
ValueType Parser::compileExpression()
{
    return tINTEGER;
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
    if(prox == BEGIN)
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

    if(prox == BEGIN)
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

        prox = _lexer.nextToken();

        if(prox != COMMA && prox != CLOSE_PARENTESIS)
            throw "V�rgula esperada";

        n++;
    }

    return n;
}

void Parser::compileProcedure()
{
    TokenType prox = _lexer.getToken();

    if (prox != PROCEDURE)
        throw "Declara��o de procedimento esperada";

    prox = _lexer.nextToken();
    if (prox != NAME)
        throw "Nome da fun��o esperado";

    const char* name = _lexer.getName();

    Parameter* parameters;
    int n = compileParameterDeclaration(&parameters);

    compileCompositeCommand();

    Procedure p(name, 0, n, parameters);
    addProcedure(p);
}

void Parser::compileFunction()
{

}

void Parser::compileVariableAttribution()
{

}

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
