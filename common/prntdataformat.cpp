#include "prntdataformat.h"
#include <QDebug>

Prntdataformat::Prntdataformat(QObject *parent) : QObject(parent)
{
}

// print character, length = repeat, alight=left/center/right
bool Prntdataformat::codeChars(char c, int repeat, Justify justify)
{
    QString s;
    for (int i=1; i<repeat; i++) {
        s += c;
    }

    return codeLine(s, justify);
}

// print string and feed new line, at pos, alight=left/center/right
bool Prntdataformat::codeLine(const QString &str, Justify justify, int pos)
{
    if (pos >= 0) {
        if (justify == Justify_right) {
            prntTextHorztab((quint8)(pos-str.length()));
            prntTextAlight(Justify_left);
        } else {
            prntTextHorztab((quint8)pos);
            prntTextAlight(justify);
        }
    }
    else if (justify != Justify_none) {
        prntTextAlight(justify);
    }

    prntLine(str);

    return true;
}

// print string, at pos, alight=left/center/right
bool Prntdataformat::codeText(const QString &str, Justify justify, int pos)
{
    if (pos >= 0) {
        if (justify == Justify_right) {
            prntTextHorztab((quint8)(pos-str.length()));
            prntTextAlight(Justify_left);
        } else {
            prntTextHorztab((quint8)pos);
            prntTextAlight(justify);
        }
    }
    else if (justify != Justify_none) {
        prntTextAlight(justify);
    }

    prntText(str);

    return true;
}

