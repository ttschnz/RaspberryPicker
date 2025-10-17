import tkinter as tk
import serial.tools.list_ports

from RaspberryPicker.state import State

class ControlCenter(tk.Tk):
    controller_states = ("MANUAL","IDLE","PROGRAM")
    controller_programs = ("CLOSE","RELEASE","DROP","RESET","CALIBRATE_COLOR")
    sorting_states = ("LARGE","SMALL","IDLE")
    door_states = ("OPEN_SMALL","OPEN_LARGE","CLOSED")

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        paddings = {'padx': 2, 'pady': 2}
        self.state = State(self)

        self.title("Control Centre")
        self.config(bg="#E4E2E2")
        self.geometry("920x520")
        self.minsize(700, 500)

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
        self.main_frame.columnconfigure(0, weight=1,minsize=200)
        self.main_frame.columnconfigure(1, weight=0,minsize=300)
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
        self.controller_state_options = tk.OptionMenu(
            self.controller_frame,
            self.state.values["controller.state"],
            *self.controller_states
        )
        self.controller_state_options.grid(column=1, row=0, sticky=tk.W, **paddings)

        self.controller_program_label = tk.Label(self.controller_frame, text="Controller Program")
        self.controller_program_label.grid(column=0, row=1, sticky=tk.W, **paddings)
        self.controller_program_options = tk.OptionMenu(
            self.controller_frame,
            self.state.values["controller.program"],
            *self.controller_programs
        )
        self.controller_program_options.grid(column=1, row=1, sticky=tk.W, **paddings)

        # gripper
        self.gripper_frame = tk.Frame(self.right_frame, borderwidth=1, relief=tk.RIDGE)
        self.gripper_frame.grid(column=0, row=1, sticky=tk.NSEW)


        self.raspberry_size_label = tk.Label(self.gripper_frame, text="Size")
        self.raspberry_size_label.grid(column=0, row=0, sticky=tk.W, **paddings)
        self.raspberry_size_value = tk.Entry(self.gripper_frame, textvariable=self.state.values["gripper.raspberry_size"], state=tk.DISABLED, width=10)
        self.raspberry_size_value.grid(column=1, row=0, sticky=tk.W, **paddings)

        self.raspberry_probability_small_label = tk.Label(self.gripper_frame, text="p_s")
        self.raspberry_probability_small_label.grid(column=2, row=0, sticky=tk.W, **paddings)
        self.raspberry_probability_small_value = tk.Entry(self.gripper_frame, textvariable=self.state.values["gripper.berry_p_small"], state=tk.DISABLED, width=5)
        self.raspberry_probability_small_value.grid(column=3, row=0, sticky=tk.W, **paddings)

        self.raspberry_probability_large_label = tk.Label(self.gripper_frame, text="p_l")
        self.raspberry_probability_large_label.grid(column=4, row=0, sticky=tk.W, **paddings)
        self.raspberry_probability_large_value = tk.Entry(self.gripper_frame, textvariable=self.state.values["gripper.berry_p_large"], state=tk.DISABLED, width=5)
        self.raspberry_probability_large_value.grid(column=5, row=0, sticky=tk.W, **paddings)


        self.raspberry_ripeness_label = tk.Label(self.gripper_frame, text="Ripeness")
        self.raspberry_ripeness_label.grid(column=0, row=1, sticky=tk.W, **paddings)
        self.raspberry_ripeness_value = tk.Entry(self.gripper_frame, textvariable=self.state.values["gripper.raspberry_ripeness"], state=tk.DISABLED, width=10)
        self.raspberry_ripeness_value.grid(column=1, row=1, sticky=tk.W, **paddings)


        self.raspberry_ripeness_r_label = tk.Label(self.gripper_frame, text="r")
        self.raspberry_ripeness_r_label.grid(column=2, row=1, sticky=tk.W, **paddings)
        self.raspberry_ripeness_r_value = tk.Entry(self.gripper_frame, textvariable=self.state.values["gripper.ripeness.r"], state=tk.DISABLED, width=5)
        self.raspberry_ripeness_r_value.grid(column=3, row=1, sticky=tk.W, **paddings)

        self.raspberry_ripeness_g_label = tk.Label(self.gripper_frame, text="g")
        self.raspberry_ripeness_g_label.grid(column=4, row=1, sticky=tk.W, **paddings)
        self.raspberry_ripeness_g_value = tk.Entry(self.gripper_frame, textvariable=self.state.values["gripper.ripeness.g"], state=tk.DISABLED, width=5)
        self.raspberry_ripeness_g_value.grid(column=5, row=1, sticky=tk.W, **paddings)

        self.raspberry_ripeness_b_label = tk.Label(self.gripper_frame, text="b")
        self.raspberry_ripeness_b_label.grid(column=6, row=1, sticky=tk.W, **paddings)
        self.raspberry_ripeness_b_value = tk.Entry(self.gripper_frame, textvariable=self.state.values["gripper.ripeness.b"], state=tk.DISABLED, width=5)
        self.raspberry_ripeness_b_value.grid(column=7, row=1, sticky=tk.W, **paddings)

        # basket
        self.basket_frame = tk.Frame(self.right_frame, borderwidth=1, relief=tk.RIDGE)
        self.basket_frame.grid(column=0, row=2, sticky=tk.NSEW)

        self.basket_fill_label = tk.Label(self.basket_frame, text="Fill")
        self.basket_fill_label.grid(column=0, row=0, sticky=tk.W, **paddings)

        self.basket_fill_small_label = tk.Label(self.basket_frame, text="Small")
        self.basket_fill_small_label.grid(column=1, row=0, sticky=tk.W, **paddings)
        self.basket_fill_small_value = tk.Entry(self.basket_frame, textvariable=self.state.values["basket.fill_count.small"], state=tk.DISABLED, width=5)
        self.basket_fill_small_value.grid(column=2, row=0, sticky=tk.W, **paddings)

        self.basket_fill_large_label = tk.Label(self.basket_frame, text="Large")
        self.basket_fill_large_label.grid(column=3, row=0, sticky=tk.W, **paddings)
        self.basket_fill_large_value = tk.Entry(self.basket_frame, textvariable=self.state.values["basket.fill_count.large"], state=tk.DISABLED, width=5)
        self.basket_fill_large_value.grid(column=4, row=0, sticky=tk.W, **paddings)

        self.basket_sorting_label = tk.Label(self.basket_frame, text="Sorting")
        self.basket_sorting_label.grid(column=0, row=1, sticky=tk.W, **paddings)

        self.basket_sorting_state_label = tk.Label(self.basket_frame, text="State")
        self.basket_sorting_state_label.grid(column=1, row=1, sticky=tk.W, **paddings)
        self.basket_sorting_state_options = tk.OptionMenu(
            self.basket_frame,
            self.state.values["basket.sorting.state"],
            *self.sorting_states
        )
        self.basket_sorting_state_options.grid(column=2, row=1, sticky=tk.W, **paddings)

        self.basket_sorting_position_label = tk.Label(self.basket_frame, text="Position")
        self.basket_sorting_position_label.grid(column=3, row=1, sticky=tk.W, **paddings)
        self.basket_sorting_position_label = tk.Entry(self.basket_frame, textvariable=self.state.values["basket.sorting.position"], state=tk.DISABLED, width=5)
        self.basket_sorting_position_label.grid(column=4, row=1, sticky=tk.W, **paddings)



        self.basket_door_label = tk.Label(self.basket_frame, text="Door")
        self.basket_door_label.grid(column=0, row=2, sticky=tk.W, **paddings)

        self.basket_door_state_label = tk.Label(self.basket_frame, text="State")
        self.basket_door_state_label.grid(column=1, row=2, sticky=tk.W, **paddings)
        self.basket_door_state_options = tk.OptionMenu(
            self.basket_frame,
            self.state.values["basket.door.state"],
            *self.door_states
        )
        self.basket_door_state_options.grid(column=2, row=2, sticky=tk.W, **paddings)

        self.basket_door_position_label = tk.Label(self.basket_frame, text="Position")
        self.basket_door_position_label.grid(column=3, row=2, sticky=tk.W, **paddings)
        self.basket_door_position_label = tk.Entry(self.basket_frame, textvariable=self.state.values["basket.door.position"], state=tk.DISABLED, width=5)
        self.basket_door_position_label.grid(column=4, row=2, sticky=tk.W, **paddings)

        self.update_state()

    def update_port_list(self):
        self.port_list_menu.delete(0, 'end')  # clear the menu
        for port in serial.tools.list_ports.comports():

            port_id = port.device
            port_name = port.name
            port_desc = port.manufacturer.split(" (")[0] if not (port.manufacturer is None) else None

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

        self.state.update_color_sensor_plot()
        self.after(75, self.update_state)


if __name__ == "__main__":
    main_window = ControlCenter()
    main_window.mainloop()
