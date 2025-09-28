#include "Dialog.hpp"
#include "UpdateScreen.hpp"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QtEnvironmentVariables>
#include <print>
#include <hyprutils/string/VarList.hpp>

using namespace Hyprutils::String;

int main(int argc, char* argv[]) {
    // disable logs to not trash the stdout
    qputenv("QT_LOGGING_RULES", QByteArray("*.debug=false;qml=false"));

    auto dialog = new CUpdateScreen();

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "--new-version") {
            if (i + 1 >= argc) {
                std::println(stderr, "--new-version requires a parameter");
                return 1;
            }

            dialog->newVersion = argv[i + 1];

            i++;
            continue;
        }

        std::println(stderr, "invalid arg {}", argv[i]);
        return 1;
    }

    if (dialog->newVersion.isEmpty()) {
        std::println(stderr, "missing --new-version");
        return 1;
    }

    QGuiApplication app(argc, argv);
    app.setApplicationName("Hyprland Updated!");
    app.setApplicationDisplayName("Hyprland Updated");

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
        QQuickStyle::setStyle("org.hyprland.style");

    // This entire mechanism fucking sucks,
    // but I also suck at qml and I want to avoid spawning a new process as it takes a while.
    auto popup     = new CDialog();
    popup->title   = "Information";
    popup->text    = "If you wish to disable this dialog, set ecosystem:no_update_news to true in your Hyprland config.";
    popup->buttons = {"ok"};

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("updateScreen", dialog);
    engine.rootContext()->setContextProperty("dialog", popup);
    engine.load("qrc:/qt/qml/org/hyprland/update-screen/main.qml");

    return app.exec();
}
