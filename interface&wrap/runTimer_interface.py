# -*- coding: utf-8 -*-
'''
Created on Mon Jun 18 12:32:23 2018

@author: Kazantsev Andrey
'''
import os
import sys
import glob
import platform

import runTimer as rT

from PyQt5.QtWidgets import (QMainWindow, QApplication, QWidget, QAction,
                             QLabel, QSizePolicy, QPushButton, QFrame,
                              QVBoxLayout, QHBoxLayout, QRadioButton,
                              QCheckBox, QGridLayout, QTextEdit, QLineEdit,
                              QFileDialog, QMessageBox, QDesktopWidget)
from PyQt5.QtGui import QIcon, QGuiApplication
from PyQt5.QtCore import QRect, Qt

import matplotlib
matplotlib.use('Qt5Agg')

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure



if 'Windows' in platform.platform():
    DELIMETER = '\\'
    SYSTEM_MAX_WIDHT = 505
    SYSTEM_MAX_HEIGHT = 700
    LEN_FORM_DIR = 200

elif 'Linux' in platform.platform():
    DELIMETER = '/'
    SYSTEM_MAX_WIDHT = 700
    SYSTEM_MAX_HEIGHT = 720
    LEN_FORM_DIR = 301



class aboutProgramm(QWidget):

    def __init__(self, parent=None):
        super(aboutProgramm, self).__init__()

        self.help_label = QTextEdit(self)
        self.title = 'О программе'
        self.left = 100
        self.top = 100
        self.width = 400
        self.height = 480
        self.initUI()

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.setWindowIcon(QIcon('images' + DELIMETER + 'pulse.png'))
        val_text = ('<body>' +
                    '<h4 align="left">Описание</h4>' +
                    '<p align="justify">' +
                    'Программа подготовлена в рамках проекта по разработке ' +
                    'научно-методического навигационного обеспечения ' +
                    'космических миссий по пульсарам. \n\n' +
                    'Разработка элементов ПММ в обеспечение отработки' +
                    ' технологий формирования пульсарной шкалы времени.\n\n' +
                    '</p>' +
                    '<h4 align="left">Шифр:</h4>' +
                    '<p align="justify">СЧ НИР «Вызов-Перспектива-3-ФИАН»</p>' +
                    '</body>')

        self.help_label.setText(val_text)
        self.help_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.help_label.setAlignment(Qt.AlignLeft)
        self.help_label.setGeometry(QRect(20, 20, 400, 480))


class drawPlot(QWidget):

    def __init__(
            self, data, up_title, OX_lable, OY_lable, mode, idx_geom):
        super().__init__()
        self.title = up_title
        self.idx_geom = idx_geom
        self.left, self.top, self.width, self.height = self.get_geom()
        self.data = data
        self.mode = mode
        self.up_title = up_title
        self.OX_lable = OX_lable
        self.OY_lable = OY_lable

        self.initUI()

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.setWindowIcon(QIcon('images' + DELIMETER + 'pulse.png'))

        m = PlotCanvas(
                self.data, self.up_title, self.OX_lable, self.OY_lable, self.mode)

        hbox = QGridLayout()
        # addWidget(QWidget, row, column, rows, columns)
        hbox.addWidget(m, 0, 0)
        # Упаковка
        #box = QHBoxLayout()
        #box.addWidget(m, 0, Qt.AlignTop | Qt.AlignCenter)
        #print(dir(self.resolution))
        self.setLayout(hbox)

    def get_geom(self):
        resolution = QDesktopWidget().screenGeometry()
        height = int(resolution.height()/3)
        width = int(height*1.5)
        if self.idx_geom == 1:
            left = resolution.width() - width
            top = 0
        elif self.idx_geom == 2:
            left = resolution.width() - 2*width
            top = 0
        elif self.idx_geom == 3:
            left = resolution.width() - width
            top = height - 50
        elif self.idx_geom == 4:
            left = resolution.width() - 2*width
            top = height - 50
        elif self.idx_geom == 5:
            left = resolution.width() - width
            top = 2*height - 50


        return left, top, width, height

class PlotCanvas(FigureCanvas):

    def __init__(self, data, up_title, OX_lable, OY_lable, mode):

        width=4
        height=3
        dpi=100
        self.data = data
        self.up_title = up_title
        self.OX_lable = OX_lable
        self.OY_lable = OY_lable
        self.mode = mode
        fig = Figure(figsize=(width, height), dpi=dpi)
        #self.axes = fig.add_subplot(111)

        FigureCanvas.__init__(self, fig)
        self.setParent(None)

        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        if mode == '1d':
            self.plot()
        elif mode == '2d':
            self.plot2d()


    def plot(self):
        ax = self.figure.add_subplot(111)
        ax.plot(self.data, 'r-')
        ax.set_title(self.up_title, fontsize=10)
        ax.set_xlabel(self.OX_lable, fontsize=10)
        ax.set_ylabel(self.OY_lable, fontsize=10)
        ax.grid()
        self.draw()

    def plot2d(self):
        ax = self.figure.add_subplot(111)
        ax.imshow(self.data)
        ax.set_title(self.up_title, fontsize=10)
        ax.set_xlabel(self.OX_lable, fontsize=10)
        ax.set_ylabel(self.OY_lable, fontsize=10)
        ax.grid()
        self.draw()


class WarningBox(QMessageBox):

    def __init__(self, title, text):
        super().__init__()
        self.title = title
        self.text = text
        self.initUI()

    def initUI(self):
        self.setIcon(QMessageBox.Warning)
        self.setWindowTitle('Предупреждение!')
        self.setText(self.title)
        self.setInformativeText(self.text)
        self.setStandardButtons(
                QMessageBox.Ok)
        self.setDefaultButton(QMessageBox.Ok)

class QuestionBox(QMessageBox):

    def __init__(self, title, text):
        super().__init__()
        self.title = title
        self.text = text
        self.initUI()

    def initUI(self):
        self.setIcon(QMessageBox.Question)
        self.setWindowTitle('Вопрос!')
        self.setText(self.title)
        self.setInformativeText(self.text)
        self.setStandardButtons(
                QMessageBox.Yes | QMessageBox.No)
        self.setDefaultButton(QMessageBox.Yes)




