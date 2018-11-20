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
    typedef enum {
        //大类型
        Program=0,StmtK,ExpK,
        //语句的类型
        IfK,ElseK,WhileK,AssignK,DeclK,ReadK,WriteK,
        //表达式类型
        OperationK,ConditionK,TermK,FactorK,LiteralK,VarK,
        //运算符的类型
        plusOp, minusOp,multiOp,divideOp
    } TreeNodeK;
    
    static string getTrueType(TreeNodeK k){
        switch (k) {
            case 0:
                return "Program";
            case 1:
                return "Stmt";
            case 2:
                return "Exp";
            case 3:
                return "If";
            case 4:
                return "Else";
            case 5:
                return "While";
            case 6:
                return "Assign";
            case 7:
                return "Decl";
            case 8:
                return "Read";
            case WriteK:
                return "Write";
            case OperationK:
                return "Operation";
            case ConditionK:
                return "Condtion";
            case TermK:
                return "Term";
            case FactorK:
                return "Factor";
            case LiteralK:
                return "Literal";
            case VarK:
                return "Variable";
            case plusOp:
                return "PlusOp";
            case minusOp:
                return "MinusOp";
            case multiOp:
                return "MultiOp";
            case divideOp:
                return "DivideOp";
        }
        return "";
    }
}



#endif /* TreeNodeType_h */
