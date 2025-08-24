#include "clang/AST/Attr.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <filesystem>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class SpecialFuncPrinter : public MatchFinder::MatchCallback {
public:
  SpecialFuncPrinter(std::filesystem::path output_file)
      : output_file(output_file) {}

  void run(const MatchFinder::MatchResult &Result) override {
    if (const FunctionDecl *FD =
            Result.Nodes.getNodeAs<FunctionDecl>("specialFunc")) {
      ASTContext *Context = Result.Context;

      if (!FD->isThisDeclarationADefinition()) {
        return;
      }

      for (auto *Attr : FD->attrs()) {
        if (const auto *AA = dyn_cast<AnnotateAttr>(Attr)) {
          if (AA->getAnnotation() == "special") {
            llvm::outs() << "Generating metadata for function: "
                         << FD->getNameAsString() << "\n";

            unsigned numArgs = FD->getNumParams();
            std::vector<uint64_t> sizes;
            std::vector<uint64_t> aligns;

            for (unsigned i = 0; i < numArgs; i++) {
              const ParmVarDecl *Param = FD->getParamDecl(i);
              QualType QT = Param->getType();

              sizes.push_back(Context->getTypeSizeInChars(QT).getQuantity());
              aligns.push_back(Context->getTypeAlignInChars(QT).getQuantity());
            }

            generateMetadataFile(FD->getNameAsString(), numArgs, sizes, aligns);
          }
        }
      }
    }
  }

  void generateMetadataFile(const std::string &funcName, unsigned numArgs,
                            const std::vector<uint64_t> &sizes,
                            const std::vector<uint64_t> &aligns) {
    std::error_code EC;
    llvm::raw_fd_ostream out(output_file.string(), EC,
                             llvm::sys::fs::OF_Append);

    out << "extern \"C\" {\n";
    out << "  unsigned " << funcName << "_arg_num = " << numArgs << ";\n";
    out << "  unsigned " << funcName << "_arg_sizes[" << numArgs << "] = {";
    for (unsigned i = 0; i < numArgs; i++) {
      if (i)
        out << ", ";
      out << sizes[i];
    }
    out << "};\n";
    out << "  unsigned " << funcName << "_arg_aligns[" << numArgs << "] = {";
    for (unsigned i = 0; i < numArgs; i++) {
      if (i)
        out << ", ";
      out << aligns[i];
    }
    out << "};\n";
    out << "}\n";
    out << "\n";
  }

private:
  std::filesystem::path output_file;
};

static llvm::cl::OptionCategory MyToolCategory("my-annotation-matcher options");
static llvm::cl::opt<std::string>
    OutputFilename("out", llvm::cl::desc("Specify output file"),
                   llvm::cl::value_desc("filename"),
                   llvm::cl::init("generated_metadata.cpp"),
                   llvm::cl::cat(MyToolCategory));

void registerMatchers(MatchFinder &Finder, SpecialFuncPrinter &Printer) {
  auto Matcher = functionDecl(hasAttr(attr::Annotate)).bind("specialFunc");
  Finder.addMatcher(Matcher, &Printer);
}

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  const std::filesystem::path output_file = std::string(OutputFilename);
  SpecialFuncPrinter Collector(output_file);
  MatchFinder Finder;
  registerMatchers(Finder, Collector);

  std::filesystem::remove(output_file);
  int Ret = Tool.run(newFrontendActionFactory(&Finder).get());
  if (Ret != 0)
    return Ret;

  return 0;
}
