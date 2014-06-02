#include "fluxunits.h"

#include <QStringList>

FluxUnits::FluxUnits(QObject *parent):
        QAbstractListModel(parent),
        unitlist(availableUnits())
{
}

QList<FluxUnits::Unit> FluxUnits::availableUnits()
{
    QList<FluxUnits::Unit> unitlist;
    unitlist.append(FLX);
    unitlist.append(mFLX);
    unitlist.append(uFLX);
    unitlist.append(nFLX);
    unitlist.append(KFLX);
    unitlist.append(MFLX);
    unitlist.append(GFLX);
    return unitlist;
}

bool FluxUnits::valid(int unit)
{
    switch(unit)
    {
    case FLX:
    case mFLX:
    case uFLX:
    case nFLX:
    case KFLX:
    case MFLX:
    case GFLX:
        return true;
    default:
        return false;
    }
}

QString FluxUnits::name(int unit)
{
    switch(unit)
    {
    case FLX: return QString("FLX");
    case mFLX: return QString("mFLX");
    case uFLX: return QString::fromUtf8("μFLX");
    case nFLX: return QString("nFLX");
    case KFLX: return QString("KFLX");
    case MFLX: return QString("MFLX");
    case GFLX: return QString("GFLX");
    default: return QString("???");
    }
}

QString FluxUnits::description(int unit)
{
    switch(unit)
    {
    case FLX: return QString("Flux");
    case mFLX: return QString("Milli-Fluctuations (1 / 1,000)");
    case uFLX: return QString("Micro-Fluctuations (1 / 1,000,000)");
    case nFLX: return QString("Nano-Fluctuations (1 / 100,000,000");
    case KFLX: return QString("Kilo-Fluctuations (1 * 1,000");
    case MFLX: return QString("Mega-Fluctuations (1 * 1,000,000");
    case GFLX: return QString("Giga-Fluctuations (1 * 1,000,000,000");
    default: return QString("???");
    }
}

qint64 FluxUnits::factor(int unit)
{
    switch(unit)
    {
    case FLX:  return 100000000;
    case mFLX: return 100000;
    case uFLX: return 100;
    case nFLX: return 1;
    case KFLX: return 100000000000;
    case MFLX: return 100000000000000;
    case GFLX: return 100000000000000000;
    default:   return 100000000;
    }
}

int FluxUnits::amountDigits(int unit)
{
    switch(unit)
    {
    case FLX: return 11; // 10,000,000,000 (# digits, without commas)
    case mFLX: return 14; // 10,000,000,000,000
    case uFLX: return 17; // 10,000,000,000,000,000
    case nFLX: return 19; // 1,000,000,000,000,000,000
    case KFLX: return 8; // 10,000,000
    case MFLX: return 5; // 10,000
    case GFLX: return 2; // 10
    default: return 0;
    }
}

int FluxUnits::decimals(int unit)
{
    switch(unit)
    {
    case FLX: return 8;
    case mFLX: return 5;
    case uFLX: return 2;
    case nFLX: return 1;
    case KFLX: return 11;
    case MFLX: return 14;
    case GFLX: return 17;
    default: return 0;
    }
}

QString FluxUnits::format(int unit, qint64 n, bool fPlus)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if(!valid(unit))
        return QString(); // Refuse to format invalid unit
    qint64 Flux = factor(unit);
    int num_decimals = decimals(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / Flux;
    qint64 remainder = n_abs % Flux;
    QString quotient_str = QString::number(quotient);
    QString remainder_str = QString::number(remainder).rightJustified(num_decimals, '0');

    // Right-trim excess zeros after the decimal point
    int nTrim = 0;
    for (int i = remainder_str.size()-1; i>=2 && (remainder_str.at(i) == '0'); --i)
        ++nTrim;
    remainder_str.chop(nTrim);

    if (n < 0)
        quotient_str.insert(0, '-');
    else if (fPlus && n > 0)
        quotient_str.insert(0, '+');
    if(unit =! 0)
    {
        return quotient_str + QString(".") + remainder_str;
    } else {
        return quotient_str;   
    }

}

QString FluxUnits::formatWithUnit(int unit, qint64 amount, bool plussign)
{
    return format(unit, amount, plussign) + QString(" ") + name(unit);
}

bool FluxUnits::parse(int unit, const QString &value, qint64 *val_out)
{
    if(!valid(unit) || value.isEmpty())
        return false; // Refuse to parse invalid unit or empty string
    int num_decimals = decimals(unit);
    QStringList parts = value.split(".");

    if(parts.size() > 2)
    {
        return false; // More than one dot
    }
    QString whole = parts[0];
    QString decimals;

    if(parts.size() > 1)
    {
        decimals = parts[1];
    }
    if(decimals.size() > num_decimals)
    {
        return false; // Exceeds max precision
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if(str.size() > 18)
    {
        return false; // Longer numbers will exceed 63 bits
    }
    qint64 retvalue = str.toLongLong(&ok);
    if(val_out)
    {
        *val_out = retvalue;
    }
    return ok;
}

int FluxUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant FluxUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitlist.size())
    {
        Unit unit = unitlist.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(name(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}