void Prntdataformat::emphasized_on()
{
    quint8 result[3];
    result[0] = ESC;
    result[1] = 69;
    result[2] = 0xF;

    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

void Prntdataformat::emphasized_off()
{
    quint8 result[3];
    result[0] = ESC;
    result[1] = 69;
    result[2] = 0;

    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

// print barcode
bool Prntdataformat::codeBarcode(const QString &barcode, int width, int height, Justify justify, int pos)
{
    if (pos >= 0) {
        prntTextHorztab((quint8)pos);
    }

    if (justify != Justify_none) {
        prntTextAlight(justify);
    }

    prntBarcodeWidth(width);
    prnt_HRI(width);
    prntBarcodeHeight(height);
    prntBarcode(BarCode::CODE93, barcode);

    return true;
}

// feed new line
void Prntdataformat::codeFeedline(int line)
{
    prntFeedline(line);
}

// cut page
void Prntdataformat::codeCutpage(bool seperate)
{
    prntCutpage(seperate);
}

// set font type
void Prntdataformat::codeFontType(Fonttype ft)
{
    prntFonttype(ft);
}

// set font bold
void Prntdataformat::codeFontBold(bool isBold)
{
    prntFontBold(isBold);
}

// ------------------


// print character, length = repeat, alight=left/center/right
bool Prntdataformat::cmdChars(char c, int repeat, Justify justify)
{
    QString s;
    for (int i=0; i<repeat; i++) {
        s += c;
    }

    return cmdLine(s, justify);
}

// print string and feed new line, at pos, alight=left/center/right
bool Prntdataformat::cmdLine(const QString &str, Justify justify, int pos)
{
    QJsonObject jso;
    jso[PRNKEY_CMD] = Cmd_Line;
    jso[PRNKEY_LINE] = str;

    if (pos >= 0) {
        jso[PRNKEY_POS] = pos;
    }

    if (justify != Justify_none) {
        jso[PRNKEY_JUSTIFY] = justify;
    }

    _prntCmd.append(jso);

    return true;
}

// print string, at pos, alight=left/center/right
bool Prntdataformat::cmdText(const QString &str, Justify justify, int pos)
{
    QJsonObject jso;
    jso[PRNKEY_CMD] = Cmd_Text;
    jso[PRNKEY_TEXT] = str;

    if (pos >= 0) {
        jso[PRNKEY_POS] = pos;
    }

    if (justify != Justify_none) {
        jso[PRNKEY_JUSTIFY] = justify;
    }

    _prntCmd.append(jso);

    return true;
}

// print barcode
bool Prntdataformat::cmdBarcode(const QString &barcode, int width, int height, Justify justify, int pos)
{
    QJsonObject jso;
    jso[PRNKEY_CMD] = Cmd_Barcode;
    jso[PRNKEY_BARCODE] = barcode;
    jso[PRNKEY_WIDTH] = width;
    jso[PRNKEY_HEIGHT] = height;

    if (pos >= 0) {
        jso[PRNKEY_POS] = pos;
    }

    if (justify != Justify_none) {
        jso[PRNKEY_JUSTIFY] = justify;
    }

    _prntCmd.append(jso);

    return true;
}

// feed new line
void Prntdataformat::cmdFeedline(int line)
{
    QJsonObject jso;
    jso[PRNKEY_CMD] = Cmd_Feedline;
    jso[PRNKEY_LINE] = line;

    _prntCmd.append(jso);
}

// cut page
void Prntdataformat::cmdCutpage(bool seperate)
{
    QJsonObject jso;
    jso[PRNKEY_CMD] = Cmd_CutPage;
    jso[PRNKEY_CUTSEPARATE] = seperate;

    _prntCmd.append(jso);
}

// set font type
void Prntdataformat::cmdFontType(Fonttype ft)
{
    QJsonObject jso;
    jso[PRNKEY_CMD] = Cmd_Font;
    jso[PRNKEY_FONTTYPE] = ft;

    _prntCmd.append(jso);
}

// set font bold
void Prntdataformat::cmdFontBold(bool isBold)
{
    QJsonObject jso;
    jso[PRNKEY_CMD] = Cmd_Font;
    jso[PRNKEY_FONTBOLD] = isBold;

    _prntCmd.append(jso);
}

// make print object, data from json-command-list
QString Prntdataformat::makePrintDataFromJsoncmd(const QJsonArray& cmd)
{
    QJsonObject jso;
    jso["code"] = "VNG.GBC.PRN.DATA.TYPE=PRNCMD";
    jso["data"] = cmd;

    QJsonDocument jsd(jso);
    return QString::fromUtf8(jsd.toJson().data());
}

// make print object, data from list of print-command-code
QString Prntdataformat::makePrintDataFromPrncode(const QByteArray& code)
{
    QJsonObject jso;
    jso["code"] = "VNG.GBC.PRN.DATA.TYPE=PRNCODE";

    QJsonArray ja;
    for (int i=0; i<code.length(); i++) {
        ja.append(code[i]);
    }
    jso["data"] = ja;

    QJsonDocument jsd(jso);
    return QString::fromUtf8(jsd.toJson().data());
}

// transfer JSON data to printer BYTE data
QByteArray Prntdataformat::prntProcess(const QString& data)
{
    QJsonDocument jsd = QJsonDocument::fromJson(data.toUtf8());

    if (!jsd.isObject()) {
        //Print process: Invalite printer data formated! data should be JSON object
        return QByteArray();
    }

    QJsonObject jso = jsd.object();
    if ( !jso.contains("code") || !jso.contains("data") ) {
        //Print process: Unknow data format! data should be have 'code' and 'data'
        return QByteArray();
    }

    //qDebug() << "Print process: Data code=" << jso["code"].toString();

    if (jso["code"].toString().compare("VNG.GBC.PRN.DATA.TYPE=PRNCODE") == 0)
    {
        _prntCode.clear();

        QJsonArray ja = jso["data"].toArray();
        for (int i=0; i<ja.size(); i++) {
            _prntCode.append((quint8)(ja[i].toInt()));
        }

        return _prntCode;
    }
    else if (jso["code"].toString().compare("VNG.GBC.PRN.DATA.TYPE=PRNCMD") == 0)
    {
        prntInitial(CodePage::WPC1252);

        QJsonArray jsarData = jso["data"].toArray();
        for( int i=0; i < jsarData.count(); i++ )
        {
            QJsonObject jso = jsarData.at(i).toObject();

            // general command
            if (jso.contains(PRNKEY_JUSTIFY)) {
                int jus = jso[PRNKEY_JUSTIFY].toInt();
                if (jus >= 0) {
                    prntTextAlight(jus);
                }
            }
            if (jso.contains(PRNKEY_POS)) {
                int pos = jso[PRNKEY_POS].toInt();
                if (pos >= 0) {
                    prntTextHorztab((quint8)pos);
                }
            }

            switch (jso[PRNKEY_CMD].toInt())
            {
            case Cmd_Prncode:
                if (jso.contains(PRNKEY_PRNCODE)) {
                    QJsonArray ja = jso[PRNKEY_PRNCODE].toArray();
                    for (int i=0; i<ja.size(); i++) {
                        _prntCode.append((quint8)(ja[i].toInt()));
                    }
                }
                break;
            case Cmd_Codepage:
                if (jso.contains(PRNKEY_CODEPAGE)) {
                    int cp = jso[PRNKEY_CODEPAGE].toInt();
                    if (cp >= 0) {
                        prntCodetable((quint8)cp);
                    }
                }
                break;
            case Cmd_Text:
                if (jso.contains(PRNKEY_TEXT)) {
                    QString text = jso[PRNKEY_TEXT].toString();
                    if (!text.isEmpty()) {
                        prntText(text);
                    }
                }
                break;
            case Cmd_Line:
                if (jso.contains(PRNKEY_LINE)) {
                    QString text = jso[PRNKEY_LINE].toString();
                    if (!text.isEmpty()) {
                        prntLine(text);
                    }
                }
                break;
            case Cmd_Barcode:
                if (jso.contains(PRNKEY_WIDTH)) {
                    int width = jso[PRNKEY_WIDTH].toInt();
                    if (width > 0) {
                        prntBarcodeWidth(width);
                        prnt_HRI(width);
                    }
                }
                if (jso.contains(PRNKEY_HEIGHT)) {
                    int height = jso[PRNKEY_HEIGHT].toInt();
                    if (height > 0) {
                        prntBarcodeHeight(height);
                    }
                }
                if (jso.contains(PRNKEY_BARCODE)) {
                    QString barcode = jso[PRNKEY_BARCODE].toString();
                    if (!barcode.isEmpty()) {
                        prntBarcode(BarCode::CODE93, barcode);
                    }
                }
                break;
            case Cmd_Feedline:
                if (jso.contains(PRNKEY_LINE)) {
                    int line = jso[PRNKEY_LINE].toInt();
                    if (line >= 0) {
                        prntFeedline(line);
                    }
                }
                break;
            case Cmd_CutPage:
            {
                bool cutSeperate = true;
                if (jso.contains(PRNKEY_CUTSEPARATE)) {
                    cutSeperate = jso[PRNKEY_CUTSEPARATE].toBool();
                }
                prntCutpage(cutSeperate);
            }
                break;
            case Cmd_Font:
                if (jso.contains(PRNKEY_FONTTYPE)) {
                    int type = jso[PRNKEY_FONTTYPE].toInt();
                    prntFonttype((Fonttype)type);
                }
                if (jso.contains(PRNKEY_FONTBOLD)) {
                    bool bold = jso[PRNKEY_FONTBOLD].toBool();
                    prntFontBold(bold);
                }
                break;
            default:
                break;
            }
        }

        return _prntCode;
    }

    // print data format unknown
    return QByteArray();
}

// get print command to send to printer (dock 2.0)
QString Prntdataformat::getPrintCmd()
{
    return makePrintDataFromJsoncmd(_prntCmd);
}

// get print code to send to printer (dock 2.0)
QString Prntdataformat::getPrintCode()
{
    return makePrintDataFromPrncode(_prntCode);
}

QByteArray Prntdataformat::getPrintDeviceCode()
{
    return _prntCode;
}

void Prntdataformat::prntInitial(int codepage)
{
    _prntCode.clear();

    quint8 result[2];
    result[0] = ESC;
    result[1] = 64;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));

    if (codepage >= 0) {
        prntCodetable((quint8)codepage);
    }
}

