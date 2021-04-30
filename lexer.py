# num: 1; 123; 1.789; 3.14e3; 1e-3      /[0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)?/
# id                                    /[a-zA-Z_][a-zA-z_0-9]*/
# plus                                  /\+/
# minus                                 /\-/
# mult                                  /\*/
# div                                   /\//
# exp                                   /\^/
# lpar                                  /\(/
# rpar                                  /\)/
# comma                                 /,/
# eq                                    /=/

def T(s, a):
    if s == 0:
        if '0' <= a <= '9':
            return 1;
        elif ('a' <= a <= 'z') or (a == '_'):
            return 6;
        elif a == '+':
            return 7
        elif a == '-':
            return 8
        elif a == '*':
            return 9
        elif a == '/':
            return 10
        elif a == '^':
            return 11
        elif a == '(':
            return 12
        elif a == ')':
            return 13
        elif a == ',':
            return 14
    elif s == 6:
        if ('a' <= a <= 'z') or (a == '_') or ('0' <= a <= '9'):
            return 6
    elif s == 1:
        if a == '.':
            return 2
        elif a == 'e' or a == 'E':
            return 3
        elif '0' <= a <= '9':
            return 1
    elif s == 2:
        if '0' <= a <= '9':
            return 2
        elif a == 'e' or a == 'E':
            return 3
    elif s == 3:
        if '0' <= a <= '9':
            return 5
        elif a == '+' or a == '-':
            return 4
    elif s == 4:
        if '0' <= a <= '9':
            return 5
    elif s == 5:
        if '0' <= a <= '9':
            return 5
    return -1

def stateToTok(s):
    if s in [1, 2, 5]:
        return 'Num'
    elif s == 6:
        return 'Id'
    elif s == 7:
        return 'Plus'
    elif s == 8:
        return 'Minus'
    elif s == 9:
        return 'Mul'
    elif s == 10:
        return 'Div'
    elif s == 11:
        return 'Exp'
    elif s == 12:
        return 'LPar'
    elif s == 13:
        return 'RPar'
    elif s == 14:
        return 'Comma'
    else:
        return False

def getNextToken(input, startpos=0):
    s = 0;
    lastAcc = False
    lastPos = -1

    pos = 0
    while s > -1:
        tok = stateToTok(s)
        if tok != False:
            lastAcc = tok
            lastPos = pos
        if pos >= len(input): break
        a = input[pos]
        pos += 1
        s = T(s, a)
        if s == -1: break
    if lastAcc != False:
        tokStr = input[0:lastPos]
        return ((lastAcc, tokStr), input[lastPos:])

class token:
    pass
if __name__ == '__main__':
    pass

inputStr = "123.456+x^2"

while inputStr != '':
    tok, inputStr = getNextToken(inputStr)
    print(tok)
    if tok == False: break