class App(QMainWindow):

    def __init__(self):
        super().__init__()
        self.title = 'runTimer_TEST'
        self.left = 20
        self.top = 30
        self.width = 505
        self.height = 670
        self.out_dir = os.getcwd() + DELIMETER
        self.initUI()

        self.top_wins = [] # список окон отрисовок
        self.br = 0 # дефолтные значения для сравнения
        self.pulse = 0 # дефолтные значения для сравнения
        self.kkf = 0
    def initUI(self):

        '''
        Применение основных параметров окна
        '''
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.setWindowIcon(QIcon('images' + DELIMETER + 'pulse.png'))
        self.dialog = aboutProgramm(self)

        ######################################################################
        '''
        Инициализация верхней строки меню
        '''
        menubar = self.menuBar()
        commonMenu = menubar.addMenu('Общее')
        helpMenu = menubar.addMenu('Помощь')


        exitButton = QAction(QIcon('images' + DELIMETER + 'exit.png'),
                             'Выход', self)
        exitButton.setShortcut('Ctrl+Q')
        exitButton.triggered.connect(self.close)


        aboutButton = QAction(QIcon('images' + DELIMETER + 'help.png'),
                              'О программе', self)
        aboutButton.triggered.connect(self.dialog.show)


        commonMenu.addAction(exitButton)
        helpMenu.addAction(aboutButton)

        menubar.setNativeMenuBar(False) # Трюк для появления меню бара в линукс

        #######################################################################
        '''
        Первая рабочая область, на которой находятся
        радиокнопки, лайбл. для простоты начальной работы панель будет закрашена
        '''

        self.label_workstate = QLabel('Выбор режима работы')
        self.label_workstate.setObjectName('label_workstate')
        self.label_workstate.setStyleSheet(
            'QLabel#label_workstate {font: bold;}')
        self.workstate_ONEFILE = QRadioButton('Режим работы с одиночным файлом')
        self.workstate_LOOP = QRadioButton('Потоковый режим обработки данных')
        self.workstate_CONFIG = QRadioButton('Работа с конфигурационным файлом')

        layout_workstate = QVBoxLayout()
        #layout.setContentsMargins(5, 5, 5, 5)
        layout_workstate.addWidget(self.label_workstate,
                                          0, Qt.AlignTop | Qt.AlignCenter)
        layout_workstate.addWidget(self.workstate_ONEFILE,
                                          0, Qt.AlignTop | Qt.AlignLeft)
        layout_workstate.addWidget(self.workstate_LOOP,
                                          0, Qt.AlignTop | Qt.AlignLeft)
        layout_workstate.addWidget(self.workstate_CONFIG,
                                          0, Qt.AlignTop | Qt.AlignLeft)


        #self.workPanel.setLayout(layout)

        #self.setLayout(layout)
        #######################################################################
        '''
        Вторая рабочая область, на которой будут находться
        чекбоксы и лайбл. для простоты начальной работы панель будет закрашена
        '''
        self.label_paint = QLabel('Отрисовка')
        self.label_paint.setObjectName('label_paint')
        self.label_paint.setStyleSheet(
            'QLabel#label_paint {font: bold;}')
        self.paint_AP = QCheckBox('Средний профиль')
        self.paint_DSPEC = QCheckBox('Динамический спектр')
        self.paint_AFC = QCheckBox('График АЧХ')
        self.paint_FLTR = QCheckBox('Частотный фильтр')
        self.paint_CCF = QCheckBox('Корреляционная функция')
        self.redraw_button = QPushButton('Перерисовка')


        layout_paint = QVBoxLayout()
        layout_paint.addWidget(self.label_paint,
                               0, Qt.AlignTop | Qt.AlignCenter)
        layout_paint.addWidget(self.paint_AP, 0, Qt.AlignTop | Qt.AlignLeft)
        layout_paint.addWidget(self.paint_DSPEC, 0, Qt.AlignTop | Qt.AlignLeft)
        layout_paint.addWidget(self.paint_AFC, 0, Qt.AlignTop | Qt.AlignLeft)
        layout_paint.addWidget(self.paint_FLTR, 0, Qt.AlignTop | Qt.AlignLeft)
        layout_paint.addWidget(self.paint_CCF, 0, Qt.AlignTop | Qt.AlignLeft)
        layout_paint.addWidget(self.redraw_button,
                               0, Qt.AlignTop | Qt.AlignCenter)


        '''
        Кнопка, запускающая обработку
        '''
        self.procces_button = QPushButton('Начать обработку')
        self.procces_button.setObjectName('self.procces_button')
        self.procces_button.setStyleSheet(
            'QPushButton#self.procces_button {font: bold; background-color: gray;}')

        clear_button = QPushButton('Отчистить')
        close_button = QPushButton('Закрыть окна отрисовки')
        #######################################################################
        '''
        Третья рабочая область содержащая чекбоксы и кнопку для переобработки
        '''
        self.label_clear = QLabel('Чистка')
        self.label_clear.setObjectName('label_clear')
        self.label_clear.setStyleSheet(
            'QLabel#label_clear {font: bold;}')
        self.clear_ACH = QCheckBox('Фильтрация по АЧХ')
        self.clear_PULS = QCheckBox('Импульская фильтрация')
        self.reproc_button = QPushButton('Переобработка')
        # создание дефолтного функционала для чекбокса
        #self.clear_ACH.toggle()
        #self.clear_ACH.stateChanged(self.changeCheck)

        layout_clear = QVBoxLayout()
        layout_clear.addWidget(self.label_clear,
                               0, Qt.AlignTop | Qt.AlignCenter)
        layout_clear.addWidget(self.clear_ACH, 0, Qt.AlignTop | Qt.AlignLeft)
        layout_clear.addWidget(self.clear_PULS, 0, Qt.AlignTop | Qt.AlignLeft)
        layout_clear.addWidget(self.reproc_button,
                               0, Qt.AlignTop | Qt.AlignCenter)
        #######################################################################
        '''
        Текстовый блок, выводящий список обрабатываемых файлов
        '''
        self.files_box = QTextEdit()
        self.files_box.append('Список файлов')
        self.files_box.verticalScrollBar()
        '''
        Текстовый блок, выводящий лог обработки
        '''

        self.log_box = QTextEdit()
        self.log_box.append('Отображение логов')
        self.log_box.verticalScrollBar()
        #######################################################################
        '''
        Рабочая область, содержащая дополнительные настройки
        Будет размещена на сетке, в силу сложности расположения элементов
        '''

        self.label_add = QLabel(
                'Дополнительные настройки сохранения \nи обработки')
        self.label_add.setObjectName('label_add')
        self.label_add.setStyleSheet(
            'QLabel#label_add {font: bold;}')
        self.save_AP = QCheckBox('Сохранить средний профиль в файл')
        self.save_DSPEC = QCheckBox('Сохранить динамический спектр в файл')
        self.save_AFC = QCheckBox('Сохранить файлы АЧХ')
        self.save_SECPULS = QCheckBox('Сохранить последовательность импульсов')
        self.save_TOA = QCheckBox('Сохранить файл МПИ в формате ITOA')
        self.save_COR = QCheckBox('Сохранить результат кросс-корреляции в файл')
        self.save_CCF = QCheckBox('Сохранить ККФ в файл')

        self.label_tres_ACH = QLabel(
                'Порог чистки АЧХ (в ед. СКО):')
        self.label_tres_PULS = QLabel(
                'Параметр отсечки импульсных помех:')
        self.label_LOCSCALE = QLabel(
                'Локальная шкала - GMT (Ч):')
        self.label_dir = QLabel(
                'Директория для выходных файлов')

        self.form_tres_ACH = QLineEdit()
        self.form_tres_ACH.setMinimumSize(75, 20)
        self.form_tres_ACH.setMaximumSize(75, 20)
        self.form_tres_PULS = QLineEdit()
        self.form_tres_PULS.setMinimumSize(75, 20)
        self.form_tres_PULS.setMaximumSize(75, 20)
        self.form_LOCSCALE = QLineEdit()
        self.form_LOCSCALE.setMinimumSize(75, 20)
        self.form_LOCSCALE.setMaximumSize(75, 20)
        self.form_dir = QLineEdit()
        self.form_dir.setMinimumSize(LEN_FORM_DIR, 20)
        self.form_dir.setMaximumSize(LEN_FORM_DIR, 20)
        self.form_dir.setText(self.out_dir)
        self.button_dir = QPushButton('Обзор')

        layout_save = QVBoxLayout()
        layout_save.addWidget(self.label_add,
                             0, Qt.AlignTop | Qt.AlignCenter)
        layout_save.addWidget(self.save_AP,
                             0, Qt.AlignTop | Qt.AlignLeft)
        layout_save.addWidget(self.save_DSPEC,
                             0, Qt.AlignTop | Qt.AlignLeft)
        layout_save.addWidget(self.save_AFC,
                             0, Qt.AlignTop | Qt.AlignLeft)
        layout_save.addWidget(self.save_SECPULS,
                             0, Qt.AlignTop | Qt.AlignLeft)
        layout_save.addWidget(self.save_TOA,
                             0, Qt.AlignTop | Qt.AlignLeft)
        layout_save.addWidget(self.save_COR,
                             0, Qt.AlignTop | Qt.AlignLeft)
        layout_save.addWidget(self.save_CCF,
                             0, Qt.AlignTop | Qt.AlignLeft)

        layout_tres_first = QHBoxLayout()
        layout_tres_first.addWidget(self.label_tres_ACH)
        layout_tres_first.addWidget(self.form_tres_ACH)
        layout_tres_second = QHBoxLayout()
        layout_tres_second.addWidget(self.label_tres_PULS)
        layout_tres_second.addWidget(self.form_tres_PULS)
        layout_tres_third = QHBoxLayout()
        layout_tres_third.addWidget(self.label_LOCSCALE)
        layout_tres_third.addWidget(self.form_LOCSCALE, Qt.AlignRight)
        layout_label_dir = QHBoxLayout()
        layout_label_dir.addWidget(self.label_dir,
                                   0, Qt.AlignTop | Qt.AlignCenter)

        layout_form_dir = QHBoxLayout()
        layout_form_dir.addWidget(self.form_dir,
                             0, Qt.AlignRight)
        layout_form_dir.addWidget(self.button_dir,
                             0, Qt.AlignLeft)




        main_layout_add = QVBoxLayout()
        main_layout_add.addLayout(layout_save, 0)
        main_layout_add.addLayout(layout_tres_first, 0)
        main_layout_add.addLayout(layout_tres_second, 0)
        main_layout_add.addLayout(layout_tres_third, 0)
        main_layout_add.addLayout(layout_label_dir, 0)
        main_layout_add.addLayout(layout_form_dir, 0)

        '''
        grid_add = QGridLayout()
        grid_add.addLayout(layout_save, 0, 0)
        grid_add.addLayout(layout_tres_first, 1, 0)
        grid_add.addLayout(layout_tres_second, 2, 0)
        '''


         #######################################################################
        '''
        Установка значений по умолчанию
        '''
        self.workstate_ONEFILE.setChecked(True)
        self.paint_AP.setChecked(True)
        self.paint_AFC.setChecked(True)
        self.clear_ACH.setChecked(True)
        self.clear_PULS.setChecked(True)
        self.save_AP.setChecked(True)
        self.save_DSPEC.setChecked(True)
        self.save_AFC.setChecked(True)
        self.save_TOA.setChecked(True)
        self.save_COR.setChecked(True)
        self.form_tres_ACH.setText('2.0')
        self.form_tres_PULS.setText('4.0')
        self.form_LOCSCALE.setText('0.0')

        #######################################################################
        '''
        Установка описаний в статусбар
        '''
        self.statusBar().showMessage('Готов')
        exitButton.setStatusTip('Закрыть приложение')
        aboutButton.setStatusTip('Краткая информация о прграмме')
        self.workstate_ONEFILE.setStatusTip('Пока что просто радио кнопка')
        self.workstate_LOOP.setStatusTip('Пока что просто радио кнопка')
        self.workstate_CONFIG.setStatusTip('Пока что просто радио кнопка')
        self.form_dir.setStatusTip('Директория для файлов результатов обработки')
        self.redraw_button.setStatusTip('Запуск перерисовки графиков')
        self.reproc_button.setStatusTip('Запуск переобработки данных')
        self.button_dir.setStatusTip('Выбор директори для файлов результатов обработки')
        self.procces_button.setStatusTip('Запуск обработки согласно выбранному режимы')

        self.files_box.setStatusTip('Вывод обрабатываемых файлов в виде списка')
        self.log_box.setStatusTip(
                'Отображение информации о ходе обработки данных')
        clear_button.setStatusTip('Отчистка истории обработки')
        close_button.setStatusTip('Закрытие всех окон отрисовки')





        #######################################################################
        '''
        Подключение функций к кнопкам
        '''

        self.redraw_button.clicked.connect(self.redraw)
        self.reproc_button.clicked.connect(self.reproccesing)
        self.button_dir.clicked.connect(self.review_out_dir)
        self.procces_button.clicked.connect(self.start_proccesing)
        clear_button.clicked.connect(self.clear_log_box)
        close_button.clicked.connect(self.close_top_wins)




        #######################################################################
        '''
        Компановка рабочих областей на виджеты
        '''
        frame_workstate = QFrame()
        frame_workstate.setObjectName('frame_workstate')
        frame_workstate.setLayout(layout_workstate)
        frame_workstate.setStyleSheet(
                'QFrame#frame_workstate {border:1px solid black;}')

        frame_paint = QFrame()
        frame_paint.setObjectName('frame_paint')
        frame_paint.setStyleSheet(
                'QFrame#frame_paint {border: 1px solid black; }')
        frame_paint.setLayout(layout_paint)

        frame_clear = QFrame()
        frame_clear.setObjectName('frame_clear')
        frame_clear.setStyleSheet(
                'QFrame#frame_clear {border: 1px solid black; }')
        frame_clear.setLayout(layout_clear)
        frame_add = QFrame()
        frame_add.setObjectName('frame_add')
        frame_add.setStyleSheet(
                'QFrame#frame_add {border: 1px solid black; }')
        frame_add.setLayout(main_layout_add)
        #######################################################################
        '''
        Компановка виджетов и кнопки на сетку
        '''




        hbox = QGridLayout()
        # addWidget(QWidget, row, column, rows, columns)
        hbox.addWidget(frame_workstate, 0, 0, 2, 4)
        hbox.addWidget(frame_paint, 0, 4, 5, 4)
        hbox.addWidget(self.procces_button, 11, 4, 1, 4)
        hbox.addWidget(frame_clear, 5, 4, 2, 4)
        hbox.addWidget(self.files_box, 7, 4, 2, 4)
        hbox.addWidget(self.log_box, 12, 0, 4, 8)
        hbox.addWidget(frame_add, 2, 0, 7, 4)
        hbox.addWidget(clear_button, 17, 0)
        hbox.addWidget(close_button, 17, 1)



        '''
        hbox.addWidget(frame_workstate, 0, 0, 1, 1)
        hbox.addWidget(frame_paint, 0, 1, 2, 1)
        hbox.addWidget(self.procces_button, 4, 1)
        hbox.addWidget(frame_clear, 2, 1 )
        hbox.addWidget(self.files_box, 3, 1)
        hbox.addWidget(self.log_box, 5, 0, 2, 2)
        hbox.addWidget(frame_add, 1, 0, 2, 1)
        '''
        #######################################################################
        '''
        Создание главного виджета и компановка на него сетки
        '''

        main_panel = QWidget()
        main_panel.setLayout(hbox)
        main_panel.setMaximumSize(SYSTEM_MAX_WIDHT, SYSTEM_MAX_HEIGHT)
        self.setCentralWidget(main_panel)
        self.show()

    #Хочешь работать с переменными из тела класса?
    #Используй при их создании self и добавляй в функцию только его.

    def close_top_wins(self):
        for item in self.top_wins:
            item.close()

    def check_value(self):
        try:
            float(self.form_tres_ACH.text())
        except ValueError:
            return 1

        try:
            float(self.form_tres_PULS.text())
        except ValueError:
            return 2

        try:
            float(self.form_LOCSCALE.text())
        except ValueError:
            return 3

        if os.path.isdir(self.form_dir.text()):
            pass
        else:
            return 4

        if len(self.form_dir.text()) < 50:
            pass
        else:
            return 5

        return 0

    def check_conf(self, conf_name):
        conf_dic = {}
        list_conf = ['FRfilter', 'getIndImpulses',
                     'FRcleaningCut', 'removeSpikes',
                     'tplFile', 'inputDir', 'outputDir',
                     'channelMask', 'getDynSpectrum',
                     'utcCorrection', 'startFileNumber',
                     'getFR', 'nRuns', 'SpikeCleaningCut',
                     'useTrueNormalisation']

        try:
            with open(conf_name, 'r') as conf_file:
                for n_line in range(15):
                    first, second, *non = conf_file.readline().split()
                    conf_dic[first] = second
        except:
            return 1

        for item in list_conf:
            if item in conf_dic:
                pass
            else:
                return 1


        return 0

    def check_file(self, item):
        with open(item, 'rb') as file:
            check_line = file.readline()
        if check_line[0] == 110:
            return 0
        else:
            return 6

    def clearing(self, pulse):
        if self.clear_ACH.isChecked():
            self.log_box.append("Удаление зашумленых каналов")
            QGuiApplication.processEvents()
            pulse.CleanFrequencyResponse(float(self.form_tres_ACH.text()))

        if self.clear_PULS.isCheckable():
            self.log_box.append("Удаление импульсных помех")
            QGuiApplication.processEvents()
            pulse.RemoveSpikes(float(self.form_tres_PULS.text()))

        return pulse

    def clearing_conf(self, pulse, conf):
        if conf.doFRfiltering:
            self.log_box.append("Удаление зашумленых каналов")
            QGuiApplication.processEvents()
            pulse.CleanFrequencyResponse(conf.nVarFR)

        if conf.doRemoveSpikes:
            self.log_box.append("Удаление импульсных помех")
            QGuiApplication.processEvents()
            pulse.RemoveSpikes(conf.nVarSpike)

        return pulse

    def ccf(self, pulse, rawdata_dir, output_dir, tpl_dir, runs, utcloc):
        finPulse=pulse.GetSumProfile()
        cor = rT.Cor()
        skkf = rT.Skkf()
        tpl = rT.Tpl()
        kkf = rT.Skkf()
        itoa = rT.Itoa()
        dtkkf=0.
        dt = finPulse.tau
        kkf.numpoint=finPulse.numpointwin
        tpl = cor.Tplread(tpl_dir)
        kkf.kkfdata = rT.FloatVector(cor.dccf(tpl, finPulse))
        maxkkf=kkf.kkfdata[0]
        maxi=0

        for i in range(finPulse.numpointwin-1):
            if (maxkkf<=kkf.kkfdata[i]):
                maxkkf=kkf.kkfdata[i]
                maxi=i

        kkf.kk = rT.FloatVector([0,0,0,0,0])
        for i in range(5):
            kkf.kk[i]=kkf.kkfdata[maxi-2+i]

        if finPulse.rtype == 'DPP1':
            startdelay=1000.*dt
        else:
            startdelay=0.

        CMprf= cor.CofM(finPulse.prfdata, finPulse.numpointwin, finPulse.tau)
        CMtpl = cor.CofM(tpl.tpldata, tpl.numpoint, tpl.tau)

        self.log_box.append("Центр Масс профиля: " + str(CMprf))
        self.log_box.append("Центр Масс шаблона: " + str(CMtpl))
        QGuiApplication.processEvents()
        if (CMprf >= CMtpl):
            kkf.maxp=(startdelay+1000.*dt*
                      (maxi+cor.ApproxMax(
                              kkf.kk[0],
                              kkf.kk[1],
                              kkf.kk[2],
                              kkf.kk[3],
                              kkf.kk[4])))
        else:
            kkf.maxp=(-1000.*finPulse.numpointwin*
                      finPulse.tau+startdelay+1000.*dt*
                      (maxi+cor.ApproxMax(
                              kkf.kk[0],
                              kkf.kk[1],
                              kkf.kk[2],
                              kkf.kk[3],
                              kkf.kk[4])))

        itoa.TOAMJD = cor.utc2mjd(finPulse, utcloc, kkf.maxp)
        itoa.sMJD = cor.utc2mjds(finPulse, utcloc, kkf.maxp)

        kkf.snr = cor.SNR(finPulse)
        kkf.errmax = cor.CalcErrorW50(finPulse, kkf.snr)
        itoa.psrname = finPulse.psrname

        itoa.TOAerr = kkf.errmax

        if (kkf.errmax<=999.):
            itoa.TOAerr = kkf.errmax
        else:
            itoa.TOAerr = 999.0

        itoa.freq = finPulse.freq
        itoa.ddm = 0.0 # поправка вносится в файл itoa только по многочастотным данным
        itoa.obscode = finPulse.obscode

        # Вывод информации в лог.
        self.log_box.append('ККФ:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<')
        self.log_box.append('Значения ККФ в окрестности максимума:')
        self.log_box.append(str(kkf.kk[0]) + ' ' +
                                str(kkf.kk[1]) + ' ' +
                                str(kkf.kk[2]) + ' ' +
                                str(kkf.kk[3]) + ' ' +
                                str(kkf.kk[4]))
        QGuiApplication.processEvents()
        self.log_box.append(
                'Max ККФ   мкс  ||  ошибка   мкс  || отношение сигнал/шум')
        self.log_box.append(str(kkf.maxp) + ' ' +
                            str(kkf.errmax) + ' ' +
                            str(kkf.snr))
        QGuiApplication.processEvents()
        self.log_box.append('МПИ:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<')
        self.log_box.append(
                'Пульсар |    МПИ(MJD)  | Err(мкс)| Частота(МГц)  | поправка DM(мкс) | Код обсерватории')
        self.log_box.append(itoa.psrname + ' ' +
                            str(itoa.sMJD[:-1]) + ' ' +
                            str(itoa.TOAerr) + ' ' +
                            str(itoa.freq) + ' ' +
                            str(itoa.ddm) + ' ' +
                            itoa.obscode)
        QGuiApplication.processEvents()


        if self.workstate_ONEFILE.isChecked():
            if self.save_TOA.isChecked():
                cor.TOAWrite(output_dir, itoa)
                self.log_box.append(
                        'МПИ добавленны в файл: ' + output_dir + 'toa' )
                QGuiApplication.processEvents()
            if self.save_COR.isChecked():
                cor.KKFWrite(output_dir, kkf)
                self.log_box.append(
                        'Параметры кросс-корреляции добавлены в файл: '
                        + output_dir + 'kkf')
                QGuiApplication.processEvents()
            if self.save_CCF.isChecked():
                cor.KKFdatWrite(output_dir, kkf, runs)
                self.log_box.append('ККФ записана в файл: ' +
                                    output_dir + runs + '.kkf')
                QGuiApplication.processEvents()

        else:
            if self.save_TOA.isChecked():
                cor.TOAWrite(output_dir, itoa)
                self.log_box.append('МПИ добавленны в файл: ' +
                                    output_dir + 'toa')
                QGuiApplication.processEvents()
            if self.save_COR.isChecked():
                cor.KKFWrite(output_dir, kkf)
                self.log_box.append('Параметры кросс-корреляции добавлены в файл: '
                                    + output_dir + 'kkf')
                QGuiApplication.processEvents()
            if self.save_CCF.isChecked():
                cor.KKFdatWrite(output_dir, kkf, runs)
                self.log_box.append('ККФ записана в файл: ' + output_dir +
                                    runs + '.kkf')
                QGuiApplication.processEvents()

        return kkf, itoa

    def writting(self, pulse, fileName):
        if not self.save_SECPULS.isChecked():
            pulse.SumPerChannelPeriods()
            self.log_box.append('Суммирование периодов в каждом канале')
            QGuiApplication.processEvents()
            pulse.DoCompensation()
            self.log_box.append('Компенсация дисперсионного запаздывания')
            if self.save_DSPEC.isChecked():
                QGuiApplication.processEvents()
                pulse.PrintChannelSumProfile(self.form_dir.text())
                self.log_box.append('Динамический спектр записан в файл: ' +
                                    self.form_dir.text() +
                                    'bands_' + os.path.basename(fileName) +
                                    '.prf')
        if self.save_SECPULS.isChecked():
            if  not self.save_DSPEC.isChecked():
                self.log_box.append('Компенсация дисперсионного запаздывания')
                QGuiApplication.processEvents()
                pulse.DoCompensation()
                pulse.PrintCompensatedImpulses(self.form_dir.text())
                self.log_box.append(
                        'Массив индивидуальных импульсов записан в файл: ' +
                               self.form_dir.text() + DELIMETER + 'compPulses_'
                               + os.path.basename(fileName) + '.prf')
            if self.save_DSPEC.isChecked():
                self.log_box.append('Компенсация дисперсионного запаздывания')
                QGuiApplication.processEvents()
                pulse.DoCompensation()
                pulse.PrintCompensatedImpulses(self.form_dir.text())
                self.log_box.append(
                        'Массив индивидуальных импульсов записан в файл: ' +
                               self.form_dir.text() + 'compPulses_'
                               + os.path.basename(fileName) + '.prf')
                self.log_box.append('Суммирование периодов в каждом канале')
                QGuiApplication.processEvents()
                pulse.SumPerChannelPeriods()
                self.log_box.append('Компенсация дисперсионного запаздывания')
                QGuiApplication.processEvents()
                pulse.DoCompensation()
                pulse.PrintChannelSumProfile(self.form_dir.text())
                self.log_box.append('Динамический спектр записан в файл: ' +
                                    self.form_dir.text() +
                                    'bands_' + os.path.basename(fileName) +
                                    '.prf')
        QGuiApplication.processEvents()
        pulse.SumPeriods()
        if self.save_AP.isChecked():
            pulse.PrintSumProfile(self.form_dir.text())
            self.log_box.append('Средний профиль записан в файл:' +
                                self.form_dir.text() +
                                os.path.basename(fileName) + '.prf')
            QGuiApplication.processEvents()
        if self.save_AFC.isChecked():
            pulse.PrintFrequencyResponse(self.form_dir.text())
            self.log_box.append('АЧХ записана в файл:' +
                                self.form_dir.text() +
                                os.path.basename(fileName) + '.fr')
            self.log_box.append('АЧХ после маски записана в файл:' +
                                self.form_dir.text() +
                                'masked_' + os.path.basename(fileName) + '.fr')
            QGuiApplication.processEvents()
        return pulse

    def redraw(self):

        if type(self.br) == type(self.pulse):
            msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Сначала необходимо произвести обработку!')
            msg.exec_()
            return None

        self.close_top_wins()
        if self.paint_AP.isChecked():
            sumpulse = self.pulse.GetSumPeriodsVec()
            self.draw_AP = drawPlot(
                    sumpulse, 'Средний профиль пульсара',
                    'Номер точки в окне', 'Амплитуда', '1d', 1)
            self.draw_AP.show()
            self.top_wins.append(self.draw_AP)

        if self.paint_AFC.isChecked():
            afc = []
            for i in range(512):
                afc.append(self.br.GetFreqResponse(i))
            self.draw_AFC = drawPlot(
                    afc, 'Амплитудно-частотная характеристика',
                    'Номер частотного канала', 'Амплитуда', '1d', 2)
            self.draw_AFC.show()
            self.top_wins.append(self.draw_AFC)

        if self.paint_DSPEC.isChecked():
            pulse_for_dyn = rT.PulseExtractor(self.br)
            pulse_for_dyn.ReadMask("examples" + DELIMETER + "channel_mask.dat")
            pulse_for_dyn.NormaliseToUnity(False)
            pulse_for_dyn.FillMaskFRweights()
            pulse_for_dyn = self.clearing(pulse_for_dyn)
            pulse_for_dyn.SumPerChannelPeriods()
            list_din = []
            for i in range(512):
                list_din.append(self.pulse.GetChannelSumProfile(i))
            self.draw_DSPEC = drawPlot(
                    list_din, 'Динамический спектр сеанса',
                    'Номер точки в окне', 'Номер частотного канала', '2d', 3)
            self.draw_DSPEC.show()
            self.top_wins.append(self.draw_DSPEC)

        if self.paint_FLTR.isChecked():
            mask = self.pulse.GetChannelMask()
            self.draw_MASK = drawPlot(mask, 'Частотный фильтр',
                                      'Номер частотного канала',
                                      'Амплитуда', '1d', 4)
            self.draw_MASK.show()
            self.top_wins.append(self.draw_MASK)

        if self.paint_CCF.isChecked():
            self.draw_CCF = drawPlot(self.kkf.kkfdata,
                                     'Кросс-корреляционная функция',
                                     'Отсчет', 'Амплитуда', '1d', 5)
            self.draw_CCF.show()
            self.top_wins.append(self.draw_CCF)



    def reproccesing(self):
        if type(self.br) == type(self.pulse):
            msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Сначала необходимо произвести обработку!')
            msg.exec_()
            return None

        test_value = self.check_value()

        if test_value == 0:
            self.log_box.clear()
            self.log_box.append('Пройдена проверка вводимых данных')
        elif test_value == 1:
            msg = WarningBox(
                    'Работа программы приостановлена.',
                    'Введено неккоректное значение порога чистки АЧХ!' +
                    'Неккоректное значение изменено на значение по умолчанию.')

            msg.exec_()
            self.form_tres_ACH.setText('2.0')
            return None
        elif test_value == 2:
            msg = WarningBox(
                    'Работа программы приостановлена.',
                    'Введено неккоректное значение параметра отсечки импульсных помех!' +
                    'Неккоректное значение изменено на значение по умолчанию.')
            msg.exec_()
            self.form_tres_PULS.setText('4.0')
            return None
        elif test_value == 3:
            msg = WarningBox(
                    'Работа программы приостановлена.',
                    'Введено неккоректное значение коррекции UTC!' +
                    'Неккоректное значение изменено на значение по умолчанию.')
            msg.exec_()
            self.form_LOCSCALE.setText('0.0')
            return None
        elif test_value == 4:
            msg = WarningBox(
                    'Работа программы приостановлена.',
                    'Введен неккоректный путь выходной директории!' +
                    'Неккоректный путь изменен на путь по умолчанию.')
            msg.exec_()
            self.form_dir.setText(os.getcwd() + DELIMETER)
            return None
        elif test_value == 5:
            msg = WarningBox(
                    'Работа программы приостановлена.',
                    'Введен слишком длинный путь выходной директории!' +
                    'Путь к директории не должен превышать 49 символов. ' +
                    'Неккоректный путь изменен на путь по умолчанию.')
            msg.exec_()
            self.form_dir.setText(os.getcwd() + DELIMETER)
            return None


        self.pulse = rT.PulseExtractor(self.br)
        self.log_box.append('Чтение маски из файла: examples' + DELIMETER + 'channel_mask.dat')
        QGuiApplication.processEvents()
        self.pulse.ReadMask("examples" + DELIMETER +"channel_mask.dat")
        self.pulse.NormaliseToUnity(False)
        self.pulse.FillMaskFRweights()
        QGuiApplication.processEvents()
        self.pulse = self.clearing(self.pulse)
        self.pulse = self.writting(self.pulse, self.fileName)

        if self.tpl_file:
            self.kkf, self.itoa = self.ccf(
                    self.pulse, os.path.dirname(self.fileName),
                    self.form_dir.text(), self.tpl_file,
                    os.path.basename(self.fileName),
                    float(self.form_LOCSCALE.text()))
        else:
            self.paint_CCF.setChecked(False)


    def start_proccesing(self):

        #self.procces_button.setEnabled(False) #Отключение кнопки, для избегания зависания

        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog

        if self.workstate_ONEFILE.isChecked():

            test_value = self.check_value()

            if test_value == 0:
                self.log_box.clear()
                self.log_box.append('Пройдена проверка вводимых данных')
            elif test_value == 1:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введено неккоректное значение порога чистки АЧХ!' +
                        'Неккоректное значение изменено на значение по умолчанию.')

                msg.exec_()
                self.form_tres_ACH.setText('2.0')
                return None
            elif test_value == 2:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введено неккоректное значение параметра отсечки импульсных помех!' +
                        'Неккоректное значение изменено на значение по умолчанию.')
                msg.exec_()
                self.form_tres_PULS.setText('4.0')
                return None
            elif test_value == 3:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введено неккоректное значение коррекции UTC!' +
                        'Неккоректное значение изменено на значение по умолчанию.')
                msg.exec_()
                self.form_LOCSCALE.setText('0.0')
                return None
            elif test_value == 4:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введен неккоректный путь выходной директории!' +
                        'Неккоректный путь изменен на путь по умолчанию.')
                msg.exec_()
                self.form_dir.setText(os.getcwd() + DELIMETER)
                return None
            elif test_value == 5:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введен слишком длинный путь выходной директории!' +
                        'Путь к директории не должен превышать 49 символов. ' +
                        'Неккоректный путь изменен на путь по умолчанию.')
                msg.exec_()
                self.form_dir.setText(os.getcwd() + DELIMETER)
                return None

            self.fileName, _ = QFileDialog.getOpenFileName(
                    self,'Выберите один файл для обработки',
                    '', 'Все файлы (*)', options=options)

            if self.fileName:
                pass
            else:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Не выбран файл для обработки')
                msg.exec_()
                return None

            test_value = self.check_file(self.fileName)

            if test_value == 0:
                self.log_box.append('Пройдена проверка файла')
            elif test_value == 6:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Выбран некорректный файл')
                msg.exec_()
                return None

            self.files_box.clear();
            self.files_box.append(os.path.basename(self.fileName))
            self.log_box.append('Загружается файл: ' +
                    os.path.basename(self.fileName) +
                    '. Это может занять некоторое время.')
            QGuiApplication.processEvents() # Работает как обновление интерфейса
            self.br = rT.BaseRun()
            self.br.ReadRAWData(
                    os.path.basename(self.fileName),
                    os.path.dirname(self.fileName) + DELIMETER,
                    self.form_dir.text())
            self.log_box.append(
                    'Файл ' + os.path.basename(self.fileName) + ' загружен.')
            self.pulse = rT.PulseExtractor(self.br)
            self.log_box.append(
                    'Чтение маски из файла: examples' + DELIMETER + 'channel_mask.dat')
            QGuiApplication.processEvents()
            self.pulse.ReadMask("examples" + DELIMETER + "channel_mask.dat")
            self.pulse.NormaliseToUnity(False)
            self.pulse.FillMaskFRweights()
            QGuiApplication.processEvents()
            self.pulse = self.clearing(self.pulse)
            self.pulse = self.writting(self.pulse, self.fileName)

            self.tpl_file, _ = QFileDialog.getOpenFileName(
                    self,'Выберите файл шаблона для корреляции',
                    '', 'Файл шаблона (*.tpl);; Текстовый файл (*.txt)',
                    options=options)
            if self.tpl_file:
                self.kkf, self.itoa = self.ccf(
                        self.pulse, os.path.dirname(self.fileName),
                        self.form_dir.text(), self.tpl_file,
                        os.path.basename(self.fileName),
                        float(self.form_LOCSCALE.text()))
            else:
                self.paint_CCF.setChecked(False)

            msg = QuestionBox('Данные для визуализации готовы',
                                  'Провести отрисовку выбранных графиков?')
            reply = msg.exec_()
            if reply == QMessageBox.Yes:

                if self.paint_AP.isChecked():
                    sumpulse = self.pulse.GetSumPeriodsVec()
                    self.draw_AP = drawPlot(
                            sumpulse, 'Средний профиль пульсара',
                            'Номер точки в окне', 'Амплитуда', '1d', 1)
                    self.draw_AP.show()
                    self.top_wins.append(self.draw_AP)

                if self.paint_AFC.isChecked():
                    afc = []
                    for i in range(512):
                        afc.append(self.br.GetFreqResponse(i))
                    self.draw_AFC = drawPlot(
                            afc, 'Амплитудно-частотная характеристика',
                            'Номер частотного канала', 'Амплитуда', '1d', 2)
                    self.draw_AFC.show()
                    self.top_wins.append(self.draw_AFC)

                if self.paint_DSPEC.isChecked():
                    pulse_for_dyn = rT.PulseExtractor(self.br)
                    pulse_for_dyn.ReadMask("examples" + DELIMETER + "channel_mask.dat")
                    pulse_for_dyn.NormaliseToUnity(False)
                    pulse_for_dyn.FillMaskFRweights()
                    pulse_for_dyn = self.clearing(pulse_for_dyn)
                    pulse_for_dyn.SumPerChannelPeriods()
                    list_din = []
                    for i in range(512):
                        list_din.append(self.pulse.GetChannelSumProfile(i))
                    self.draw_DSPEC = drawPlot(
                            list_din, 'Динамический спектр сеанса',
                            'Номер точки в окне',
                            'Номер частотного канала', '2d', 3)
                    self.draw_DSPEC.show()
                    self.top_wins.append(self.draw_DSPEC)

                if self.paint_FLTR.isChecked():
                    mask = self.pulse.GetChannelMask()
                    self.draw_MASK = drawPlot(mask, 'Частотный фильтр',
                                              'Номер частотного канала',
                                              'Амплитуда', '1d', 4)
                    self.draw_MASK.show()
                    self.top_wins.append(self.draw_MASK)

                if self.paint_CCF.isChecked():
                    self.draw_CCF = drawPlot(self.kkf.kkfdata,
                                             'Кросс-корреляционная функция',
                                             'Отсчет', 'Амплитуда', '1d', 5)
                    self.draw_CCF.show()
                    self.top_wins.append(self.draw_CCF)

            elif reply == QMessageBox.No:
                pass

        elif self.workstate_LOOP.isChecked():

            test_value = self.check_value()

            if test_value == 0:
                self.log_box.clear()
                self.log_box.append('Пройдена проверка вводимых данных')
            elif test_value == 1:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введено неккоректное значение порога чистки АЧХ!' +
                        'Неккоректное значение изменено на значение по умолчанию.')

                msg.exec_()
                self.form_tres_ACH.setText('2.0')
                return None
            elif test_value == 2:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введено неккоректное значение параметра отсечки импульсных помех!' +
                        'Неккоректное значение изменено на значение по умолчанию.')
                msg.exec_()
                self.form_tres_PULS.setText('4.0')
                return None
            elif test_value == 3:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введено неккоректное значение коррекции UTC!' +
                        'Неккоректное значение изменено на значение по умолчанию.')
                msg.exec_()
                self.form_LOCSCALE.setText('0.0')
                return None
            elif test_value == 4:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введен неккоректный путь выходной директории!' +
                        'Неккоректный путь изменен на путь по умолчанию.')
                msg.exec_()
                self.form_dir.setText(os.getcwd() + DELIMETER)
                return None
            elif test_value == 5:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Введен слишком длинный путь выходной директории!' +
                        'Путь к директории не должен превышать 49 символов. ' +
                        'Неккоректный путь изменен на путь по умолчанию.')
                msg.exec_()
                self.form_dir.setText(os.getcwd() + DELIMETER)
                return None

            self.dirLoop = QFileDialog.getExistingDirectory(
                    self,'Выберете каталог с файлами для обработки',
                    options=options)

            if self.dirLoop:
                pass
            else:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Не выбран каталог')
                msg.exec_()
                return None

            self.file_list = sorted(glob.glob(self.dirLoop + DELIMETER + '*'))
            self.files_box.clear()
            for file in self.file_list:

                self.files_box.append(os.path.basename(file))

            QGuiApplication.processEvents()
            self.tpl_file, _ = QFileDialog.getOpenFileName(
                    self,'Выберите файл шаблона для корреляции',
                    '', 'Файл шаблона (*.tpl);; Текстовый файл (*.txt)',
                    options=options)

            if self.tpl_file:
                pass
            else:
                msg = QuestionBox('Не выбран файл шаблона для корреляции',
                                  'Продолжить работу программы?')
                reply = msg.exec_()
                if reply == QMessageBox.Yes:
                    pass
                elif reply == QMessageBox.No:
                    msg = WarningBox(
                        'Работа программы приостановлена.',
                        '')
                    msg.exec_()
                    return None
            for item in self.file_list:
                test_value = self.check_file(item)

                if test_value == 0:
                    self.log_box.append('Пройдена проверка файла')
                    QGuiApplication.processEvents()
                elif test_value == 6:
                    msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Выбран некорректный файл')
                    msg.exec_()
                    return None

                self.log_box.append('Загружается файл: ' +
                    os.path.basename(item) +
                    '. Это может занять некоторое время.')

                QGuiApplication.processEvents() # Работает как обновление интерфейса
                self.br = rT.BaseRun()
                self.br.ReadRAWData(
                        os.path.basename(item),
                        os.path.dirname(item) + DELIMETER,
                        self.form_dir.text())
                self.log_box.append(
                        'Файл ' + os.path.basename(item) + ' загружен.')
                self.pulse = rT.PulseExtractor(self.br)
                self.log_box.append(
                        'Чтение маски из файла: examples' + DELIMETER + 'channel_mask.dat')
                QGuiApplication.processEvents()
                self.pulse.ReadMask("examples" + DELIMETER + "channel_mask.dat")
                self.pulse.NormaliseToUnity(False)
                self.pulse.FillMaskFRweights()
                QGuiApplication.processEvents()
                self.pulse = self.clearing(self.pulse)
                self.pulse = self.writting(self.pulse, item)
                if self.tpl_file:
                    kkf, itoa = self.ccf(self.pulse, os.path.dirname(item),
                                     self.form_dir.text(), self.tpl_file,
                                     os.path.basename(item),
                                     float(self.form_LOCSCALE.text()))
                else:
                    pass

        elif self.workstate_CONFIG.isChecked():
            self.conf_file, _ = QFileDialog.getOpenFileName(
                    self,'Выберите конфигурационный файл',
                    '', 'Конф. файл (*.cfg);; Текстовый файл (*.txt)',
                    options=options)
            if self.conf_file:
                pass
            else:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Не выбран конфигурационный файл!')
                msg.exec_()
                return None

            test_value = self.check_conf(self.conf_file)

            if test_value == 0:
                self.log_box.clear()
                self.log_box.append('Пройдена конфигурационного файла')
                QGuiApplication.processEvents()
            else:
                msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Выбран неккоректный конфигурационный файл!')
                msg.exec_()
                return None

            self.log_box.append(
                    'Чтение конфигурационного файла: ' + self.conf_file)
            conf = rT.ReadConfig(self.conf_file)
            # Устновка управляющих элементов согласно считанному конфигурационному файлу
            self.files_box.clear()
            for file in conf.runs:

                self.files_box.append(os.path.basename(file))

            for item in conf.runs:

                test_value = self.check_file(conf.rawdata_dir + item)
                if test_value == 0:
                    self.log_box.append('Пройдена проверка файла')
                    QGuiApplication.processEvents()
                elif test_value == 6:
                    msg = WarningBox(
                        'Работа программы приостановлена.',
                        'Выбран некорректный файл')
                    msg.exec_()
                    return None

                self.log_box.append(
                        'Загружается файл ' + item +
                        '. Это может занять некоторое время.')
                QGuiApplication.processEvents()
                br = rT.BaseRun()
                br.ReadRAWData(item, conf.rawdata_dir, conf.output_dir)
                self.log_box.append('Файл ' + item + ' загружен.')
                QGuiApplication.processEvents()
                pulse = rT.PulseExtractor(br)
                pulse.ReadMask(conf.maskfile)
                self.log_box.append('Чтение маски из файла:' + conf.maskfile)
                QGuiApplication.processEvents()
                pulse.NormaliseToUnity(not conf.useTrueNorm)
                pulse.FillMaskFRweights()
                self.log_box.append('Вычисление весов каналов по АЧХ')
                QGuiApplication.processEvents()
                pulse = self.clearing_conf(pulse, conf)

                if not conf.getIndImpulses:
                    pulse.SumPerChannelPeriods()
                    self.log_box.append('Суммирование периодов в каждом канале')
                    QGuiApplication.processEvents()
                    pulse.DoCompensation()
                    self.log_box.append('Компенсация дисперсионного запаздывания')
                    QGuiApplication.processEvents()

                    if (conf.getDynSpectrum):
                        pulse.PrintChannelSumProfile(conf.output_dir)
                        self.log_box.append(
                                'Динамический спектр записан в файл:' +
                                conf.output_dir + 'bands_' + item + '.prf')
                        QGuiApplication.processEvents()

                if (conf.getIndImpulses):
                    if  not conf.getDynSpectrum:
                        self.log_box.append(
                                'Компенсация дисперсионного запаздывания')
                        QGuiApplication.processEvents()
                        pulse.DoCompensation()
                        pulse.PrintCompensatedImpulses(conf.output_dir)
                        self.log_box.append(
                                'Массив индивидуальных импульсов записан в файл: '
                                + conf.output_dir + 'compPulses_' + item +
                                '.prf');
                        QGuiApplication.processEvents()

                    if conf.getDynSpectrum:
                        pulse.DoCompensation()
                        self.log_box.append(
                                'Компенсация дисперсионного запаздывания')
                        QGuiApplication.processEvents()
                        pulse.PrintCompensatedImpulses(conf.output_dir)
                        self.log_box.append(
                                'Массив индивидуальных импульсов записан в файл: '
                                + conf.output_dir + 'compPulses_' + item +
                                '.prf');
                        QGuiApplication.processEvents()
                        pulse.SumPerChannelPeriods()
                        self.log_box.append(
                                'Суммирование периодов в каждом канале');
                        QGuiApplication.processEvents()
                        pulse.DoCompensation()
                        self.log_box.append(
                                'Компенсация дисперсионного запаздывания');
                        QGuiApplication.processEvents()
                        pulse.PrintChannelSumProfile(conf.output_dir)
                        self.log_box.append(
                                'Динамический спектр записан в файл:' +
                                conf.output_dir + 'bands_' + item + '.prf');
                        QGuiApplication.processEvents()

                pulse.SumPeriods()
                pulse.PrintSumProfile(conf.output_dir)

                if conf.getFR:
                    pulse.PrintFrequencyResponse(conf.output_dir)
                    self.log_box.append('АЧХ записана в файл:' +
                                       conf.output_dir + item + '.fr')
                    self.log_box.append('АЧХ после маски записана в файл:' +
                                       conf.output_dir + 'masked_' + item +
                                       '.fr')
                    QGuiApplication.processEvents()

                kkf, itoa = self.ccf(
                        pulse, conf.rawdata_dir,
                        conf.output_dir, conf.tplfile,
                        item, conf.utccorr)
                self.log_box.append(
                        "######################################################")
                QGuiApplication.processEvents()

        msg = WarningBox(
                    'Данные успешно обработаны',
                    '')
        msg.exec_()
        #self.procces_button.setEnabled(True)

    def review_out_dir(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self.out_dir = str(QFileDialog.getExistingDirectory(
                self,
                'Выберите директорию для выходных файлов',
                options=options))
        self.form_dir.setText(self.out_dir)

    def clear_log_box(self):
        self.log_box.clear()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())
