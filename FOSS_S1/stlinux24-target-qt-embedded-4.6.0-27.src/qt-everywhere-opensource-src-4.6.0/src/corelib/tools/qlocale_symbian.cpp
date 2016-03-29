/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDate>
#include <QLocale>
#include <QTime>
#include <QVariant>
#include <QThread>

#include <e32std.h>
#include "private/qcore_symbian_p.h"


QT_BEGIN_NAMESPACE

// Located in qlocale.cpp
extern void getLangAndCountry(const QString &name, QLocale::Language &lang, QLocale::Country &cntry);

static TExtendedLocale _s60Locale;

// Type definitions for runtime resolved function pointers
typedef void (*FormatFunc)(TTime&, TDes&, const TDesC&, const TLocale&);
typedef TPtrC (*FormatSpecFunc)(TExtendedLocale&);

// Runtime resolved functions
static FormatFunc ptrTimeFormatL = NULL;
static FormatSpecFunc ptrGetTimeFormatSpec = NULL;
static FormatSpecFunc ptrGetLongDateFormatSpec = NULL;
static FormatSpecFunc ptrGetShortDateFormatSpec = NULL;

// Default functions if functions cannot be resolved
static void defaultTimeFormatL(TTime&, TDes& des, const TDesC&, const TLocale&)
{
    des.Zero();
}

static TPtrC defaultFormatSpec(TExtendedLocale&)
{
    return TPtrC(KNullDesC);
}

/*
  Definition of struct for mapping Symbian to ISO locale
*/
struct symbianToISO {
    int symbian_language;
    char iso_name[8];
};


/*
  Mapping from Symbian to ISO locale
*/
static const symbianToISO symbian_to_iso_list[] = {
    { ELangEnglish,             "en_GB" },
    { ELangFrench,              "fr_FR" },
    { ELangGerman,              "de_DE" },
    { ELangSpanish,             "es_ES" },
    { ELangItalian,             "it_IT" },
    { ELangSwedish,             "sv_SE" },
    { ELangDanish,              "da_DK" },
    { ELangNorwegian,           "no_NO" },
    { ELangFinnish,             "fi_FI" },
    { ELangAmerican,            "en_US" },
    { ELangPortuguese,          "pt_PT" },
    { ELangTurkish,             "tr_TR" },
    { ELangIcelandic,           "is_IS" },
    { ELangRussian,             "ru_RU" },
    { ELangHungarian,           "hu_HU" },
    { ELangDutch,               "nl_NL" },
    { ELangBelgianFlemish,      "nl_BE" },
    { ELangCzech,               "cs_CZ" },
    { ELangSlovak,              "sk_SK" },
    { ELangPolish,              "pl_PL" },
    { ELangSlovenian,           "sl_SI" },
    { ELangTaiwanChinese,       "zh_TW" },
    { ELangHongKongChinese,     "zh_HK" },
    { ELangPrcChinese,          "zh_CN" },
    { ELangJapanese,            "ja_JP" },
    { ELangThai,                "th_TH" },
    { ELangArabic,              "ar_AE" },
    { ELangTagalog,             "tl_PH" },
    { ELangBulgarian,           "bg_BG" },
    { ELangCatalan,             "ca_ES" },
    { ELangCroatian,            "hr_HR" },
    { ELangEstonian,            "et_EE" },
    { ELangFarsi,               "fa_IR" },
    { ELangCanadianFrench,      "fr_CA" },
    { ELangGreek,               "el_GR" },
    { ELangHebrew,              "he_IL" },
    { ELangHindi,               "hi_IN" },
    { ELangIndonesian,          "id_ID" },
    { ELangLatvian,             "lv_LV" },
    { ELangLithuanian,          "lt_LT" },
    { ELangMalay,               "ms_MY" },
    { ELangBrazilianPortuguese, "pt_BR" },
    { ELangRomanian,            "ro_RO" },
    { ELangSerbian,             "sr_YU" },
    { ELangLatinAmericanSpanish, "es" },
    { ELangUkrainian,           "uk_UA" },
    { ELangUrdu,                "ur_PK" }, // India/Pakistan
    { ELangVietnamese,          "vi_VN" },
#ifdef __E32LANG_H__
// 5.0
    { ELangBasque,              "eu_ES" },
    { ELangGalician,            "gl_ES" },
#endif
#if !defined(__SERIES60_31__)
    { ELangEnglish_Apac,        "en" },
    { ELangEnglish_Taiwan,      "en_TW" },
    { ELangEnglish_HongKong,    "en_HK" },
    { ELangEnglish_Prc,         "en_CN" },
    { ELangEnglish_Japan,       "en_JP"},
    { ELangEnglish_Thailand,    "en_TH" },
    { ELangMalay_Apac,          "ms" }
#endif
};

