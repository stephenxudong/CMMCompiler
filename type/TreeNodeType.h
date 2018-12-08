//
//  TreeNodeType.h
//  CMM
//
//  Created by Stephen Liao on 2018/10/10.
//  Copyright © 2018 Stephen. All rights reserved.
//

#ifndef TreeNodeType_h
#define TreeNodeType_h

#include <string>
using std::string;
//namespace tnt{
//    //节点类型，是语句还是表达式
//    typedef enum {PROGRAM,StmtK,ExpK,Opk} NodeKind;
//    //语句的类型
//    typedef enum {IfK,ElseK,WhileK,AssignK,DeclK,ReadK,WriteK} StmtKind;
//    //表达式类型
//    typedef enum {OperationK,ConditionK,TermK,FactorK,LiteralK,VarK} ExpKind;
//    //运算符的类型
//    typedef enum {plusOp, minusOp,multiOp,divideOp} Opkind;
//}

namespace TreeNodeType{
//    typedef enum {
//        //大类型
//        Program=0,StmtK,ExpK,ErrorK,
//        //语句的类型
//        IfK,ElseK,ForK,WhileK,AssignK,DeclK,ReadK,WriteK,
//        //表达式类型
//        OperationK,ConditionK,TermK,FactorK,LiteralK,VarK,
//        //运算符的类型
//        plusOp, minusOp,multiOp,divideOp
//    } TreeNodeK;
    
    static const string Programk = "Program";
    static const string Stmtk = "Stmt";
    static const string ExpK = "Exp";
    static const string ErrorK = "Error";
    static const string Ifk = "If";
    static const string ElseK = "Else";
    static const string declK = "declear";
    static const string ReadK = "read";
    static const string WriteK = "write";
    static const string WhileK = "while";
    static const string ForK = "for";
    static const string ConditionK = "condition";
    static const string IdentifierK = "identifier";

    static const string operatorK = "operator";

    static const string assign = "=";

//    static string getTrueType(TreeNodeK k){
//        switch (k) {
//            case Program:
//                return "Program";
//            case StmtK:
//                return "Stmt";
//            case ExpK:
//                return "Exp";
//            case ErrorK:
//                return "Error";
//            case IfK:
//                return "If";
//            case ElseK:
//                return "Else";
//            case ForK:
//                return "For";
//            case WhileK:
//                return "While";
//            case AssignK:
//                return "Assign";
//            case DeclK:
//                return "Decl";
//            case ReadK:
//                return "Read";
//            case WriteK:
//                return "Write";
//            case OperationK:
//                return "Operation";
//            case ConditionK:
//                return "Condtion";
//            case TermK:
//                return "Term";
//            case FactorK:
//                return "Factor";
//            case LiteralK:
//                return "Literal";
//            case VarK:
//                return "Variable";
//            case plusOp:
//                return "PlusOp";
//            case minusOp:
//                return "MinusOp";
//            case multiOp:
//                return "MultiOp";
//            case divideOp:
//                return "DivideOp";
//        }
//        return "";
//    }
}



#endif /* TreeNodeType_h */
