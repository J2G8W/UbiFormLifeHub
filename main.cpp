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
    emit s->window->createNewLabel(1);
    emit s->window->updateText("Connection made",thisLabelVal);

    while(true){
        try {
            auto msg = dre->receiveMessage();
            emit s->window->updateText(QString::fromStdString(msg->stringify()),thisLabelVal);
        } catch (ValidationError &e){
            emit s->window->updateText(QString::fromUtf8(e.what()), thisLabelVal);
        } catch (std::logic_error &e) {
            std::cout << "Receive ending due to: " << e.what() << std::endl;
            break;
        }
    }
}

void notificationSubscriber(Endpoint* endpoint, void* userData){
    startupInfo* s = static_cast<startupInfo*>(userData);
    auto dre = s->component->castToDataReceiverEndpoint(endpoint);

    int thisLabelVal = s->labelVal;
    s->labelVal++;
    emit s->window->createNewLabel(3);
    emit s->window->updateText("Connection made",thisLabelVal);

    while(true){
        try {
            auto msg = dre->receiveMessage();

            if(msg->hasMember("colour")){
                auto colour = msg->getMoveObject("colour");
                int r = colour->getInteger("r");
                int g = colour->getInteger("g");
                int b = colour->getInteger("b");
                QColor c(r,g,b);
                emit s->window->updateColour(c, thisLabelVal);
            }
            std::string output;
            if(msg->hasMember("phoneName")){
                output.append("Phone Name: ").append(msg->getString("phoneName")).append("\n");
            }
            output.append("App: ").append(msg->getString("appName")).append("\n");
            output.append("Title: ").append(msg->getString("messageTitle")).append("\n");
            output.append("Text: ").append(msg->getString("messageText")).append("\n");

            emit s->window->updateText(QString::fromStdString(output),thisLabelVal);
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
    FILE* fp = fopen("Manifest.json", "r");
    if(fp == nullptr){
        std::cerr << "Unable to find Manifest.json" << std::endl;
        exit(-1);
    }
    component->specifyManifest(fp);
    fclose(fp);
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

    component->registerStartupFunction("weatherSubscriber",generalSubscriber, s);
    component->registerStartupFunction("notificationSubscriber",notificationSubscriber, s);

    component->getBackgroundRequester().requestRemoteListenThenDial("tcp://192.168.1.155",48459,"weatherSubscriber","weatherPublisher");
    component->getBackgroundRequester().requestRemoteListenThenDial("tcp://192.168.1.236",8000,"notificationSubscriber","notificationPublisher");

    return app.exec();
}
