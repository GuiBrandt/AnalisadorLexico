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
            r = &_procedure[i];
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
            r = &_function[i];
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
 * Verifica se um procedimento existe
 *
 * \param name Nome do procedimento
 */
char Parser::hasProcedure(const char* name)
{
    int i;
    for (i = 0; i < _procedureCount; i++)
        if (strcmp(_procedure[i].getName(), name) == 0)
            return 1;

    return 0;
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

    // Sobe um n�vel
    increaseLevel();

    prox = _lexer.nextToken();

    while (prox != END)
    {
        compileCommand();
        prox = _lexer.getToken();
    }

    prox = _lexer.nextToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

    // Desce um n�vel
    decreaseLevel();

    prox = _lexer.nextToken();
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
        if (hasFunction(_lexer.getName()) || hasProcedure(_lexer.getName()))
            compileMethodCall();
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

    prox = _lexer.getToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();
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

    prox = _lexer.nextToken();
    if (prox != SEMICOLON)
        throw "';' esperado";

    _lexer.nextToken();
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

            try {
                getProcedure(name);
                t = (ValueType)-1;
            } catch (const char* e0) {
                try {
                    getFunction(name);
                    t = (ValueType)-2;
                } catch (const char* e1) {
                    if (_lexer.getToken() != NAME)
                        throw e1;

                    Variable resultVector = getVariable(name);
                    t = resultVector.getType();
                }
            }

            if (t == (ValueType)-2)
                compileFunctionCall();
            else if (t == (ValueType)-1)
                throw "Express�o inv�lida: Procedimento n�o retorna um valor.";

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
                throw "N�o � poss�vel aplicar '!' ao tipo Boolean";

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

    prox = _lexer.nextToken();
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

    prox = _lexer.nextToken();
    ValueType type = compileExpression();
    if (type != tBOOLEAN)
        throw "Express�o booleana esperada";

    prox = _lexer.getToken();
    if (prox != DO)
        throw "'do' esperado";

    prox = _lexer.nextToken();
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

        if (prox != NAME)
            throw "Nome do par�metro esperado";

        (*params)[n].setName(_lexer.getName());
        (*params)[n].setLevel(_level);

        prox = _lexer.nextToken();
        if (prox != COLON)
            throw "':' esperado";

        prox = _lexer.nextToken();
        if (prox != INTEGER && prox != BOOLEAN)
            throw "Tipo do par�metro esperado";

        (*params)[n].setType(prox == INTEGER ? tINTEGER : tBOOLEAN);

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

    Parameter* parameters;
    int n = compileParameterDeclaration(&parameters);

    increaseLevel();

    int i;
    for (i = 0; i < n; i++)
    {
        Variable v(parameters[i].getName(), _level, parameters[i].getType());
        addVariable(v);
    }

    Procedure p(name, _level - 1, n, parameters);
    addProcedure(p);

    prox = _lexer.getToken();
    compileCompositeCommand();

    decreaseLevel();
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

    increaseLevel();

    int i;
    for (i = 0; i < n; i++)
    {
        Variable v(parameters[i].getName(), _level, parameters[i].getType());
        addVariable(v);
    }

    Function f(name, _level - 1, type, n, parameters);
    addFunction(f);

    prox = _lexer.nextToken();
    compileCompositeCommand();

    decreaseLevel();
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
        throw "'=' esperado.";

    prox = _lexer.nextToken();
    ValueType t = compileExpression();

    if (t != v.getType())
        throw "Tipo inv�lido para a vari�vel.";

    prox = _lexer.getToken();
    if (prox != SEMICOLON)
        throw "';' esperado.";

    _lexer.nextToken();
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

    if (hasFunction(name))
        compileFunctionCall();
    else
        compileProcedureCall();

    prox = _lexer.nextToken();
    if (prox != SEMICOLON)
        throw "';' esperado";
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
