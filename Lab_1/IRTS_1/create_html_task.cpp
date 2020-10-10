#include "create_html_task.h"

CreateHtmlTask::CreateHtmlTask(EphemerisContainer *calculatedEphemeris,
                               EphemerisContainer *readedEphemeris,
                               QObject *parent) :
    QObject(parent)
  , _calculatedEphemeris(calculatedEphemeris)
  , _readedEphemeris(readedEphemeris)
{
    setAutoDelete(false);
}


void CreateHtmlTask::run() {
    QTextStream out(&_html);
    out.setRealNumberPrecision(3);
    out.setRealNumberNotation(QTextStream::FixedNotation);

    out << "-----------------------------------<br><br>";

    EphemerisContainer::AllEphemeris readedEphemeris;
    if(_readedEphemeris != nullptr)
        readedEphemeris = _readedEphemeris->allEphemeris();

    _countOfProccessing = 0;
    for(auto ephemerisInOneDay : _calculatedEphemeris->allEphemeris()) {
        for(auto ephemeris : ephemerisInOneDay) {
            EphemerisContainer::EphemerisInOneDay rEphemerisInOneDay;
            if(readedEphemeris.contains(ephemeris->dateTime.date()))
                rEphemerisInOneDay = readedEphemeris[ephemeris->dateTime.date()];
            Ephemeris* rEphemeris = nullptr;
            if(rEphemerisInOneDay.contains(ephemeris->dateTime.time()))
                rEphemeris = rEphemerisInOneDay[ephemeris->dateTime.time()];
            bool reqExp = rEphemeris != nullptr;

            out << ephemeris->dateTime.toString("dddd dd.MM.yy hh.mm.ss.zzz") << "<br>";
            out <<  "<table border=\"1\" cellpadding=\"5\">";
            out <<      "<tr>";
            out <<          "<td rowspan=\"2\"/>";
            out <<       tr("<td colspan=\"3\" style=\"text-align: center;\">Вычисленные значения</td>");
            if(reqExp) {
                out <<   tr("<td colspan=\"3\" style=\"text-align: center;\">Считанные значения</td>");
                out <<   tr("<td colspan=\"3\" style=\"text-align: center;\">Разница в показаниях</td>");
            }
            out <<      "</tr>";

            out <<      "<tr>";
            out <<          "<td style=\"text-align: center;\">x</td>";
            out <<          "<td style=\"text-align: center;\">y</td>";
            out <<          "<td style=\"text-align: center;\">z</td>";
            if(reqExp) {
                out <<      "<td style=\"text-align: center;\">x</td>";
                out <<      "<td style=\"text-align: center;\">y</td>";
                out <<      "<td style=\"text-align: center;\">z</td>";
                out <<      "<td style=\"text-align: center;\">dx</td>";
                out <<      "<td style=\"text-align: center;\">dy</td>";
                out <<      "<td style=\"text-align: center;\">dz</td>";
            }
            out <<      "</tr>";

            for(
                auto iter = ephemeris->satelliteInfo.begin();
                iter != ephemeris->satelliteInfo.end();
                ++iter) {
                out <<      "<tr>";
                out <<     ("<td>" + tr("Спутник №") + QString::number(iter.key()) + "</td>");
                out <<     "<td style=\"text-align: right;\">"; out << iter->x / 1000; out << tr("км</td>");
                out <<     "<td style=\"text-align: right;\">"; out << iter->y / 1000; out << tr("км</td>");
                out <<     "<td style=\"text-align: right;\">"; out << iter->z / 1000; out << tr("км</td>");
                if(reqExp) {
                    if(rEphemeris->satelliteInfo.contains(iter.key())) {
                        auto info = rEphemeris->satelliteInfo[iter.key()];
                        out <<     "<td style=\"text-align: right;\">"; out << info.x; out << tr("км</td>");
                        out <<     "<td style=\"text-align: right;\">"; out << info.y; out << tr("км</td>");
                        out <<     "<td style=\"text-align: right;\">"; out << info.z; out << tr("км</td>");
                        out <<     "<td style=\"text-align: right;\">"; out << info.x * 1000 - iter->x; out << tr("м</td>");
                        out <<     "<td style=\"text-align: right;\">"; out << info.y * 1000 - iter->y; out << tr("м</td>");
                        out <<     "<td style=\"text-align: right;\">"; out << info.z * 1000 - iter->z; out << tr("м</td>");
                    } else {
                        out <<     "<td style=\"text-align: center;\">nan</td>";
                        out <<     "<td style=\"text-align: center;\">nan</td>";
                        out <<     "<td style=\"text-align: center;\">nan</td>";
                        out <<     "<td style=\"text-align: center;\">nan</td>";
                        out <<     "<td style=\"text-align: center;\">nan</td>";
                        out <<     "<td style=\"text-align: center;\">nan</td>";
                    }
                }
                out <<      "</tr>";
            }
            out << "</table><br><br>";
            ++_countOfProccessing;
        }
    }
    emit finished();
}

QString CreateHtmlTask::html() const {
    return _html;
}

int CreateHtmlTask::countOfProccessing() const {
    return _countOfProccessing;
}

EphemerisContainer *CreateHtmlTask::calculatedEphemeris() const {
    return _calculatedEphemeris;
}

EphemerisContainer *CreateHtmlTask::readedEphemeris() const {
    return _readedEphemeris;
}
