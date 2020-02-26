"""
This tool allows you to generate configuration messages for the Artemis Global Tracker
which you can then upload via Serial (USB) or send via the Rock7 Operations
Send-A-Message feature.

Based on AFU.py (Artemis Firmware Updater) which is based on gist by Stefan Lehmann:
https://gist.github.com/stlehmann/bea49796ad47b1e7f658ddde9620dff1

MIT license

"""
from typing import Iterator, Tuple

from serial.tools.list_ports import comports
import serial

from PyQt5.QtCore import QSettings, QProcess, QTimer, Qt
from PyQt5.QtWidgets import QWidget, QLabel, QComboBox, QGridLayout, \
    QPushButton, QApplication, QLineEdit, QFileDialog, QPlainTextEdit, QCheckBox
from PyQt5.QtGui import QCloseEvent, QTextCursor

import struct

# Setting constants
SETTING_PORT_NAME = 'COM1'
SETTING_FILE_LOCATION = 'C:'

guiVersion = 'v1.0'


def gen_serial_ports() -> Iterator[Tuple[str, str]]:
    """Return all available serial ports."""
    ports = comports()
    return ((p.description, p.device) for p in ports)

# noinspection PyArgumentList


class MainWidget(QWidget):
    """Main Widget."""

    def __init__(self, parent: QWidget = None) -> None:
        super().__init__(parent)
 
        # File location line edit
        self.msg_label = QLabel(self.tr('Configuration File (.pkl):'))
        self.fileLocation_lineedit = QLineEdit()
        self.msg_label.setBuddy(self.msg_label)
        self.fileLocation_lineedit.setEnabled(False)
        self.fileLocation_lineedit.returnPressed.connect(
            self.on_browse_btn_pressed)

        # Browse for new file button
        self.browse_btn = QPushButton(self.tr('Browse'))
        self.browse_btn.setEnabled(True)
        self.browse_btn.pressed.connect(self.on_browse_btn_pressed)

        # Load Config Button
        self.load_config_btn = QPushButton(self.tr('Load Config'))
        self.load_config_btn.pressed.connect(self.on_load_config_btn_pressed)

        # Save Config Button
        self.save_config_btn = QPushButton(self.tr('Save Config'))
        self.save_config_btn.pressed.connect(self.on_save_config_btn_pressed)

        # Calc Config Button
        self.calc_config_btn = QPushButton(self.tr('Calculate Config'))
        self.calc_config_btn.pressed.connect(self.on_calc_config_btn_pressed)

        # Port Combobox
        self.port_label = QLabel(self.tr('COM Port:'))
        self.port_combobox = QComboBox()
        self.port_label.setBuddy(self.port_combobox)
        self.update_com_ports()

        # Refresh Button
        self.refresh_btn = QPushButton(self.tr('Refresh'))
        self.refresh_btn.pressed.connect(self.on_refresh_btn_pressed)

        # Open Port Button
        self.open_port_btn = QPushButton(self.tr('Open Port'))
        self.open_port_btn.pressed.connect(self.on_open_port_btn_pressed)

        # Upload Button
        self.upload_btn = QPushButton(self.tr('Upload Config'))
        self.upload_btn.pressed.connect(self.on_upload_btn_pressed)

        # Terminal Bar
        self.terminal_label = QLabel(self.tr('Serial Terminal:'))

        # Terminal Window
        self.terminal = QPlainTextEdit()

        # Messages Bar
        self.messages_label = QLabel(self.tr('Warnings / Errors:'))

        # Messages Window
        self.messages = QPlainTextEdit()

        # Config Bar
        self.config_label = QLabel(self.tr('Configuration Message:'))

        # Config Window
        self.config = QPlainTextEdit()

        # FLAGS1 Labels
        FLAGS1_header = QLabel(self.tr('FLAGS1:'))
        FLAGS1_header.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        label_BINARY = QLabel(self.tr('Send message in binary format'))
        label_DEST = QLabel(self.tr('Forward messages to DEST'))
        label_HIPRESS = QLabel(self.tr('Enable HIPRESS alarm messages'))
        label_LOPRESS = QLabel(self.tr('Enable LOPRESS alarm messages'))
        label_HITEMP = QLabel(self.tr('Enable HITEMP alarm messages'))
        label_LOTEMP = QLabel(self.tr('Enable LOTEMP alarm messages'))
        label_HIHUMID = QLabel(self.tr('Enable HIHUMID alarm messages'))
        label_LOHUMID = QLabel(self.tr('Enable LOHUMID alarm messages'))

        label_BINARY.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_DEST.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_HIPRESS.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_LOPRESS.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_HITEMP.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_LOTEMP.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_HIHUMID.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_LOHUMID.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        
        # FLAGS1 CheckBoxes
        self.checkbox_F1_BINARY = QCheckBox()
        self.checkbox_F1_DEST = QCheckBox()
        self.checkbox_F1_HIPRESS = QCheckBox()
        self.checkbox_F1_LOPRESS = QCheckBox()
        self.checkbox_F1_HITEMP = QCheckBox()
        self.checkbox_F1_LOTEMP = QCheckBox()
        self.checkbox_F1_HIHUMID = QCheckBox()
        self.checkbox_F1_LOHUMID = QCheckBox()

        # FLAGS2 Labels
        FLAGS2_header = QLabel(self.tr('FLAGS2:'))
        FLAGS2_header.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        label_GEOFENCE = QLabel(self.tr('Enable GeoFence alarm messages'))
        label_INSIDE = QLabel(self.tr('Alarm when inside the GeoFence'))
        label_LOWBATT = QLabel(self.tr('Enable LOWBATT alarm messages'))
        label_RING = QLabel(self.tr('Monitor Ring Channel continuously'))

        label_GEOFENCE.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_INSIDE.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_LOWBATT.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_RING.setAlignment(Qt.AlignRight | Qt.AlignVCenter)

        # FLAGS2 CheckBoxes
        self.checkbox_F2_GEOFENCE = QCheckBox()
        self.checkbox_F2_INSIDE = QCheckBox()
        self.checkbox_F2_LOWBATT = QCheckBox()
        self.checkbox_F2_RING = QCheckBox()

        # USERFUNC Labels
        USERFUNC_header = QLabel(self.tr('USERFUNCs:'))
        USERFUNC_header.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_USERFUNC1 = QLabel(self.tr('Execute USERFUNC1'))
        label_USERFUNC2 = QLabel(self.tr('Execute USERFUNC2'))
        label_USERFUNC3 = QLabel(self.tr('Execute USERFUNC3'))
        label_USERFUNC4 = QLabel(self.tr('Execute USERFUNC4'))
        label_USERFUNC5 = QLabel(self.tr('Execute USERFUNC5'))
        label_USERFUNC6 = QLabel(self.tr('Execute USERFUNC6'))
        label_USERFUNC7 = QLabel(self.tr('Execute USERFUNC7'))
        label_USERFUNC8 = QLabel(self.tr('Execute USERFUNC8'))

        # USERFUNC CheckBoxes
        self.checkbox_USERFUNC1 = QCheckBox()
        self.checkbox_USERFUNC2 = QCheckBox()
        self.checkbox_USERFUNC3 = QCheckBox()
        self.checkbox_USERFUNC4 = QCheckBox()
        self.checkbox_USERFUNC5 = QCheckBox()
        self.checkbox_USERFUNC6 = QCheckBox()
        self.checkbox_USERFUNC7 = QCheckBox()
        self.checkbox_USERFUNC8 = QCheckBox()

        # USERFUNC5-8 Value
        self.USERFUNC5_val = QLineEdit()
        self.USERFUNC6_val = QLineEdit()
        self.USERFUNC7_val = QLineEdit()
        self.USERFUNC8_val = QLineEdit()

        # MOFIELDS Labels

        MOFIELDS_header = QLabel(self.tr('MOFIELDS:'))
        SWVER_label = QLabel(self.tr('SWVER'))
        SOURCE_label = QLabel(self.tr('SOURCE'))
        BATTV_label = QLabel(self.tr('BATTV'))
        PRESS_label = QLabel(self.tr('PRESS'))
        TEMP_label = QLabel(self.tr('TEMP'))
        HUMID_label = QLabel(self.tr('HUMID'))
        YEAR_label = QLabel(self.tr('YEAR'))
        MONTH_label = QLabel(self.tr('MONTH'))
        DAY_label = QLabel(self.tr('DAY'))
        HOUR_label = QLabel(self.tr('HOUR'))
        MIN_label = QLabel(self.tr('MIN'))
        SEC_label = QLabel(self.tr('SEC'))
        MILLIS_label = QLabel(self.tr('MILLIS'))
        DATETIME_label = QLabel(self.tr('DATETIME'))
        LAT_label = QLabel(self.tr('LAT'))
        LON_label = QLabel(self.tr('LON'))
        ALT_label = QLabel(self.tr('ALT'))
        SPEED_label = QLabel(self.tr('SPEED'))
        HEAD_label = QLabel(self.tr('HEAD'))
        SATS_label = QLabel(self.tr('SATS'))

        MOFIELDS_header.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        SWVER_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        SOURCE_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        BATTV_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        PRESS_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        TEMP_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HUMID_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        YEAR_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        MONTH_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        DAY_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HOUR_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        MIN_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        SEC_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        MILLIS_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        DATETIME_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LAT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LON_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        ALT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        SPEED_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HEAD_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        SATS_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)

        PDOP_label = QLabel(self.tr('PDOP'))
        FIX_label = QLabel(self.tr('FIX'))
        USERVAL1_label = QLabel(self.tr('USERVAL1'))
        USERVAL2_label = QLabel(self.tr('USERVAL2'))
        USERVAL3_label = QLabel(self.tr('USERVAL3'))
        USERVAL4_label = QLabel(self.tr('USERVAL4'))
        USERVAL5_label = QLabel(self.tr('USERVAL5'))
        USERVAL6_label = QLabel(self.tr('USERVAL6'))
        USERVAL7_label = QLabel(self.tr('USERVAL7'))
        USERVAL8_label = QLabel(self.tr('USERVAL8'))
        MOFIELDS_label = QLabel(self.tr('MOFIELDS'))
        FLAGS1_label = QLabel(self.tr('FLAGS1'))
        FLAGS2_label = QLabel(self.tr('FLAGS2'))
        DEST_label = QLabel(self.tr('DEST'))
        HIPRESS_label = QLabel(self.tr('HIPRESS'))
        LOPRESS_label = QLabel(self.tr('LOPRESS'))
        HITEMP_label = QLabel(self.tr('HITEMP'))
        LOTEMP_label = QLabel(self.tr('LOTEMP'))
        HIHUMID_label = QLabel(self.tr('HIHUMID'))
        LOHUMID_label = QLabel(self.tr('LOHUMID'))

        PDOP_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        FIX_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL1_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL2_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL3_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL4_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL5_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL6_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL7_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        USERVAL8_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        MOFIELDS_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        FLAGS1_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        FLAGS2_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        DEST_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HIPRESS_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOPRESS_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HITEMP_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOTEMP_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HIHUMID_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOHUMID_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)

        GEOFNUM_label = QLabel(self.tr('GEOFNUM'))
        GEOF1LAT_label = QLabel(self.tr('GEOF1LAT'))
        GEOF1LON_label = QLabel(self.tr('GEOF1LON'))
        GEOF1RAD_label = QLabel(self.tr('GEOF1RAD'))
        GEOF2LAT_label = QLabel(self.tr('GEOF2LAT'))
        GEOF2LON_label = QLabel(self.tr('GEOF2LON'))
        GEOF2RAD_label = QLabel(self.tr('GEOF2RAD'))
        GEOF3LAT_label = QLabel(self.tr('GEOF3LAT'))
        GEOF3LON_label = QLabel(self.tr('GEOF3LON'))
        GEOF3RAD_label = QLabel(self.tr('GEOF3RAD'))
        GEOF4LAT_label = QLabel(self.tr('GEOF4LAT'))
        GEOF4LON_label = QLabel(self.tr('GEOF4LON'))
        GEOF4RAD_label = QLabel(self.tr('GEOF4RAD'))
        WAKEINT_label = QLabel(self.tr('WAKEINT'))
        ALARMINT_label = QLabel(self.tr('ALARMINT'))
        TXINT_label = QLabel(self.tr('TXINT'))
        LOWBATT_label = QLabel(self.tr('LOWBATT'))
        DYNMODEL_label = QLabel(self.tr('DYNMODEL'))

        GEOFNUM_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF1LAT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF1LON_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF1RAD_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF2LAT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF2LON_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF2RAD_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF3LAT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF3LON_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF3RAD_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF4LAT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF4LON_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF4RAD_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        WAKEINT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        ALARMINT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        TXINT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOWBATT_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        DYNMODEL_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        
        # MOFIELDS CheckBoxes

        self.checkbox_SWVER = QCheckBox()
        self.checkbox_SOURCE = QCheckBox()
        self.checkbox_BATTV = QCheckBox()
        self.checkbox_PRESS = QCheckBox()
        self.checkbox_TEMP = QCheckBox()
        self.checkbox_HUMID = QCheckBox()
        self.checkbox_YEAR = QCheckBox()
        self.checkbox_MONTH = QCheckBox()
        self.checkbox_DAY = QCheckBox()
        self.checkbox_HOUR = QCheckBox()
        self.checkbox_MIN = QCheckBox()
        self.checkbox_SEC = QCheckBox()
        self.checkbox_MILLIS = QCheckBox()
        self.checkbox_DATETIME = QCheckBox()
        self.checkbox_LAT = QCheckBox()
        self.checkbox_LON = QCheckBox()
        self.checkbox_ALT = QCheckBox()
        self.checkbox_SPEED = QCheckBox()
        self.checkbox_HEAD = QCheckBox()
        self.checkbox_SATS = QCheckBox()
       
        self.checkbox_PDOP = QCheckBox()
        self.checkbox_FIX = QCheckBox()
        self.checkbox_USERVAL1 = QCheckBox()
        self.checkbox_USERVAL2 = QCheckBox()
        self.checkbox_USERVAL3 = QCheckBox()
        self.checkbox_USERVAL4 = QCheckBox()
        self.checkbox_USERVAL5 = QCheckBox()
        self.checkbox_USERVAL6 = QCheckBox()
        self.checkbox_USERVAL7 = QCheckBox()
        self.checkbox_USERVAL8 = QCheckBox()
        self.checkbox_MOFIELDS = QCheckBox()
        self.checkbox_FLAGS1 = QCheckBox()
        self.checkbox_FLAGS2 = QCheckBox()
        self.checkbox_DEST = QCheckBox()
        self.checkbox_HIPRESS = QCheckBox()
        self.checkbox_LOPRESS = QCheckBox()
        self.checkbox_HITEMP = QCheckBox()
        self.checkbox_LOTEMP = QCheckBox()
        self.checkbox_HIHUMID = QCheckBox()
        self.checkbox_LOHUMID = QCheckBox()
        
        self.checkbox_GEOFNUM = QCheckBox()
        self.checkbox_GEOF1LAT = QCheckBox()
        self.checkbox_GEOF1LON = QCheckBox()
        self.checkbox_GEOF1RAD = QCheckBox()
        self.checkbox_GEOF2LAT = QCheckBox()
        self.checkbox_GEOF2LON = QCheckBox()
        self.checkbox_GEOF2RAD = QCheckBox()
        self.checkbox_GEOF3LAT = QCheckBox()
        self.checkbox_GEOF3LON = QCheckBox()
        self.checkbox_GEOF3RAD = QCheckBox()
        self.checkbox_GEOF4LAT = QCheckBox()
        self.checkbox_GEOF4LON = QCheckBox()
        self.checkbox_GEOF4RAD = QCheckBox()
        self.checkbox_WAKEINT = QCheckBox()
        self.checkbox_ALARMINT = QCheckBox()
        self.checkbox_TXINT = QCheckBox()
        self.checkbox_LOWBATT = QCheckBox()
        self.checkbox_DYNMODEL = QCheckBox()
        
        # Field Value Labels

        Values_label = QLabel(self.tr('Field Values:'))
        Include_label = QLabel(self.tr('Include'))
        FLAGS1_val_label = QLabel(self.tr('FLAGS1'))
        FLAGS2_val_label = QLabel(self.tr('FLAGS2'))
        MOFIELDS_val_label = QLabel(self.tr('MOFIELDS'))
        SOURCE_val_label = QLabel(self.tr('SOURCE'))
        DEST_val_label = QLabel(self.tr('DEST'))
        HIPRESS_val_label = QLabel(self.tr('HIPRESS'))
        LOPRESS_val_label = QLabel(self.tr('LOPRESS'))
        HITEMP_val_label = QLabel(self.tr('HITEMP'))
        LOTEMP_val_label = QLabel(self.tr('LOTEMP'))
        HIHUMID_val_label = QLabel(self.tr('HIHUMID'))
        LOHUMID_val_label = QLabel(self.tr('LOHUMID'))
        GEOFNUM_val_label = QLabel(self.tr('GEOFNUM'))
        GEOF1LAT_val_label = QLabel(self.tr('GEOF1LAT'))
        GEOF1LON_val_label = QLabel(self.tr('GEOF1LON'))
        GEOF1RAD_val_label = QLabel(self.tr('GEOF1RAD'))
        GEOF2LAT_val_label = QLabel(self.tr('GEOF2LAT'))
        GEOF2LON_val_label = QLabel(self.tr('GEOF2LON'))
        GEOF2RAD_val_label = QLabel(self.tr('GEOF2RAD'))
        GEOF3LAT_val_label = QLabel(self.tr('GEOF3LAT'))
        GEOF3LON_val_label = QLabel(self.tr('GEOF3LON'))
        GEOF3RAD_val_label = QLabel(self.tr('GEOF3RAD'))
        GEOF4LAT_val_label = QLabel(self.tr('GEOF4LAT'))
        GEOF4LON_val_label = QLabel(self.tr('GEOF4LON'))
        GEOF4RAD_val_label = QLabel(self.tr('GEOF4RAD'))
        WAKEINT_val_label = QLabel(self.tr('WAKEINT'))
        ALARMINT_val_label = QLabel(self.tr('ALARMINT'))
        TXINT_val_label = QLabel(self.tr('TXINT'))
        LOWBATT_val_label = QLabel(self.tr('LOWBATT'))
        DYNMODEL_val_label = QLabel(self.tr('DYNMODEL'))

        Values_label.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        Include_label.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        FLAGS1_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        FLAGS2_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        MOFIELDS_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        SOURCE_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        DEST_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HIPRESS_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOPRESS_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HITEMP_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOTEMP_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        HIHUMID_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOHUMID_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOFNUM_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF1LAT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF1LON_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF1RAD_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF2LAT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF2LON_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF2RAD_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF3LAT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF3LON_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF3RAD_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF4LAT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF4LON_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        GEOF4RAD_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        WAKEINT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        ALARMINT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        TXINT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        LOWBATT_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        DYNMODEL_val_label.setAlignment(Qt.AlignRight | Qt.AlignVCenter)

        # Field Value Checkboxes

        self.checkbox_val_FLAGS1 = QCheckBox()
        self.checkbox_val_FLAGS2 = QCheckBox()
        self.checkbox_val_MOFIELDS = QCheckBox()
        self.checkbox_val_SOURCE = QCheckBox()
        self.checkbox_val_DEST = QCheckBox()
        self.checkbox_val_HIPRESS = QCheckBox()
        self.checkbox_val_LOPRESS = QCheckBox()
        self.checkbox_val_HITEMP = QCheckBox()
        self.checkbox_val_LOTEMP = QCheckBox()
        self.checkbox_val_HIHUMID = QCheckBox()
        self.checkbox_val_LOHUMID = QCheckBox()      
        self.checkbox_val_GEOFNUM = QCheckBox()
        self.checkbox_val_GEOF1LAT = QCheckBox()
        self.checkbox_val_GEOF1LON = QCheckBox()
        self.checkbox_val_GEOF1RAD = QCheckBox()
        self.checkbox_val_GEOF2LAT = QCheckBox()
        self.checkbox_val_GEOF2LON = QCheckBox()
        self.checkbox_val_GEOF2RAD = QCheckBox()
        self.checkbox_val_GEOF3LAT = QCheckBox()
        self.checkbox_val_GEOF3LON = QCheckBox()
        self.checkbox_val_GEOF3RAD = QCheckBox()
        self.checkbox_val_GEOF4LAT = QCheckBox()
        self.checkbox_val_GEOF4LON = QCheckBox()
        self.checkbox_val_GEOF4RAD = QCheckBox()
        self.checkbox_val_WAKEINT = QCheckBox()
        self.checkbox_val_ALARMINT = QCheckBox()
        self.checkbox_val_TXINT = QCheckBox()
        self.checkbox_val_LOWBATT = QCheckBox()
        self.checkbox_val_DYNMODEL = QCheckBox()

        # Field Value Values

        self.val_SOURCE = QLineEdit()
        self.val_DEST = QLineEdit()
        self.val_HIPRESS = QLineEdit()
        self.val_LOPRESS = QLineEdit()
        self.val_HITEMP = QLineEdit()
        self.val_LOTEMP = QLineEdit()
        self.val_HIHUMID = QLineEdit()
        self.val_LOHUMID = QLineEdit()      
        self.val_GEOFNUM = QLineEdit()
        self.val_GEOF1LAT = QLineEdit()
        self.val_GEOF1LON = QLineEdit()
        self.val_GEOF1RAD = QLineEdit()
        self.val_GEOF2LAT = QLineEdit()
        self.val_GEOF2LON = QLineEdit()
        self.val_GEOF2RAD = QLineEdit()
        self.val_GEOF3LAT = QLineEdit()
        self.val_GEOF3LON = QLineEdit()
        self.val_GEOF3RAD = QLineEdit()
        self.val_GEOF4LAT = QLineEdit()
        self.val_GEOF4LON = QLineEdit()
        self.val_GEOF4RAD = QLineEdit()
        self.val_WAKEINT = QLineEdit()
        self.val_ALARMINT = QLineEdit()
        self.val_TXINT = QLineEdit()
        self.val_LOWBATT = QLineEdit()
        self.val_DYNMODEL = QLineEdit()

        # Field Units
        
        FLAGS1_units = QLabel(self.tr(''))
        FLAGS2_units = QLabel(self.tr(''))
        MOFIELDS_units = QLabel(self.tr(''))
        SOURCE_units = QLabel(self.tr('(Serial Only)'))
        DEST_units = QLabel(self.tr(''))
        HIPRESS_units = QLabel(self.tr('mbar'))
        LOPRESS_units = QLabel(self.tr('mbar'))
        HITEMP_units = QLabel(self.tr('C'))
        LOTEMP_units = QLabel(self.tr('C'))
        HIHUMID_units = QLabel(self.tr('%RH'))
        LOHUMID_units = QLabel(self.tr('%RH'))
        GEOFNUM_units = QLabel(self.tr('Num+Conf: 00 to 44'))
        GEOF1LAT_units = QLabel(self.tr('Degrees'))
        GEOF1LON_units = QLabel(self.tr('Degrees'))
        GEOF1RAD_units = QLabel(self.tr('m'))
        GEOF2LAT_units = QLabel(self.tr('Degrees'))
        GEOF2LON_units = QLabel(self.tr('Degrees'))
        GEOF2RAD_units = QLabel(self.tr('m'))
        GEOF3LAT_units = QLabel(self.tr('Degrees'))
        GEOF3LON_units = QLabel(self.tr('Degrees'))
        GEOF3RAD_units = QLabel(self.tr('m'))
        GEOF4LAT_units = QLabel(self.tr('Degrees'))
        GEOF4LON_units = QLabel(self.tr('Degrees'))
        GEOF4RAD_units = QLabel(self.tr('m'))
        WAKEINT_units = QLabel(self.tr('seconds'))
        ALARMINT_units = QLabel(self.tr('minutes'))
        TXINT_units = QLabel(self.tr('minutes'))
        LOWBATT_units = QLabel(self.tr('V'))
        DYNMODEL_units = QLabel(self.tr('0,2-10'))
        
        FLAGS1_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        FLAGS2_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        MOFIELDS_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        SOURCE_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        DEST_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        HIPRESS_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        LOPRESS_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        HITEMP_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        LOTEMP_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        HIHUMID_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        LOHUMID_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOFNUM_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF1LAT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF1LON_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF1RAD_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF2LAT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF2LON_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF2RAD_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF3LAT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF3LON_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF3RAD_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF4LAT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF4LON_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        GEOF4RAD_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        WAKEINT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        ALARMINT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        TXINT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        LOWBATT_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        DYNMODEL_units.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        
        # Arrange Layout
        
        layout = QGridLayout()
        
        layout.addWidget(self.msg_label, 0, 0)
        layout.addWidget(self.fileLocation_lineedit, 0, 1)
        layout.addWidget(self.browse_btn, 0, 2)

        layout.addWidget(self.load_config_btn, 1, 2)

        layout.addWidget(self.save_config_btn, 2, 2)

        layout.addWidget(self.calc_config_btn, 3, 2)

        layout.addWidget(self.port_label, 5, 0)
        layout.addWidget(self.port_combobox, 5, 1)
        layout.addWidget(self.refresh_btn, 5, 2)

        layout.addWidget(self.open_port_btn, 6, 2)

        layout.addWidget(self.upload_btn, 7, 2)

        layout.addWidget(self.terminal_label, 8, 0)

        layout.addWidget(self.terminal, 9, 0, 9, 3)

        layout.addWidget(self.messages_label, 18, 0)

        layout.addWidget(self.messages, 19, 0, 5, 3)

        layout.addWidget(self.config_label, 24, 0)

        layout.addWidget(self.config, 25, 0, 5, 3)

        layout.addWidget(FLAGS1_header, 0, 4)
        layout.addWidget(label_BINARY, 1, 4)
        layout.addWidget(label_DEST, 2, 4)
        layout.addWidget(label_HIPRESS, 3, 4)
        layout.addWidget(label_LOPRESS, 4, 4)
        layout.addWidget(label_HITEMP, 5, 4)
        layout.addWidget(label_LOTEMP, 6, 4)
        layout.addWidget(label_HIHUMID, 7, 4)
        layout.addWidget(label_LOHUMID, 8, 4)

        layout.addWidget(self.checkbox_F1_BINARY, 1, 5)
        layout.addWidget(self.checkbox_F1_DEST, 2, 5)
        layout.addWidget(self.checkbox_F1_HIPRESS, 3, 5)
        layout.addWidget(self.checkbox_F1_LOPRESS, 4, 5)
        layout.addWidget(self.checkbox_F1_HITEMP, 5, 5)
        layout.addWidget(self.checkbox_F1_LOTEMP, 6, 5)
        layout.addWidget(self.checkbox_F1_HIHUMID, 7, 5)
        layout.addWidget(self.checkbox_F1_LOHUMID, 8, 5)
        
        layout.addWidget(FLAGS2_header, 0, 6)
        layout.addWidget(label_GEOFENCE, 1, 6)
        layout.addWidget(label_INSIDE, 2, 6)
        layout.addWidget(label_LOWBATT, 3, 6)
        layout.addWidget(label_RING, 4, 6)

        layout.addWidget(self.checkbox_F2_GEOFENCE, 1, 7)
        layout.addWidget(self.checkbox_F2_INSIDE, 2, 7)
        layout.addWidget(self.checkbox_F2_LOWBATT, 3, 7)
        layout.addWidget(self.checkbox_F2_RING, 4, 7)

        layout.addWidget(USERFUNC_header, 0, 8)
        layout.addWidget(label_USERFUNC1, 1, 8)
        layout.addWidget(label_USERFUNC2, 2, 8)
        layout.addWidget(label_USERFUNC3, 3, 8)
        layout.addWidget(label_USERFUNC4, 4, 8)
        layout.addWidget(label_USERFUNC5, 5, 8)
        layout.addWidget(label_USERFUNC6, 6, 8)
        layout.addWidget(label_USERFUNC7, 7, 8)
        layout.addWidget(label_USERFUNC8, 8, 8)

        layout.addWidget(self.checkbox_USERFUNC1, 1, 9)
        layout.addWidget(self.checkbox_USERFUNC2, 2, 9)
        layout.addWidget(self.checkbox_USERFUNC3, 3, 9)
        layout.addWidget(self.checkbox_USERFUNC4, 4, 9)
        layout.addWidget(self.checkbox_USERFUNC5, 5, 9)
        layout.addWidget(self.checkbox_USERFUNC6, 6, 9)
        layout.addWidget(self.checkbox_USERFUNC7, 7, 9)
        layout.addWidget(self.checkbox_USERFUNC8, 8, 9)

        layout.addWidget(self.USERFUNC5_val, 5,10)
        layout.addWidget(self.USERFUNC6_val, 6,10)
        layout.addWidget(self.USERFUNC7_val, 7,10)
        layout.addWidget(self.USERFUNC8_val, 8,10)

        layout.addWidget(MOFIELDS_header, 9, 6)

        layout.addWidget(SWVER_label, 10, 4)
        layout.addWidget(SOURCE_label, 11, 4)
        layout.addWidget(BATTV_label, 12, 4)
        layout.addWidget(PRESS_label, 13, 4)
        layout.addWidget(TEMP_label, 14, 4)
        layout.addWidget(HUMID_label, 15, 4)
        layout.addWidget(YEAR_label, 16, 4)
        layout.addWidget(MONTH_label, 17, 4)
        layout.addWidget(DAY_label, 18, 4)
        layout.addWidget(HOUR_label, 19, 4)
        layout.addWidget(MIN_label, 20, 4)
        layout.addWidget(SEC_label, 21, 4)
        layout.addWidget(MILLIS_label, 22, 4)
        layout.addWidget(DATETIME_label, 23, 4)
        layout.addWidget(LAT_label, 24, 4)
        layout.addWidget(LON_label, 25, 4)
        layout.addWidget(ALT_label, 26, 4)
        layout.addWidget(SPEED_label, 27, 4)
        layout.addWidget(HEAD_label, 28, 4)
        layout.addWidget(SATS_label, 29, 4)

        layout.addWidget(self.checkbox_SWVER, 10, 5)
        layout.addWidget(self.checkbox_SOURCE, 11, 5)
        layout.addWidget(self.checkbox_BATTV, 12, 5)
        layout.addWidget(self.checkbox_PRESS, 13, 5)
        layout.addWidget(self.checkbox_TEMP, 14, 5)
        layout.addWidget(self.checkbox_HUMID, 15, 5)
        layout.addWidget(self.checkbox_YEAR, 16, 5)
        layout.addWidget(self.checkbox_MONTH,17 , 5)
        layout.addWidget(self.checkbox_DAY, 18, 5)
        layout.addWidget(self.checkbox_HOUR, 19, 5)
        layout.addWidget(self.checkbox_MIN, 20, 5)
        layout.addWidget(self.checkbox_SEC, 21, 5)
        layout.addWidget(self.checkbox_MILLIS, 22, 5)
        layout.addWidget(self.checkbox_DATETIME, 23, 5)
        layout.addWidget(self.checkbox_LAT, 24, 5)
        layout.addWidget(self.checkbox_LON, 25, 5)
        layout.addWidget(self.checkbox_ALT, 26, 5)
        layout.addWidget(self.checkbox_SPEED, 27, 5)
        layout.addWidget(self.checkbox_HEAD, 28, 5)
        layout.addWidget(self.checkbox_SATS, 29, 5)

        layout.addWidget(PDOP_label, 10, 6)
        layout.addWidget(FIX_label, 11, 6)
        layout.addWidget(USERVAL1_label, 12, 6)
        layout.addWidget(USERVAL2_label, 13, 6)
        layout.addWidget(USERVAL3_label, 14, 6)
        layout.addWidget(USERVAL4_label, 15, 6)
        layout.addWidget(USERVAL5_label, 16, 6)
        layout.addWidget(USERVAL6_label, 17, 6)
        layout.addWidget(USERVAL7_label, 18, 6)
        layout.addWidget(USERVAL8_label, 19, 6)
        layout.addWidget(MOFIELDS_label, 20, 6)
        layout.addWidget(FLAGS1_label, 21, 6)
        layout.addWidget(FLAGS2_label, 22, 6)
        layout.addWidget(DEST_label, 23, 6)
        layout.addWidget(HIPRESS_label, 24, 6)
        layout.addWidget(LOPRESS_label, 25, 6)
        layout.addWidget(HITEMP_label, 26, 6)
        layout.addWidget(LOTEMP_label, 27, 6)
        layout.addWidget(HIHUMID_label, 28, 6)
        layout.addWidget(LOHUMID_label, 29, 6)

        layout.addWidget(self.checkbox_PDOP, 10, 7)
        layout.addWidget(self.checkbox_FIX, 11, 7)
        layout.addWidget(self.checkbox_USERVAL1, 12, 7)
        layout.addWidget(self.checkbox_USERVAL2, 13, 7)
        layout.addWidget(self.checkbox_USERVAL3, 14, 7)
        layout.addWidget(self.checkbox_USERVAL4, 15, 7)
        layout.addWidget(self.checkbox_USERVAL5, 16, 7)
        layout.addWidget(self.checkbox_USERVAL6, 17, 7)
        layout.addWidget(self.checkbox_USERVAL7, 18, 7)
        layout.addWidget(self.checkbox_USERVAL8, 19, 7)
        layout.addWidget(self.checkbox_MOFIELDS, 20, 7)
        layout.addWidget(self.checkbox_FLAGS1, 21, 7)
        layout.addWidget(self.checkbox_FLAGS2, 22, 7)
        layout.addWidget(self.checkbox_DEST, 23, 7)
        layout.addWidget(self.checkbox_HIPRESS, 24, 7)
        layout.addWidget(self.checkbox_LOPRESS, 25, 7)
        layout.addWidget(self.checkbox_HITEMP, 26, 7)
        layout.addWidget(self.checkbox_LOTEMP, 27, 7)
        layout.addWidget(self.checkbox_HIHUMID, 28, 7)
        layout.addWidget(self.checkbox_LOHUMID, 29, 7)
        
        layout.addWidget(GEOFNUM_label, 10, 8)
        layout.addWidget(GEOF1LAT_label, 11, 8)
        layout.addWidget(GEOF1LON_label, 12, 8)
        layout.addWidget(GEOF1RAD_label, 13, 8)
        layout.addWidget(GEOF2LAT_label, 14, 8)
        layout.addWidget(GEOF2LON_label, 15, 8)
        layout.addWidget(GEOF2RAD_label, 16, 8)
        layout.addWidget(GEOF3LAT_label, 17, 8)
        layout.addWidget(GEOF3LON_label, 18, 8)
        layout.addWidget(GEOF3RAD_label, 19, 8)
        layout.addWidget(GEOF4LAT_label, 20, 8)
        layout.addWidget(GEOF4LON_label, 21, 8)
        layout.addWidget(GEOF4RAD_label, 22, 8)
        layout.addWidget(WAKEINT_label, 23, 8)
        layout.addWidget(ALARMINT_label, 24, 8)
        layout.addWidget(TXINT_label, 25, 8)
        layout.addWidget(LOWBATT_label, 26, 8)
        layout.addWidget(DYNMODEL_label, 27, 8)

        layout.addWidget(self.checkbox_GEOFNUM, 10, 9)
        layout.addWidget(self.checkbox_GEOF1LAT, 11, 9)
        layout.addWidget(self.checkbox_GEOF1LON, 12, 9)
        layout.addWidget(self.checkbox_GEOF1RAD, 13, 9)
        layout.addWidget(self.checkbox_GEOF2LAT, 14, 9)
        layout.addWidget(self.checkbox_GEOF2LON, 15, 9)
        layout.addWidget(self.checkbox_GEOF2RAD, 16, 9)
        layout.addWidget(self.checkbox_GEOF3LAT, 17, 9)
        layout.addWidget(self.checkbox_GEOF3LON, 18, 9)
        layout.addWidget(self.checkbox_GEOF3RAD, 19, 9)
        layout.addWidget(self.checkbox_GEOF4LAT, 20, 9)
        layout.addWidget(self.checkbox_GEOF4LON, 21, 9)
        layout.addWidget(self.checkbox_GEOF4RAD, 22, 9)
        layout.addWidget(self.checkbox_WAKEINT, 23, 9)
        layout.addWidget(self.checkbox_ALARMINT, 24, 9)
        layout.addWidget(self.checkbox_TXINT, 25, 9)
        layout.addWidget(self.checkbox_LOWBATT, 26, 9)
        layout.addWidget(self.checkbox_DYNMODEL, 27, 9)
                         
        layout.addWidget(Values_label, 0, 11)
        layout.addWidget(FLAGS1_val_label, 1, 11)
        layout.addWidget(FLAGS2_val_label, 2, 11)
        layout.addWidget(MOFIELDS_val_label, 3, 11)
        layout.addWidget(SOURCE_val_label, 4, 11)
        layout.addWidget(DEST_val_label, 5, 11)
        layout.addWidget(HIPRESS_val_label, 6, 11)
        layout.addWidget(LOPRESS_val_label, 7, 11)
        layout.addWidget(HITEMP_val_label, 8, 11)
        layout.addWidget(LOTEMP_val_label, 9, 11)
        layout.addWidget(HIHUMID_val_label, 10, 11)
        layout.addWidget(LOHUMID_val_label, 11, 11)
        layout.addWidget(GEOFNUM_val_label, 12, 11)
        layout.addWidget(GEOF1LAT_val_label, 13, 11)
        layout.addWidget(GEOF1LON_val_label, 14, 11)
        layout.addWidget(GEOF1RAD_val_label, 15, 11)
        layout.addWidget(GEOF2LAT_val_label, 16, 11)
        layout.addWidget(GEOF2LON_val_label, 17, 11)
        layout.addWidget(GEOF2RAD_val_label, 18, 11)
        layout.addWidget(GEOF3LAT_val_label, 19, 11)
        layout.addWidget(GEOF3LON_val_label, 20, 11)
        layout.addWidget(GEOF3RAD_val_label, 21, 11)
        layout.addWidget(GEOF4LAT_val_label, 22, 11)
        layout.addWidget(GEOF4LON_val_label, 23, 11)
        layout.addWidget(GEOF4RAD_val_label, 24, 11)
        layout.addWidget(WAKEINT_val_label, 25, 11)
        layout.addWidget(ALARMINT_val_label, 26, 11)
        layout.addWidget(TXINT_val_label, 27, 11)
        layout.addWidget(LOWBATT_val_label, 28, 11)
        layout.addWidget(DYNMODEL_val_label, 29, 11)

        layout.addWidget(Include_label, 0, 12)
        layout.addWidget(self.checkbox_val_FLAGS1, 1, 12)
        layout.addWidget(self.checkbox_val_FLAGS2, 2, 12)
        layout.addWidget(self.checkbox_val_MOFIELDS, 3, 12)
        layout.addWidget(self.checkbox_val_SOURCE, 4, 12)
        layout.addWidget(self.checkbox_val_DEST, 5, 12)
        layout.addWidget(self.checkbox_val_HIPRESS, 6, 12)
        layout.addWidget(self.checkbox_val_LOPRESS, 7, 12)
        layout.addWidget(self.checkbox_val_HITEMP, 8, 12)
        layout.addWidget(self.checkbox_val_LOTEMP, 9, 12)
        layout.addWidget(self.checkbox_val_HIHUMID, 10, 12)
        layout.addWidget(self.checkbox_val_LOHUMID, 11, 12)
        layout.addWidget(self.checkbox_val_GEOFNUM, 12, 12)
        layout.addWidget(self.checkbox_val_GEOF1LAT, 13, 12)
        layout.addWidget(self.checkbox_val_GEOF1LON, 14, 12)
        layout.addWidget(self.checkbox_val_GEOF1RAD, 15, 12)
        layout.addWidget(self.checkbox_val_GEOF2LAT, 16, 12)
        layout.addWidget(self.checkbox_val_GEOF2LON, 17, 12)
        layout.addWidget(self.checkbox_val_GEOF2RAD, 18, 12)
        layout.addWidget(self.checkbox_val_GEOF3LAT, 19, 12)
        layout.addWidget(self.checkbox_val_GEOF3LON, 20, 12)
        layout.addWidget(self.checkbox_val_GEOF3RAD, 21, 12)
        layout.addWidget(self.checkbox_val_GEOF4LAT, 22, 12)
        layout.addWidget(self.checkbox_val_GEOF4LON, 23, 12)
        layout.addWidget(self.checkbox_val_GEOF4RAD, 24, 12)
        layout.addWidget(self.checkbox_val_WAKEINT, 25, 12)
        layout.addWidget(self.checkbox_val_ALARMINT, 26, 12)
        layout.addWidget(self.checkbox_val_TXINT, 27, 12)
        layout.addWidget(self.checkbox_val_LOWBATT, 28, 12)
        layout.addWidget(self.checkbox_val_DYNMODEL, 29, 12)

        layout.addWidget(self.val_SOURCE, 4, 13)
        layout.addWidget(self.val_DEST, 5, 13)
        layout.addWidget(self.val_HIPRESS, 6, 13)
        layout.addWidget(self.val_LOPRESS, 7, 13)
        layout.addWidget(self.val_HITEMP, 8, 13)
        layout.addWidget(self.val_LOTEMP, 9, 13)
        layout.addWidget(self.val_HIHUMID, 10, 13)
        layout.addWidget(self.val_LOHUMID, 11, 13)
        layout.addWidget(self.val_GEOFNUM, 12, 13)
        layout.addWidget(self.val_GEOF1LAT, 13, 13)
        layout.addWidget(self.val_GEOF1LON, 14, 13)
        layout.addWidget(self.val_GEOF1RAD, 15, 13)
        layout.addWidget(self.val_GEOF2LAT, 16, 13)
        layout.addWidget(self.val_GEOF2LON, 17, 13)
        layout.addWidget(self.val_GEOF2RAD, 18, 13)
        layout.addWidget(self.val_GEOF3LAT, 19, 13)
        layout.addWidget(self.val_GEOF3LON, 20, 13)
        layout.addWidget(self.val_GEOF3RAD, 21, 13)
        layout.addWidget(self.val_GEOF4LAT, 22, 13)
        layout.addWidget(self.val_GEOF4LON, 23, 13)
        layout.addWidget(self.val_GEOF4RAD, 24, 13)
        layout.addWidget(self.val_WAKEINT, 25, 13)
        layout.addWidget(self.val_ALARMINT, 26, 13)
        layout.addWidget(self.val_TXINT, 27, 13)
        layout.addWidget(self.val_LOWBATT, 28, 13)
        layout.addWidget(self.val_DYNMODEL, 29, 13)

        layout.addWidget(FLAGS1_units, 1, 14)
        layout.addWidget(FLAGS2_units, 2, 14)
        layout.addWidget(MOFIELDS_units, 3, 14)
        layout.addWidget(SOURCE_units, 4, 14)
        layout.addWidget(DEST_units, 5, 14)
        layout.addWidget(HIPRESS_units, 6, 14)
        layout.addWidget(LOPRESS_units, 7, 14)
        layout.addWidget(HITEMP_units, 8, 14)
        layout.addWidget(LOTEMP_units, 9, 14)
        layout.addWidget(HIHUMID_units, 10, 14)
        layout.addWidget(LOHUMID_units, 11, 14)
        layout.addWidget(GEOFNUM_units, 12, 14)
        layout.addWidget(GEOF1LAT_units, 13, 14)
        layout.addWidget(GEOF1LON_units, 14, 14)
        layout.addWidget(GEOF1RAD_units, 15, 14)
        layout.addWidget(GEOF2LAT_units, 16, 14)
        layout.addWidget(GEOF2LON_units, 17, 14)
        layout.addWidget(GEOF2RAD_units, 18, 14)
        layout.addWidget(GEOF3LAT_units, 19, 14)
        layout.addWidget(GEOF3LON_units, 20, 14)
        layout.addWidget(GEOF3RAD_units, 21, 14)
        layout.addWidget(GEOF4LAT_units, 22, 14)
        layout.addWidget(GEOF4LON_units, 23, 14)
        layout.addWidget(GEOF4RAD_units, 24, 14)
        layout.addWidget(WAKEINT_units, 25, 14)
        layout.addWidget(ALARMINT_units, 26, 14)
        layout.addWidget(TXINT_units, 27, 14)
        layout.addWidget(LOWBATT_units, 28, 14)
        layout.addWidget(DYNMODEL_units, 29, 14)

        self.setLayout(layout)

        self.load_settings()

        # Make the text edit windows read-only
        self.terminal.setReadOnly(True)
        self.messages.setReadOnly(True)
        self.config.setReadOnly(True)

        # Set up a timer to display the incoming serial data
        self.timer = QTimer()
        self.timer.setInterval(250)
        self.timer.timeout.connect(self.recurring_timer)
        self.timer.start()

    def recurring_timer(self) -> None:
        """Timer to handle serial data receive"""
        try:
            if self.ser.isOpen():
                if self.ser.inWaiting() > 0:
                    # Although the text edit window is read-only, the user can still
                    # click in it and move the (hidden) cursor. New text will be added
                    # at the cursor position. To prevent this, we need to move the
                    # cursor to the end of the window first. The ensureCursorVisible
                    # will make the window scroll automatically so the latest text
                    # is visible.
                    self.terminal.moveCursor(QTextCursor.End)
                    self.terminal.ensureCursorVisible()
                    self.terminal.insertPlainText(str(self.ser.read(self.ser.inWaiting()),'utf-8'))
                    self.terminal.ensureCursorVisible()
        except:
            pass

    def load_settings(self) -> None:
        """Load settings on startup."""
        self.settings = QSettings()
        
        port_name = self.settings.value(SETTING_PORT_NAME)
        if port_name is not None:
            index = self.port_combobox.findData(port_name)
            if index > -1:
                self.port_combobox.setCurrentIndex(index)

        msg = self.settings.value(SETTING_FILE_LOCATION)
        if msg is not None:
            self.fileLocation_lineedit.setText(msg)

    def save_settings(self) -> None:
        """Save settings on shutdown."""
        self.settings = QSettings()
        self.settings.setValue(SETTING_PORT_NAME, self.port)
        self.settings.setValue(SETTING_FILE_LOCATION,
                          self.fileLocation_lineedit.text())

    def show_error_message(self, msg: str) -> None:
        """Show a Message Box with the error message."""
        QMessageBox.critical(self, QApplication.applicationName(), str(msg))

    def update_com_ports(self) -> None:
        """Update COM Port list in GUI."""
        self.port_combobox.clear()
        for name, device in gen_serial_ports():
            self.port_combobox.addItem(name, device)

    @property
    def port(self) -> str:
        """Return the current serial port."""
        return self.port_combobox.currentData()

    def closeEvent(self, event: QCloseEvent) -> None:
        """Handle Close event of the Widget."""
        self.timer.stop()

        try:
            self.save_settings()
        except:
            pass

        try:
            if self.ser:
                self.ser.close()
        except:
            pass
        
        event.accept()

    def on_load_config_btn_pressed(self) -> None:
        pass

    def on_save_config_btn_pressed(self) -> None:
        pass

    def on_open_port_btn_pressed(self) -> None:
        """Check if port is available"""
        portAvailable = False
        ports = comports()
        for p in ports:
            if (p.device == self.port):
                portAvailable = True
        if (portAvailable == False):
            self.messages.moveCursor(QTextCursor.End)
            self.messages.ensureCursorVisible()
            self.messages.appendPlainText("Port No Longer Available!")
            self.messages.ensureCursorVisible()
            return

        try:
            if self.ser.isOpen():
                self.messages.moveCursor(QTextCursor.End)
                self.messages.ensureCursorVisible()
                self.messages.appendPlainText("Port Is Already Open!")
                self.messages.ensureCursorVisible()
                return
        except:
            pass
        
        try:
            self.ser = serial.Serial(self.port, 115200, timeout=0.01)
        except:
            self.messages.moveCursor(QTextCursor.End)
            self.messages.ensureCursorVisible()
            self.messages.appendPlainText("Could Not Open The Port!")
            self.messages.ensureCursorVisible()

    def on_calc_config_btn_pressed(self) -> None:
        self.messages.clear() # Clear the message window
        config_str = "02" # Add the STX
        # FLAGS1
        flags = 0
        if self.checkbox_F1_BINARY.isChecked(): flags = flags | 0b10000000
        if self.checkbox_F1_DEST.isChecked(): flags = flags | 0b01000000
        if self.checkbox_F1_HIPRESS.isChecked(): flags = flags | 0b00100000
        if self.checkbox_F1_LOPRESS.isChecked(): flags = flags | 0b00010000
        if self.checkbox_F1_HITEMP.isChecked(): flags = flags | 0b00001000
        if self.checkbox_F1_LOTEMP.isChecked(): flags = flags | 0b00000100
        if self.checkbox_F1_HIHUMID.isChecked(): flags = flags | 0b00000010
        if self.checkbox_F1_LOHUMID.isChecked(): flags = flags | 0b00000001
        if self.checkbox_val_FLAGS1.isChecked():
            config_str = config_str + "31{0:0{1}x}".format(flags, 2)
        elif flags > 0:
            self.messages.moveCursor(QTextCursor.End)
            self.messages.appendPlainText("Warning: FLAGS1 has bits set but Include checkbox is not checked")
            self.messages.ensureCursorVisible()
        # FLAGS2
        flags = 0
        if self.checkbox_F2_GEOFENCE.isChecked(): flags = flags | 0b10000000
        if self.checkbox_F2_INSIDE.isChecked(): flags = flags | 0b01000000
        if self.checkbox_F2_LOWBATT.isChecked(): flags = flags | 0b00100000
        if self.checkbox_F2_RING.isChecked(): flags = flags | 0b00010000
        if self.checkbox_val_FLAGS2.isChecked():
            config_str = config_str + "32{0:0{1}x}".format(flags, 2)
        elif flags > 0:
            self.messages.moveCursor(QTextCursor.End)
            self.messages.appendPlainText("Warning: FLAGS2 has bits set but Include checkbox is not checked")
            self.messages.ensureCursorVisible()
        # MOFIELDS
        flags = 0
        if self.checkbox_SWVER.isChecked():     flags = flags | 0x080000000000000000000000
        if self.checkbox_SOURCE.isChecked():    flags = flags | 0x008000000000000000000000
        if self.checkbox_BATTV.isChecked():     flags = flags | 0x004000000000000000000000
        if self.checkbox_PRESS.isChecked():     flags = flags | 0x002000000000000000000000
        if self.checkbox_TEMP.isChecked():      flags = flags | 0x001000000000000000000000
        if self.checkbox_HUMID.isChecked():     flags = flags | 0x000800000000000000000000
        if self.checkbox_YEAR.isChecked():      flags = flags | 0x000400000000000000000000
        if self.checkbox_MONTH.isChecked():     flags = flags | 0x000200000000000000000000
        if self.checkbox_DAY.isChecked():       flags = flags | 0x000100000000000000000000
        if self.checkbox_HOUR.isChecked():      flags = flags | 0x000080000000000000000000
        if self.checkbox_MIN.isChecked():       flags = flags | 0x000040000000000000000000
        if self.checkbox_SEC.isChecked():       flags = flags | 0x000020000000000000000000
        if self.checkbox_MILLIS.isChecked():    flags = flags | 0x000010000000000000000000
        if self.checkbox_DATETIME.isChecked():  flags = flags | 0x000008000000000000000000
        if self.checkbox_LAT.isChecked():       flags = flags | 0x000004000000000000000000
        if self.checkbox_LON.isChecked():       flags = flags | 0x000002000000000000000000
        if self.checkbox_ALT.isChecked():       flags = flags | 0x000001000000000000000000
        if self.checkbox_SPEED.isChecked():     flags = flags | 0x000000800000000000000000
        if self.checkbox_HEAD.isChecked():      flags = flags | 0x000000400000000000000000
        if self.checkbox_SATS.isChecked():      flags = flags | 0x000000200000000000000000
        if self.checkbox_PDOP.isChecked():      flags = flags | 0x000000100000000000000000
        if self.checkbox_FIX.isChecked():       flags = flags | 0x000000080000000000000000
        if self.checkbox_USERVAL1.isChecked():  flags = flags | 0x000000008000000000000000
        if self.checkbox_USERVAL2.isChecked():  flags = flags | 0x000000004000000000000000
        if self.checkbox_USERVAL3.isChecked():  flags = flags | 0x000000002000000000000000
        if self.checkbox_USERVAL4.isChecked():  flags = flags | 0x000000001000000000000000
        if self.checkbox_USERVAL5.isChecked():  flags = flags | 0x000000000800000000000000
        if self.checkbox_USERVAL6.isChecked():  flags = flags | 0x000000000400000000000000
        if self.checkbox_USERVAL7.isChecked():  flags = flags | 0x000000000200000000000000
        if self.checkbox_USERVAL8.isChecked():  flags = flags | 0x000000000100000000000000
        if self.checkbox_MOFIELDS.isChecked():  flags = flags | 0x000000000000800000000000
        if self.checkbox_FLAGS1.isChecked():    flags = flags | 0x000000000000400000000000
        if self.checkbox_FLAGS2.isChecked():    flags = flags | 0x000000000000200000000000
        if self.checkbox_DEST.isChecked():      flags = flags | 0x000000000000100000000000
        if self.checkbox_HIPRESS.isChecked():   flags = flags | 0x000000000000080000000000
        if self.checkbox_LOPRESS.isChecked():   flags = flags | 0x000000000000040000000000
        if self.checkbox_HITEMP.isChecked():    flags = flags | 0x000000000000020000000000
        if self.checkbox_LOTEMP.isChecked():    flags = flags | 0x000000000000010000000000
        if self.checkbox_HIHUMID.isChecked():   flags = flags | 0x000000000000008000000000
        if self.checkbox_LOHUMID.isChecked():   flags = flags | 0x000000000000004000000000
        if self.checkbox_GEOFNUM.isChecked():   flags = flags | 0x000000000000002000000000
        if self.checkbox_GEOF1LAT.isChecked():  flags = flags | 0x000000000000001000000000
        if self.checkbox_GEOF1LON.isChecked():  flags = flags | 0x000000000000000800000000
        if self.checkbox_GEOF1RAD.isChecked():  flags = flags | 0x000000000000000400000000
        if self.checkbox_GEOF2LAT.isChecked():  flags = flags | 0x000000000000000200000000
        if self.checkbox_GEOF2LON.isChecked():  flags = flags | 0x000000000000000100000000
        if self.checkbox_GEOF2RAD.isChecked():  flags = flags | 0x000000000000000080000000
        if self.checkbox_GEOF3LAT.isChecked():  flags = flags | 0x000000000000000040000000
        if self.checkbox_GEOF3LON.isChecked():  flags = flags | 0x000000000000000020000000
        if self.checkbox_GEOF3RAD.isChecked():  flags = flags | 0x000000000000000010000000
        if self.checkbox_GEOF4LAT.isChecked():  flags = flags | 0x000000000000000008000000
        if self.checkbox_GEOF4LON.isChecked():  flags = flags | 0x000000000000000004000000
        if self.checkbox_GEOF4RAD.isChecked():  flags = flags | 0x000000000000000002000000
        if self.checkbox_WAKEINT.isChecked():   flags = flags | 0x000000000000000001000000
        if self.checkbox_ALARMINT.isChecked():  flags = flags | 0x000000000000000000800000
        if self.checkbox_TXINT.isChecked():     flags = flags | 0x000000000000000000400000
        if self.checkbox_LOWBATT.isChecked():   flags = flags | 0x000000000000000000200000
        if self.checkbox_DYNMODEL.isChecked():  flags = flags | 0x000000000000000000100000
        if self.checkbox_val_MOFIELDS.isChecked():
            config_str = config_str + "30{0:0{1}x}".format(flags, 24)
        elif flags > 0:
            self.messages.moveCursor(QTextCursor.End)
            self.messages.appendPlainText("Warning: MOFIELDS has bits set but Include checkbox is not checked")
            self.messages.ensureCursorVisible()
        # Fields Values
        if self.checkbox_val_SOURCE.isChecked():
            if self.val_SOURCE.text().isdigit():
                try:
                    value = int(self.val_SOURCE.text())
                    if (value < 0) or (value > 9999999):
                        self.messages.appendPlainText("Error: the value for SOURCE is not valid!")
                    else:
                        config_str = config_str + "08" + struct.pack('<I', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for SOURCE is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for SOURCE is not valid!")
        if self.checkbox_val_DEST.isChecked():
            if self.val_DEST.text().isdigit():
                try:
                    value = int(self.val_DEST.text())
                    if (value < 0) or (value > 9999999):
                        self.messages.appendPlainText("Error: the value for DEST is not valid!")
                    else:
                        config_str = config_str + "33" + struct.pack('<I', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for DEST is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for DEST is not valid!")
        if self.checkbox_val_HIPRESS.isChecked():
            if self.val_HIPRESS.text().isdigit():
                try:
                    value = int(self.val_HIPRESS.text())
                    if (value < 0) or (value > 1084):
                        self.messages.appendPlainText("Error: the value for HIPRESS is not valid!")
                    else:
                        config_str = config_str + "34" + struct.pack('<H', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for HIPRESS is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for HIPRESS is not valid!")
        if self.checkbox_val_LOPRESS.isChecked():
            if self.val_LOPRESS.text().isdigit():
                try:
                    value = int(self.val_LOPRESS.text())
                    if (value < 0) or (value > 1084):
                        self.messages.appendPlainText("Error: the value for LOPRESS is not valid!")
                    else:
                        config_str = config_str + "35" + struct.pack('<H', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for LOPRESS is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for LOPRESS is not valid!")
        if self.checkbox_val_HITEMP.isChecked():
            try:
                value = float(self.val_HITEMP.text())
                if (value < -40.0) or (value > 85.0):
                    self.messages.appendPlainText("Error: the value for HITEMP is not valid!")
                else:
                    value = int(value * 100.0)
                    config_str = config_str + "36" + struct.pack('<h', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for HITEMP is not valid!")
        if self.checkbox_val_LOTEMP.isChecked():
            try:
                value = float(self.val_LOTEMP.text())
                if (value < -40.0) or (value > 85.0):
                    self.messages.appendPlainText("Error: the value for LOTEMP is not valid!")
                else:
                    value = int(value * 100.0)
                    config_str = config_str + "37" + struct.pack('<h', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for LOTEMP is not valid!")
        if self.checkbox_val_HIHUMID.isChecked():
            try:
                value = float(self.val_HIHUMID.text())
                if (value < 0.0) or (value > 100.0):
                    self.messages.appendPlainText("Error: the value for HIHUMID is not valid!")
                else:
                    value = int(value * 100.0)
                    config_str = config_str + "38" + struct.pack('<H', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for HIHUMID is not valid!")
        if self.checkbox_val_LOHUMID.isChecked():
            try:
                value = float(self.val_LOHUMID.text())
                if (value < 0.0) or (value > 100.0):
                    self.messages.appendPlainText("Error: the value for LOHUMID is not valid!")
                else:
                    value = int(value * 100.0)
                    config_str = config_str + "39" + struct.pack('<H', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for LOHUMID is not valid!")
        if self.checkbox_val_GEOFNUM.isChecked():
            if self.val_GEOFNUM.text().isdigit():
                try:
                    value = float(self.val_GEOFNUM.text())
                    numf = int(value / 10)
                    conf = int(value % 10)
                    if (numf < 0) or (numf > 4) or (conf < 0) or (conf > 4):
                        self.messages.appendPlainText("Error: the value for GEOFNUM is not valid!")
                    else:
                        value = (numf * 16) + conf
                        config_str = config_str + "3a{0:0{1}x}".format(value, 2)
                except:
                    self.messages.appendPlainText("Error: the value for GEOFNUM is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for GEOFNUM is not valid!")
        if self.checkbox_val_GEOF1LAT.isChecked():
            try:
                value = float(self.val_GEOF1LAT.text())
                if (value < -90.0) or (value >= 90.0):
                    self.messages.appendPlainText("Error: the value for GEOF1LAT is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "3b" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF1LAT is not valid!")
        if self.checkbox_val_GEOF1LON.isChecked():
            try:
                value = float(self.val_GEOF1LON.text())
                if (value < -180.0) or (value >= 180.0):
                    self.messages.appendPlainText("Error: the value for GEOF1LON is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "3c" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF1LON is not valid!")
        if self.checkbox_val_GEOF1RAD.isChecked():
            try:
                value = float(self.val_GEOF1RAD.text())
                if (value < 0.0) or (value > 100000.0):
                    self.messages.appendPlainText("Error: the value for GEOF1RAD is not valid!")
                else:
                    value = int(value * 100.0) # Convert to cm
                    config_str = config_str + "3d" + struct.pack('<I', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF1RAD is not valid!")
        if self.checkbox_val_GEOF2LAT.isChecked():
            try:
                value = float(self.val_GEOF2LAT.text())
                if (value < -90.0) or (value >= 90.0):
                    self.messages.appendPlainText("Error: the value for GEOF2LAT is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "3e" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF2LAT is not valid!")
        if self.checkbox_val_GEOF2LON.isChecked():
            try:
                value = float(self.val_GEOF2LON.text())
                if (value < -180.0) or (value >= 180.0):
                    self.messages.appendPlainText("Error: the value for GEOF2LON is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "3f" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF2LON is not valid!")
        if self.checkbox_val_GEOF2RAD.isChecked():
            try:
                value = float(self.val_GEOF2RAD.text())
                if (value < 0.0) or (value > 100000.0):
                    self.messages.appendPlainText("Error: the value for GEOF2RAD is not valid!")
                else:
                    value = int(value * 100.0) # Convert to cm
                    config_str = config_str + "40" + struct.pack('<I', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF2RAD is not valid!")
        if self.checkbox_val_GEOF3LAT.isChecked():
            try:
                value = float(self.val_GEOF3LAT.text())
                if (value < -90.0) or (value >= 90.0):
                    self.messages.appendPlainText("Error: the value for GEOF3LAT is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "41" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF3LAT is not valid!")
        if self.checkbox_val_GEOF3LON.isChecked():
            try:
                value = float(self.val_GEOF3LON.text())
                if (value < -180.0) or (value >= 180.0):
                    self.messages.appendPlainText("Error: the value for GEOF3LON is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "42" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF3LON is not valid!")
        if self.checkbox_val_GEOF3RAD.isChecked():
            try:
                value = float(self.val_GEOF3RAD.text())
                if (value < 0.0) or (value > 100000.0):
                    self.messages.appendPlainText("Error: the value for GEOF3RAD is not valid!")
                else:
                    value = int(value * 100.0) # Convert to cm
                    config_str = config_str + "43" + struct.pack('<I', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF3RAD is not valid!")
        if self.checkbox_val_GEOF4LAT.isChecked():
            try:
                value = float(self.val_GEOF4LAT.text())
                if (value < -90.0) or (value >= 90.0):
                    self.messages.appendPlainText("Error: the value for GEOF4LAT is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "44" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF4LAT is not valid!")
        if self.checkbox_val_GEOF4LON.isChecked():
            try:
                value = float(self.val_GEOF4LON.text())
                if (value < -180.0) or (value >= 180.0):
                    self.messages.appendPlainText("Error: the value for GEOF4LON is not valid!")
                else:
                    value = int(value * 1E7) # Convert to degrees ^ 10-7
                    config_str = config_str + "45" + struct.pack('<i', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF4LON is not valid!")
        if self.checkbox_val_GEOF4RAD.isChecked():
            try:
                value = float(self.val_GEOF4RAD.text())
                if (value < 0.0) or (value > 100000.0):
                    self.messages.appendPlainText("Error: the value for GEOF4RAD is not valid!")
                else:
                    value = int(value * 100.0) # Convert to cm
                    config_str = config_str + "46" + struct.pack('<I', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for GEOF4RAD is not valid!")
        if self.checkbox_val_WAKEINT.isChecked():
            if self.val_WAKEINT.text().isdigit():
                try:
                    value = int(self.val_WAKEINT.text())
                    if (value < 0) or (value > 3600):
                        self.messages.appendPlainText("Error: the value for WAKEINT is not valid!")
                    else:
                        config_str = config_str + "47" + struct.pack('<H', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for WAKEINT is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for WAKEINT is not valid!")
        if self.checkbox_val_ALARMINT.isChecked():
            if self.val_ALARMINT.text().isdigit():
                try:
                    value = int(self.val_ALARMINT.text())
                    if (value < 0) or (value > 1440):
                        self.messages.appendPlainText("Error: the value for ALARMINT is not valid!")
                    else:
                        config_str = config_str + "48" + struct.pack('<H', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for ALARMINT is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for ALARMINT is not valid!")
        if self.checkbox_val_TXINT.isChecked():
            if self.val_TXINT.text().isdigit():
                try:
                    value = int(self.val_TXINT.text())
                    if (value < 0) or (value > 1440):
                        self.messages.appendPlainText("Error: the value for TXINT is not valid!")
                    else:
                        config_str = config_str + "49" + struct.pack('<H', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for TXINT is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for TXINT is not valid!")
        if self.checkbox_val_LOWBATT.isChecked():
            try:
                value = float(self.val_LOWBATT.text())
                if (value < 0.0) or (value > 9.99):
                    self.messages.appendPlainText("Error: the value for LOWBATT is not valid!")
                else:
                    value = int(value * 100.0)
                    config_str = config_str + "4a" + struct.pack('<H', value).hex() # Little-endian hex
            except:
                self.messages.appendPlainText("Error: the value for LOWBATT is not valid!")
        if self.checkbox_val_DYNMODEL.isChecked():
            if self.val_DYNMODEL.text().isdigit():
                try:
                    value = int(self.val_DYNMODEL.text())
                    if (value < 0) or (value == 1) or (value > 10):
                        self.messages.appendPlainText("Error: the value for DYNMODEL is not valid!")
                    else:
                        config_str = config_str + "4b{0:0{1}x}".format(value, 2)
                except:
                    self.messages.appendPlainText("Error: the value for DYNMODEL is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for DYNMODEL is not valid!")
        # USERFUNCs
        if self.checkbox_USERFUNC1.isChecked():
            config_str = config_str + "58"
        if self.checkbox_USERFUNC2.isChecked():
            config_str = config_str + "59"
        if self.checkbox_USERFUNC3.isChecked():
            config_str = config_str + "5a"
        if self.checkbox_USERFUNC4.isChecked():
            config_str = config_str + "5b"
        if self.checkbox_USERFUNC5.isChecked():
            if self.USERFUNC5_val.text().isdigit():
                try:
                    value = int(self.USERFUNC5_val.text())
                    if (value < 0) or (value > 65535):
                        self.messages.appendPlainText("Error: the value for USERFUNC5 is not valid!")
                    else:
                        config_str = config_str + "5c" + struct.pack('<H', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for USERFUNC5 is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for USERFUNC5 is not valid!")
        if self.checkbox_USERFUNC6.isChecked():
            if self.USERFUNC6_val.text().isdigit():
                try:
                    value = int(self.USERFUNC6_val.text())
                    if (value < 0) or (value > 65535):
                        self.messages.appendPlainText("Error: the value for USERFUNC6 is not valid!")
                    else:
                        config_str = config_str + "5d" + struct.pack('<H', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for USERFUNC6 is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for USERFUNC6 is not valid!")
        if self.checkbox_USERFUNC7.isChecked():
            if self.USERFUNC7_val.text().isdigit():
                try:
                    value = int(self.USERFUNC7_val.text())
                    if (value < 0) or (value > 4294967295):
                        self.messages.appendPlainText("Error: the value for USERFUNC7 is not valid!")
                    else:
                        config_str = config_str + "5e" + struct.pack('<I', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for USERFUNC7 is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for USERFUNC7 is not valid!")
        if self.checkbox_USERFUNC8.isChecked():
            if self.USERFUNC8_val.text().isdigit():
                try:
                    value = int(self.USERFUNC8_val.text())
                    if (value < 0) or (value > 4294967295):
                        self.messages.appendPlainText("Error: the value for USERFUNC8 is not valid!")
                    else:
                        config_str = config_str + "5f" + struct.pack('<I', value).hex() # Little-endian hex
                except:
                    self.messages.appendPlainText("Error: the value for USERFUNC8 is not valid!")
            else:
                self.messages.appendPlainText("Error: the value for USERFUNC8 is not valid!")
        # Add the ETX
        config_str = config_str + "03"
        # Calculate and append the checksum bytes
        csuma = 0
        csumb = 0
        for i in range(0, len(config_str), 2):
            pair = int(config_str[i:i+2], 16) # Grab a pair of hex digits as an int
            csuma = csuma + pair
            csumb = csumb + csuma
        config_str = config_str + "{0:0{1}x}".format((csuma%256), 2) # Add the checksum bytes to the message
        config_str = config_str + "{0:0{1}x}".format((csumb%256), 2)
        # Display the message            
        self.config.clear() # Clear the config window
        self.config.appendPlainText(config_str) # Display the config message

    def on_refresh_btn_pressed(self) -> None:
        self.update_com_ports()

    def on_upload_btn_pressed(self) -> None:
        """Check if port is available"""
        portAvailable = False
        ports = comports()
        for p in ports:
            if (p.device == self.port):
                portAvailable = True
        if (portAvailable == False):
            self.messages.moveCursor(QTextCursor.End)
            self.messages.ensureCursorVisible()
            self.messages.appendPlainText("Port No Longer Available!")
            self.messages.ensureCursorVisible()
            return

        """Check if file exists"""
        fileExists = False
        try:
            f = open(self.fileLocation_lineedit.text())
            fileExists = True
        except IOError:
            fileExists = False
        finally:
            if (fileExists == False):
                self.messages.moveCursor(QTextCursor.End)
                self.messages.ensureCursorVisible()
                self.messages.appendPlainText("File Not Found!")
                self.messages.ensureCursorVisible()
                return
            f.close()

        self.messages.appendPlainText("Boo!\n")

    def on_browse_btn_pressed(self) -> None:
        """Open dialog to select bin file."""
        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getOpenFileName(
            None,
            "Select Configuration File",
            "",
            "Configuration Files (*.pkl);;All Files (*)",
            options=options)
        if fileName:
            self.fileLocation_lineedit.setText(fileName)
     
if __name__ == '__main__':
    from sys import exit as sysExit
    app = QApplication([])
    app.setOrganizationName('SparkX')
    app.setApplicationName('Artemis Global Tracker Configuration Tool')
    w = MainWidget()
    w.show()
    sysExit(app.exec_())
