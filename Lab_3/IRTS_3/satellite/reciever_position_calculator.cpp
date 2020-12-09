#include "reciever_position_calculator.h"

RecieverPositionCalculator::RecieverPositionCalculator()
{

}

RecieverPosition *RecieverPositionCalculator::calculate(ObservationData *observ, SatelliteMessagesContainer *container) {
    struct RequiredData {
        int satelliteNumber;

        double pseudorange;
        double pseudorangeCorrected;

        NavigationMessage *message;
        SatelliteCalcData *emissionData;

        ~RequiredData() {
            delete emissionData;
        }
    };

    /* Скорость света*/
    double c = 299792458;

    /// Для повышения точности вычисления координат (отправка сигнала)
    auto function = [c, observ, container] (int satelliteNumber) -> RequiredData* {
        auto *message = container->nearbyMessage(satelliteNumber, observ->dateTime);
        if(message == nullptr) return nullptr;

        RequiredData *data = new RequiredData;

        data->message = message;
        data->satelliteNumber = satelliteNumber;
        data->pseudorange = observ->gps[satelliteNumber].c1;

        data->emissionData = new SatelliteCalcData;
        auto calculated = data->emissionData;

        QTime time = observ->dateTime.time();

        /// Расчитываем положение спутника на момент отправки сигнала
        double dtime = observ->gps[satelliteNumber].c1 / c;
        double dayNumber = std::floor(message->TOE / 86400);
        calculated->time  = dayNumber * 86400;
        calculated->time += time.hour() * 3600;
        calculated->time += time.minute() * 60;
        calculated->time += time.second();
        calculated->time += time.msec() / 1000.0;
        calculated->time -= dtime;

        timeSinceEra(calculated, message);
        correctedAverageMovement(calculated, message);
        meanAnomaly(calculated, message);
        eccentricAnomaly(calculated, message);
        trueAnomaly(calculated, message);
        latitudeArgument(calculated, message);
        correctedLatitudeArgument(calculated, message);
        correctedRadiusVector(calculated, message);
        correctedInclinationAngle(calculated, message);
        orbitalSatellitePosition(calculated);
        correcntedLongitude(calculated, message);
        satellitePosition(calculated);

        double tk = message->TOC.msecsTo(observ->dateTime);
        tk /= 1000;
        tk -= dtime;

        double offset = tk;
        offset *= message->timeShiftAcceleration;
        offset += message->timeShiftVelocity;
        offset *= tk;
        offset += message->timeShift;
        double dTR = -4.44280763e-10;
        dTR *= message->eccentricity;
        dTR *= std::sqrt(message->semiMajorAxis);
        dTR *= std::sin(data->emissionData->eccentricAnomaly);

        offset += dTR;
        offset -= message->TGD;

        /// Вычисляем уточненную псевдодальность
        data->pseudorangeCorrected = data->pseudorange;
        data->pseudorangeCorrected += offset * c;

        return data;
    };

    QVector<RequiredData*> satellitesInfo;
    for(auto iter = observ->gps.begin(); iter != observ->gps.end(); ++iter) {
        auto data = function(iter.key());
        if(data != nullptr)
            satellitesInfo.append(data);
    }

    if(satellitesInfo.size() < 4) {
        for(auto info : satellitesInfo)
            delete info;
        return nullptr;
    }


    double eps = 1e-7;
    double dr = std::numeric_limits<double>::max();

    MatrixOnRow<double> position(4, 1);
    MatrixOnRow<double> dPosition(4, 1);
    MatrixOnRow<double> difference(satellitesInfo.size(), 1);
    MatrixOnRow<double> derivative(satellitesInfo.size(), 4);
    /// Почему у нас не квадратная матрица?
    /// Плак плак, прощай быстрая скорость вычислений
    /// Привет псевдообратная матрица
    MatrixOnRow<double> transposedDerivative(4, satellitesInfo.size());

    /// Переменные, в которые заносятся промежеточные результаты
    MatrixOnRow<double> E(4, 4);
    for(int i = 0; i != E.rows(); ++i) {
        for(int j = 0; j != E.columns(); ++j) {
            E.setCell(i, j, i == j ? 1 : 0);
        }
    }

    MatrixOnRow<double> JT_J;
    MatrixOnRow<double> INV_J;

    while(dr > eps) {
        for(int i = 0; i != satellitesInfo.size(); ++i) {
            auto data = satellitesInfo[i];
            double dx = position.cell(0, 0) - data->emissionData->satellitePosX;
            double dy = position.cell(1, 0) - data->emissionData->satellitePosY;
            double dz = position.cell(2, 0) - data->emissionData->satellitePosZ;

            double vectorLength = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2) + std::pow(dz, 2));

            /// Записываем частные производные
            derivative.setCell(i, 0, dx / vectorLength);
            derivative.setCell(i, 1, dy / vectorLength);
            derivative.setCell(i, 2, dz / vectorLength);
            derivative.setCell(i, 3, c);

            difference(i, 0) = vectorLength + position.cell(3, 0) * c - data->pseudorangeCorrected;
        }

        for(int i = 0; i != derivative.rows(); ++i) {
            for(int j = 0; j != derivative.columns(); ++j){
                transposedDerivative.setCell(j, i, derivative.cell(i, j));
            }
        }

        MatrixOperations::mult(JT_J, transposedDerivative, derivative);
        MatrixOperations::slay(JT_J, E); std::swap(JT_J, E);
        MatrixOperations::mult(INV_J, JT_J, transposedDerivative);
        MatrixOperations::mult(dPosition, INV_J, difference);
        MatrixOperations::sub(position, dPosition);

        dr  = std::pow(dPosition.cell(0, 0), 2);
        dr += std::pow(dPosition.cell(1, 0), 2);
        dr += std::pow(dPosition.cell(2, 0), 2);
        dr  = std::sqrt(dr);
    }

    RecieverPosition *result = new RecieverPosition;
    result->x  = position.cell(0, 0);
    result->y  = position.cell(1, 0);
    result->z  = position.cell(2, 0);
    result->dt = position.cell(3, 0);

    for(auto info : satellitesInfo)
        delete info;

    return result;
}