/*!
    Returns ISO name corresponding to the Symbian locale code \a sys_fmt.
*/
QByteArray qt_symbianLocaleName(int code)
{
    //Number of Symbian to ISO locale mappings
    static const int symbian_to_iso_count
        = sizeof(symbian_to_iso_list)/sizeof(symbianToISO);

    int cmp = code - symbian_to_iso_list[0].symbian_language;
    if (cmp < 0)
        return 0;

    if (cmp == 0)
        return symbian_to_iso_list[0].iso_name;

    int begin = 0;
    int end = symbian_to_iso_count;

    while (end - begin > 1) {
        uint mid = (begin + end)/2;

        const symbianToISO *elt = symbian_to_iso_list + mid;
        int cmp = code - elt->symbian_language;
        if (cmp < 0)
            end = mid;
        else if (cmp > 0)
            begin = mid;
        else
            return elt->iso_name;
    }

    return 0;
}


// order is: normal, abbr, nmode, nmode+abbr
static const char *us_locale_dep[] = {
    "MM", "dd", "yyyy", "MM", "dd",
    "M", "d", "yy", "M", "d",
    "MMMM", "dd", "yyyy", "MMMM", "dd",
    "MMM", "d", "yy", "MMM", "d" };

static const char *eu_locale_dep[] = {
    "dd", "MM", "yyyy", "dd", "MM",
    "d", "M", "yy", "d", "M",
    "dd", "MMMM", "yyyy", "dd", "MMMM",
    "d", "MMM", "yy", "d", "MMM" };

static const char *jp_locale_dep[] = {
    "yyyy", "MM", "dd", "MM", "dd",
    "yy", "M", "d", "M", "d",
    "yyyy", "MMMM", "dd", "MMMM", "dd",
    "yy", "MMM", "d", "MMM", "d" };

