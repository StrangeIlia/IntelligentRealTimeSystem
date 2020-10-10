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
    out.setRealNumberPrecision(5);
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
            out <<       tr("<td colspan=\"3\">Вычисленные значения</td>");
            if(reqExp) {
                out <<   tr("<td colspan=\"3\">Считанные значения</td>");
                out <<   tr("<td colspan=\"3\">Разница в показаниях</td>");
            }
            out <<      "</tr>";

            out <<      "<tr>";
            out <<          "<td>x</td>";
            out <<          "<td>y</td>";
            out <<          "<td>z</td>";
            if(reqExp) {
                out <<      "<td>x</td>";
                out <<      "<td>y</td>";
                out <<      "<td>z</td>";
                out <<      "<td>dx</td>";
                out <<      "<td>dy</td>";
                out <<      "<td>dz</td>";
            }
            out <<      "</tr>";

            for(
                auto iter = ephemeris->satelliteInfo.begin();
                iter != ephemeris->satelliteInfo.end();
                ++iter) {
                out <<      "<tr>";
                out <<     ("<td>" + tr("Спутник №") + QString::number(iter.key()) + "</td>");
                out <<     "<td>"; out << iter->x / 1000; out << tr("км</td>");
                out <<     "<td>"; out << iter->y / 1000; out << tr("км</td>");
                out <<     "<td>"; out << iter->z / 1000; out << tr("км</td>");
                if(reqExp) {
                    if(rEphemeris->satelliteInfo.contains(iter.key())) {
                        auto info = rEphemeris->satelliteInfo[iter.key()];
                        out <<     "<td>"; out << info.x / 1000; out << tr("км</td>");
                        out <<     "<td>"; out << info.y / 1000; out << tr("км</td>");
                        out <<     "<td>"; out << info.z / 1000; out << tr("км</td>");
                        out <<     "<td>"; out << (info.x - iter->x) * 1000; out << tr("м</td>");
                        out <<     "<td>"; out << (info.y - iter->y) * 1000; out << tr("м</td>");
                        out <<     "<td>"; out << (info.z - iter->z) * 1000; out << tr("м</td>");
                    } else {
                        out <<     "<td>nan</td>";
                        out <<     "<td>nan</td>";
                        out <<     "<td>nan</td>";
                        out <<     "<td>nan</td>";
                        out <<     "<td>nan</td>";
                        out <<     "<td>nan</td>";
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
