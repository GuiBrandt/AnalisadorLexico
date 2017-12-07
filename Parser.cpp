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
    _function = (Function*)malloc(sizeof(Function)* _functionSize);

    _runningFunction = NULL;

    _isInsideConditional = 0;
}

/**
 * Obt�m a linha do arquivo sendo lida pelo compilador
 */
int Parser::currentLine() const throw ()
{
    return _lexer.currentLine();
}

/**
 * Obt�m a coluna do arquivo sendo lida pelo compilador
 */
int Parser::currentColumn() const throw ()
{
    return _lexer.currentColumn();
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
        if (_level - _var[i].getLevel() < _level - level && strcmp(_var[i].getName(), name) == 0)
        {
            r = &_var[i];
            level = _var[i].getLevel();
        }
    }

    if (r == NULL)
        throw "S�mbolo n�o encontrado";
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
            r = &_function[i];
            level = _function[i].getLevel();
        }
    }

    if (r == NULL)
        throw "S�mbolo n�o encontrado";
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
    printf("add variable %s at level %d\r\n", v.getName(), v.getLevel());

    if (hasSymbolAtLevel(v.getName(), v.getLevel()))
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
 * Adiciona uma fun��o
 */
void Parser::addFunction(const Function& f)
{
    printf("add function %s at level %d\r\n", f.getName(), f.getLevel());

    if (hasSymbolAtLevel(f.getName(), f.getLevel()))
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
    printf("delete function %s at level %d\r\n", name, _level);

    int i;
    for (i = _functionCount - 1; i >= 0; i--)
    {
        if (_function[i].getLevel() == _level && strcmp(_function[i].getName(), name) == 0)
        {
            int j;
            for(j = i; j < _functionCount - 1; j++)
                _function[j] = _function[j + 1];
            _functionCount--;
            return;
        }
    }

    throw "Fun��o n�o encontrada";
}

/**
 * Apaga uma vari�vel
 */
void Parser::deleteVariable(const char* name)
{
    printf("delete variable %s at level %d\r\n", name, _level);

    int i;
    for (i = _varCount - 1; i >= 0; i++)
    {
        if (_var[i].getLevel() == _level && strcmp(_var[i].getName(), name) == 0)
        {
            int j;
            for (j = i; j < _varCount - 1; j++)
                _var[j] = _var[j + 1];
            _varCount--;

            return;
        }
    }

    throw "Vari�vel n�o encontrada";
}

/**
 * Verifica se uma fun��o existe
 *
 * \param name Nome da fun��o
 */
char Parser::hasFunction(const char* name)
{
    int i;
    for (i = 0; i < _functionCount; i++)
        if (strcmp(_function[i].getName(), name) == 0)
            return 1;

    return 0;
}

/**
 * Verifica se uma vari�vel existe
 *
 * \param name Nome da vari�vel
 */
char Parser::hasVariable(const char* name)
{
    int i;
    for (i = 0; i < _varCount; i++)
        if (strcmp(_var[i].getName(), name) == 0)
            return 1;

    return 0;
}

/**
 * Aumenta o n�vel
 */
void Parser::increaseLevel()
{
    _level++;
    printf("level up to %d\r\n", _level);
}

/**
 * Apaga todos os s�mbolos do n�vel atual e diminui o n�vel
 */
void Parser::decreaseLevel()
{
    printf("level down to %d\r\n", _level - 1);

    int i;

    for (i = _varCount - 1; i > 0; i--)
        if (_var[i].getLevel() == _level)
            deleteVariable(_var[i].getName());

    for (i = _functionCount - 1; i > 0; i--)
        if (_function[i].getLevel() == _level)
            deleteFunction(_function[i].getName());

    _level--;
}

/**
 * Compila o cabe�alho do programa
 */
void Parser::compileProgram()
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

    prox = _lexer.nextToken();

    if (prox == VAR)
        compileVariableDeclaration();

    prox = _lexer.getToken();
    while (prox == PROCEDURE || prox == FUNCTION)
    {
        if (prox == PROCEDURE)
            compileProcedure();
        else
            compileFunction();

        prox = _lexer.getToken();
    }

    compileCompositeCommand();

    if (_lexer.hasMoreTokens())
        throw "Fim de arquivo esperado";
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

    prox = _lexer.nextToken();

    while (prox != END)
    {
        compileCommand();
        prox = _lexer.getToken();
    }

    prox = _lexer.nextToken();
}

