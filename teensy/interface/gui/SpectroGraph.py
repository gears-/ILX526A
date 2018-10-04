from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

class SpectroGraph(MplCanvas):
    """A canvas that updates itself with a new plot."""

    def __init__(self, *args, **kwargs):
        MplCanvas.__init__(self, *args, **kwargs)
        #timer = QtCore.QTimer(self)
        #timer.timeout.connect(self.update_figure)
        #timer.start(1000)

    def compute_initial_figure(self):
        pass
        
    def update_figure(self):
        pass

