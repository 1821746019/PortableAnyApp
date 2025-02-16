module;
#include <Windows.h>
export module AdminOps;

import std;
constexpr auto serviceName = BS_TARGET_NAME "Service";


export namespace AdminOps {
class Registry {
  std::wstring mountHive(const std::wstring& path) {
    QLocalSocket socket;
    socket.connectToServer(serviceName);
    if (!socket.waitForConnected()) {
      throw std::runtime_error("Cannot connect to service");
    }
    QJsonObject req;
    req["target"] = "registry";
    req["action"] = "mount";
    req["data"] = QJsonObject{{"path", QString::fromStdWString(path)}};
    // send request
    QJsonDocument doc(req);
    socket.write(doc.toJson());
    if (!socket.waitForBytesWritten(5000)) {
      throw std::runtime_error("Failed to send request: " + socket.errorString().toStdString());
    }

    // 读取响应
    if (!socket.waitForReadyRead(5000)) {
      throw std::runtime_error("No response received: " + socket.errorString().toStdString());
    }
    QByteArray responseData = socket.readAll(); 
    QJsonDocument res = QJsonDocument::fromJson(responseData);
    if (res["status"].toString() != "ok") {
      throw std::runtime_error("Failed to mount hive: " + res["message"].toString().toStdString());
    }
    return res["path"].toString().toStdWString();
  }
};

}  // namespace AdminOps