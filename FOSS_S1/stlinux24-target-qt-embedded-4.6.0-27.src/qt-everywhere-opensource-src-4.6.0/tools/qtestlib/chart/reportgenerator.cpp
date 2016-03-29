/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include "reportgenerator.h"

// Report generator file utility functions

QList<QByteArray> readLines(const QString &fileName)
{
    QList<QByteArray> lines;
    QFile f(fileName);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    while(!f.atEnd())
       lines.append(f.readLine());
    return lines;
}

void writeLines(const QString &fileName, const QList<QByteArray> &lines)
{
    QFile f(fileName);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    foreach(const QByteArray line, lines)
       f.write(line);
}

void writeFile(const QString &fileName, const QByteArray &contents)
{
    QFile f(fileName);
    f.open(QIODevice::WriteOnly | QIODevice::Append);
    f.write(contents);
}

// Report generator database utility functions

QStringList select(const QString &field, const QString &tableName)
{
    QSqlQuery query;
    query.prepare("SELECT DISTINCT " + field +" FROM " + tableName);
    bool ok  = query.exec(); 
    Q_UNUSED(ok);
//    if (!ok)
//        qDebug() << "select unique ok" << ok;

    QStringList values;
    while (query.next()) {
        values += query.value(0).toString();
    }
    return values;
}

QStringList selectUnique(const QString &field, const QString &tableName)
{
    QSqlQuery query;
    query.prepare("SELECT DISTINCT " + field +" FROM " + tableName);
    bool ok  = query.exec(); 
    Q_UNUSED(ok);
//    if (!ok)
//        qDebug() << "select unique ok" << ok;

    QStringList values;
    while (query.next()) {
        values += query.value(0).toString();
    }
    return values;
}

QSqlQuery selectFromSeries(const QString &serie, const QString &column, const QString &tableName, const QString &seriesName)
{
    QSqlQuery query;
    if (serie == QString())
        query.prepare("SELECT " + column + " FROM " + tableName);
    else
        query.prepare("SELECT " + column + " FROM " + tableName + " WHERE " + seriesName + "='" + serie + "'");
    /*bool ok  =*/ query.exec(); 
    

//    qDebug() <<  "selectDataFromSeries ok?" <<  ok << query.size();
    return query;
}

int countDataFromSeries(const QString &serie, const QString &tableName, const QString &seriesName)
{
//    qDebug() << "count" << serie << "in" << tableName;
    QSqlQuery query;
    query.prepare("SELECT COUNT(Result) FROM " + tableName + " WHERE" + seriesName + "='" + serie + "'");
    bool ok  = query.exec(); 
    if (!ok) {
        qDebug() << "query fail" << query.lastError();
    }
    
    qDebug() <<  "countDataFromSeries ok?" <<  ok << query.size();
    query.next();
    return query.value(0).toInt();
}

// Report generator output utility functions

QList<QByteArray> printData(const QString &tableName, const QString &seriesName, const QString &indexName)
{
    QList<QByteArray> output;
    QStringList series = selectUnique(seriesName, tableName); 
//    qDebug() << "series" << series;
    if (series.isEmpty())
        series+=QString();
    
    foreach (QString serie, series) {
        QSqlQuery data = selectFromSeries(serie, "Result", tableName, seriesName);
        QSqlQuery labels = selectFromSeries(serie, indexName, tableName, seriesName);

        QByteArray dataLine = "dataset.push({ data: [";
        int i = 0;
        while (data.next() && labels.next()) {
            QString label = labels.value(0).toString();

            QString labelString;
            bool ok;
            label.toInt(&ok);
            if (ok)
                labelString = label;
        //    else
                labelString = QString::number(i);

            dataLine += ("[" + labelString + ", " + data.value(0).toString() + "]");
            
            ++i;
            if (data.next()) {
                dataLine += ", ";
                data.previous();
            }
        }
        dataLine += "], label : \"" + serie + "\" });\n";
        output.append(dataLine);
    }
    return output;
}

