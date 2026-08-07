#include "parser.hpp"
#include "dot.hpp"
#include "evaluator.hpp"
#include "version.hpp"
#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <memory>
#include <string>

static void do_eval(const std::string& file) {
    Parser parser(file);
    auto program = parser.parse();

    Evaluator evaluator;
    program->accept(evaluator);
}

static void do_graph(const std::string& file) {
    Parser parser(file);
    auto program = parser.parse();

    DotGenerator generator;
    program->accept(generator);
}

int main(int argc, char** argv)
{
    CLI::App app("PL/0 language tool");
    app.set_version_flag("--version", VERSION "+" GIT_HASH);

    auto file = std::make_shared<std::string>();

    auto eval_app = app.add_subcommand("eval", "Execute a source file");
    eval_app->add_option("file", *file, "Source file")->required()->check(CLI::ExistingFile);
    eval_app->callback([file]() { do_eval(*file); });

    auto graph_app = app.add_subcommand("graph", "Produce a Graphviz dot representation of a source file");
    graph_app->add_option("file", *file, "Source file")->required()->check(CLI::ExistingFile);
    graph_app->callback([file]() { do_graph(*file); });

    app.require_subcommand();

    try {
        app.parse(argc, argv);
    } catch (CLI::ParseError& error) {
        return app.exit(error);
    } catch (std::exception& error) {
        fmt::println(stderr, "error: {}", error.what());
        return 1;
    }

    return 0;
}
