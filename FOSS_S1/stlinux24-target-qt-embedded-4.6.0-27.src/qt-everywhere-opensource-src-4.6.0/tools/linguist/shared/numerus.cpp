/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "translator.h"

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>

#include <private/qtranslator_p.h>

QT_BEGIN_NAMESPACE

static const uchar englishStyleRules[] =
    { Q_EQ, 1 };
static const uchar frenchStyleRules[] =
    { Q_LEQ, 1 };
static const uchar latvianRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11, Q_NEWRULE,
      Q_NEQ, 0 };
static const uchar icelandicRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11 };
static const uchar irishStyleRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 2 };
static const uchar slovakRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_BETWEEN, 2, 4 };
static const uchar macedonianRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_NEWRULE,
      Q_MOD_10 | Q_EQ, 2 };
static const uchar lithuanianRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11, Q_NEWRULE,
      Q_MOD_10 | Q_NEQ, 0, Q_AND, Q_MOD_100 | Q_NOT_BETWEEN, 10, 19 };
static const uchar russianStyleRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11, Q_NEWRULE,
      Q_MOD_10 | Q_BETWEEN, 2, 4, Q_AND, Q_MOD_100 | Q_NOT_BETWEEN, 10, 19 };
static const uchar polishRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_MOD_10 | Q_BETWEEN, 2, 4, Q_AND, Q_MOD_100 | Q_NOT_BETWEEN, 10, 19 };
static const uchar romanianRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 0, Q_OR, Q_MOD_100 | Q_BETWEEN, 1, 19 };
static const uchar slovenianRules[] =
    { Q_MOD_100 | Q_EQ, 1, Q_NEWRULE,
      Q_MOD_100 | Q_EQ, 2, Q_NEWRULE,
      Q_MOD_100 | Q_BETWEEN, 3, 4 };
static const uchar malteseRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 0, Q_OR, Q_MOD_100 | Q_BETWEEN, 1, 10, Q_NEWRULE,
      Q_MOD_100 | Q_BETWEEN, 11, 19 };
static const uchar welshRules[] =
    { Q_EQ, 0, Q_NEWRULE,
      Q_EQ, 1, Q_NEWRULE,
      Q_BETWEEN, 2, 5, Q_NEWRULE,
      Q_EQ, 6 };
static const uchar arabicRules[] =
    { Q_EQ, 0, Q_NEWRULE,
      Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 2, Q_NEWRULE,
      Q_MOD_100 | Q_BETWEEN, 3, 10, Q_NEWRULE,
      Q_MOD_100 | Q_NOT | Q_BETWEEN, 0, 2 };
static const uchar tagalogRules[] =
    { Q_LEQ, 1, Q_NEWRULE,
      Q_MOD_10 | Q_EQ, 4, Q_OR, Q_MOD_10 | Q_EQ, 6, Q_OR, Q_MOD_10 | Q_EQ, 9 };
static const uchar catalanRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_LEAD_1000 | Q_EQ, 11 };

static const char * const japaneseStyleForms[] = { "Universal Form", 0 };
static const char * const englishStyleForms[] = { "Singular", "Plural", 0 };
static const char * const frenchStyleForms[] = { "Singular", "Plural", 0 };
static const char * const icelandicForms[] = { "Singular", "Plural", 0 };
static const char * const latvianForms[] = { "Singular", "Plural", "Nullar", 0 };
static const char * const irishStyleForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const slovakForms[] = { "Singular", "Paucal", "Plural", 0 };
static const char * const macedonianForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const lithuanianForms[] = { "Singular", "Paucal", "Plural", 0 };
static const char * const russianStyleForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const polishForms[] = { "Singular", "Paucal", "Plural", 0 };
static const char * const romanianForms[] = { "Singular", "Paucal", "Plural", 0 };
static const char * const slovenianForms[] = { "Singular", "Dual", "Trial", "Plural", 0 };
static const char * const malteseForms[] =
    { "Singular", "Paucal", "Greater Paucal", "Plural", 0 };
static const char * const welshForms[] =
    { "Nullar", "Singular", "Dual", "Sexal", "Plural", 0 };
static const char * const arabicForms[] =
    { "Nullar", "Singular", "Dual", "Minority Plural", "Plural", "Plural (100-102, ...)", 0 };
static const char * const tagalogForms[] =
    { "Singular", "Plural (consonant-ended)", "Plural (vowel-ended)", 0 };
static const char * const catalanForms[] = { "Singular", "Undecal (11)", "Plural", 0 };

#define EOL QLocale::C