// Determines if a line chart should be used. Returns true if the first label is numerical.
bool useLineChart(const QString &tableName, const QString &seriesName, const QString &indexName)
{
    QList<QByteArray> output;
    QStringList series = selectUnique(seriesName, tableName);
	if (series.isEmpty())
		return false;

    QSqlQuery data = selectFromSeries(series[0], indexName, tableName, seriesName);

    if (data.next()) {
        QString label = data.value(0).toString();
        bool ok;
        label.toDouble(&ok);
        return ok;
    }

    return false;
}

int countLabels(const QString &tableName, const QString &seriesName, const QString &indexName)
{
    QStringList series = selectUnique(seriesName, tableName);
        if (series.isEmpty())
            return 0;
    QSqlQuery data = selectFromSeries(series[0], indexName, tableName, seriesName);
    int count = 0;
    while (data.next())
        count++;

    return count;
}


QList<QByteArray> printLabels(const QString &tableName, const QString &seriesName, const QString &indexName)
{
    QList<QByteArray> output;
    QStringList series = selectUnique(seriesName, tableName);
	if (series.isEmpty())
		return QList<QByteArray>();

        QSqlQuery data = selectFromSeries(series[0], indexName, tableName, seriesName);

        int count = 0;
        while (data.next())
            count++;

        data.first(); data.previous();

        const int labelCount = 10;
        int skip = count / labelCount;
       
        QByteArray dataLine;
        int i = 0;
        while (data.next()) {
            dataLine += ("[" + QByteArray::number(i) + ",\"" + data.value(0).toString() + "\"]");
            ++i;
            if (data.next()) {
                dataLine += ", ";
                data.previous();
            }

            // skip labels.
            i += skip;
            for (int j = 0; j < skip; ++j)
                data.next();
        }
        dataLine += "\n";
        output.append(dataLine);
    return output;
}

QByteArray printSeriesLabels(const QString &tableName, const QString &seriesColumnName)
{
    QByteArray output;
    QStringList series = selectUnique(seriesColumnName, tableName);
 	if (series.isEmpty())
        return "[];\n";

    output += "[";
    
    foreach(const QString &serie, series) {
        output += "\"" + serie.toLocal8Bit() + "\",";
    }
    output.chop(1); //remove last comma
    output += "]\n";
    return output;
}

void addJavascript(QList<QByteArray> *output, const QString &fileName)
{
    output->append("<script type=\"text/javascript\">\n");
    (*output) += readLines(fileName);
    output->append("</script>\n");
}

void addJavascript(QList<QByteArray> *output)
{
    addJavascript(output, ":3rdparty/prototype.js");
    addJavascript(output, ":3rdparty/excanvas.js");
    addJavascript(output, ":3rdparty/flotr.js");
}

TempTable selectRows(const QString &sourceTable, const QString &column, const QString &value)
{
    TempTable tempTable(resultsTable);
    
    QSqlQuery query;
    query.prepare("INSERT INTO " + tempTable.name() + " SELECT * FROM " + sourceTable +
                  " WHERE " + column + "='" + value + "'");
    execQuery(query);
    
//    displayTable(tempTable.name());
    
    return tempTable;
}

TempTable mergeVersions(const QString &)
{

//  QtVersion - As series
//  Result - (free)
//  Idx - match
//  TestName - match
//  CaseName - match

//  (Series - average)
/*
    TempTable tempTable(resultsTable);
    QStringlist versions = selectUnique("QtVersions", sourceTable);

    QSqlQuery oneVersion = select(WHERE QtVersions = versions.at(0))
    while (oneVersion.next) {
        QSqlQuery otherversions = selectMatches(QStringList() << "TestName" << "TestCaseName" << "Idx")
        while (otherversions.next) {
            insert(temptable
        }
    }
*/
    return TempTable("");
}

QStringList fieldPriorityList = QStringList() << "Idx" << "Series" << "QtVersion";

struct IndexSeriesFields
{
    QString index;
    QString series;
};