/*!
    Returns a Qt version of the given \a sys_fmt Symbian locale format string.
*/
static QString s60ToQtFormat(const QString &sys_fmt)
{
    TLocale *locale = _s60Locale.GetLocale();

    QString result;
    QString other;
    QString qtformatchars = QString::fromLatin1("adhmsyzAHM");

    QChar c;
    int i = 0;
    bool open_escape = false;
    bool abbrev_next = false;
    bool locale_indep_ordering = false;
    bool minus_mode = false;
    bool plus_mode = false;
    bool n_mode = false;
    TTimeFormat tf = locale->TimeFormat();

    while (i < sys_fmt.size()) {

        c = sys_fmt.at(i);

        // let formatting thru
        if (c.unicode() == '%') {
            // if we have gathered string, concat it
            if (!other.isEmpty()) {
                result += other;
                other.clear();
            }
            // if we have open escape, end it
            if (open_escape) {
                result += QLatin1Char('\'');
                open_escape = false;
            }

            ++i;
            if (i >= sys_fmt.size())
                break;

            c = sys_fmt.at(i);

            // process specials
            abbrev_next = c.unicode() == '*';
            plus_mode = c.unicode() == '+';
            minus_mode = c.unicode() == '-';

            if (abbrev_next || plus_mode || minus_mode) {
                ++i;
                if (i >= sys_fmt.size())
                    break;

                c = sys_fmt.at(i);

                if (plus_mode || minus_mode) {
                    // break on undefined plus/minus mode
                    if (c.unicode() != 'A' && c.unicode() != 'B')
                        break;
                }
            }

            switch (c.unicode()) {
                case 'F':
                {
                    // locale indep mode on
                    locale_indep_ordering = true;
                    break;
                }

                case '/':
                {
                    // date sep 0-3
                    ++i;
                    if (i >= sys_fmt.size())
                        break;

                    c = sys_fmt.at(i);
                    if (c.isDigit() && c.digitValue() <= 3) {
                        TChar s = locale->DateSeparator(c.digitValue());
                        TUint val = s;
                        // some indexes return zero for empty
                        if (val > 0)
                            result += QChar(val);
                    }
                    break;
                }

                case 'D':
                {
                    if (!locale_indep_ordering)
                        break;

                    if (!abbrev_next)
                        result += QLatin1String("dd");
                    else
                        result += QLatin1Char('d');

                    break;
                }

                case 'M':
                {
                    if (!locale_indep_ordering)
                        break;

                    if (!n_mode) {
                        if (!abbrev_next)
                            result += QLatin1String("MM");
                        else
                            result += QLatin1String("M");
                    } else {
                        if (!abbrev_next)
                            result += QLatin1String("MMMM");
                        else
                            result += QLatin1String("MMM");
                    }

                    break;
                }

                case 'N':
                {
                    n_mode = true;

                    if (!locale_indep_ordering)
                        break;

                    if (!abbrev_next)
                        result += QLatin1String("MMMM");
                    else
                        result += QLatin1String("MMM");

                    break;
                }

                case 'Y':
                {
                    if (!locale_indep_ordering)
                        break;

                    if (!abbrev_next)
                        result += QLatin1String("yyyy");
                    else
                        result += QLatin1String("yy");

                    break;
                }

                case 'E':
                {
                    if (!abbrev_next)
                        result += QLatin1String("dddd");
                    else
                        result += QLatin1String("ddd");

                    break;
                }

                case ':':
                {
                    // timesep 0-3
                    ++i;
                    if (i >= sys_fmt.size())
                        break;

                    c = sys_fmt.at(i);
                    if (c.isDigit() && c.digitValue() <= 3) {
                        TChar s = locale->TimeSeparator(c.digitValue());
                        TUint val = s;
                        // some indexes return zero for empty
                        if (val > 0)
                            result += QChar(val);
                    }

                    break;
                }

                case 'J':
                {
                    if (tf == ETime24 && !abbrev_next)
                        result += QLatin1String("hh");
                    else
                        result += QLatin1Char('h');

                    break;
                }

                case 'H':
                {
                    if (!abbrev_next)
                        result += QLatin1String("hh");
                    else
                        result += QLatin1Char('h');

                    break;
                }

                case 'I':
                {
                    result += QLatin1Char('h');
                    break;
                }

                case 'T':
                {
                    if (!abbrev_next)
                        result += QLatin1String("mm");
                    else
                        result += QLatin1Char('m');

                    break;
                }

                case 'S':
                {
                    if (!abbrev_next)
                        result += QLatin1String("ss");
                    else
                        result += QLatin1Char('s');

                    break;
                }

                case 'B':
                {
                    // only done for 12h clock
                    if (tf == ETime24)
                        break;
                }

                    // fallthru to A
                case 'A': {
                    // quickie to get capitalization, can't use s60 string as is because Qt 'hh' format's am/pm logic
                    TAmPmName ampm = TAmPmName();
                    TChar first(ampm[0]);
                    QString qtampm = QString::fromLatin1(first.IsUpper() ? "AP" : "ap");

                    int pos = locale->AmPmSymbolPosition();

                    if ((minus_mode && pos != ELocaleBefore) ||
                        (plus_mode && pos != ELocaleAfter))
                        break;

                    if (!abbrev_next && locale->AmPmSpaceBetween()) {
                        if (pos == ELocaleBefore)
                            qtampm.append(QLatin1Char(' '));
                        else
                            qtampm.prepend(QLatin1Char(' '));
                    }

                    result += qtampm;
                    }
                    break;

                case '.': {
                        // decimal sep
                        TChar s = locale->DecimalSeparator();
                        TUint val = s;
                        if (val > 0)
                            result += QChar(val);
                    }
                    break;

                case 'C':
                {
                    // six digits in s60, three digits in qt
                    if (!abbrev_next) {
                        result += QLatin1String("zzz");
                    } else {
                        // next char is number from 0-6, how many digits to display
                        ++i;
                        if (i >= sys_fmt.size())
                            break;

                        c = sys_fmt.at(i);

                        if (c.isDigit()) {
                            // try to match wanted digits
                            QChar val(c.digitValue());

                            if (val >= 3) {
                                result += QLatin1String("zzz");
                            } else if (val > 0) {
                                result += QLatin1Char('z');
                            }
                        }
                    }
                    break;
                }

                // these cases fallthru
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                {

                    // shouldn't parse these with %F
                    if (locale_indep_ordering)
                        break;

                    TDateFormat df = locale->DateFormat();

                    const char **locale_dep;
                    switch (df) {
                        default: // fallthru to american
                        case EDateAmerican:
                            locale_dep = us_locale_dep;
                            break;
                        case EDateEuropean:
                            locale_dep = eu_locale_dep;
                            break;
                        case EDateJapanese:
                            locale_dep = jp_locale_dep;
                            break;
                    }
                    int offset = 0;
                    if (abbrev_next)
                        offset += 5;
                    if (n_mode)
                        offset += 10;

                    result += QLatin1String(locale_dep[offset + (c.digitValue()-1)]);
                    break;
                }

                case '%': // fallthru percent
                {
                // any junk gets copied as is
                }
                default:
                {
                    result += c;
                    break;
                }

                case 'Z': // Qt doesn't support these :(
                case 'X':
                case 'W':
                {
                    break;
                }
            }
        } else {
            // double any single quotes, don't begin escape
            if (c.unicode() == '\'') {
                // end open escape
                if (open_escape) {
                    result += other;
                    other.clear();
                    result += QLatin1Char('\'');
                    open_escape = false;
                }

                other += c;
            }

            // gather chars and escape them in one go if any format chars are found
            if (!open_escape && qtformatchars.indexOf(c) != -1) {
                result += QLatin1Char('\'');
                open_escape = true;
            }
            other += c;
        }

        ++i;
    }

    if (!other.isEmpty())
        result += other;
    if (open_escape)
        result += QLatin1Char('\'');

    return result;
}

