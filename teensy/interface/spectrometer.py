#!/usr/bin/python3

import sys

import matplotlib
matplotlib.use('Qt5Agg') # Make sure that we are using QT5

from PyQt5.QtWidgets import QApplication


if __name__ == '__main__':
    qApp = QApplication(sys.argv)

    aw = ApplicationWindow()
    aw.show()
    sys.exit(qApp.exec_())
