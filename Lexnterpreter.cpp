#include "Lexnterpreter.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <fstream>

using namespace std;

enum OperatorType {Mult, Div, Add, Sub};
enum TokenType {Unassigned, Number, Identifier, Operator, String};

struct Token
{
    TokenType type=Unassigned;
    string value;

    int uniID;
    
    bool inString;
    bool inBrackets;

    OperatorType opType;
};

int throwError(string msg, int errorCode)
{
    cout << "  [!] Syntax Error (Error Code: " + to_string(errorCode) + "):\n          " + msg;
    return errorCode;
}

vector<Token*> LexInput(vector<string> inp)
{
    bool inString = false;
    bool inBrackets = false;

    int sCharCount = 0; // stringCharCount
    int bCharCount = 0; // bracketCharCount
    
    vector<Token*> tokens;
    Token* currentToken = new Token();
    
    for (int i = 0; i < inp.size(); i++)
    {
        inString = false;
        inBrackets = false;

        if (sCharCount % 2 != 0)
        {
            throwError("Expected \", got nothing. Try locating where a string hasn't ended?", 1);
            break;
        }
        
        if (bCharCount % 2 != 0)
        {
            throwError("Expected ), got nothing. Try locating where a bracket hasn't ended?", 1);
            break;
        }
        
        for (char c : inp[i])
        {
            // update string/bracket stuff
            if (c == '"')
            {
                tokens.push_back(currentToken);

                currentToken = new Token();
                currentToken->type = TokenType::Operator;
                
                sCharCount++;
            }
            else if (c == ')')
            {
                tokens.push_back(currentToken);

                currentToken = new Token();
                currentToken->type = TokenType::Operator;
                
                bCharCount++;
            }
            else if (c == ':' || c == '!' || c == '=' || c == '%' || c == '&' || c == '*' || c == '/' || c == '+' || c == '-' || c == '(' || c == '>' || c == '<' || c == '\\' || c == '\'' || c == ' ')
            {
                tokens.push_back(currentToken);
                
                currentToken = new Token();
                currentToken->type = TokenType::Operator;
            }

            if (currentToken->type == TokenType::Operator)
            {
                currentToken->value += c;
                tokens.push_back(currentToken);
                
                currentToken = new Token();

                continue;
            }

            bool pInString = inString;
            bool pInBrackets = inBrackets;
            
            inString = !sCharCount % 2 == 0;
            inBrackets = !bCharCount % 2 == 0;

            if (inString && !pInString || inBrackets && !pInBrackets)
            {
                tokens.push_back(currentToken);
                currentToken = new Token();
            }

            if (currentToken->type == TokenType::Operator)
            {
                // Skip past if this is an operator    
                tokens.push_back(currentToken);
                currentToken = new Token();
            }

            currentToken->value += c;
            Token propertyToken = *currentToken;
            
            propertyToken.inBrackets = inBrackets;
            propertyToken.inString = inString;

            if (!propertyToken.inString)
            {
                if (!isdigit(c) && currentToken->type != TokenType::Operator)
                {
                    currentToken->type = TokenType::Identifier;
                }

                if (isdigit(c) && propertyToken.type == TokenType::Unassigned)
                {
                    currentToken->type = TokenType::Number;
                }
            } else
            {
                currentToken->type = TokenType::String;
            }
        }

        // new token per line
        currentToken->value += "\n";
        tokens.push_back(currentToken);
        currentToken = new Token();
    }
    
    return tokens;
}

int main (int argc, char *argv[])
{
    time_t timestamp;
    time(&timestamp); // The &timestamp means it goes to the address of the timestamp variable, so it gets changed!
    
    cout << "sol started [t" + to_string(timestamp) + "]\n";
    if (argc < 2)
        return throwError("Wrong number of arguments: Cannot be a file", -1);

    string filePath = argv[1];
    cout << "processing: " << filePath << '\n';

    ifstream file(filePath);
    if (!file || !file.is_open())
    {
        return throwError("Cannot open file", 0);
    }

    vector<string> lines;
    string line;

    while (getline(file, line))
    {
        lines.push_back(line);
    }

    vector<Token*> tokens = LexInput(lines);
    string total;

    cout << "Unassigned: " << TokenType::Unassigned;
    cout << "\nNumber: " << TokenType::Number;
    cout << "\nIdentifier: " << TokenType::Identifier;
    cout << "\nOperator: " << TokenType::Operator;
    cout << "\nString: " << TokenType::String;
    
    for (Token* token : tokens)
    {
        cout << "token type: " << (token->type) << "\ntoken value: " << token->value << "\nis in bracket?: " << to_string(token->inBrackets) << "\nis in string?: " << to_string(token->inString) << "\n";
        total += token->value;
    }

    cout << total << '\n';
    
    return 0;
}