/**
 * Compila um comando simples
 */
void Parser::compileCommand()
{
    compileNestedCommand();

    if (_lexer.getToken() != SEMICOLON)
        throw "';' esperado";
    _lexer.nextToken();
}

/**
 * Compila um comando dentro de outro (como no if e no while)
 * basicamente igual ao compileCommand, mas n�o exige ';'
 */
void Parser::compileNestedCommand()
{
    TokenType prox = _lexer.getToken();

    // Para o IF e para o WHILE n�o � necess�rio o ';' porque o pr�prio comando dentro deles
    // exige isso
    if (prox == IF)
        compileIf();
    else if (prox == WHILE)
        compileWhile();
    else if (prox == BEGIN)
        compileCompositeCommand();
    else if (prox == NAME)
    {
        if (hasFunction(_lexer.getName()))
        {
            const char* name = _lexer.getName();

            if (_lexer.nextToken() == SETTER)
                compileFunctionReturn(name);
            else
                compileFunctionCall(name);
        }
        else
            compileVariableAttribution();
    }
    else if (prox == WRITE)
        compileWrite();
    else if (prox == READ)
        compileRead();
    else if (prox == END_OF_FILE)
        throw "Fim de arquivo inesperado";
    else
        throw "Token inesperado.";
}

/**
 * Compila uma chamada de escrita
 */
void Parser::compileWrite()
{
    TokenType prox = _lexer.getToken();

    if (prox != WRITE)
        throw "'write' esperado";

    prox = _lexer.nextToken();
    compileExpression();
}

/**
 * Compila uma chamada de leitura
 */
void Parser::compileRead()
{
    TokenType prox = _lexer.getToken();

    if (prox != READ)
        throw "'read' esperado";

    prox = _lexer.nextToken();
    if (prox != NAME)
        throw "Nome esperado";

    getVariable(_lexer.getName());
}

/**
 * Verifica se uma token � um operador
 *
 * \param tokenType Tipo do token a ser verificado
 * \return 1 se for operador, 0 se n�o
 */
char isoperator(TokenType tokenType)
{
    return tokenType >= SUM && tokenType <= SMALLER_OR_EQUAL;
}

/**
 * Compila uma express�o
 *
 * Essa fun��o em especial usa recursos do C++ ao inv�s de C porque deus me
 * livre de implementar todas as estruturas de dados que seriam necess�rias pra
 * fazer uma convers�o de sequ�ncia infixa em posfixa na m�o.
 *
 * \return O tipo da express�o compilada
 */
