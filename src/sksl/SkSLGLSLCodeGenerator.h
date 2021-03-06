/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
 
#ifndef SKSL_GLSLCODEGENERATOR
#define SKSL_GLSLCODEGENERATOR

#include <stack>
#include <tuple>
#include <unordered_map>

#include "SkSLCodeGenerator.h"
#include "ir/SkSLBinaryExpression.h"
#include "ir/SkSLBoolLiteral.h"
#include "ir/SkSLConstructor.h"
#include "ir/SkSLDoStatement.h"
#include "ir/SkSLExtension.h"
#include "ir/SkSLFloatLiteral.h"
#include "ir/SkSLIfStatement.h"
#include "ir/SkSLIndexExpression.h"
#include "ir/SkSLInterfaceBlock.h"
#include "ir/SkSLIntLiteral.h"
#include "ir/SkSLFieldAccess.h"
#include "ir/SkSLForStatement.h"
#include "ir/SkSLFunctionCall.h"
#include "ir/SkSLFunctionDeclaration.h"
#include "ir/SkSLFunctionDefinition.h"
#include "ir/SkSLPrefixExpression.h"
#include "ir/SkSLPostfixExpression.h"
#include "ir/SkSLProgramElement.h"
#include "ir/SkSLReturnStatement.h"
#include "ir/SkSLStatement.h"
#include "ir/SkSLSwizzle.h"
#include "ir/SkSLTernaryExpression.h"
#include "ir/SkSLVarDeclarations.h"
#include "ir/SkSLVarDeclarationsStatement.h"
#include "ir/SkSLVariableReference.h"
#include "ir/SkSLWhileStatement.h"

namespace SkSL {

#define kLast_Capability SpvCapabilityMultiViewport

struct GLCaps {
    GLCaps() {}

    int fVersion = 400;
    enum {
        kGL_Standard,
        kGLES_Standard
    } fStandard = kGL_Standard;
    bool fIsCoreProfile = false;
    bool fUsesPrecisionModifiers = false;
    bool fMustDeclareFragmentShaderOutput = false;
    bool fShaderDerivativeSupport = true;
    // extension string to enable derivative support, or null if unnecessary
    std::string fShaderDerivativeExtensionString;
    // The Tegra3 compiler will sometimes never return if we have min(abs(x), y)
    bool fCanUseMinAndAbsTogether = true;
    // On Intel GPU there is an issue where it misinterprets an atan argument (second argument only,
    // apparently) of the form "-<expr>" as an int, so we rewrite it as "-1.0 * <expr>" to avoid
    // this problem
    bool fMustForceNegatedAtanParamToFloat = false;
};

/**
 * Converts a Program into GLSL code.
 */
class GLSLCodeGenerator : public CodeGenerator {
public:
    enum Precedence {
        kParentheses_Precedence    =  1,
        kPostfix_Precedence        =  2,
        kPrefix_Precedence         =  3,
        kMultiplicative_Precedence =  4,
        kAdditive_Precedence       =  5,
        kShift_Precedence          =  6,
        kRelational_Precedence     =  7,
        kEquality_Precedence       =  8,
        kBitwiseAnd_Precedence     =  9,
        kBitwiseXor_Precedence     = 10,
        kBitwiseOr_Precedence      = 11,
        kLogicalAnd_Precedence     = 12,
        kLogicalXor_Precedence     = 13,
        kLogicalOr_Precedence      = 14,
        kTernary_Precedence        = 15,
        kAssignment_Precedence     = 16,
        kSequence_Precedence       = 17,
        kTopLevel_Precedence       = 18
    };

    GLSLCodeGenerator(const Context* context, GLCaps caps)
    : fContext(*context)
    , fCaps(caps) {}

    void generateCode(const Program& program, std::ostream& out) override;

private:
    void write(const char* s);

    void writeLine();

    void writeLine(const char* s);

    void write(const std::string& s);

    void writeLine(const std::string& s);

    void writeType(const Type& type);

    void writeExtension(const Extension& ext);

    void writeInterfaceBlock(const InterfaceBlock& intf);

    void writeFunctionStart(const FunctionDeclaration& f);
    
    void writeFunctionDeclaration(const FunctionDeclaration& f);

    void writeFunction(const FunctionDefinition& f);

    void writeLayout(const Layout& layout);

    void writeModifiers(const Modifiers& modifiers, bool globalContext);
    
    void writeGlobalVars(const VarDeclaration& vs);

    void writeVarDeclarations(const VarDeclarations& decl, bool global);

    void writeVariableReference(const VariableReference& ref);

    void writeExpression(const Expression& expr, Precedence parentPrecedence);
    
    void writeIntrinsicCall(const FunctionCall& c);

    void writeMinAbsHack(Expression& absExpr, Expression& otherExpr);

    void writeFunctionCall(const FunctionCall& c);

    void writeConstructor(const Constructor& c);

    void writeFieldAccess(const FieldAccess& f);

    void writeSwizzle(const Swizzle& swizzle);

    void writeBinaryExpression(const BinaryExpression& b, Precedence parentPrecedence);

    void writeTernaryExpression(const TernaryExpression& t, Precedence parentPrecedence);

    void writeIndexExpression(const IndexExpression& expr);

    void writePrefixExpression(const PrefixExpression& p, Precedence parentPrecedence);

    void writePostfixExpression(const PostfixExpression& p, Precedence parentPrecedence);

    void writeBoolLiteral(const BoolLiteral& b);

    void writeIntLiteral(const IntLiteral& i);

    void writeFloatLiteral(const FloatLiteral& f);

    void writeStatement(const Statement& s);

    void writeBlock(const Block& b);

    void writeIfStatement(const IfStatement& stmt);

    void writeForStatement(const ForStatement& f);

    void writeWhileStatement(const WhileStatement& w);

    void writeDoStatement(const DoStatement& d);

    void writeReturnStatement(const ReturnStatement& r);

    const Context& fContext;
    const GLCaps fCaps;
    std::ostream* fOut = nullptr;
    std::stringstream fHeader;
    std::string fFunctionHeader;
    Program::Kind fProgramKind;
    int fVarCount = 0;
    int fIndentation = 0;
    bool fAtLineStart = false;
    // Keeps track of which struct types we have written. Given that we are unlikely to ever write 
    // more than one or two structs per shader, a simple linear search will be faster than anything 
    // fancier.
    std::vector<const Type*> fWrittenStructs;
    // true if we have run into usages of dFdx / dFdy
    bool fFoundDerivatives = false;
};

}

#endif