IndexSeriesFields selectFields(const QString &table)
{
    IndexSeriesFields fields;
    foreach (QString field, fieldPriorityList) {
//        qDebug() << "unique" << field << selectUnique(field, table).count();
        QStringList rows = selectUnique(field, table);

        if (rows.count() <= 1 && rows.join("") == QString(""))
            continue;

        if (fields.index.isEmpty()) {
            fields.index = field;
            continue;
        }

        if (fields.series.isEmpty()) {
            fields.series = field;
            break;
        }
    }
    return fields;
}

TempTable selectTestCase(const QString &testCase, const QString &sourceTable)
{
    return selectRows(sourceTable, QLatin1String("TestCaseName"), testCase);
}

QString field(const QSqlQuery &query, const QString &name)
{
    return query.value(query.record().indexOf(name)).toString();
}

QSqlQuery selectAllResults(const QString &tableName)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM " + tableName);
    execQuery(query);
    return query;
}

void printTestCaseResults(const QString &testCaseName)
{
//    QStringList testCases = selectUnique("TestCaseName", "Results");
    qDebug() << "";
    qDebug() << "Results for benchmark" << testCaseName;
    TempTable temptable = selectTestCase(testCaseName, "Results");
    QSqlQuery query = selectAllResults(temptable.name());
    if (query.isActive() == false) {
        qDebug() << "No results";
        return;
    }
    
    query.next();

    if (field(query, "Idx") == QString()) {
        do {
            qDebug() << "Result:" << field(query, "result");
        } while (query.next());
    } else if (field(query, "Series") == QString()) {
        do {
            qDebug() << field(query, "Idx") << " : " << field(query, "result");
        } while (query.next());
    } else {
        do {
            qDebug() << field(query, "Series") << " - " <<  field(query, "Idx") << " : " << field(query, "result");
        } while (query.next());
    }

    qDebug() << "";
}


// ReportGenerator implementation

ReportGenerator::ReportGenerator()
{
	m_colorScheme = QList<QByteArray>() << "#a03b3c" << "#3ba03a" << "#3a3ba0" << "#3aa09f" << "#39a06b" << "#a09f39";
}


void ReportGenerator::writeReport(const QString &tableName, const QString &fileName, bool combineQtVersions)
{
    QStringList testCases = selectUnique("TestCaseName", tableName);
    QList<QByteArray> lines = readLines(":benchmark_template.html");
    QList<QByteArray> output;

    foreach(QByteArray line, lines) {
        if (line.contains("<! Chart Here>")) {
            foreach (const QString testCase, testCases) {
                TempTable testCaseTable = selectTestCase(testCase, tableName);
                output += writeChart(testCaseTable.name(), combineQtVersions);
            }
         } else if (line.contains("<! Title Here>")) {
            QStringList name = selectUnique("TestName", tableName);
            output += "Test: " + name.join("").toLocal8Bit();
         } else if (line.contains("<! Description Here>")) {
            output += selectUnique("TestTitle", tableName).join("").toLocal8Bit();
        } else if (line.contains("<! Javascript Here>")){
            addJavascript(&output);
         } else {
            output.append(line);
        }
    }

    m_fileName = fileName;

    writeLines(m_fileName, output);
    qDebug() << "Wrote report to" << m_fileName;
}

void ReportGenerator::writeReports()
{
/*
    QStringList versions = selectUnique("QtVersion", "Results");

 //   qDebug() << "versions" << versions;

    foreach (QString version, versions) {
        QString fileName = "results-"  + version  + ".html";
        TempTable versionTable = selectRows("Results", "QtVersion", version);
        writeReport(versionTable.name(), fileName, false);
    }
*/
    writeReport("Results", "results.html", false);
}

QString ReportGenerator::fileName()
{
    return m_fileName;
}