ValueType Parser::compileExpression()
{
    TokenType prox;

    // TokenType para '-' un�rio prefixado, n�o faz sentido existir para o
    // lexer mas aqui ele � necess�rio
    static const TokenType NEGATIVE = (TokenType)(UNKNOWN + 1);

    // Prioridades
    static std::map<TokenType, int> priority = {
        { OPEN_PARENTESIS,   4 },
        { NOT,               3 },
        { MULTIPLY,          3 },
        { DIVIDE,            3 },
        { MOD,               3 },
        { NEGATIVE,          3 },
        { SUM,               2 },
        { SUBTRACT,          2 },
        { EQUAL,             1 },
        { DIFFERENT,         1 },
        { GREATER_OR_EQUAL,  1 },
        { SMALLER_OR_EQUAL,  1 },
        { SMALLER_THAN,      1 },
        { GREATER_THAN,      1 },
        { AND,               0 },
        { OR,                0 },
        { XOR,               0 },
        { CLOSE_PARENTESIS, -1 }
    };

    // Vetor com a sequ�ncia posfixa
    std::vector<TokenType> resultVector;

    // Pilha usada como intermedi�rio para os operadores
    std::stack<TokenType> symbolStack;

    bool hasLeftValue = false;
    int hasOpenParentesis = 0;

    for (
        prox = _lexer.getToken();

        prox == TRUE     || prox == FALSE           ||
        prox == NUMBER   || prox == NAME            ||
        isoperator(prox) || prox == OPEN_PARENTESIS ||
        (prox == CLOSE_PARENTESIS && hasOpenParentesis);

        prox = _lexer.nextToken()
    ) {
        // Se for um valor, seu tipo � adicionado direto ao vetor
        if (prox == NUMBER)
        {
            resultVector.push_back(INTEGER);
            hasLeftValue = true;
        }

        else if (prox == TRUE || prox == FALSE)
        {
            resultVector.push_back(BOOLEAN);
            hasLeftValue = true;
        }
        else if (prox == NAME)
        {
            ValueType t;

            const char* name = _lexer.getName();

            if (hasFunction(name))
            {
                _lexer.nextToken();
                t = compileFunctionCall(name);
            }
            else
            {
                Variable resultVector = getVariable(name);
                t = resultVector.getType();
            }

            if (t == tVOID)
                throw "Express�o inv�lida: procedimento n�o tem valor";

            if (t == tINTEGER)
                resultVector.push_back(INTEGER);
            else
                resultVector.push_back(BOOLEAN);

            hasLeftValue = true;
        }

        // Se n�o for valor, deve ser um operador
        else
        {
            if (prox == OPEN_PARENTESIS)
                hasOpenParentesis++;

            if (!hasLeftValue && (prox == SUM || prox == SUBTRACT))
            {
                if (prox == SUBTRACT)
                    resultVector.push_back(NEGATIVE);
                else
                    continue;   // '+' prefixado � in�til
            }

            // Esvazia a pilha enquanto a prioridade do que estiver no topo for
            // maior que a do operador atual
            while (symbolStack.size() > 0 && (
                symbolStack.top() == OPEN_PARENTESIS ?
                    prox == CLOSE_PARENTESIS :
                        priority[symbolStack.top()] >= priority[prox])
            ) {
                TokenType op = symbolStack.top();
                symbolStack.pop();

                if (op != OPEN_PARENTESIS)
                    resultVector.push_back(op);
                else {
                    hasOpenParentesis--;
                    if (prox == CLOSE_PARENTESIS)
                        break;
                }
            }

            hasLeftValue = false;

            // A n�o ser que seja um fecha par�ntesis, que n�o aparece, coloca
            // o operador na pilha
            if (prox != CLOSE_PARENTESIS)
                symbolStack.push(prox);
        }
    }

    // Esvazia o que sobrou na pilha
    while (symbolStack.size() > 0)
    {
        TokenType op = symbolStack.top();
        symbolStack.pop();

        if (op != OPEN_PARENTESIS && op != CLOSE_PARENTESIS)
            resultVector.push_back(op);
    }

    // Pilha de valores
    std::stack<TokenType> valueStack;

    unsigned int i;
    for (i = 0; i < resultVector.size(); i++)
    {
        if (resultVector[i] == INTEGER || resultVector[i] == BOOLEAN)
            valueStack.push(resultVector[i]);

        // Operador de '-' un�rio
        else if (resultVector[i] == NEGATIVE)
        {
            if (valueStack.empty())
                throw "Express�o inv�lida";

            if (valueStack.top() != INTEGER)
                throw "N�o � poss�vel aplicar '-' ao tipo Boolean";

            // Teoricamente aqui ele tira um da pilha e p�e outro, mas como
            // n�o tem valores ia s� tirar INTEGER e colocar de novo.
            // Desnecessauro.
        }

        // Operador de nega��o l�gico
        else if (resultVector[i] == NOT)
        {
            if (valueStack.empty())
                throw "Express�o inv�lida";

            if (valueStack.top() != BOOLEAN)
                throw "N�o � poss�vel aplicar 'not' ao tipo Integer";

            // Teoricamente aqui ele tira um da pilha e p�e outro, mas como
            // n�o tem valores ia s� tirar BOOLEAN e colocar de novo.
            // Desnecessauro 2.
        }

        // Aritm�ticos
        else if ((int)resultVector[i] >= (int)SUM &&
                 (int)resultVector[i] <= (int)MOD)
        {
            if (valueStack.size() < 2)
                throw "Express�o inv�lida";

            TokenType a, b;

            a = valueStack.top();
            valueStack.pop();

            b = valueStack.top();
            valueStack.pop();

            if (a != INTEGER || b != INTEGER)
                throw "N�o � poss�vel usar operadores aritm�ticos com o tipo Boolean";

            // O resultado � sempre inteiro
            valueStack.push(INTEGER);
        }

        // L�gicos
        else if ((int)resultVector[i] >= (int)AND &&
                 (int)resultVector[i] <= (int)XOR)
        {
            if (valueStack.size() < 2)
                throw "Express�o inv�lida";

            TokenType a, b;

            a = valueStack.top();
            valueStack.pop();

            b = valueStack.top();
            valueStack.pop();

            if (a != BOOLEAN || b != BOOLEAN)
                throw "N�o � poss�vel usar operadores l�gicos com o tipo Integer";

            // O resultado � sempre booleano
            valueStack.push(BOOLEAN);
        }

        // Comparadores
        else if ((int)resultVector[i] >= (int)EQUAL &&
                 (int)resultVector[i] <= (int)SMALLER_OR_EQUAL)
        {
            if (valueStack.size() < 2)
                throw "Express�o inv�lida";

            TokenType a, b;

            a = valueStack.top();
            valueStack.pop();

            b = valueStack.top();
            valueStack.pop();

            // Se forem tipos diferentes, um � integer e outro � boolean com
            // certeza, e isso t� errado.
            if (a != b)
                throw "N�o � poss�vel comparar os tipos Integer e Boolean";

            // O resultado de um comparador � sempre booleanp
            valueStack.push(BOOLEAN);
        }
        else
            throw "Express�o inv�lida";
    }

    if (valueStack.size() != 1)
        throw "Express�o inv�lida";

    return valueStack.top() == INTEGER ? tINTEGER : tBOOLEAN;
}

