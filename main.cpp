#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtWidgets>
#include "generalsubsrciberwidget.h"

#include "UbiForm/Component.h"

struct startupInfo{
    Component * component;
    QWidget * window;
    int* nextPositionX;
    int* nextPositionY;
};

void generalSubscirber(Endpoint* endpoint, void* userData){
    startupInfo* s = static_cast<startupInfo*>(userData);
    auto dre = s->component->castToDataReceiverEndpoint(endpoint);

    QLabel label(s->window);
    label.move(*(s->nextPositionX),*(s->nextPositionY));
    s->nextPositionY += 20;
    QString initText("Started endpoint: ");
    initText.append(dre->getEndpointId().c_str());
    label.setText(initText);
    label.show();
    while (true){
        QString newText;
        try{
            auto message = dre->receiveMessage();
            newText.append(message->stringify().c_str());
        }catch(ValidationError &e){
            newText.append("Validation error of received message: ");
            newText.append(e.what());
        }catch(std::logic_error &e){
            // NNG error or SocketOpen error means break
            break;
        }
        label.setText(newText);
    }
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    QWidget* window = new QWidget;
    window->resize(320, 240);
    window->show();
    window->setWindowTitle(
        QApplication::translate("toplevel", "Top-level widget"));


    Component* component = new Component;
    component->getComponentManifest().setName("LifeHub");
    std::shared_ptr<EndpointSchema> es = std::make_shared<EndpointSchema>();
    component->getComponentManifest().addEndpoint(SocketType::Subscriber, "general",es, nullptr);
    component->startBackgroundListen();

    try{
        component->getResourceDiscoveryConnectionEndpoint().registerWithHub("tcp://192.168.1.236:7999");
    }catch(std::logic_error &e){
        // EMPTY
    }

    while(component->getResourceDiscoveryConnectionEndpoint().getResourceDiscoveryHubs().empty()){
        component->getResourceDiscoveryConnectionEndpoint().searchForResourceDiscoveryHubs();
    }

    startupInfo* s = new startupInfo();
    s->component = component;
    s->window = window;
    s->nextPositionX = new int(10);
    s->nextPositionY = new int(10);

    component->registerStartupFunction("general",generalSubscirber, s);

    return app.exec();
}