static const QLocale::Language japaneseStyleLanguages[] = {
    QLocale::Afan,
    QLocale::Armenian,
    QLocale::Bhutani,
    QLocale::Bislama,
    QLocale::Burmese,
    QLocale::Chinese,
    QLocale::FijiLanguage,
    QLocale::Guarani,
    QLocale::Hungarian,
    QLocale::Indonesian,
    QLocale::Japanese,
    QLocale::Javanese,
    QLocale::Korean,
    QLocale::Malay,
    QLocale::NauruLanguage,
    QLocale::Persian,
    QLocale::Sundanese,
    QLocale::Thai,
    QLocale::Tibetan,
    QLocale::Turkish,
    QLocale::Vietnamese,
    QLocale::Yoruba,
    QLocale::Zhuang,
    EOL
};

static const QLocale::Language englishStyleLanguages[] = {
    QLocale::Abkhazian,
    QLocale::Afar,
    QLocale::Afrikaans,
    QLocale::Albanian,
    QLocale::Amharic,
    QLocale::Assamese,
    QLocale::Aymara,
    QLocale::Azerbaijani,
    QLocale::Bashkir,
    QLocale::Basque,
    QLocale::Bengali,
    QLocale::Bihari,
    // Missing: Bokmal,
    QLocale::Bulgarian,
    QLocale::Cambodian,
    QLocale::Cornish,
    QLocale::Corsican,
    QLocale::Danish,
    QLocale::Dutch,
    QLocale::English,
    QLocale::Esperanto,
    QLocale::Estonian,
    QLocale::Faroese,
    QLocale::Finnish,
    // Missing: Friulian,
    QLocale::Frisian,
    QLocale::Galician,
    QLocale::Georgian,
    QLocale::German,
    QLocale::Greek,
    QLocale::Greenlandic,
    QLocale::Gujarati,
    QLocale::Hausa,
    QLocale::Hebrew,
    QLocale::Hindi,
    QLocale::Interlingua,
    QLocale::Interlingue,
    QLocale::Italian,
    QLocale::Kannada,
    QLocale::Kashmiri,
    QLocale::Kazakh,
    QLocale::Kinyarwanda,
    QLocale::Kirghiz,
    QLocale::Kurdish,
    QLocale::Kurundi,
    QLocale::Laothian,
    QLocale::Latin,
    // Missing: Letzeburgesch,
    QLocale::Lingala,
    QLocale::Malagasy,
    QLocale::Malayalam,
    QLocale::Marathi,
    QLocale::Mongolian,
    // Missing: Nahuatl,
    QLocale::Nepali,
    // Missing: Northern Sotho,
    QLocale::Norwegian,
    QLocale::Nynorsk,
    QLocale::Occitan,
    QLocale::Oriya,
    QLocale::Pashto,
    QLocale::Portuguese,
    QLocale::Punjabi,
    QLocale::Quechua,
    QLocale::RhaetoRomance,
    QLocale::Sesotho,
    QLocale::Setswana,
    QLocale::Shona,
    QLocale::Sindhi,
    QLocale::Singhalese,
    QLocale::Siswati,
    QLocale::Somali,
    QLocale::Spanish,
    QLocale::Swahili,
    QLocale::Swedish,
    QLocale::Tajik,
    QLocale::Tamil,
    QLocale::Tatar,
    QLocale::Telugu,
    QLocale::TongaLanguage,
    QLocale::Tsonga,
    QLocale::Turkmen,
    QLocale::Twi,
    QLocale::Uigur,
    QLocale::Urdu,
    QLocale::Uzbek,
    QLocale::Volapuk,
    QLocale::Wolof,
    QLocale::Xhosa,
    QLocale::Yiddish,
    QLocale::Zulu,
    EOL
};
static const QLocale::Language frenchStyleLanguages[] = {
    // keep synchronized with frenchStyleCountries
    QLocale::Breton,
    QLocale::French,
    QLocale::Portuguese,
    // Missing: Filipino,
    QLocale::Tigrinya,
    // Missing: Walloon
    EOL
};
static const QLocale::Language latvianLanguage[] = { QLocale::Latvian, EOL };
static const QLocale::Language icelandicLanguage[] = { QLocale::Icelandic, EOL };
static const QLocale::Language irishStyleLanguages[] = {
    QLocale::Divehi,
    QLocale::Gaelic,
    QLocale::Inuktitut,
    QLocale::Inupiak,
    QLocale::Irish,
    QLocale::Manx,
    QLocale::Maori,
    // Missing: Sami,
    QLocale::Samoan,
    QLocale::Sanskrit,
    EOL
};
static const QLocale::Language slovakLanguages[] = { QLocale::Slovak, QLocale::Czech, EOL };
static const QLocale::Language macedonianLanguage[] = { QLocale::Macedonian, EOL };
static const QLocale::Language lithuanianLanguage[] = { QLocale::Lithuanian, EOL };
static const QLocale::Language russianStyleLanguages[] = {
    QLocale::Bosnian,
    QLocale::Byelorussian,
    QLocale::Croatian,
    QLocale::Russian,
    QLocale::Serbian,
    QLocale::SerboCroatian,
    QLocale::Ukrainian,
    EOL
};
static const QLocale::Language polishLanguage[] = { QLocale::Polish, EOL };
static const QLocale::Language romanianLanguages[] = {
    QLocale::Moldavian,
    QLocale::Romanian,
    EOL
};
static const QLocale::Language slovenianLanguage[] = { QLocale::Slovenian, EOL };
static const QLocale::Language malteseLanguage[] = { QLocale::Maltese, EOL };
static const QLocale::Language welshLanguage[] = { QLocale::Welsh, EOL };
static const QLocale::Language arabicLanguage[] = { QLocale::Arabic, EOL };
static const QLocale::Language tagalogLanguage[] = { QLocale::Tagalog, EOL };
static const QLocale::Language catalanLanguage[] = { QLocale::Catalan, EOL };

