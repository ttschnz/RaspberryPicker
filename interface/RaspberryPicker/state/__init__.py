import serial

class State:
    port = None
    baudrate = 9600
    values = {}
    arduino = None

    def __init__(self):
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
            self.values[key] = value

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