/**
 * Compila um condicional
 */
void Parser::compileIf()
{
    TokenType prox = _lexer.getToken();
    if (prox != IF)
        throw "If esperado";

    prox = _lexer.nextToken();
    ValueType type = compileExpression();
    if (type != tBOOLEAN)
        throw "Express�o booleana esperada";

    prox = _lexer.getToken();
    if (prox != THEN)
        throw "'then' esperado";

    _isInsideConditional = 1;

    prox = _lexer.nextToken();
    compileNestedCommand();

    _isInsideConditional = 0;

    prox = _lexer.getToken();
    if (prox == ELSE)
    {
        prox = _lexer.nextToken();
        compileNestedCommand();
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

    prox = _lexer.nextToken();
    ValueType type = compileExpression();
    if (type != tBOOLEAN)
        throw "Express�o booleana esperada";

    prox = _lexer.getToken();
    if (prox != DO)
        throw "'do' esperado";

    _isInsideConditional = 1;

    prox = _lexer.nextToken();
    compileNestedCommand();

    _isInsideConditional = 0;
}

/**
 * Compila uma lista de par�metros
 */
int Parser::compileParameterDeclaration(Parameter** params)
{
    TokenType prox = _lexer.getToken();
    if (prox != OPEN_PARENTESIS)
        throw "Abre par�nteses esperado";

    int n = 0;

    int sz = 8;
    *params = (Parameter*)malloc(sz * sizeof(Parameter));

    prox = _lexer.nextToken();

    while (prox != CLOSE_PARENTESIS)
    {
        if (n >= sz)
        {
            Parameter* aux = (Parameter*)malloc(sizeof(Parameter) * sz * 2);
            memcpy(aux, *params, sz * sizeof(Parameter));
            sz *= 2;

            free(*params);
            *params = aux;
        }

        char var = 0;

        if (prox == VAR)
        {
            var = 1;
            prox = _lexer.nextToken();
        }

        if (prox != NAME)
            throw "Nome do par�metro esperado";

        Parameter p(_lexer.getName(), _level, tVOID, var);

        prox = _lexer.nextToken();
        if (prox != COLON)
            throw "':' esperado";

        prox = _lexer.nextToken();
        if (prox != INTEGER && prox != BOOLEAN)
            throw "Tipo do par�metro esperado";

        p.setType(prox == INTEGER ? tINTEGER : tBOOLEAN);

        Variable v(p.getName(), _level, p.getType());
        addVariable(v);

        prox = _lexer.nextToken();

        if (prox != COMMA && prox != CLOSE_PARENTESIS)
            throw "')' esperado";

        n++;

        if (prox == COMMA)
            prox = _lexer.nextToken();
    }

    _lexer.nextToken();

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

    prox = _lexer.nextToken();

    increaseLevel();

    Parameter* parameters;
    int n = compileParameterDeclaration(&parameters);

    prox = _lexer.getToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

    prox = _lexer.nextToken();

    Function p(name, _level - 1, tVOID, n, parameters);
    addFunction(p);

    prox = _lexer.getToken();

    if (prox == VAR)
        compileVariableDeclaration();

    prox = _lexer.getToken();

    while (prox == PROCEDURE || prox == FUNCTION)
    {
        printf("lmao\r\n");
        if (prox == PROCEDURE)
            compileProcedure();
        else
            compileFunction();

        prox = _lexer.getToken();
    }

    printf("1\r\n");
    compileCompositeCommand();
    printf("2\r\n");

    decreaseLevel();

    prox = _lexer.getToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();
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

    increaseLevel();

    Parameter* parameters;
    int n = compileParameterDeclaration(&parameters);

    prox = _lexer.getToken();
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
    if (prox != SEMICOLON)
        throw "';' esperado";

    Function f(name, _level - 1, type, n, parameters);
    addFunction(f);

    prox = _lexer.nextToken();

    if (prox == VAR)
        compileVariableDeclaration();

    prox = _lexer.getToken();

    while (prox == PROCEDURE || prox == FUNCTION)
    {
        printf("lmfao\r\n");
        if (prox == PROCEDURE)
            compileProcedure();
        else
            compileFunction();

        prox = _lexer.getToken();
    }

    _runningFunction = name;
    _returnType = tVOID;

    printf("asd\r\n");
    compileCompositeCommand();
    printf("bsd\r\n");

    _runningFunction = NULL;
    if (_returnType == tVOID)
        throw "� poss�vel que a fun��o n�o retorne nenhum valor.";
    _returnType = tVOID;

    decreaseLevel();

    prox = _lexer.getToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();
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
}

/**
 * Compila a lista de par�metros passada para uma chamada de fun��o ou
 * procedimento
 *
 * \param method Ponteiro para a fun��o ou procedimento sendo chamado
 */
void Parser::compileParameters(const Function& method)
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
        ValueType type;
        if (!method.getParameters()[n].isReference())
        {
            type = compileExpression();
            prox = _lexer.getToken();
        }
        else if (prox == NAME)
        {
            const char* name = _lexer.getName();

            prox = _lexer.nextToken();

            if (prox != COMMA || prox != CLOSE_PARENTESIS)
                throw "Par�metro por refer�ncia esperado.";
            else if (getVariable(name).getType() != method.getParameters()[n].getType())
                throw "Tipo inv�lido para este par�metro.";
        }


        if (prox == COMMA)
            prox = _lexer.nextToken();
        else if (prox != CLOSE_PARENTESIS)
            throw "')' esperado.";

        if (n + 1 > method.getParameterCount())
            throw "N�mero inv�lido de par�metros.";

        if (method.getParameters()[n].getType() != type)
            throw "Tipo inv�lido para este par�metro.";

        n++;
    }

    if (n < method.getParameterCount())
        throw "N�mero inv�lido de par�metros.";
}

/**
 * Compila uma chamada de fun��o
 *
 * \return O tipo da fun��o
 */
ValueType Parser::compileFunctionCall(const char* name)
{
    Function f = getFunction(name);
    compileParameters(f);
    return f.getReturnType();
}

/**
 * Compila um retorno de fun��o
 *
 * \param name Nome da fun��o
 */
void Parser::compileFunctionReturn(const char* name)
{
    printf("returning from %s\r\n", name);

    if (strcmp(name, _runningFunction) != 0)
        throw "Opera��o inv�lida.";

    TokenType prox = _lexer.getToken();
    if (prox != SETTER)
        throw "':=' esperado.";

    _lexer.nextToken();

    ValueType type = compileExpression();
    Function f = getFunction(name);

    if (f.getReturnType() != type)
        throw "Tipo inv�lido para retorno da fun��o!";

    if (!_isInsideConditional)
        _returnType = type;
}
