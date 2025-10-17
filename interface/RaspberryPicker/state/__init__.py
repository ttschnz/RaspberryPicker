import serial
import tkinter as tk
from collections import defaultdict
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
import numpy

class KeyDefaultDict(defaultdict):
    #default_factory: Callable[[str], _VT] | None
    def __missing__(self, key):
        if self.default_factory is None:
            raise KeyError( key )
        else:
            ret = self[key] = self.default_factory(key)
            return ret

class ValueVar(tk.StringVar):
    def __init__(self, state, master: tk.Misc | None = None, value: str | None = None, name: str | None = None) -> None:
        self.state = state
        super().__init__(master, value, name)

    def set(self, value: str) -> None:
        super().set(value)
        self.state.send(self._name, value)
        pass

class State:
    port = None
    baudrate = 9600
    arduino = None
    fig=None

    def __init__(self, control_center):
        self.control_center = control_center
        self.values = KeyDefaultDict(lambda key: ValueVar(self, self.control_center, value="", name=key))
        self.color_sensor_values = {"r":[], "g":[], "b":[]}

        pass

    def set_port(self, new_comport: None | str)->None:
        self.port = new_comport
        try:
            self.arduino = serial.Serial(self.port, self.baudrate)
        except:
            self.arduino = None

    def get_port(self)->str:
        return self.port if not (self.port is None) else ""

    def listen_values(self)->bool:
        if self.arduino is None:
            return False

        if self.arduino.is_open == False:
            return False

        if self.arduino.in_waiting==0:
            return False

        while (self.arduino.in_waiting>0):
            line = self.arduino.readline().decode('utf-8')
            print(f"> {line}")
            key, value = line.split("=",1)
            self.values[key].set(value)

            # gripper.ripeness.g
            if key.startswith("gripper.ripeness."):
                self.color_sensor_values[key.split(".")[-1]].append(int(value))

        return True

    def send(self, key, value)->bool:
        print(f"< {key}={value}")
        if self.arduino is None:
            return False

        if self.arduino.is_open == False:
            return False


        content = f"{key}={value}\r\n"
        self.arduino.write(content.encode('utf-8'))
        return True


    def update_color_sensor_plot(self):
        self.color_sensor_values["r"].append([int(x) for x in numpy.random.random(2)*100])
        self.color_sensor_values["g"].append([int(x) for x in numpy.random.random(2)*100])
        self.color_sensor_values["b"].append([int(x) for x in numpy.random.random(2)*100])
        if self.fig is None:
            self.fig = Figure(figsize = (5, 2), dpi = 100)
            self.plt = self.fig.add_subplot(111)
            self.canvas = FigureCanvasTkAgg(self.fig,master=self.control_center.gripper_frame)

        self.plt.clear()

        self.plt.plot(self.color_sensor_values["r"], color='red', label='Red Line')
        self.plt.plot(self.color_sensor_values["g"], color='green', label='Green Line')
        self.plt.plot(self.color_sensor_values["b"], color='blue', label='Blue Line')

        self.canvas.draw()
        self.canvas.get_tk_widget().grid(column=0, row=3, sticky=tk.W, columnspan=8)
        pass