/*!
    Retrieves Symbian locale decimal separator.
*/
static QString symbianDecimalPoint()
{
    TLocale *locale = _s60Locale.GetLocale();

    TChar decPoint = locale->DecimalSeparator();
    int val = decPoint;
    return QChar(val);
}

/*!
    Retrieves Symbian locale group separator.
*/
static QString symbianGroupSeparator()
{
    TLocale *locale = _s60Locale.GetLocale();

    TChar grpSep = locale->ThousandsSeparator();
    int val = grpSep;
    return QChar(val);
}

/*!
    Retrieves Symbian locale zero digit.
*/
static QString symbianZeroDigit()
{
    TLocale *locale = _s60Locale.GetLocale();

    // TDigitType enumeration value returned by TLocale
    // will always correspond to zero digit unicode value.
    TDigitType digit = locale->DigitType();
    return QChar(digit);
}

/*!
    Retrieves a day name from Symbian locale. The \a day is an integer
    from 1 to 7. When \a short_format is true the method returns
    the day in short format. Otherwise it returns the day in a long format.
*/
static QString symbianDayName(int day, bool short_format)
{
    day -= 1;

    if (day < 0 || day > 6)
        return QString();

    if (short_format) {
        return qt_TDes2QString(TDayNameAbb(TDay(day)));
    } else {
        return qt_TDes2QString(TDayName(TDay(day)));
    }
}

