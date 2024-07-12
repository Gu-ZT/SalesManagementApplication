#include "AbstractInterface.h"

void AbstractInterface::setMainWindow(QWidget *pMainWindow) {
    this->mainWindow = pMainWindow;
}

QWidget *AbstractInterface::getMainWindow() {
    return this->mainWindow;
}

void AbstractInterface::listen(const QString &eventName, const std::function<void()> &func) {
    QList<std::function<void()>> list = this->events.value(eventName, {});
    list.append(func);
    if (!this->events.contains(eventName)) {
        this->events.insert(eventName, list);
    }
}

void AbstractInterface::apply(const QString &eventName) {
    QList<std::function<void()>> *list = nullptr;
    if (this->events.contains(eventName)) {
        list = &this->events.find(eventName).value();
    }
    if (list == nullptr) return;
    for (const auto &item: *list) {
        item();
    }
}

void AbstractInterface::initInterface() {
}
