#ifndef PRNTDATAFORMAT_H
#define PRNTDATAFORMAT_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

// VNG - PRINT DATA - FORMAT
//
// <T>C<T> //Type=Command: Font, Feedline
// <T>D<T> //Type=Data
// <DATA>T=Text, P=Position, A=Alight<DATA>
//

#define PRNKEY_CMD      "Cmd"
#define PRNKEY_CODEPAGE "Codepage"
#define PRNKEY_TEXT     "Text"
#define PRNKEY_LINE     "Line"
#define PRNKEY_BARCODE  "Barcode"
#define PRNKEY_FEEDLINE "Feedline"
#define PRNKEY_CUTPAGE  "Cutpage"
#define PRNKEY_CUTSEPARATE "CutSeperate"
#define PRNKEY_FONT     "Font"
#define PRNKEY_FONTTYPE "Type"
#define PRNKEY_FONTBOLD "Bold"
#define PRNKEY_JUSTIFY  "Justify"
#define PRNKEY_POS      "Pos"
#define PRNKEY_WIDTH    "Width"
#define PRNKEY_HEIGHT   "Height"
#define PRNKEY_PRNCODE  "Prncode"

class Prntdataformat : public QObject
{
    Q_OBJECT
public:

    enum Command {
        Cmd_Prncode,    // machine print code
        Cmd_Codepage,   // codepage
        Cmd_Text,       // print text
        Cmd_Line,       // print text and feed new line
        Cmd_Barcode,    // print barcode
        Cmd_Feedline,   // new line
        Cmd_CutPage,    // cut page
        Cmd_Font,       // font type normal/bold
    };

    enum Justify {
        Justify_none=-1,
        Justify_left=0,
        Justify_center=1,
        Justify_right=2,
    };

    enum Fonttype {
        font_A = 0,
        font_B = 1,
        font_C = 2,
    };

    enum Underline {
        udl_none = 0,
        udl_1dot = 1,
        udl_2dot = 2,
    };

    explicit Prntdataformat(QObject *parent = 0);

    // print character, length = repeat, alight=left/center/right
    bool codeChars(char c, int repeat, Justify justify=Justify_none);

    // print string and feed new line, at pos, alight=left/center/right
    bool codeLine(const QString &str, Justify justify=Justify_none, int pos=-1);

    // print string, at pos, alight=left/center/right
    bool codeText(const QString &str, Justify justify=Justify_none, int pos=-1);

    void emphasized_on();
    void emphasized_off();
    void prntUnderline(Underline mode);

    // print barcode
    bool codeBarcode(const QString &barcode, int width, int height, Justify justify=Justify_none, int pos=-1);

    // feed new line
    void codeFeedline(int line=0);

    // cut page
    void codeCutpage(bool seperate=true);

    // set font type
    void codeFontType(Fonttype ft=font_A);

    // set font bold
    void codeFontBold(bool isBold=true);

    // get print code to send to printer (dock 2.0)
    QString getPrintCode();

    QByteArray getPrintDeviceCode();

    // make print object, data from list of print-command-code
    static QString makePrintDataFromPrncode(const QByteArray& code);

    QString TestPrintCmd();
    QByteArray TestPrintCode();

private:

    // print character, length = repeat, alight=left/center/right
    bool cmdChars(char c, int repeat, Justify justify=Justify_none);

    // print string and feed new line, at pos, alight=left/center/right
    bool cmdLine(const QString &str, Justify justify=Justify_none, int pos=-1);

    // print string, at pos, alight=left/center/right
    bool cmdText(const QString &str, Justify justify=Justify_none, int pos=-1);

    // print barcode
    bool cmdBarcode(const QString &barcode, int width, int height, Justify justify=Justify_none, int pos=-1);

    // feed new line
    void cmdFeedline(int line=0);

    // cut page
    void cmdCutpage(bool seperate=true);

    // set font type
    void cmdFontType(Fonttype ft=font_A);

    // set font bold
    void cmdFontBold(bool isBold=true);

    // make print object, data from json-command-list
    QString makePrintDataFromJsoncmd(const QJsonArray& cmd);

    // transfer JSON data to printer BYTE data
    QByteArray prntProcess(const QString& data);

    // get print command to send to printer (dock 2.0)
    QString getPrintCmd();

signals:

public slots:

private:
    static const quint8 ESC = 27;
    static const quint8 FS = 28;
    static const quint8 GS = 29;
    static const quint8 DLE = 16;
    static const quint8 EOT = 4;
    static const quint8 ENQ = 5;
    static const quint8 SP = 32;
    static const quint8 HT = 9;
    static const quint8 LF = 10;
    static const quint8 CR = 13;
    static const quint8 FF = 12;
    static const quint8 CAN = 24;

    /**
     * CodePage table
     */
    class CodePage {
        public:
            static const quint8 PC437       = 0;
            static const quint8 KATAKANA    = 1;
            static const quint8 PC850       = 2;
            static const quint8 PC860       = 3;
            static const quint8 PC863       = 4;
            static const quint8 PC865       = 5;
            static const quint8 WPC1252     = 16;
            static const quint8 PC866       = 17;
            static const quint8 PC852       = 18;
            static const quint8 PC858       = 19;
            static const quint8 TCVN_3_30   = 30;
            static const quint8 TCVN_3_31   = 31;
            static const quint8 WPC1258     = 52;
    };

    /**
     * BarCode table
     */
    class BarCode {
        public:
            static const quint8 UPC_A       = 0;
            static const quint8 UPC_E       = 1;
            static const quint8 EAN13       = 2;
            static const quint8 EAN8        = 3;
            static const quint8 CODE39      = 4;
            static const quint8 ITF         = 5;
            static const quint8 NW7         = 6;
            static const quint8 CODE93      = 72;
            static const quint8 CODE128     = 73;
    };

    QJsonArray _prntCmd;
    QByteArray _prntCode;

    void prntInitial(int codepage=-1);
    void prntCodetable(quint8 codepage);
    void prntTextAlight(quint8 justify);
    void prntTextHorztab(quint8 pos);
    void prntText(QString text);
    void prntLine(QString text);
    void prntBarcodeWH(quint8 width, quint8 dot);
    void prntBarcodeWidth(quint8 width);
    void prntBarcodeHeight(quint8 dot);
    void prntBarcode(quint8 type, QString barcode);
    void prntFeedline(int n=0);
    void prntCutpage(bool seperate=true);
    void prntFonttype(Fonttype ft=font_A);
    void prntFontBold(bool isBold=true);
    void prnt_HRI(quint8 pos); //human readable interpretation
};

#endif // PRNTDATAFORMAT_H
