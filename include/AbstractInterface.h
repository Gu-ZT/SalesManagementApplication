#ifndef APPLICATION_ABSTRACT_INTERFACE_H
#define APPLICATION_ABSTRACT_INTERFACE_H

#include <QWidget>

class AbstractInterface {
public:
    AbstractInterface() = default;

    void setMainWindow(QWidget *pMainWindow);

    QWidget *getMainWindow();

    void listen(const QString& eventName, const std::function<void()>& func);

    void apply(const QString& eventName);

    virtual void initInterface();

private:
    QWidget *mainWindow = nullptr;
    QMap<QString, QList<std::function<void()>>> events;
};

#endif // APPLICATION_ABSTRACT_INTERFACE_H
