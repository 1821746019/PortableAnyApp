
module;
// #include <QtGui/QGuiApplication>
#include <QDebug>
#include <QtCore/QtCore>

#include <Windows.h>
export module _;
// export module _;
import std;
import Hooker;

using namespace std;
decltype(&QJsonDocument::fromJson) fromJson_raw = &QJsonDocument::fromJson;
QJsonDocument fromJson_mod(const QByteArray &json, QJsonParseError *error) {
  auto procLoginAndVIP = [](const QByteArray &json) {
    QString json_new(json);
    QHash<QRegularExpression, QString> replaceDict{
        {QRegularExpression(R"("expires_in":\d+)"),
         R"("expires_in":3600)"},
        {QRegularExpression(R"("expire_date":[^,]+)"),
         R"("expire_date":"2099-09-21T00:00:00.000Z")"},
      {
        QRegularExpression(R"("type":\d,"status":\d)"),R"("type":1,"status":0)"}
    };
    for (auto &k : replaceDict.keys()) {
      auto match = k.match(json_new);
      if (match.hasMatch()) {
        auto i = "hit";
      }
      json_new = json_new.replace(k, replaceDict[k]);
    }

    return json_new.toLocal8Bit();
  };
  QByteArray json_new = procLoginAndVIP(json);
  QJsonDocument ret = fromJson_raw(json_new, error);
  //qDebug() << rawRet.toVariant();
  return ret;
}
// decltype(&QIODevice::readAll) readAll_raw = &QIODevice::readAll;
// QByteArray readAll_mod(QIODevice *reply) {
//   QByteArray rawRet = (reply->*readAll_raw)();
//   QString res = rawRet;
//   // reply.readA
//   return rawRet;
// }
void hook() {
  // QMessageBox::information(nullptr, "why", "why i cannot hook");
  DetoursHooker hooker;
  hooker.endeque({{&fromJson_raw, &fromJson_mod}});
  hooker.setHook();
}
extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson,
                                 LPVOID lpReserved) {

  // DisableThreadLibraryCalls(hModule);
  if (dwReson == DLL_PROCESS_ATTACH) {
    hook();
  }
  return TRUE;
}