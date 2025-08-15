#include "clang/AST/Attr.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

#include "clang/AST/AST.h"
#include "clang/AST/Attr.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;

class SpecialFuncPrinter : public MatchFinder::MatchCallback {
public:
  void run(const MatchFinder::MatchResult &Result) override {
    const FunctionDecl *FD =
        Result.Nodes.getNodeAs<FunctionDecl>("specialFunc");
    if (!FD || !FD->hasBody())
      return;

    // annotate("special") チェック
    bool IsSpecial = false;
    for (const Attr *A : FD->attrs()) {
      if (const AnnotateAttr *AA = dyn_cast<AnnotateAttr>(A)) {
        if (AA->getAnnotation() == "special") {
          IsSpecial = true;
          break;
        }
      }
    }
    if (!IsSpecial)
      return;

    const SourceManager &SM = *Result.SourceManager;
    const LangOptions &LangOpts = Result.Context->getLangOpts();

    // 関数宣言の出力
    SourceLocation FuncBegin = FD->getBeginLoc();
    const Stmt *Body = FD->getBody();
    if (FuncBegin.isValid() && Body->getBeginLoc().isValid()) {
      SourceLocation DeclLoc = SM.getFileLoc(FuncBegin);
      unsigned DeclLine = SM.getSpellingLineNumber(DeclLoc);
      llvm::outs() << "#line " << DeclLine << " \"" << SM.getFilename(DeclLoc)
                   << "\"\n";

      SourceRange DeclRange(FuncBegin,
                            Body->getBeginLoc().getLocWithOffset(-1));
      bool Invalid;
      StringRef DeclText = Lexer::getSourceText(
          CharSourceRange::getCharRange(DeclRange), SM, LangOpts, &Invalid);
      if (!Invalid)
        llvm::outs() << DeclText << "\n";
    }

    // 関数本体の出力
    SourceLocation BodyBeginLoc = SM.getFileLoc(Body->getBeginLoc());
    SourceLocation BodyEndLoc = SM.getFileLoc(Body->getEndLoc());
    bool Invalid;
    StringRef BodyText = Lexer::getSourceText(
        CharSourceRange::getTokenRange(SourceRange(BodyBeginLoc, BodyEndLoc)),
        SM, LangOpts, &Invalid);
    if (!Invalid) {
      SmallVector<StringRef, 16> Lines;
      BodyText.split(Lines, '\n');

      for (auto &Line : Lines) {
        // 空行はそのまま出す
        if (Line.trim().empty()) {
          llvm::outs() << Line << "\n";
          continue;
        }
        // 行ごとに #line を振る
        unsigned LineNo = SM.getSpellingLineNumber(BodyBeginLoc);
        llvm::outs() << "#line " << LineNo << " \""
                     << SM.getFilename(BodyBeginLoc) << "\"\n";
        llvm::outs() << Line << "\n";
        BodyBeginLoc =
            BodyBeginLoc.getLocWithOffset(Line.size() + 1); // 次行に進める
      }
    }
  }
};

void registerMatchers(clang::ast_matchers::MatchFinder &Finder,
                      SpecialFuncPrinter &Printer) {
  Finder.addMatcher(functionDecl(hasAttr(attr::Annotate)).bind("specialFunc"),
                    &Printer);
}

static llvm::cl::OptionCategory MyToolCategory("my-clang-check options");

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }

  CommonOptionsParser &OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  SpecialFuncPrinter Printer;
  clang::ast_matchers::MatchFinder Finder;
  registerMatchers(Finder, Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