static const QLocale::Country frenchStyleCountries[] = {
    // keep synchronized with frenchStyleLanguages
    QLocale::AnyCountry,
    QLocale::AnyCountry,
    QLocale::Brazil,
    QLocale::AnyCountry
};
struct NumerusTableEntry {
    const uchar *rules;
    int rulesSize;
    const char * const *forms;
    const QLocale::Language *languages;
    const QLocale::Country *countries;
};

static const NumerusTableEntry numerusTable[] = {
    { 0, 0, japaneseStyleForms, japaneseStyleLanguages, 0 },
    { englishStyleRules, sizeof(englishStyleRules), englishStyleForms, englishStyleLanguages, 0 },
    { frenchStyleRules, sizeof(frenchStyleRules), frenchStyleForms, frenchStyleLanguages,
      frenchStyleCountries },
    { latvianRules, sizeof(latvianRules), latvianForms, latvianLanguage, 0 },
    { icelandicRules, sizeof(icelandicRules), icelandicForms, icelandicLanguage, 0 },
    { irishStyleRules, sizeof(irishStyleRules), irishStyleForms, irishStyleLanguages, 0 },
    { slovakRules, sizeof(slovakRules), slovakForms, slovakLanguages, 0 },
    { macedonianRules, sizeof(macedonianRules), macedonianForms, macedonianLanguage, 0 },
    { lithuanianRules, sizeof(lithuanianRules), lithuanianForms, lithuanianLanguage, 0 },
    { russianStyleRules, sizeof(russianStyleRules), russianStyleForms, russianStyleLanguages, 0 },
    { polishRules, sizeof(polishRules), polishForms, polishLanguage, 0 },
    { romanianRules, sizeof(romanianRules), romanianForms, romanianLanguages, 0 },
    { slovenianRules, sizeof(slovenianRules), slovenianForms, slovenianLanguage, 0 },
    { malteseRules, sizeof(malteseRules), malteseForms, malteseLanguage, 0 },
    { welshRules, sizeof(welshRules), welshForms, welshLanguage, 0 },
    { arabicRules, sizeof(arabicRules), arabicForms, arabicLanguage, 0 },
    { tagalogRules, sizeof(tagalogRules), tagalogForms, tagalogLanguage, 0 },
    { catalanRules, sizeof(catalanRules), catalanForms, catalanLanguage, 0 }
};

static const int NumerusTableSize = sizeof(numerusTable) / sizeof(numerusTable[0]);

// magic number for the file
static const int MagicLength = 16;
static const uchar magic[MagicLength] = {
    0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
    0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};

bool getNumerusInfo(QLocale::Language language, QLocale::Country country,
                           QByteArray *rules, QStringList *forms)
{
    while (true) {
        for (int i = 0; i < NumerusTableSize; ++i) {
            const NumerusTableEntry &entry = numerusTable[i];
            for (int j = 0; entry.languages[j] != EOL; ++j) {
                if (entry.languages[j] == language
                        && ((!entry.countries && country == QLocale::AnyCountry)
                            || (entry.countries && entry.countries[j] == country))) {
                    if (rules) {
                        *rules = QByteArray::fromRawData(reinterpret_cast<const char *>(entry.rules),
                                                    entry.rulesSize);
                    }
                    if (forms) {
                        forms->clear();
                        for (int k = 0; entry.forms[k]; ++k)
                            forms->append(QLatin1String(entry.forms[k]));
                    }
                    return true;
                }
            }
        }

        if (country == QLocale::AnyCountry)
            break;
        country = QLocale::AnyCountry;
    }
    return false;
}

QT_END_NAMESPACE