RecieverPosition *RecieverPositionCalculator::calculate(RecieverData *reciever, SatelliteMessagesContainer *container) {
    class Enumerator {
        int i = -1;
        RecieverData *reciever;
    public:
        Enumerator(RecieverData *reciever) {
            this->reciever = reciever;
        }

        void reset() {
            i = -1;
        }

        int next() {
            ++i;
            while(i < reciever->observations.size()) {
                if(reciever->observations[i]->gps.size() > 4) {
                    return i;
                }
                ++i;
            }
            return -1;
        }
    };

    int observationNumber;
    Enumerator enumerator(reciever);
    while((observationNumber = enumerator.next()) != -1) {
        auto result = calculate(reciever->observations[observationNumber], container);
        if(result != nullptr) return result;
    }

    return nullptr;
}


RecieverPosition *RecieverPositionCalculator::calculate(ObservationData *observ, RecieverPosition refStPos,
                                                        ObservationData *refStData, SatelliteMessagesContainer *container) {
    if(observ->dateTime != refStData->dateTime) return nullptr;

    struct RequiredData {
        int satelliteNumber;

        double pseudorange;
        double pseudorangeCorrected;

        NavigationMessage *message;
        SatelliteCalcData *emissionData;

        ~RequiredData() {
            delete emissionData;
        }
    };

    /* Скорость света*/
    double c = 299792458;

    /// Для повышения точности вычисления координат (отправка сигнала)
    auto function = [c, observ, &refStPos, refStData, container] (int satelliteNumber) -> RequiredData* {
        auto *message = container->nearbyMessage(satelliteNumber, observ->dateTime);
        if(message == nullptr) return nullptr;

        RequiredData *data = new RequiredData;

        data->message = message;
        data->satelliteNumber = satelliteNumber;
        data->pseudorange = observ->gps[satelliteNumber].c1;



        data->emissionData = new SatelliteCalcData;
        auto calculated = data->emissionData;

        QTime time = observ->dateTime.time();

        /// Расчитываем положение спутника на момент отправки сигнала
        double dtime = observ->gps[satelliteNumber].c1 / c;
        double dayNumber = std::floor(message->TOE / 86400);
        calculated->time  = dayNumber * 86400;
        calculated->time += time.hour() * 3600;
        calculated->time += time.minute() * 60;
        calculated->time += time.second();
        calculated->time += time.msec() / 1000.0;
        calculated->time -= dtime;

        timeSinceEra(calculated, message);
        correctedAverageMovement(calculated, message);
        meanAnomaly(calculated, message);
        eccentricAnomaly(calculated, message);
        trueAnomaly(calculated, message);
        latitudeArgument(calculated, message);
        correctedLatitudeArgument(calculated, message);
        correctedRadiusVector(calculated, message);
        correctedInclinationAngle(calculated, message);
        orbitalSatellitePosition(calculated);
        correcntedLongitude(calculated, message);
        satellitePosition(calculated);

        double pseudorangeReferenceStation = refStData->gps[satelliteNumber].c1;

        double diff_x = std::pow(refStPos.x - calculated->satellitePosX, 2);
        double diff_y = std::pow(refStPos.y - calculated->satellitePosY, 2);
        double diff_z = std::pow(refStPos.z - calculated->satellitePosZ, 2);

        double caclRange = std::sqrt(diff_x + diff_y + diff_z);

        /// Вычисляем уточненную псевдодальность
        data->pseudorangeCorrected = data->pseudorange;
        data->pseudorangeCorrected += caclRange - pseudorangeReferenceStation;
        return data;
    };

    QHash<int, ObservationDataRow> gps;
    for(auto iter = observ->gps.begin(); iter != observ->gps.end(); ++iter) {
        if(refStData->gps.contains(iter.key())) gps.insert(iter.key(), iter.value());
    }

    if(gps.size() < 4) return nullptr;

    QVector<RequiredData*> satellitesInfo;
    for(auto iter = gps.begin(); iter != gps.end(); ++iter) {
        auto data = function(iter.key());
        if(data != nullptr)
            satellitesInfo.append(data);
    }

    if(satellitesInfo.size() < 4) {
        for(auto info : satellitesInfo)
            delete info;
        return nullptr;
    }


    double eps = 1e-7;
    double dr = std::numeric_limits<double>::max();

    MatrixOnRow<double> position(4, 1);
    MatrixOnRow<double> dPosition(4, 1);
    MatrixOnRow<double> difference(satellitesInfo.size(), 1);
    MatrixOnRow<double> derivative(satellitesInfo.size(), 4);
    /// Почему у нас не квадратная матрица?
    /// Плак плак, прощай быстрая скорость вычислений
    /// Привет псевдообратная матрица
    MatrixOnRow<double> transposedDerivative(4, satellitesInfo.size());

    /// Переменные, в которые заносятся промежеточные результаты
    MatrixOnRow<double> E(4, 4);
    for(int i = 0; i != E.rows(); ++i) {
        for(int j = 0; j != E.columns(); ++j) {
            E.setCell(i, j, i == j ? 1 : 0);
        }
    }

    MatrixOnRow<double> JT_J;
    MatrixOnRow<double> INV_J;

    while(dr > eps) {
        for(int i = 0; i != satellitesInfo.size(); ++i) {
            auto data = satellitesInfo[i];
            double dx = position.cell(0, 0) - data->emissionData->satellitePosX;
            double dy = position.cell(1, 0) - data->emissionData->satellitePosY;
            double dz = position.cell(2, 0) - data->emissionData->satellitePosZ;

            double vectorLength = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2) + std::pow(dz, 2));

            /// Записываем частные производные
            derivative.setCell(i, 0, dx / vectorLength);
            derivative.setCell(i, 1, dy / vectorLength);
            derivative.setCell(i, 2, dz / vectorLength);
            derivative.setCell(i, 3, c);

            difference(i, 0) = vectorLength + position.cell(3, 0) * c - data->pseudorangeCorrected;
        }

        for(int i = 0; i != derivative.rows(); ++i) {
            for(int j = 0; j != derivative.columns(); ++j){
                transposedDerivative.setCell(j, i, derivative.cell(i, j));
            }
        }

        MatrixOperations::mult(JT_J, transposedDerivative, derivative);
        MatrixOperations::slay(JT_J, E); std::swap(JT_J, E);
        MatrixOperations::mult(INV_J, JT_J, transposedDerivative);
        MatrixOperations::mult(dPosition, INV_J, difference);
        MatrixOperations::sub(position, dPosition);

        dr  = std::pow(dPosition.cell(0, 0), 2);
        dr += std::pow(dPosition.cell(1, 0), 2);
        dr += std::pow(dPosition.cell(2, 0), 2);
        dr  = std::sqrt(dr);
    }

    RecieverPosition *result = new RecieverPosition;
    result->x  = position.cell(0, 0);
    result->y  = position.cell(1, 0);
    result->z  = position.cell(2, 0);
    result->dt = position.cell(3, 0);

    for(auto info : satellitesInfo)
        delete info;

    return result;
}

RecieverPosition *RecieverPositionCalculator::calculate(RecieverData *reciever, RecieverData *referenceStation, SatelliteMessagesContainer *container) {
    RecieverPosition position;
    position.x = referenceStation->X;
    position.y = referenceStation->Y;
    position.z = referenceStation->Z;
    position.dt = 0; /// Хоть это поле и игнорируется, но инициализация никогда не мешает хд

    for(int i = 0; i != reciever->observations.size(); ++i) {
        if(reciever->observations[i]->gps.size() > 4) {
            for(int j = 0; j != referenceStation->observations.size(); ++j) {
                if(referenceStation->observations[j]->dateTime == reciever->observations[i]->dateTime) {
                    auto result = calculate(reciever->observations[i], position, referenceStation->observations[j], container);
                    if(result != nullptr) return result;
                }
            }
        }
    }
    return nullptr;
}