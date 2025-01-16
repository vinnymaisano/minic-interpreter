#include "val.h"

Value Value::operator+(const Value& op) const {
    ValType a = GetType(); // in the scope of a value class member function
    ValType b = op.GetType();

    switch(a) {
        case VSTRING:
            switch(b) {
                case VSTRING:
                    return Value(GetString() + op.GetString());
                case VCHAR:
                    return Value(GetString() + op.GetChar());
                default:
                    break;
            }
            break;
        case VCHAR:
            switch(b) {
                case VSTRING:
                    return Value(GetChar() + op.GetString());
                case VCHAR:
                    return Value(GetChar() + op.GetChar());
                case VINT:
                    return Value(GetChar() + op.GetInt());
                case VREAL:
                    return Value(GetChar() + op.GetReal());
                default:
                    break;
            }
            break;
        case VINT:
            switch(b) {
                case VCHAR:
                    return Value(GetInt() + op.GetChar());
                case VINT:
                    return Value(GetInt() + op.GetInt());
                case VREAL:
                    return Value(GetInt() + op.GetReal());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VINT:
                    return Value(GetReal() + op.GetInt());
                case VREAL:
                    return Value(GetReal() + op.GetReal());
                case VCHAR:
                    return Value(GetReal() + op.GetChar());
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return Value();
}

Value Value::operator-(const Value& op) const {
    ValType a = GetType(); // in the scope of a value class member function
    ValType b = op.GetType();

    switch(a) {
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() - op.GetChar());
                case VINT:
                    return Value(GetChar() - op.GetInt());
                case VREAL:
                    return Value(GetChar() - op.GetReal());
                default:
                    break;
            }
            break;
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() - op.GetInt());
                case VCHAR:
                    return Value(GetInt() - op.GetChar());
                case VREAL:
                    return Value(GetInt() - op.GetReal());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VREAL:
                    return Value(GetReal() - op.GetReal());
                case VINT:
                    return Value(GetReal() - op.GetInt());
                case VCHAR:
                    return Value(GetReal() - op.GetChar());
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return Value();
}

Value Value::operator*(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch(a) {
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() * op.GetChar());
                case VINT:
                    return Value(GetChar() * op.GetInt());
                case VREAL:
                    return Value(GetChar() * op.GetReal());
                default:
                    break;
            }
            break;
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() * op.GetInt());
                case VCHAR:
                    return Value(GetInt() * op.GetChar());
                case VREAL:
                    return Value(GetInt() * op.GetReal());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VREAL:
                    return Value(GetReal() * op.GetReal());
                case VINT:
                    return Value(GetReal() * op.GetInt());
                case VCHAR:
                    return Value(GetReal() * op.GetChar());
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return Value();
}

Value Value::operator/(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch(a) {
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() / op.GetChar());
                case VINT:
                    return Value(GetChar() / op.GetInt());
                case VREAL:
                    return Value(GetChar() / op.GetReal());
                default:
                    break;
            }
            break;
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() / op.GetInt());
                case VCHAR:
                    return Value(GetInt() / op.GetChar());
                case VREAL:
                    return Value(GetInt() / op.GetReal());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VREAL:
                    return Value(GetReal() / op.GetReal());
                case VINT:
                    //cout << "divide float by int" << endl;
                    return Value(GetReal() / op.GetInt());
                case VCHAR:
                    return Value(GetReal() / op.GetChar());
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return Value();
}

Value Value::operator%(const Value & op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch(a) {
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() % op.GetChar());
                case VINT:
                    return Value(GetChar() % op.GetInt());
                default:
                    break;
            }
            break;
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() % op.GetInt());
                case VCHAR:
                    return Value(GetInt() % op.GetChar());
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return Value();
}

Value Value::operator==(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch(a) {
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() == op.GetInt());
                case VREAL:
                    return Value(GetInt() == op.GetReal());
                case VCHAR:
                    return Value(GetInt() == op.GetChar());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VINT:
                    return Value(GetReal() == op.GetInt());
                case VREAL:
                    return Value(GetReal() == op.GetReal());
                default:
                    break;
            }
            break;
        case VSTRING:
            switch(b) {
                case VSTRING:
                    return Value(GetString() == op.GetString());
                default:
                    break;
            }
            break;
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() == op.GetChar());
                case VINT:
                    return Value(GetChar() == op.GetInt());
                default:
                    break;
            }
            break;
        case VBOOL:
            switch(b) {
                case VBOOL:
                    return Value(GetBool() == op.GetBool());
                default:
                    break;
            }
            break;
        default:
            break;
    }
    
    return Value();
}

