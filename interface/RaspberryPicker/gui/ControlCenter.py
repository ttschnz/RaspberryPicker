import tkinter as tk
import serial.tools.list_ports

from RaspberryPicker.state import State

class ControlCenter(tk.Tk):
    controller_states = ("MANUAL","IDLE","PROGRAM")
    controller_programs = ("CLOSE","RELEASE","DROP","RESET","CALIBRATE_COLOR")


    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        paddings = {'padx': 2, 'pady': 2}

        self.state = State()

        self.title("Control Centre")
        self.config(bg="#E4E2E2")
        self.geometry("700x370")

        self.menu = tk.Menu(self)
        self.config(menu=self.menu)

        file_menu = tk.Menu(self.menu, tearoff=0)
        file_menu.add_command(
            label="Exit",
            command=self.destroy,
        )
        self.menu.add_cascade(label="File", menu=file_menu)


        self.port_list_menu = tk.Menu(self.menu, tearoff=0)
        self.selected_port = tk.StringVar(value=self.state.port)
        # Refresh ports whenever menu is about to appear
        self.port_list_menu.configure(postcommand=self.update_port_list)
        self.menu.add_cascade(label="Port", menu=self.port_list_menu)

        self.status_bar = tk.Label(self, text="initialising", anchor=tk.W, relief=tk.SUNKEN, bd=1)
        self.status_bar.pack(fill=tk.X, side=tk.BOTTOM, ipadx=2)

        self.main_frame = tk.Frame(self)
        self.main_frame.pack(fill=tk.BOTH, expand=True)
        self.main_frame.columnconfigure(0, weight=2)
        self.main_frame.columnconfigure(1, weight=3)
        self.main_frame.rowconfigure(0, weight=1)


        # main controller on the right
        self.right_frame = tk.Frame(self.main_frame, borderwidth=1, relief=tk.RIDGE)
        self.right_frame.grid(column=1, row=0, sticky=tk.NSEW)
        self.right_frame.rowconfigure(0, weight=1)
        self.right_frame.rowconfigure(1, weight=1)
        self.right_frame.rowconfigure(2, weight=2)
        self.right_frame.columnconfigure(0, weight=1)

        # controller
        self.controller_frame = tk.Frame(self.right_frame, borderwidth=1, relief=tk.RIDGE)
        self.controller_frame.grid(column=0, row=0, sticky=tk.NSEW)

        self.controller_state_label = tk.Label(self.controller_frame, text="Controller State")
        self.controller_state_label.grid(column=0, row=0, sticky=tk.W, **paddings)
        self.controller_state_var = tk.StringVar(self, value=self.controller_states[0])
        self.controller_state_options = tk.OptionMenu(
            self.controller_frame,
            self.controller_state_var,
            *self.controller_states,
            command=lambda x: self.state.send("controller.state",x))
        self.controller_state_options.grid(column=1, row=0, sticky=tk.W, **paddings)

        self.controller_program_label = tk.Label(self.controller_frame, text="Controller Program")
        self.controller_program_label.grid(column=0, row=1, sticky=tk.W, **paddings)
        self.controller_program_var = tk.StringVar(self, value=self.controller_programs[0])
        self.controller_program_options = tk.OptionMenu(
            self.controller_frame,
            self.controller_program_var,
            *self.controller_programs,
            command=lambda x: self.state.send("controller.program", x))
        self.controller_program_options.grid(column=1, row=1, sticky=tk.W, **paddings)

        # gripper
        self.gripper_frame = tk.Frame(self.right_frame, borderwidth=1, relief=tk.RIDGE)
        self.gripper_frame.grid(column=0, row=1, sticky=tk.NSEW)

        # basket
        self.basket_frame = tk.Frame(self.right_frame, borderwidth=1, relief=tk.RIDGE)
        self.basket_frame.grid(column=0, row=2, sticky=tk.NSEW)

        self.update_state()

    def update_port_list(self):
        self.port_list_menu.delete(0, 'end')  # clear the menu
        for port in serial.tools.list_ports.comports():

            port_id = port.device
            port_name = port.name
            port_desc = port.description if not (port.description == "n/a") else None

            is_current = (port_id == self.state.port)
            self.port_list_menu.add_radiobutton(
                label=f"{port_name} ({port_desc})" if not (port_desc is None) else port_name,
                variable=self.selected_port,
                value=port_id,
                command=lambda x=port_id: self.state.set_port(x)
            )
        # Update the variable to reflect the current selected port
        self.selected_port.set(self.state.get_port())


    def update_state(self)->None:
        if self.state.arduino is None:
            self.status_bar.config(text=f"Not connected ({self.state.get_port()})")
        else:
            self.status_bar.config(text=f"Connected to {self.state.get_port()}")

        if "controller.state" in self.state.values:
            self.controller_state_var.set(self.state.values["controller.state"])
        if "controller.program" in self.state.values:
            self.controller_program_var.set(self.state.values["controller.program"])

        # if "gripper.berry_p_small" in self.state.values:
        #     self.gripper_berry_p_small_var.set(self.state.values["gripper.berry_p_small"])
        # if "gripper.berry_p_large" in self.state.values:
        #     self.gripper_berry_p_large_var.set(self.state.values["gripper.berry_p_large"])
        # if "gripper.raspberry_size" in self.state.values:
        #     self.gripper_raspberry_size_var.set(self.state.values["gripper.raspberry_size"])
        # if "gripper.raspberry_ripeness" in self.state.values:
        #     self.gripper_raspberry_ripeness_var.set(self.state.values["gripper.raspberry_ripeness"])
        # if "gripper.ripeness.r" in self.state.values:
        #     self.gripper_ripeness_r_var.set(self.state.values["gripper.ripeness.r"])
        # if "gripper.ripeness.g" in self.state.values:
        #     self.gripper_ripeness_g_var.set(self.state.values["gripper.ripeness.g"])
        # if "gripper.ripeness.b" in self.state.values:
        #     self.gripper_ripeness_b_var.set(self.state.values["gripper.ripeness.b"])

        # if "basket.fill_count.small" in self.state.values:
        #     self.basket_fill_count_small_var.set(self.state.values["basket.fill_count.small"])
        # if "basket.fill_count.large" in self.state.values:
        #     self.basket_fill_count_large_var.set(self.state.values["basket.fill_count.large"])
        # if "basket.sorting.state" in self.state.values:
        #     self.basket_sorting_state_var.set(self.state.values["basket.sorting.state"])
        # if "basket.sorting.position" in self.state.values:
        #     self.basket_sorting_position_var.set(self.state.values["basket.sorting.position"])
        # if "basket.door.position" in self.state.values:
        #     self.basket_door_position_var.set(self.state.values["basket.door.position"])
        # if "basket.door.state" in self.state.values:
        #     self.basket_door_state_var.set(self.state.values["basket.door.state"])

        self.after(100, self.update_state)


if __name__ == "__main__":
    main_window = ControlCenter()
    main_window.mainloop()