void Prntdataformat::prntCodetable(quint8 codepage)
{
    quint8 result[3];
    result[0] = ESC;
    result[1] = 116;
    result[2] = codepage;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

/**
 * Turn underline mode on, set at 1-dot width
 * ESC - n
 * @return bytes for this command
 */
void Prntdataformat::prntUnderline(Underline mode)
{
    quint8 result[3];
    result[0] = ESC;
    result[1] = 45;
    result[2] = mode;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

// justify: 0=left; 1=center; 2=right
void Prntdataformat::prntTextAlight(quint8 justify)
{
    quint8 result[3];
    result[0] = ESC;
    result[1] = 97;
    result[2] = justify;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

void Prntdataformat::prntTextHorztab(quint8 pos)
{
    quint8 result[4];
    result[0] = ESC;
    result[1] = 68;
    result[2] = pos;
    result[3] = 0;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

void Prntdataformat::prntText(QString text)
{
    if( !text.isEmpty() )
    {
        QByteArray bytes = text.toLatin1();
        _prntCode.append((const char *)bytes.data(), bytes.length());
    }
}

void Prntdataformat::prntLine(QString text)
{
    prntText(text);
    prntFeedline(0);
}

void Prntdataformat::prntBarcodeWH(quint8 width, quint8 dot)
{
    prntBarcodeWidth(width);
    prntBarcodeHeight(dot);
}

void Prntdataformat::prntBarcodeWidth(quint8 width)
{
    quint8 result[3];
    result[0] = GS;
    result[1] = 119;
    result[2] = width;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

void Prntdataformat::prntBarcodeHeight(quint8 dot)
{
    quint8 result[3];
    result[0] = GS;
    result[1] = 104;
    result[2] = dot;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

void Prntdataformat::prntBarcode(quint8 type, QString barcode)
{
    QByteArray barcodebytes = barcode.toLatin1();
    QByteArray result(4+barcodebytes.length(), 0);
    result[0] = GS;
    result[1] = 107;
    result[2] = type;
    result[3] = barcodebytes.length();
    int idx = 4;

    for ( int i = 0; i < barcodebytes.length(); i++ )
    {
        result[idx] = barcodebytes[i];
        idx++;
    }
    _prntCode.append((const char *)result.data(), result.length());
}

// n<=0: feed line; n>0: print data and feed n-line
void Prntdataformat::prntFeedline(int n)
{
    if ( n <=0 )
    {
        quint8 result[1];
        result[0] = LF;
        _prntCode.append((const char *)result, (qint64)sizeof(result) /sizeof(quint8));
    }
    else
    {
        quint8 result[3];
        result[0] = ESC;
        result[1] = 100;
        result[2] = n;
        _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
    }
}

// seperate=true: cut page seperate out, seperate=false: keep one-page-point uncut
void Prntdataformat::prntCutpage(bool seperate)
{
    quint8 result[4];
    result[0] = GS;
    result[1] = 86;
    result[2] = seperate ? 65 : 66;
    result[3] = 0;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

void Prntdataformat::prntFonttype(Fonttype ft)
{
    quint8 result[3];
    result[0] = ESC;
    result[1] = 77;
    result[2] = ft;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

/**
 * double height width mode on Font A
 * ESC ! n
 * @return bytes for this command
 */
void Prntdataformat::prntFontBold(bool isBold)
{
    quint8 result[3];
    result[0] = ESC;
    result[1] = 33;
    result[2] = isBold ? 56 : 0;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

//human readable interpretation
void Prntdataformat::prnt_HRI(quint8 pos)
{
    quint8 result[3];
    result[0] = GS;
    result[1] = 72;
    result[2] = pos;
    _prntCode.append((const char *)result, sizeof(result) /sizeof(quint8));
}

QString Prntdataformat::TestPrintCmd()
{
    _prntCmd.empty();

    cmdFontType(font_A);
    cmdFontBold(true);
    cmdLine("TITLE CMD", Justify_center);
    cmdFontBold(false);

    cmdLine("Tang 5-TTTM Flemington");
    cmdLine("184 Le Dai Hanh, P.15, Q.11");
    cmdFeedline();

    cmdLine("May:\tMAY-01", Justify_left, 10);
    cmdLine("Hoa don:\t123456789");
    cmdLine("Ngay:\t2017-02-16 15:26:39");

    cmdChars('=', 32, Justify_center);

    cmdText("TEN", Justify_left);
    cmdText("\tSL", Justify_none, 11);
    cmdText("\tD.GIA", Justify_none, 17);
    cmdLine("\tT.TIEN", Justify_none, 25);

    cmdFeedline(1);

    cmdChars('-', 32, Justify_center);

    // ...
    cmdFeedline(2);
    cmdChars('-', 32);

    cmdFeedline(1);

    cmdBarcode("3579102468", 2, 80, Justify_center);
    cmdFeedline(1);

    cmdLine("Cam on quy khach");
    cmdLine("Hen gap line");

    cmdFeedline(3);
    cmdCutpage();

    return makePrintDataFromJsoncmd(_prntCmd);
}

QByteArray Prntdataformat::TestPrintCode()
{
    prntInitial(CodePage::WPC1252);

    prntFonttype(Fonttype::font_A);
    prntFontBold(true);
    prntTextAlight(Justify_center);
    prntLine("TITLE CMD");
    prntFontBold(false);

    prntLine("Tang 5-TTTM Flemington");
    prntLine("184 Le Dai Hanh, P.15, Q.11");
    prntFeedline();

    prntTextAlight(Justify_left);
    prntTextHorztab(10);
    prntLine("May:\tMAY-01");
    prntLine("Hoa don:\t0123456789");
    prntLine("Ngay:\t2017-02-22 10:31:48");

    prntTextAlight(Justify_center);
    prntLine("================================");

    prntTextAlight(Justify_left);
    prntText("TEN");

    prntTextHorztab(11);
    prntText("\tSL");

    prntTextHorztab(17);
    prntText("\tD.GIA");

    prntTextHorztab(25);
    prntLine("\tT.TIEN");

    prntFeedline(1);

    prntTextAlight(Justify_center);
    prntLine("--------------------------------");

    // ...
    prntFeedline(2);

    prntTextAlight(Justify_center);
    prntLine("--------------------------------");

    prntFeedline(1);

    prntTextAlight(Justify_center);
    //prntBarcodeWH(2, 80);
    prntBarcodeWidth(2);
    prnt_HRI(2);
    prntBarcodeHeight(80);
    prntBarcode(BarCode::CODE93, "3579102468");

    prntFeedline(1);

    prntTextAlight(Justify_center);
    prntLine("Cam on quy khach");
    prntLine("Hen gap lai");

    prntFeedline(3);
    prntCutpage();

    return _prntCode;
}