/*!
    Retrieves a month name from Symbian locale. The \a month is an integer
    from 1 to 12. When \a short_format is true the method returns
    the month in short format. Otherwise it returns the month in a long format.
*/
static QString symbianMonthName(int month, bool short_format)
{
    month -= 1;
    if (month < 0 || month > 11)
        return QString();

    if (short_format) {
        return qt_TDes2QString(TMonthNameAbb(TMonth(month)));
    } else {
        return qt_TDes2QString(TMonthName(TMonth(month)));
    }
}

/*!
    Retrieves date format from Symbian locale and
    transforms it to Qt format.

    When \a short_format is true the method returns
    short date format. Otherwise it returns the long format.
*/
static QString symbianDateFormat(bool short_format)
{
    TPtrC dateFormat;

    if (short_format) {
        dateFormat.Set(ptrGetShortDateFormatSpec(_s60Locale));
    } else {
        dateFormat.Set(ptrGetLongDateFormatSpec(_s60Locale));
    }

    return s60ToQtFormat(qt_TDesC2QString(dateFormat));
}

/*!
    Retrieves time format from Symbian locale and
    transforms it to Qt format.
*/
static QString symbianTimeFormat()
{
    return s60ToQtFormat(qt_TDesC2QString(ptrGetTimeFormatSpec(_s60Locale)));
}

/*!
    Returns localized string representation of given \a date
    formatted with Symbian locale date format.

    If \a short_format is true the format will be a short version.
    Otherwise it uses a longer version.
*/
static QString symbianDateToString(const QDate &date, bool short_format)
{
    int month = date.month() - 1;
    int day = date.day() - 1;
    int year = date.year();

    TDateTime dateTime;
    dateTime.Set(year, TMonth(month), day, 0, 0, 0, 0);

    TTime timeStr(dateTime);
    TBuf<KMaxLongDateFormatSpec*2> buffer;

    TPtrC dateFormat;
    if (short_format) {
        dateFormat.Set(ptrGetShortDateFormatSpec(_s60Locale));
    } else {
        dateFormat.Set(ptrGetLongDateFormatSpec(_s60Locale));
    }

    TRAPD(err, ptrTimeFormatL(timeStr, buffer, dateFormat, *_s60Locale.GetLocale());)

    if (err == KErrNone)
        return qt_TDes2QString(buffer);
    else
        return QString();
}

/*!
    Returns localized string representation of given \a time
    formatted with Symbian locale time format.
*/
static QString symbianTimeToString(const QTime &time)
{
    int hour = time.hour();
    int minute = time.minute();
    int second = time.second();
    int milliseconds = 0;

    TDateTime dateTime;
    dateTime.Set(0, TMonth(0), 0, hour, minute, second, milliseconds);

    TTime timeStr(dateTime);
    TBuf<KMaxTimeFormatSpec*2> buffer;

    TRAPD(err, ptrTimeFormatL(
        timeStr,
        buffer,
        ptrGetTimeFormatSpec(_s60Locale),
        *_s60Locale.GetLocale());
    )

    if (err == KErrNone)
        return qt_TDes2QString(buffer);
    else
        return QString();
}

/*!
    Returns the measurement system stored in Symbian locale

    \sa QLocale::MeasurementSystem
*/
static QLocale::MeasurementSystem symbianMeasurementSystem()
{
    TLocale *locale = _s60Locale.GetLocale();

    TUnitsFormat unitFormat = locale->UnitsGeneral();
    if (unitFormat == EUnitsImperial)
        return QLocale::ImperialSystem;
    else
        return QLocale::MetricSystem;
}

