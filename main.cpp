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
    std::shared_ptr<EndpointSchema> schema = s->component->getComponentManifest().getReceiverSchema(endpoint->getEndpointType());
    int numEntries = schema->getAllProperties().size();
    emit s->window->createNewLabel(numEntries);
    emit s->window->updateText("Connection made",thisLabelVal);

    while(true){
        try {
            auto msg = dre->receiveMessage();
            std::string output;
            for(const std::string& key : msg->getKeys()){
                output.append(key).append(" : ");
                try{
                    switch(schema->getValueType(key)){
                    case ValueType::String:
                        output.append("\"").append(msg->getString(key)).append("\"\n");
                        break;
                    case ValueType::Array:
                        output.append("Array\n");
                        break;
                    case ValueType::Object:
                        output.append(msg->getMoveObject(key)->stringify()).append("\n");
                        break;
                    case ValueType::Number:
                        output.append(std::to_string(msg->getInteger(key))).append("\n");
                        break;
                    default:
                        break;
                    }
                } catch (AccessError& e){
                    continue;
                }
            }
            emit s->window->updateText(QString::fromStdString(output),thisLabelVal);
        } catch (ValidationError &e){
            emit s->window->updateText(QString::fromUtf8(e.what()), thisLabelVal);
        } catch (std::logic_error &e) {
            std::cout << "Receive ending due to: " << e.what() << std::endl;
            break;
        }
    }
}


void endpointAdditionCallback(std::string endpointType, void* userData){
    startupInfo* s = static_cast<startupInfo*>(userData);
    if (s->component->getComponentManifest().hasEndpoint(endpointType) &&
            s->component->getComponentManifest().getConnectionParadigm(endpointType) == "subscriber"){
        s->component->registerStartupFunction(endpointType,generalSubscriber,userData);
    }
}

void notificationSubscriber(Endpoint* endpoint, void* userData){
    startupInfo* s = static_cast<startupInfo*>(userData);
    auto dre = s->component->castToDataReceiverEndpoint(endpoint);

    int thisLabelVal = s->labelVal;
    s->labelVal++;
    emit s->window->createNewLabel(4);
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
            }else{
                emit s->window->updateColour(QColor(0,0,0), thisLabelVal);
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
        component->getResourceDiscoveryConnectionEndpoint().registerWithHub("tcp://192.168.1.236:7999");
    }catch(std::logic_error &e){
        // EMPTY
    }

    while(component->getResourceDiscoveryConnectionEndpoint().getResourceDiscoveryHubs().empty()){
        component->getResourceDiscoveryConnectionEndpoint().searchForResourceDiscoveryHubs();
    }


    startupInfo* s = new startupInfo();
    s->window = window;
    s->component = component;

    //component->registerStartupFunction("weatherSubscriber",generalSubscriber, s);
    component->registerStartupFunction("notificationSubscriber",notificationSubscriber, s);

    component->getComponentManifest().registerEndpointAdditionCallback(endpointAdditionCallback,s);
    //component->getBackgroundRequester().requestRemoteListenThenDial("tcp://192.168.1.155",48459,"weatherSubscriber","weatherPublisher");
    //component->getBackgroundRequester().requestRemoteListenThenDial("tcp://192.168.1.236",8000,"notificationSubscriber","notificationPublisher");

    return app.exec();
}