Value Value::operator!=(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch(a) {
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() != op.GetInt());
                case VREAL:
                    return Value(GetInt() != op.GetReal());
                case VCHAR:
                    return Value(GetInt() != op.GetChar());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VINT:
                    return Value(GetReal() != op.GetInt());
                case VREAL:
                    return Value(GetReal() != op.GetReal());
                default:
                    break;
            }
            break;
        case VSTRING:
            switch(b) {
                case VSTRING:
                    return Value(GetString() != op.GetString());
                default:
                    break;
            }
            break;
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() != op.GetChar());
                case VINT:
                    return Value(GetChar() != op.GetInt());
                default:
                    break;
            }
            break;
        case VBOOL:
            switch(b) {
                case VBOOL:
                    return Value(GetBool() != op.GetBool());
                default:
                    break;
            }
            break;
        default:
            break;
    }
    
    return Value();
}

Value Value::operator>(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();    

    switch(a) {
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() > op.GetInt());
                case VREAL:
                    return Value(GetInt() > op.GetReal());
                case VCHAR:
                    return Value(GetInt() > op.GetChar());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VINT:
                    return Value(GetReal() > op.GetInt());
                case VREAL:
                    return Value(GetReal() > op.GetReal());
                default:
                    break;
            }
            break;
        case VSTRING:
            switch(b) {
                case VSTRING:
                    return Value(GetString() > op.GetString());
                case VCHAR:
                    if (GetString().length() == 1) return Value(GetString()[0] > op.GetChar());
                    else break;
                default:
                    break;
            }
            break;
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() > op.GetChar());
                case VSTRING:
                    if (op.GetString().length() == 1) return Value(GetChar() > op.GetString()[0]);
                    else break;
                case VINT:
                    return Value(GetChar() > op.GetInt());
                default:
                    break;
            }
            break;
        case VBOOL:
            switch(b) {
                case VBOOL:
                    return Value(GetBool() > op.GetBool());
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return Value();
}

Value Value::operator<(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();    

    switch(a) {
        case VINT:
            switch(b) {
                case VINT:
                    return Value(GetInt() < op.GetInt());
                case VREAL:
                    return Value(GetInt() < op.GetReal());
                case VCHAR:
                    return Value(GetInt() < op.GetChar());
                default:
                    break;
            }
            break;
        case VREAL:
            switch(b) {
                case VINT:
                    return Value(GetReal() < op.GetInt());
                case VREAL:
                    return Value(GetReal() < op.GetReal());
                default:
                    break;
            }
            break;
        case VSTRING:
            switch(b) {
                case VSTRING:
                    return Value(GetString() < op.GetString());
                case VCHAR:
                    if (GetString().length() == 1) return Value(GetString()[0] < op.GetChar());
                    else break;
                default:
                    break;
            }
            break;
        case VCHAR:
            switch(b) {
                case VCHAR:
                    return Value(GetChar() < op.GetChar());
                case VSTRING:
                    if (op.GetString().length() == 1) return Value(GetChar() < op.GetString()[0]);
                    else break;
                case VINT:
                    return Value(GetChar() < op.GetInt());
                default:
                    break;
            }
            break;
        case VBOOL:
            switch(b) {
                case VBOOL:
                    return Value(GetBool() < op.GetBool());
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return Value();
}

Value Value::operator&&(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch(a) {
        case VBOOL:
            switch(b) {
                case VBOOL:
                    return Value(GetBool() && op.GetBool());
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return Value();
}

Value Value::operator||(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch(a) {
        case VBOOL:
            switch(b) {
                case VBOOL:
                    return Value(GetBool() || op.GetBool());
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return Value();
}

Value Value::operator!(void) const {
    ValType a = GetType();

    switch (a) {
        case VBOOL:
            return Value(!GetBool());
        default:
            break;
    }
    
    return Value();
}