QLocale QSystemLocale::fallbackLocale() const
{
    // load system data before query calls
    static QBasicAtomicInt initDone = Q_BASIC_ATOMIC_INITIALIZER(0);
    if (initDone.testAndSetRelaxed(0, 1)) {
        _s60Locale.LoadSystemSettings();

        // Initialize platform version dependent function pointers
        ptrTimeFormatL = reinterpret_cast<FormatFunc>
            (qt_resolveS60PluginFunc(S60Plugin_TimeFormatL));
        ptrGetTimeFormatSpec = reinterpret_cast<FormatSpecFunc>
            (qt_resolveS60PluginFunc(S60Plugin_GetTimeFormatSpec));
        ptrGetLongDateFormatSpec = reinterpret_cast<FormatSpecFunc>
            (qt_resolveS60PluginFunc(S60Plugin_GetLongDateFormatSpec));
        ptrGetShortDateFormatSpec = reinterpret_cast<FormatSpecFunc>
            (qt_resolveS60PluginFunc(S60Plugin_GetShortDateFormatSpec));
        if (!ptrTimeFormatL)
            ptrTimeFormatL = &defaultTimeFormatL;
        if (!ptrGetTimeFormatSpec)
            ptrGetTimeFormatSpec = &defaultFormatSpec;
        if (!ptrGetLongDateFormatSpec)
            ptrGetLongDateFormatSpec = &defaultFormatSpec;
        if (!ptrGetShortDateFormatSpec)
            ptrGetShortDateFormatSpec = &defaultFormatSpec;
        bool ret = initDone.testAndSetRelease(1, 2);
        Q_ASSERT(ret);
        Q_UNUSED(ret);
    }
    while(initDone != 2)
        QThread::yieldCurrentThread();

    TLanguage lang = User::Language();
    QString locale = QLatin1String(qt_symbianLocaleName(lang));
    return QLocale(locale);
}

/*!
    Generic query method for locale data. Provides indirection.
    Denotes the \a type of the query
    with \a in as input data depending on the query.

    \sa QSystemLocale::QueryType
*/
QVariant QSystemLocale::query(QueryType type, QVariant in = QVariant()) const
{
    switch(type) {
        case DecimalPoint:
            return symbianDecimalPoint();
        case GroupSeparator:
            return symbianGroupSeparator();

        case ZeroDigit:
             return symbianZeroDigit();

        case DayNameLong:
        case DayNameShort:
            return symbianDayName(in.toInt(), (type == DayNameShort) );

        case MonthNameLong:
        case MonthNameShort:
            return symbianMonthName(in.toInt(), (type == MonthNameShort) );

        case DateFormatLong:
        case DateFormatShort:
            return symbianDateFormat( (type == DateFormatShort) );
        case TimeFormatLong:
        case TimeFormatShort:
            return symbianTimeFormat();
        case DateTimeFormatLong:
        case DateTimeFormatShort:
            return symbianDateFormat( (type == DateTimeFormatShort) ) + QLatin1Char(' ') + symbianTimeFormat();
        case DateToStringShort:
        case DateToStringLong:
             return symbianDateToString(in.toDate(), (type == DateToStringShort) );
        case TimeToStringShort:
        case TimeToStringLong:
             return symbianTimeToString(in.toTime());
        case DateTimeToStringShort:
        case DateTimeToStringLong: {
                const QDateTime dt = in.toDateTime();
                return symbianDateToString(dt.date(), (type == DateTimeToStringShort) )
                       + QLatin1Char(' ') + symbianTimeToString(dt.time());
            }
        case MeasurementSystem:
            return static_cast<int>(symbianMeasurementSystem());
        case LanguageId:
        case CountryId: {
                TLanguage language = User::Language();
                QString locale = QLatin1String(qt_symbianLocaleName(language));
                QLocale::Language lang;
                QLocale::Country cntry;
                getLangAndCountry(locale, lang, cntry);
                if (type == LanguageId)
                    return lang;
                // few iso codes have no country and will use this
                if (cntry == QLocale::AnyCountry)
                    return fallbackLocale().country();

                return cntry;
            }
        case NegativeSign:
        case PositiveSign:
            break;
        case AMText:
            return qt_TDes2QString(TAmPmName(TAmPm(EAm)));
        case PMText:
            return qt_TDes2QString(TAmPmName(TAmPm(EPm)));
        default:
            break;
    }
    return QVariant();
}

QT_END_NAMESPACE