QList<QByteArray> ReportGenerator::writeChart(const QString &tableName, bool combineQtVersions)
{
    QSqlQuery query;
    query.prepare("SELECT TestName, Series, Idx, Result, ChartWidth, ChartHeight, Title, TestCaseName, ChartType, QtVersion FROM " + tableName);
    execQuery(query);
       
    QString seriesName;
    QString indexName;
    
    if (combineQtVersions) {
        IndexSeriesFields fields = selectFields(tableName);
        seriesName = fields.series;
        indexName = fields.index;
    } else {
        seriesName = "Series";
        indexName = "Idx";
    }

    QList<QByteArray> data = printData(tableName, seriesName, indexName);
    QList<QByteArray> labels = printLabels(tableName, seriesName, indexName);
    QByteArray seriesLabels = printSeriesLabels(tableName, seriesName);
    QByteArray useLineChartString = useLineChart(tableName, seriesName, indexName) ? "true" : "false" ;

    query.next();
    QString testName = query.value(0).toString();
    QSize size(query.value(4).toInt(), query.value(5).toInt());
//    QString title = "Test Function: " + query.value(7).toString() + " - " +  query.value(6).toString();
    QString title = "Test Function: " + query.value(7).toString();
    QString chartId = "\"" + query.value(7).toString() + "\"";
    QString formId = "\"" + query.value(7).toString() + "form\"";
    QString chartTypeFormId = "\"" + query.value(7).toString() + "chartTypeform\"";
    QString scaleFormId = "\"" + query.value(7).toString() + "scaleform\"";
    QString type = query.value(8).toString();

    // Skip chart generation if there isn't enough data.
    if (countLabels(tableName, seriesName, indexName) < 2) {
        qDebug() << title.toAscii() << "No chartable data. (See the \"series\" test function"
                                    << "in examples/qtestlib/tutorial5 for an example.) ";
        return QList<QByteArray>() << title.toAscii() << " (no chartable data)"; // TODO: genrate text table here.
    }

//    QString qtVersion = query.value(9).toString();
    query.previous();

    QString sizeString = "height=\"" + QString::number(size.height()) + "\" width=\"" + QString::number(size.width()) + "\"";

    QString fillString;
    if (type == "LineChart")
        fillString = "false";
    else
        fillString = "true";

    QByteArray colors = printColors(tableName, seriesName);

    QList<QByteArray> lines = readLines(":chart_template.html");
    QList<QByteArray> output;

    foreach(QByteArray line, lines) {
        if (line.contains("<! Test Name Here>")) {
            output.append(title.toLocal8Bit());
        } else if (line.contains("<! Chart ID Here>")) {
            output += chartId.toLocal8Bit();
        } else if (line.contains("<! Form ID Here>")) {
            output += formId.toLocal8Bit();
        } else if (line.contains("<! ChartTypeForm ID Here>")) {
            output += chartTypeFormId.toLocal8Bit();    
        } else if (line.contains("<! ScaleForm ID Here>")) {
            output += scaleFormId.toLocal8Bit();    
        } else if (line.contains("<! Size>")) {
            output += sizeString.toLocal8Bit();
        } else if (line.contains("<! ColorScheme Here>")) {
            output += colors;
        } else if (line.contains("<! Data Goes Here>")) {
            output += data;
        } else if (line.contains("<! Labels Go Here>")) {
            output += labels;
        } else if (line.contains("<! Use Line Chart Here>")) {
            output += useLineChartString + ";";
        } else if (line.contains("<! Chart Type Here>")) {
            output += "\"" + type.toLocal8Bit() + "\"";
        } else if (line.contains("<! Fill Setting Here>")) {
            output += fillString.toLocal8Bit();            
        } else if (line.contains("<! Series Labels Here>")) {
            output += seriesLabels;
        } else {
            output.append(line);
        }
    }

    return output;
}

QByteArray ReportGenerator::printColors(const QString &tableName, const QString &seriesName)
{
    QByteArray colors;
    int i = 0;
    QStringList series = selectUnique(seriesName, tableName);
    foreach (const QString &serie, series) {
        colors.append("'" + serie.toLocal8Bit() + "': '" + m_colorScheme.at(i % m_colorScheme.count()) + "',\n");
        ++ i;
    }
    colors.chop(2); // remove last comma
    colors.append("\n");
    return colors;
}

