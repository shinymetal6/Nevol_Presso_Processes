#ifndef NEVOL_DWTOOL_H
#define NEVOL_DWTOOL_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class Nevol_DwTool; }
QT_END_NAMESPACE
#define WAIT_REPLY              5000

class Nevol_DwTool : public QMainWindow
{
    Q_OBJECT

public:
    Nevol_DwTool(QWidget *parent = nullptr);
    ~Nevol_DwTool();

private slots:
    void on_Port_comboBox_currentTextChanged(const QString &arg1);

    void on_SelectEEFile_pushButton_clicked();

    void on_DownloadEEFile_pushButton_clicked();

    void on_Run_pushButton_clicked();

    void on_SelectAudioFile_pushButton_clicked();

    void on_DownloadAudioFile_pushButton_clicked();

    void on_Play_pushButton_clicked();

    void on_PlaySound_pushButton_clicked();

    void on_VersionSound_pushButton_clicked();

    void on_SelectSoundFile_pushButton_clicked();

    void on_DownloadSoundFile_pushButton_clicked();

private:
    Ui::Nevol_DwTool *ui;

    int serial_tx( QByteArray hex_line);
    int serial_rx( void);
    void create_buf_and_tx(char    *data);
    void download_program(void);
    void download_audio(void);
    void download_sound(void);

    QSerialPort serial;
    int serial_started;
    QByteArray serial_reply;

    unsigned char block_number,csum;
    int packets;
    int packets_errors;

    QString program_filename;
    QString program_basename;
    QString program_file_name;
    QString program_fileversion;
    int program_file_size;

    QString audio_filename;
    QString audio_basename;
    QString audio_file_name;
    QString audio_fileversion;
    int audio_file_size;

    QString sound_filename;
    QString sound_sequence_filename;
    QString sound_sequence_file_name;
    int sound_sequence_file_size;
    int sound_sequence_number;

    QByteArray blob;
    int program_number;


    int timer0Id;
    int timerint;

};
#endif // NEVOL_DWTOOL_H
