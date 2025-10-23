import tkinter as tk
import serial.tools.list_ports
import customtkinter as ctk

from RaspberryPicker.state import State

class ControlCenter(ctk.CTk):
    controller_states = ("MANUAL", "IDLE", "PROGRAM")
    controller_programs = ("CLOSE", "RELEASE", "DROP", "RESET", "CALIBRATE_COLOR")
    sorting_states = ("LARGE", "SMALL", "IDLE")
    door_states = ("OPEN_SMALL", "OPEN_LARGE", "CLOSED")
    gripper_states = ("OPEN", "CLOSED")

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        paddings = {'padx': 10, 'pady':5}
        self.state_manager = State(self)

        # Window settings
        self.title("Control Centre")
        self.geometry("920x550")
        self.minsize(700, 550)
        ctk.set_appearance_mode("system")  # or "light"/"dark"
        ctk.set_default_color_theme("green")

        # Menu bar (still uses tkinter's Menu since CTk doesn't provide one)
        # self.menu = ctk.CTkMenu(self)
        try:
            import tkinter as tk
            self.menu = tk.Menu(self)
            self.config(menu=self.menu)

            file_menu = tk.Menu(self.menu, tearoff=0)
            file_menu.add_command(label="Exit", command=self.destroy)
            self.menu.add_cascade(label="File", menu=file_menu)

            self.port_list_menu = tk.Menu(self.menu, tearoff=0)
            self.selected_port = ctk.StringVar(value=self.state_manager.port)
            self.port_list_menu.configure(postcommand=self.update_port_list)
            self.menu.add_cascade(label="Port", menu=self.port_list_menu)
        except Exception:
            pass

        # Status bar
        self.status_bar = ctk.CTkLabel(self, text="initialising", anchor="w")
        self.status_bar.pack(fill="x", side="bottom", padx=2, pady=2)

        # Main layout
        self.main_frame = ctk.CTkFrame(self)
        self.main_frame.pack(fill="both", expand=True)

        self.main_frame.grid_columnconfigure(0, weight=1, minsize=200)
        self.main_frame.grid_columnconfigure(1, weight=0, minsize=300)
        self.main_frame.grid_rowconfigure(0, weight=1)
        # Left panel: log
        self.logs = ctk.CTkTextbox(self.main_frame, state=ctk.DISABLED)
        self.logs.grid(column=0, row=0, sticky=ctk.NSEW)


        # Right panel
        self.right_frame = ctk.CTkFrame(self.main_frame)
        self.right_frame.grid(column=1, row=0, sticky="nsew", padx=5, pady=5)
        self.right_frame.grid_rowconfigure((0, 1, 2), weight=1)
        self.right_frame.grid_columnconfigure(0, weight=1)

        # Controller section
        self.controller_frame = ctk.CTkFrame(self.right_frame)
        self.controller_frame.grid(column=0, row=0, sticky="nsew", **paddings)

        ctk.CTkLabel(self.controller_frame, text="Controller State").grid(column=0, row=0, sticky="w", **paddings)
        self.controller_state_options = ctk.CTkOptionMenu(
            self.controller_frame,
            variable=self.state_manager.values["controller.state"],
            values=list(self.controller_states)
        )
        self.controller_state_options.grid(column=1, row=0, sticky="w", **paddings)

        ctk.CTkLabel(self.controller_frame, text="Controller Program").grid(column=0, row=1, sticky="w", **paddings)
        self.controller_program_options = ctk.CTkOptionMenu(
            self.controller_frame,
            variable=self.state_manager.values["controller.program"],
            values=list(self.controller_programs)
        )
        self.controller_program_options.grid(column=1, row=1, sticky="w", **paddings)

        # Gripper section
        self.gripper_frame = ctk.CTkFrame(self.right_frame)
        self.gripper_frame.grid(column=0, row=1, sticky="nsew", **paddings)

        ctk.CTkLabel(self.gripper_frame, text="Gripper").grid(column=0, row=0, sticky="w", **paddings)
        self.gripper_gripper_state_options = ctk.CTkOptionMenu(
            self.gripper_frame,
            variable=self.state_manager.values["gripper.gripper_state"],
            values=list(self.gripper_states)
        )
        self.gripper_gripper_state_options.grid(column=1, row=0, sticky="w", **paddings)

        ctk.CTkLabel(self.gripper_frame, text="Position").grid(column=2, row=0, sticky="w", **paddings)
        ctk.CTkEntry(self.gripper_frame, textvariable=self.state_manager.values["gripper.plate_distance"], state="disabled", width=80).grid(column=3, row=0, **paddings)


        ctk.CTkLabel(self.gripper_frame, text="Size").grid(column=0, row=1, sticky="w", **paddings)
        ctk.CTkEntry(self.gripper_frame, textvariable=self.state_manager.values["gripper.raspberry_size"], state="disabled", width=80).grid(column=1, row=1, **paddings)

        ctk.CTkLabel(self.gripper_frame, text="p_s").grid(column=2, row=1, sticky="w", **paddings)
        ctk.CTkEntry(self.gripper_frame, textvariable=self.state_manager.values["gripper.berry_p_small"], state="disabled", width=50).grid(column=3, row=1, **paddings)

        ctk.CTkLabel(self.gripper_frame, text="p_l").grid(column=4, row=1, sticky="w", **paddings)
        ctk.CTkEntry(self.gripper_frame, textvariable=self.state_manager.values["gripper.berry_p_large"], state="disabled", width=50).grid(column=5, row=1, **paddings)

        ctk.CTkLabel(self.gripper_frame, text="Ripeness").grid(column=0, row=2, sticky="w", **paddings)
        ctk.CTkEntry(self.gripper_frame, textvariable=self.state_manager.values["gripper.raspberry_ripeness"], state="disabled", width=80).grid(column=1, row=2, **paddings)

        for i, (color, var) in enumerate([("r", "gripper.ripeness.r"), ("g", "gripper.ripeness.g"), ("b", "gripper.ripeness.b")]):
            ctk.CTkLabel(self.gripper_frame, text=color).grid(column=2 + i*2, row=2, sticky="w", **paddings)
            ctk.CTkEntry(self.gripper_frame, textvariable=self.state_manager.values[var], state="disabled", width=50).grid(column=3 + i*2, row=2, **paddings)

        # Basket section
        self.basket_frame = ctk.CTkFrame(self.right_frame)
        self.basket_frame.grid(column=0, row=2, sticky="nsew", **paddings)

        ctk.CTkLabel(self.basket_frame, text="Fill").grid(column=0, row=0, sticky="w", **paddings)
        ctk.CTkLabel(self.basket_frame, text="Small").grid(column=1, row=0, sticky="w", **paddings)
        ctk.CTkEntry(self.basket_frame, textvariable=self.state_manager.values["basket.fill_count.small"], state="disabled", width=50).grid(column=2, row=0, **paddings)

        ctk.CTkLabel(self.basket_frame, text="Large").grid(column=3, row=0, sticky="w", **paddings)
        ctk.CTkEntry(self.basket_frame, textvariable=self.state_manager.values["basket.fill_count.large"], state="disabled", width=50).grid(column=4, row=0, **paddings)

        ctk.CTkLabel(self.basket_frame, text="Sorting").grid(column=0, row=1, sticky="w", **paddings)
        ctk.CTkLabel(self.basket_frame, text="State").grid(column=1, row=1, sticky="w", **paddings)
        ctk.CTkOptionMenu(self.basket_frame, variable=self.state_manager.values["basket.sorting.state"], values=list(self.sorting_states)).grid(column=2, row=1, **paddings)

        ctk.CTkLabel(self.basket_frame, text="Position").grid(column=3, row=1, sticky="w", **paddings)
        ctk.CTkEntry(self.basket_frame, textvariable=self.state_manager.values["basket.sorting.position"], state="disabled", width=50).grid(column=4, row=1, **paddings)

        ctk.CTkLabel(self.basket_frame, text="Door").grid(column=0, row=2, sticky="w", **paddings)
        ctk.CTkLabel(self.basket_frame, text="State").grid(column=1, row=2, sticky="w", **paddings)
        ctk.CTkOptionMenu(self.basket_frame, variable=self.state_manager.values["basket.door.state"], values=list(self.door_states)).grid(column=2, row=2, **paddings)

        ctk.CTkLabel(self.basket_frame, text="Position").grid(column=3, row=2, sticky="w", **paddings)
        ctk.CTkEntry(self.basket_frame, textvariable=self.state_manager.values["basket.door.position"], state="disabled", width=50).grid(column=4, row=2, **paddings)

        self.update_state()

    def update_port_list(self):
        self.port_list_menu.delete(0, 'end')  # clear the menu
        for port in serial.tools.list_ports.comports():

            port_id = port.device
            port_name = port.name
            port_desc = port.manufacturer.split(" (")[0] if not (port.manufacturer is None) else None

            is_current = (port_id == self.state_manager.port)
            self.port_list_menu.add_radiobutton(
                label=f"{port_name} ({port_desc})" if not (port_desc is None) else port_name,
                variable=self.selected_port,
                value=port_id,
                command=lambda x=port_id: self.state_manager.set_port(x)
            )
        # Update the variable to reflect the current selected port
        self.selected_port.set(self.state_manager.get_port())


    def update_state(self)->None:
        if self.state_manager.arduino is None:
            self.status_bar.configure(text=f"Not connected ({self.state_manager.get_port()})")
        else:
            self.status_bar.configure(text=f"Connected to {self.state_manager.get_port()}")

        self.state_manager.listen_values()

        self.state_manager.update_color_sensor_plot()

        self.after(75, self.update_state)


if __name__ == "__main__":
    main_window = ControlCenter()
    main_window.mainloop()
