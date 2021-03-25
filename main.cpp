#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtWidgets>
#include "window.h"

#include "UbiForm/Component.h"

struct startupInfo{
    Component * component;
    Window * window;
    int labelVal = 0;
};

void generalSubscriber(Endpoint* endpoint, void* userData){
    startupInfo* s = static_cast<startupInfo*>(userData);
    auto dre = s->component->castToDataReceiverEndpoint(endpoint);

    int thisLabelVal = s->labelVal;
    s->labelVal++;
    emit s->window->createNewLabel();
    emit s->window->updateText("Connection made",thisLabelVal);

    while(true){
        try {
            auto msg = dre->receiveMessage();
            std::cout << "Received:" << msg->stringify() << std::endl;
            emit s->window->updateText(QString::fromStdString(msg->stringify()),thisLabelVal);
        } catch (ValidationError &e){
            emit s->window->updateText(QString::fromUtf8(e.what()), thisLabelVal);
        } catch (std::logic_error &e) {
            std::cout << "Receive ending due to: " << e.what() << std::endl;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    Window* window = new Window;
    window->show();


    Component* component = new Component;
    component->getComponentManifest().setName("LifeHub");
    std::shared_ptr<EndpointSchema> es = std::make_shared<EndpointSchema>();
    component->getComponentManifest().addEndpoint(ConnectionParadigm::Subscriber, "general",es, nullptr);
    component->startBackgroundListen();


    try{
        component->getResourceDiscoveryConnectionEndpoint().registerWithHub("tcp://192.168.1.155:7999");
    }catch(std::logic_error &e){
        // EMPTY
    }

    while(component->getResourceDiscoveryConnectionEndpoint().getResourceDiscoveryHubs().empty()){
        component->getResourceDiscoveryConnectionEndpoint().searchForResourceDiscoveryHubs();
    }


    startupInfo* s = new startupInfo();
    s->window = window;
    s->component = component;

    component->registerStartupFunction("general",generalSubscriber, s);

    component->getBackgroundRequester().requestRemoteListenThenDial("tcp://192.168.1.155",39427,"general","weatherPublisher");

    return app.exec();
}
