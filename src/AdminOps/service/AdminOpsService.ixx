module;

export module _;

import std;
//import allocConsole;
using namespace std;


extern "C" int main(int argc, char* argv[]) {

  QApplication app(argc, argv);
  QLocalServer server;
  server.removeServer(BS_TARGET_NAME);
  if (!server.listen(BS_TARGET_NAME)) {
    return 1;
  }
  QObject::connect(&server, &QLocalServer::newConnection, [&] {
    QLocalSocket* client = server.nextPendingConnection();
    QObject::connect(client, &QLocalSocket::readyRead, [=] {
      QJsonDocument doc = QJsonDocument::fromJson(client->readAll());

      QString target = doc["target"].toString();
      QString action = doc["action"].toString();
      QJsonObject data = doc["data"].toObject();
      if (target == "registry") {
      }
    });
  });
  return QCoreApplication::exec();
}