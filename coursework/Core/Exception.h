//
// Created by Dimanche on 29/01/2021.
//

#ifndef CPP_EXCEPTION_H
#define CPP_EXCEPTION_H

#define ThrowIf0(argName) if (!argName) throw #argName" is null!"
/**
 * Used when such exit doesn't really exist
 */
#define DummyThrow() throw 0;
#define ShallThrow(expression) DummyThrow()

#define OverloadComparisonOperatorsDeclaration1(typeName, typeArg1) \
template <class typeArg1> \
friend bool operator==(typeName<typeArg1> lhs, typeName<typeArg1> rhs);          \
template <class typeArg1> \
friend bool operator!=(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator>(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator>=(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator<(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator<=(typeName<typeArg1> lhs, typeName<typeArg1> rhs);

#define OverloadComparisonOperatorDefinition1(typeName, typeArg1) \
template <class typeArg1> \
bool operator==(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return !typeName<typeArg1>::CompareTo(lhs, rhs); } \
template <class typeArg1> \
bool operator!=(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs); } \
template <class typeArg1> \
bool operator>(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)>0; } \
template <class typeArg1> \
bool operator>=(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)>=0; } \
template <class typeArg1> \
bool operator<(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)<0; } \
template <class typeArg1> \
bool operator<=(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)<=0; }

#define NotImplementedException() throw "function '" __PRETTY_FUNCTION__ "' not implemented yet."
#endif //CPP_EXCEPTION_H
